// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Voxel/BKEnum.h"
#include "GameFramework/Actor.h"
#include "BKChunkWorld.generated.h"

class ABKChunkBase;

UCLASS()
class ABKChunkWorld final : public AActor
{
	GENERATED_BODY()

	public:
	UPROPERTY(EditInstanceOnly, Category = "World")
	TSubclassOf<ABKChunkBase> ChunkType;

	UPROPERTY(EditInstanceOnly, Category = "World")
	int DrawDistance = 5;

	UPROPERTY(EditInstanceOnly, Category = "Chunk")
	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY(EditInstanceOnly, Category = "Chunk")
	int Size = 150;

	UPROPERTY(EditInstanceOnly, Category = "Height Map")
	EGenerationType GenerationType;

	UPROPERTY(EditInstanceOnly, Category = "Height Map")
	float Frequency = 0.03f;

	UFUNCTION(BlueprintCallable, Category = "Chunk")
	AActor* GetAdjacentActor(const AActor* ChunkActor, const FVector Normal);

	// Sets default values for this actor's properties
	ABKChunkWorld();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	int ChunkCount;

	void Generate3DWorld();
	void Generate2DWorld();

public:
	TArray<AActor*> Chunks;		// ûũ ���� �迭

	// Function to find the ChunkWorld owning a specific ChunkBase
	static ABKChunkWorld* FindOwningChunkWorld(ABKChunkBase* Chunk);
	static ABKChunkWorld* GetChunkWorld(UWorld* World);
	
};
