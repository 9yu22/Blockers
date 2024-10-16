// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BKVoxelFunctionLibrary.generated.h"

/**
 *
 */
UCLASS()
class UBKVoxelFunctionLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static const int32 ChunkSize = 150;

public:
	UFUNCTION(BlueprintPure, Category = "Voxel")
	static FIntVector WorldToBlockPosition(const FVector& Position);

	UFUNCTION(BlueprintPure, Category = "Voxel")
	static FIntVector WorldToLocalBlockPosition(const FVector& Position, const FVector& Normal, const int Size);

	UFUNCTION(BlueprintPure, Category = "Voxel")
	static FIntVector WorldToChunkPosition(const FVector& Position, const int Size);

	UFUNCTION(BlueprintPure, Category = "Voxel")
	static FIntVector GetBlockWorldPostion(const FVector& Position, const FVector& Normal, const int Size);
};
