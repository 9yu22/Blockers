// Fill out your copyright notice in the Description page of Project Settings.


#include "BKChunkBase.h"
#include "BKChunkWorld.h"
#include "BKVoxelFunctionLibrary.h"	// for localBlockPosition
#include "CollapsibleBlockComponent.h"	// for collapsible Block
#include "Materials/MaterialInstanceDynamic.h"


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

	//bool removingSuccess = ModifyVoxelData(Position, Block);

	//ClearMesh();

	//GenerateMesh();

	//ApplyMesh();

	//return removingSuccess;
	return false;
}

bool ABKChunkBase::SendModifiedVoxel(const FVector World_Position, const FVector World_Normal, const FIntVector Position, const FBlockk Block)
{
	// ��� ���� ��Ŷ�� ������ �������� ���� ����
	static FIntVector prevPosition;
	static BKEBlock prevBlock;
	if (memcmp(&prevPosition, &Position, sizeof(FIntVector) == 0 && memcmp(&prevPosition, &Position, sizeof(FIntVector) == 0))) {
		return false;
	}
	else {
		prevPosition = Position;
		prevBlock = Block.block;
	}

	// ���ǿ� ���� �Ϻ� ���ڴ� �Ⱦ��δ�. �Լ��� ���ĳ��� �̷� ������ �Ǿ��ִ�.
	USGameInstance* instance = USGameInstance::GetMyInstance(this);
	if (instance && instance->Socket != nullptr) {
		int BytesSent = 0;

		int8 index = GetMyChunkIndex();
		CS_ADD_BLOCK_PACKET new_block;
		new_block.chunk_index = index;
		new_block.size = sizeof(new_block);
		new_block.type = CS_ADD_BLOCK;

		new_block.block_hp = 
		new_block.ix = Position.X;
		new_block.iy = Position.Y;
		new_block.iz = Position.Z;
		BKEBlock block = Block.block;
		new_block.blocktype = static_cast<int8>(block);

		instance->Socket->Send((uint8*)&new_block, sizeof(new_block), BytesSent);

		//else { // ��� ���ſ��� ���� ��ǥ�� ���� ���, ��� Ÿ�Ը� ���δ�. �ε����� ��Ŷ���� �޾ƿ� ���� ���� Ŭ�󿡼� ����ϵ��� �� ��Ʈ��ũ ���ϸ� ���δ�.
		//	int8 index = GetMyChunkIndex();
		//	CS_REMOVE_BLOCK_PACKET remove_block;

		//	remove_block.chunk_index = index;
		//	remove_block.size = sizeof(remove_block);
		//	remove_block.type = CS_REMOVE_BLOCK;

		//	remove_block.ix = Position.X;
		//	remove_block.iy = Position.Y;
		//	remove_block.iz = Position.Z;

		//	remove_block.wx = World_Position.X;
		//	remove_block.wy = World_Position.Y;
		//	remove_block.wz = World_Position.Z;

		//	remove_block.nx = World_Normal.X;
		//	remove_block.ny = World_Normal.Y;
		//	remove_block.nz = World_Normal.Z;

		//	BKEBlock block = Block.block;
		//	remove_block.blocktype = static_cast<int8>(block);

		//	instance->Socket->Send((uint8*)&remove_block, sizeof(remove_block), BytesSent);
		//	//UE_LOG(LogTemp, Warning, TEXT("Send Block"));

		//	//ModifyVoxel(Position, Block);
		//}
	}
	if(instance && instance->Socket == nullptr) {
		/*bool removingSuccess = ModifyVoxel(Position, Block.block);
		return removingSuccess;*/
	}
	//ModifyVoxel(Position, Block);
	return false;
}

void ABKChunkBase::SendAddBlockPacket(const FIntVector Position, const FBlockk Block)
{
	USGameInstance* instance = USGameInstance::GetMyInstance(this);
	if (instance && instance->Socket != nullptr) {
		int BytesSent = 0;

		int8 index = GetMyChunkIndex();
		CS_ADD_BLOCK_PACKET new_block;
		new_block.chunk_index = index;
		new_block.size = sizeof(new_block);
		new_block.type = CS_ADD_BLOCK;

		new_block.block_hp = Block.health;
		new_block.ix = Position.X;
		new_block.iy = Position.Y;
		new_block.iz = Position.Z;
		BKEBlock block = Block.block;
		new_block.blocktype = static_cast<int8>(block);

		instance->Socket->Send((uint8*)&new_block, sizeof(new_block), BytesSent);
	}
}

