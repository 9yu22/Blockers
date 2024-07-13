// Fill out your copyright notice in the Description page of Project Settings.


#include "BKChunkBase.h"
#include "BKChunkWorld.h"
#include "BKVoxelFunctionLibrary.h"	// for localBlockPosition
#include "CollapsibleBlockComponent.h"	// for collapsible Block

#include "Voxel/BKEnum.h"
#include "ProceduralMeshComponent.h"

#include "Voxel/FastNoiseLite.h"
#include "../Network/SGameInstance.h"
#include "../Network/ProcessQueue.h"

// Sets default values
ABKChunkBase::ABKChunkBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh");
	Noise = new FastNoiseLite();

	// Mesh Settings
	Mesh->SetCastShadow(true);

	// Set Mesh as root
	SetRootComponent(Mesh);
}

// Called when the game starts or when spawned
void ABKChunkBase::BeginPlay()
{
	Super::BeginPlay();

	Noise->SetFrequency(Frequency);
	Noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	Noise->SetFractalType(FastNoiseLite::FractalType_FBm);

	Setup();

	GenerateHeightMap();

	GenerateMesh();

	UE_LOG(LogTemp, Warning, TEXT("Vertex Cont : %d"), VertexCount);

	ApplyMesh();

}

void ABKChunkBase::GenerateHeightMap()
{
	switch (GenerationType)
	{
	case EGenerationType::GT_3D:
		Generate3DHeightMap(GetActorLocation() / 100);
		break;
	case EGenerationType::GT_2D:
		Generate2DHeightMap(GetActorLocation() / 100);
		break;
	default:
		throw std::invalid_argument("Invalid Generation Type");
	}
}

void ABKChunkBase::ApplyMesh() const
{
	Mesh->SetMaterial(0, Material);
	Mesh->CreateMeshSection(0, MeshData.Vertices, MeshData.Triangles, MeshData.Normals, MeshData.UV0, MeshData.Colors, TArray<FProcMeshTangent>(), true);
}

void ABKChunkBase::ClearMesh()
{
	VertexCount = 0;
	MeshData.Clear();
}

bool ABKChunkBase::ModifyVoxel(const FIntVector Position, const BKEBlock Block)
{
	// ���� ���� �ȵǾ ��� ��ġ�� �����ϰ� �ϴ� �Ʒ� �ڵ�� ����
	bool checkRemovingSuccess = ModifyVoxelData(Position, Block);

	if (checkRemovingSuccess)
	{
		ClearMesh();

		GenerateMesh();

		ApplyMesh();

		return true;
	}
	else
	{
		return false;
	}
	return false;
}

bool ABKChunkBase::SendModifiedVoxel(const FVector World_Position, const FVector World_Normal, const FIntVector Position, const BKEBlock Block)
{
	// ���ǿ� ���� �Ϻ� ���ڴ� �Ⱦ��δ�. �Լ��� ���ĳ��� �̷� ������ �Ǿ��ִ�.

	USGameInstance* instance = USGameInstance::GetMyInstance(this);
	if (instance && instance->Socket != nullptr) {
		int BytesSent = 0;
		if (Block != BKEBlock::Air) // ��� �߰����� �ε����� ��� Ÿ�Ը� ���δ�.
		{
			int8 index = GetMyChunkIndex();
			CS_ADD_BLOCK_PACKET new_block;
			new_block.chunk_index = index;
			new_block.size = sizeof(new_block);
			new_block.type = CS_ADD_BLOCK;

			new_block.ix = Position.X;
			new_block.iy = Position.Y;
			new_block.iz = Position.Z;
			BKEBlock block = Block;
			new_block.blocktype = static_cast<int8>(block);

			instance->Socket->Send((uint8*)&new_block, sizeof(new_block), BytesSent);
			//UE_LOG(LogTemp, Warning, TEXT("Send Block"));

			//ModifyVoxel(Position, Block);
		}

		else { // ��� ���ſ��� ���� ��ǥ�� ���� ���, ��� Ÿ�Ը� ���δ�. �ε����� ��Ŷ���� �޾ƿ� ���� ���� Ŭ�󿡼� ����ϵ��� �� ��Ʈ��ũ ���ϸ� ���δ�.
			int8 index = GetMyChunkIndex();
			CS_REMOVE_BLOCK_PACKET remove_block;

			remove_block.chunk_index = index;
			remove_block.size = sizeof(remove_block);
			remove_block.type = CS_REMOVE_BLOCK;

			remove_block.ix = Position.X;
			remove_block.iy = Position.Y;
			remove_block.iz = Position.Z;

			remove_block.wx = World_Position.X;
			remove_block.wy = World_Position.Y;
			remove_block.wz = World_Position.Z;

			remove_block.nx = World_Normal.X;
			remove_block.ny = World_Normal.Y;
			remove_block.nz = World_Normal.Z;

			BKEBlock block = Block;
			remove_block.blocktype = static_cast<int8>(block);

			instance->Socket->Send((uint8*)&remove_block, sizeof(remove_block), BytesSent);
			//UE_LOG(LogTemp, Warning, TEXT("Send Block"));

			//ModifyVoxel(Position, Block);
		}
	}
	//if(instance->Socket == nullptr) {
	//	ModifyVoxel(Position, Block);
	//}
	bool removingSuccess = ModifyVoxel(Position, Block);
	return removingSuccess;
}

