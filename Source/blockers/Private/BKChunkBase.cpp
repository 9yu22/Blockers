// Fill out your copyright notice in the Description page of Project Settings.


#include "BKChunkBase.h"
#include "BKChunkWorld.h"

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

	//SetinstanceTag("BKChunkBase");
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

void ABKChunkBase::ModifyVoxel(const FIntVector Position, const BKEBlock Block)
{
	//if (Position.X >= Size || Position.Y >= Size || Position.Z >= Size || Position.X < 0 || Position.Y < 0 || Position.Z < 0) return Position;
	//UE_LOG(LogTemp, Warning, TEXT("Return My Chunk Index %d"), index);

	USGameInstance* instance = USGameInstance::GetMyInstance(this);
	if (instance)
	{
		int BytesSent = 0;
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
	}

	// ���� ���� �ȵǾ ��� ��ġ�� �����ϰ� �ϴ� �Ʒ� �ڵ�� ����
	ModifyVoxelData(Position, Block);

	ClearMesh();

	GenerateMesh();

	ApplyMesh();
}

void ABKChunkBase::SetOwningChunkWorld(ABKChunkWorld* NewOwner)
{
	// Chunk�� �����ϴ� Chunk World�� ����
	OwningChunkWorld = NewOwner;
}

void ABKChunkBase::ProcessBlockQueue(const FIntVector Position, const BKEBlock Block)
{
	ModifyVoxelData(Position, Block);

	ClearMesh();

	GenerateMesh();

	ApplyMesh();
}

void ABKChunkBase::SetinstanceTag(FName NewTag)
{
	if (!NewTag.IsNone())
	{
		Tags.AddUnique(NewTag);
	}
}

int8 ABKChunkBase::GetMyChunkIndex() const
{
	// �θ� ���͸� ABKChunkWorld�� ĳ����
	ABKChunkWorld* ChunkWorld = Cast<ABKChunkWorld>(GetOwner());
	if (ChunkWorld)
	{
		// ChunkWorld�� Chunks �迭�� ��ȸ�ϸ� ���� ûũ�� �ε����� ã��
		for (int32 Index = 0; Index < ChunkWorld->Chunks.Num(); ++Index)
		{
			if (ChunkWorld->Chunks[Index] == this)
			{
				return Index; // ûũ�� ã���� �ش� �ε����� ��ȯ
			}
		}
		
	}

	return INDEX_NONE; // ûũ�� ã�� ���� ��� INDEX_NONE ��ȯ
}