void ABKChunkBase::SendModifiedBlockPacket(const FVector World_Position, const FVector World_Normal, const FIntVector Position, const FBlockk Block)
{
	USGameInstance* instance = USGameInstance::GetMyInstance(this);
	if (instance && instance->Socket != nullptr) {
		int BytesSent = 0;

		if (Block.health > 0) {
			int8 index = GetMyChunkIndex();
			CS_ADD_BLOCK_PACKET new_block;
			new_block.chunk_index = index;
			new_block.size = sizeof(new_block);
			new_block.type = CS_ADD_BLOCK;

			new_block.block_hp = Block.health;
			new_block.ix = Position.X;
			new_block.iy = Position.Y;
			new_block.iz = Position.Z;
			BKEBlock block = Block.block;
			new_block.blocktype = static_cast<int8>(block);

			instance->Socket->Send((uint8*)&new_block, sizeof(new_block), BytesSent);
		}

		else {
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

			BKEBlock block = Block.block;
			remove_block.collapsibleType = Block.CollapsibleType;
			remove_block.blocktype = static_cast<int8>(block);

			instance->Socket->Send((uint8*)&remove_block, sizeof(remove_block), BytesSent);
		}
	}
	
	
}

void ABKChunkBase::SetOwningChunkWorld(ABKChunkWorld* NewOwner)
{
	// Chunk�� �����ϴ� Chunk World�� ����
	OwningChunkWorld = NewOwner;
}

void ABKChunkBase::ProcessBlock(const BlockInfo& Block)
{
	//bool modifySuccess = ModifyVoxelData(Block.index, Block.type);

	//if (modifySuccess) {

	//	ClearMesh();

	//	GenerateMesh();

	//	ApplyMesh();

	//	if (Block.type == BKEBlock::Air) {
	//		CreateBlockDestroyEffect(Block);
	//    }
	//}
}

void ABKChunkBase::AddBlock(const SC_ADD_BLOCK_PACKET& add_block)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Enter AddBlock Method")));
	FIntVector Position{ add_block.ix, add_block.iy, add_block.iz };
	FBlockk Block;
	Block.block = static_cast<BKEBlock>(add_block.blocktype);
	Block.health = add_block.block_hp;

	bool modifySuccess = ModifyVoxelData(Position, Block);

	if (modifySuccess)
	{
		ClearMesh();

		GenerateMesh();

		ApplyMesh();
	}
}

void ABKChunkBase::RemoveBlock(const SC_REMOVE_BLOCK_PACKET& remove_block)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Enter RemoveBlock Method")));
	FIntVector Position{ remove_block.ix, remove_block.iy, remove_block.iz };
	FVector world_index{ remove_block.wx, remove_block.wy, remove_block.wz };
	FVector normal{ remove_block.nx, remove_block.ny, remove_block.nz };
	FBlockk Block;
	Block.block = static_cast<BKEBlock>(remove_block.blocktype);
	//Block.CollapsibleType = remove_block.collapsibleType;

	bool modifySuccess = ModifyVoxelData(Position, Block);

	if (modifySuccess)
	{
		ClearMesh();

		GenerateMesh();

		ApplyMesh();

		CreateBlockDestroyEffect(world_index, normal, remove_block.collapsibleType);
	}
}