void ABKChunkBase::SetOwningChunkWorld(ABKChunkWorld* NewOwner)
{
	// Chunk�� �����ϴ� Chunk World�� ����
	OwningChunkWorld = NewOwner;
}

void ABKChunkBase::ProcessBlock(const BlockInfo& Block)
{
	bool removingSuccess = ModifyVoxelData(Block.index, Block.type);

	ClearMesh();

	GenerateMesh();

	ApplyMesh();

	if (Block.type == BKEBlock::Air && removingSuccess) {
		//���⿡ ��� �ı� ����Ʈ �Լ� �߰�
		CreateBlockDestroyEffect(Block);
	}
}

void ABKChunkBase::CreateBlockDestroyEffect(const BlockInfo& Block) // ����� ����� �ڸ��� ���� ���� �� �ı� ����Ʈ ����->��Ʈ�ѷ� �ڵ� ������
{
	// �̹� �ڱ� �ڽ� ûũ�� ���� �Լ��̴�. ûũ�� ���� ������ ����. this�� �Ѱܾ� �ϳ�?-> �ڱ� �ڽſ� ���� ûũ ���带 �����´�.
	ABKChunkWorld* OwningWorld = ABKChunkWorld::FindOwningChunkWorld(this);
	if (OwningWorld)
	{
		//UE_LOG(LogTemp, Log, TEXT("World Hit!"));

		// Block�� ���� ��ġ
		FVector BlockWorldPosition = Block.world_index - Block.normal;

		//FIntVector LocalBlockPosition = UBKVoxelFunctionLibrary::WorldToLocalBlockPosition(Block.world_index, Block.normal, 64);
		//// ������: Chunk Index �ҷ���
		//int32 ChunkIndex = this->GetMyChunkIndex();
		//UE_LOG(LogTemp, Log, TEXT("chunk index: %d"), ChunkIndex);

		// Modify Voxel
		//ModifyVoxel(LocalBlockPosition, Block.type);

		// BP_CollapsibleBlock ����
		FIntVector SpawnLocationInt = UBKVoxelFunctionLibrary::GetBlockWorldPostion(BlockWorldPosition, Block.normal, 64);
		FVector SpawnLocationV = FVector(static_cast<float>(SpawnLocationInt.X), static_cast<float>(SpawnLocationInt.Y), static_cast<float>(SpawnLocationInt.Z));
		FRotator SpawnRotation = FRotator::ZeroRotator; // �⺻ ȸ���� ���

		UClass* CollapsibleBlockClass = LoadClass<AActor>(nullptr, TEXT("/Game/Blockers/Blueprints/BP_CollapsibleBlock.BP_CollapsibleBlock_C"));
		if (CollapsibleBlockClass)
		{
			FActorSpawnParameters SpawnParams;
			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(CollapsibleBlockClass, SpawnLocationV, SpawnRotation);

			if (SpawnedActor)
			{
				// CollapsibleBlockComponent �߰�
				UClass* CollapsibleBlockComponentClass = UCollapsibleBlockComponent::StaticClass();
				UActorComponent* CollapsibleComponent = SpawnedActor->AddComponentByClass(CollapsibleBlockComponentClass, false, FTransform::Identity, false);

				if (CollapsibleComponent)
				{
					UE_LOG(LogTemp, Log, TEXT("CollapsibleBlockComponent Added"));

					UCollapsibleBlockComponent* CollapsibleBlockComponent = SpawnedActor->FindComponentByClass<UCollapsibleBlockComponent>();

					if (CollapsibleBlockComponent)
					{
						CollapsibleBlockComponent->DestroyActorWithDelay(3.0f);

						UClass* BombClass = LoadClass<AActor>(nullptr, TEXT("/Game/Blockers/Blueprints/BP_Bomb.BP_Bomb_C"));
						if (BombClass)
						{
							AActor* SpawnedBomb = GetWorld()->SpawnActor<AActor>(BombClass, Block.world_index, SpawnRotation);
						}
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("UCollapsibleBlockComponent is NOT found"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("CollapsibleBlockComponent Failed"));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("BP_CollapsibleBlock Class Can't Be Loaded!"));
		}
	}
}

int8 ABKChunkBase::GetMyChunkIndex() const
{
	// �θ� ���͸� ABKChunkWorld�� ĳ����
	ABKChunkWorld* ChunkWorld = Cast<ABKChunkWorld>(GetOwner());
	if (ChunkWorld)
	{
		// ChunkWorld�� Chunks �迭�� ��ȸ�ϸ� ���� ûũ�� �ε����� ã��
		for (int8 Index = 0; Index < ChunkWorld->Chunks.Num(); ++Index)
		{
			if (ChunkWorld->Chunks[Index] == this)
			{
				return Index; // ûũ�� ã���� �ش� �ε����� ��ȯ
			}
		}
		
	}

	return INDEX_NONE; // ûũ�� ã�� ���� ��� INDEX_NONE ��ȯ
}
