// Fill out your copyright notice in the Description page of Project Settings.

#include "Portal.h"
#include "../blockersCharacter.h"
#include "../Network/SGameInstance.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Portalhealth = 100.f;
	
	// Create and initialize Sphere Component
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	RootComponent = SphereComponent;

	// Set Sphere Radius and Collision Profile
	SphereComponent->InitSphereRadius(50.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Trigger"));

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
	

    GetSphereComponent()->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnBeginOverLap);

	if (PortalHpWidgetClass != nullptr)
	{
		PortalHpWidget = CreateWidget(GetWorld(), PortalHpWidgetClass);
		PortalHpWidget->AddToViewport();
	}
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Portalhealth == 0)
	{
		/*	USGameInstance* instance = USGameInstance::GetMyInstance(this);
			if (instance) {
				instance->Portals.Remove(this);
			}*/
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		//UGameplayStatics::OpenLevel(this, FName("End"));
	}

}

void APortal::OnBeginOverLap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromsweep, const FHitResult& SweepResult)
{
 
    //if (OtherActor)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("OtherActor is valid"));
    //    if (OtherActor->ActorHasTag("Test"))
    //    {

    //        UE_LOG(LogTemp, Warning, TEXT("OtherActor has 'Test' tag"));
    //        Portalhealth -= 10.f;
    //        
    //    }
    //    else
    //    {
    //        UE_LOG(LogTemp, Warning, TEXT("OtherActor does not have 'Test' tag"));
    //    }
    //}
    //else
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("OtherActor is null"));
    //}

	AblockersCharacter* enemyCharacter = Cast<AblockersCharacter>(OtherActor);

	//if (enemy != nullptr)
	//{
	//	OtherActor->Destroy();

	//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionFX, GetActorLocation(), GetActorRotation());
	//}
	if (enemyCharacter != nullptr)
	{
		//enemyCharacter->health -= 30.f;
		USGameInstance* instance = USGameInstance::GetMyInstance(this);

		if (enemyCharacter->id != id) {
			if (instance && instance->Socket != nullptr) {
				CS_CHANGE_PORTAL_HP_PACKET p;
				p.size = sizeof(p);
				p.type = CS_CHANGE_PORTAL_HP;
				p.hit_id = id;

				int BytesSent = 0;
				instance->Socket->Send((uint8*)&p, sizeof(p), BytesSent);
			}
		}		
	}
}