void ABKChunkBase::CreateBlockDestroyEffect(const FVector& world_index, const FVector& normal, uint8 collapsibleType) // ����� ����� �ڸ��� ���� ���� �� �ı� ����Ʈ ����->��Ʈ�ѷ� �ڵ� ������
{
	// �̹� �ڱ� �ڽ� ûũ�� ���� �Լ��̴�. ûũ�� ���� ������ ����. this�� �Ѱܾ� �ϳ�?-> �ڱ� �ڽſ� ���� ûũ ���带 �����´�.
	ABKChunkWorld* OwningWorld = ABKChunkWorld::FindOwningChunkWorld(this);
	if (OwningWorld)
	{
		//UE_LOG(LogTemp, Log, TEXT("World Hit!"));

		// Block�� ���� ��ġ
		FVector BlockWorldPosition = world_index - normal;

		//FIntVector LocalBlockPosition = UBKVoxelFunctionLibrary::WorldToLocalBlockPosition(Block.world_index, Block.normal, 64);
		//// ������: Chunk Index �ҷ���
		//int32 ChunkIndex = this->GetMyChunkIndex();
		//UE_LOG(LogTemp, Log, TEXT("chunk index: %d"), ChunkIndex);

		// Modify Voxel
		//ModifyVoxel(LocalBlockPosition, Block.type);

		// BP_CollapsibleBlock ����
		FIntVector SpawnLocationInt = UBKVoxelFunctionLibrary::GetBlockWorldPostion(BlockWorldPosition, normal, 64);
		FVector SpawnLocationV = FVector(static_cast<float>(SpawnLocationInt.X), static_cast<float>(SpawnLocationInt.Y), static_cast<float>(SpawnLocationInt.Z));
		FRotator SpawnRotation = FRotator::ZeroRotator; // �⺻ ȸ���� ���

		UClass* CollapsibleBlockClass;
		switch (collapsibleType) {
		case 0:
			CollapsibleBlockClass = LoadClass<AActor>(nullptr, TEXT("/Game/Blockers/Blueprints/BP_CollapsibleBlock_Stone.BP_CollapsibleBlock_Stone_C"));
			break;

		case 1:
			CollapsibleBlockClass = LoadClass<AActor>(nullptr, TEXT("/Game/Blockers/Blueprints/BP_CollapsibleBlock_Amethyst.BP_CollapsibleBlock_Amethyst_C"));
			break;

		case 2:
			CollapsibleBlockClass = LoadClass<AActor>(nullptr, TEXT("/Game/Blockers/Blueprints/BP_CollapsibleBlock_Ruby.BP_CollapsibleBlock_Ruby_C"));
			break;
			
		case 3:
			CollapsibleBlockClass = LoadClass<AActor>(nullptr, TEXT("/Game/Blockers/Blueprints/BP_CollapsibleBlock_Diamond.BP_CollapsibleBlock_Diamond_C"));
			break;

		default:
			CollapsibleBlockClass = LoadClass<AActor>(nullptr, TEXT("/Game/Blockers/Blueprints/BP_CollapsibleBlock_Stone.BP_CollapsibleBlock_Stone_C"));
			break;
		}
		
		//UClass* CollapsibleBlockClass1 = LoadClass<AActor>(nullptr, TEXT("/Game/Blockers/Blueprints/BP_CollapsibleBlock_Stone.BP_CollapsibleBlock_Stone_C"));
		//UClass* CollapsibleBlockClass2 = LoadClass<AActor>(nullptr, TEXT("/Game/Blockers/Blueprints/BP_CollapsibleBlock_Amethyst.BP_CollapsibleBlock_Amethyst_C"));
		//UClass* CollapsibleBlockClass3 = LoadClass<AActor>(nullptr, TEXT("/Game/Blockers/Blueprints/BP_CollapsibleBlock_Ruby.BP_CollapsibleBlock_Ruby_C"));
		//UClass* CollapsibleBlockClass4 = LoadClass<AActor>(nullptr, TEXT("/Game/Blockers/Blueprints/BP_CollapsibleBlock_Diamond.BP_CollapsibleBlock_Diamond_C"));
		
		
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
							AActor* SpawnedBomb = GetWorld()->SpawnActor<AActor>(BombClass, world_index, SpawnRotation);

							FTimerHandle TimerHandle;

							// Create a timer delegate to call DestroyActor function
							FTimerDelegate TimerDelegate;
							TimerDelegate.BindLambda([SpawnedBomb]()
							{
								SpawnedBomb->Destroy();
							});

							// Set the timer to call DestroyActor function after the specified delay
							GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, false);

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
