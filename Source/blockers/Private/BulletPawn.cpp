// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletPawn.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABulletPawn::ABulletPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//�ڽ� �ݶ��̴� ������Ʈ�� �����Ѵ�
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("My Box Component"));

	//������ �ڽ� �ݶ��̴� ������Ʈ�� �ֻ�� ������Ʈ�� �����Ѵ�.
	SetRootComponent(boxComp);

	//����ƽ �޽� ������Ʈ�� �����Ѵ�.
	meshComp = CreateDefaultSubobject <UStaticMeshComponent>(TEXT("My Static Mesh"));

	//�ڽ� �ݶ��̴� ������Ʈ�� �ڽ� ������Ʈ�� �����Ѵ�.
	meshComp->SetupAttachment(boxComp);

	//�ڽ� �ݶ��̴��� ũ�⸦ 50x50x50���� �����Ѵ�.
	FVector boxSize = FVector(50.0f, 50.0f, 50.0f);
	boxComp->SetBoxExtent(boxSize);
}

// Called when the game starts or when spawned
void ABulletPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABulletPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABulletPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

