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

	// ȸ�� ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	FRotator RotationRate;

	// ��Ʈ ������Ʈ�� �� SceneComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	USceneComponent* SceneComponent;

	// Mesh ������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	UStaticMeshComponent* ItemMesh;

	// ȸ�� ���ǵ带 ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pickup)
	float Speed;

	// �浹üũ�� �� Box Collider �� Overlap �Լ�
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

};
