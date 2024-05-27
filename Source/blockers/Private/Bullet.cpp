// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PracticeDestroyBlock.h"
#include "../blockersCharacter.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/SpringArmComponent.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	SetRootComponent(boxComp);
	boxComp->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));

	boxComp->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Component"));
	meshComp->SetupAttachment(boxComp);

}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	playerForward = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorForwardVector();

	bulletDirection = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation().Quaternion().GetForwardVector();

	ACharacter* PlayerCharacter = Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerCharacter)
	{
		USpringArmComponent* SpringArm = PlayerCharacter->FindComponentByClass<USpringArmComponent>();
		if (SpringArm)
		{
			// Get the spring arm length
			float SpringArmLength = SpringArm->TargetArmLength;

			startLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation() + bulletDirection * (SpringArmLength+100);
		}
	}

	boxComp->OnComponentBeginOverlap;

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnBulletOverlap);
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	
	startLocation += bulletDirection * moveSpeed * DeltaTime;

	SetActorLocation(startLocation);

	FVector endLocation = startLocation + bulletDirection * 100;

	// LineTraceByChannel�� ���̸� ���� �浹�� ����
	bHit = GetWorld()->LineTraceSingleByChannel(HitResult, startLocation, endLocation, ECollisionChannel::ECC_Visibility);

}

void ABullet::OnBulletOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	//APracticeDestroyBlock* enemy = Cast<APracticeDestroyBlock>(OtherActor);
	AblockersCharacter* enemyCharacter = Cast<AblockersCharacter>(OtherActor);

	//if (enemy != nullptr)
	//{
	//	OtherActor->Destroy();

	//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionFX, GetActorLocation(), GetActorRotation());
	//}
	if (enemyCharacter != nullptr)
	{
		//enemyCharacter->health -= 30.f;
		UE_LOG(LogTemp, Warning, TEXT("enemyCharacter hit, -30 hp!!"));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), explosionFX, GetActorLocation(), GetActorRotation());
		DestroyBullet();
	}

	

}

void ABullet::FunctionWithDelay(float delayTime)
{
	// ���ο� Ÿ�̸� �ڵ��� �����մϴ�.
	FTimerHandle NewTimerHandle;

	// 2�� �Ŀ� ȣ��� �Լ��� �����մϴ�.
	GetWorld()->GetTimerManager().SetTimer(NewTimerHandle, this, &ABullet::DestroyBullet, delayTime, false);
}

void ABullet::DestroyBullet()
{
	Destroy();
}

