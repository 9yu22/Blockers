// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "PickUpItem.generated.h"

UCLASS()
class BLOCKERS_API APickUpItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 회전 정도를 결정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	FRotator RotationRate;

	// 루트 컴포넌트가 될 SceneComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	USceneComponent* SceneComponent;

	// Mesh 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	UStaticMeshComponent* ItemMesh;

	// 회전 스피드를 결정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	float Speed;

	// 충돌체크를 할 Box Collider 와 Overlap 함수
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* BoxCollider;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, const FHitResult& SweepResult);

public: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	UTexture2D* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	int32 amount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	float Durability = 100.f;		// For Pickax, Sword, Pistol / Max: 100.0

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	float Usage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* PickUpSound;

	virtual void OnInteract();

protected:
	virtual void Show(bool visible);

	UFUNCTION(BlueprintCallable, Category = Pickup)
	virtual void UseItem() PURE_VIRTUAL(APickUpItem::UseItem);

	UFUNCTION(BlueprintCallable, Category = Pickup)
	virtual float GetBlockDamage() PURE_VIRTUAL(APickUpItem::GetBlockDamage, return 0.0f;);

	UFUNCTION(BlueprintCallable, Category = Pickup)
	virtual float GetPlayerDamage() PURE_VIRTUAL(APickUpItem::GetPlayerDamage, return 0.0f;);

};
