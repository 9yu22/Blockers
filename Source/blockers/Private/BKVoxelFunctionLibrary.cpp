// Fill out your copyright notice in the Description page of Project Settings.


#include "BKVoxelFunctionLibrary.h"
#include "BKNaiveChunk.h"

FIntVector UBKVoxelFunctionLibrary::WorldToBlockPosition(const FVector& Position)
{
	return FIntVector(Position) / 100;
}

FIntVector UBKVoxelFunctionLibrary::WorldToLocalBlockPosition(const FVector& Position, const FVector& Normal, const int Size)
{
	const auto ChunkPosition = WorldToChunkPosition(Position - Normal * 10, Size);

	auto Result = WorldToBlockPosition(Position - Normal * 10) - ChunkPosition * Size;

	// Negative Normalization
	if (ChunkPosition.X < 0) Result.X--;
	if (ChunkPosition.Y < 0) Result.Y--;
	if (ChunkPosition.Z < 0) Result.Z--;

	/*if (ChunkPosition.X == 0 && Result.X == 0 && Normal.X < 0)
		Result.Y++;*/
	if (ChunkPosition.X < 0 && Result.X == ChunkSize - 1 && Normal.X > 0)
		Result.Y--;
	/*if (ChunkPosition.Y == 0 && Result.Y == 0 && Normal.Y < 0)
		Result.Z++;*/
	if (ChunkPosition.Y < 0 && Result.Y == ChunkSize - 1 && Normal.Y > 0)
		Result.Z--;

	return Result;
}

FIntVector UBKVoxelFunctionLibrary::WorldToChunkPosition(const FVector& Position, const int Size)
{
	FIntVector Result;

	const int Factor = Size * 100;
	const auto IntPosition = FIntVector(Position);

	if (IntPosition.X < 0) Result.X = (int)(Position.X / Factor) - 1;
	else Result.X = (int)(Position.X / Factor);

	if (IntPosition.Y < 0) Result.Y = (int)(Position.Y / Factor) - 1;
	else Result.Y = (int)(Position.Y / Factor);

	if (IntPosition.Z < 0) Result.Z = (int)(Position.Z / Factor) - 1;
	else Result.Z = (int)(Position.Z / Factor);

	return Result;
}

FIntVector UBKVoxelFunctionLibrary::GetBlockWorldPostion(const FVector& Position, const FVector& Normal, const int Size)
{
	const auto ChunkPosition = WorldToChunkPosition(Position, Size);
	const auto LocalBlockPosition = WorldToLocalBlockPosition(Position, Normal, Size);

	if (ChunkPosition.X == 0 && ChunkPosition.Y == 0)
		return FIntVector(LocalBlockPosition) * 100 + FIntVector(50);
	else if (ChunkPosition.X == -1 && ChunkPosition.Y == 0)
		return FIntVector(LocalBlockPosition.X - Size, LocalBlockPosition.Y, LocalBlockPosition.Z) * 100 + FIntVector(50);
	else if (ChunkPosition.X == 0 && ChunkPosition.Y == -1)
		return FIntVector(LocalBlockPosition.X, LocalBlockPosition.Y - Size, LocalBlockPosition.Z) * 100 + FIntVector(50);
	else
		return FIntVector(LocalBlockPosition.X - Size, LocalBlockPosition.Y - Size, LocalBlockPosition.Z) * 100 + FIntVector(50);
}
