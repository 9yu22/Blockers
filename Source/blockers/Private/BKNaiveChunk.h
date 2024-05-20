// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BKChunkBase.h"
#include "Voxel/BKEnum.h"
#include "../Network/ProcessQueue.h"
#include "BKNaiveChunk.generated.h"

class FastNoiseLite;
class UProceduralMeshComponent;


// �̷������� ���ϴϱ� ť�� �ɹ� ������ ����� ������ getter ���� ��� ������ �ʿ��� �����ͷ� �����ϵ��� �غ���
/**
 *
 */
UCLASS()
class ABKNaiveChunk final : public ABKChunkBase
{
	GENERATED_BODY()

	protected:
	virtual void Setup() override;
	virtual void Generate2DHeightMap(FVector Position) override;
	virtual void Generate3DHeightMap(FVector Position) override;
	virtual void GenerateMesh() override;
	virtual void ModifyVoxelData(const FIntVector Position, BKEBlock Block) override;

private:
	struct Block {
		BKEBlock block;
		BKEDirection backDir;
	};

	TArray<Block> Blocks;
	TArray<FIntVector> splitBlocks;	// �ɰ����� ��� ��ü�� ��ǥ�� �����ϴ� �迭

	int splitBlockNum = 10;			// splitBlockNum x splitBlockNum x splitBlockNum�� �ɰ���
	const FVector BlockVertexData[8] = {
		FVector(100,100,100),
		FVector(100,0,100),
		FVector(100,0,0),
		FVector(100,100,0),
		FVector(0,0,100),
		FVector(0,100,100),
		FVector(0,100,0),
		FVector(0,0,0)
	};
	const int BlockTriangleData[24] = {
		0,1,2,3, // Forward
		5,0,3,6, // Right
		4,5,6,7, // Back
		1,4,7,2, // Left
		5,4,1,0, // Up
		3,2,7,6  // Down
	};

	bool Check(FVector Position) const;
	void CreateFace(const Block Block, BKEDirection Direction, FVector Position);
	TArray<FVector> GetFaceVertices(BKEDirection Direction, FVector Position) const;
	TArray<FVector> GetStairFaceVertices(BKEDirection Direction, const FVector Position, const FVector Ratio, const FVector relationalPosition) const;
	FVector GetPositionInDirection(BKEDirection Direction, FVector Position) const;
	FVector GetNormal(BKEDirection Direction) const;
	int GetBlockIndex(int X, int Y, int Z) const;

	int GetTextureIndex(BKEBlock Block, FVector Normal) const;
};

