// Fill out your copyright notice in the Description page of Project Settings.


#include "BKPlayerController.h"
#include "BKChunkBase.h"			// for HitResult
#include "BKChunkWorld.h"			// for server
#include "BKVoxelFunctionLibrary.h"	// for localBlockPosition

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "HealthBarWidget.h"

#include "UObject/Class.h"
#include "Kismet/KismetMathLibrary.h"	// for getFowardVector(), getRightVector()


void ABKPlayerController::BeginPlay()
{
	// ������ BeginPlay
	// �θ�
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(MappingContext, 0);
	}
}

void ABKPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABKPlayerController::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABKPlayerController::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABKPlayerController::InputMove);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABKPlayerController::Look);
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Triggered, this, &ABKPlayerController::OnLeftClick);
	}

}

void ABKPlayerController::InputMove(const FInputActionValue& value)
{
	FVector2D inputAxis = value.Get<FVector2D>();

	if (APawn* controlledPawn = GetPawn())
	{
		FVector ForwardVector = UKismetMathLibrary::GetForwardVector(GetControlRotation());
		FVector RightVector = UKismetMathLibrary::GetRightVector(GetControlRotation());

		controlledPawn->AddMovementInput(ForwardVector, inputAxis.Y);
		controlledPawn->AddMovementInput(RightVector, inputAxis.X);
	}
}


void ABKPlayerController::Jump(const FInputActionValue& Value)
{
	GetCharacter()->bPressedJump = true;
}

void ABKPlayerController::StopJumping(const FInputActionValue& Value)
{
	GetCharacter()->bPressedJump = false;
}

void ABKPlayerController::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	GetCharacter()->AddControllerYawInput(LookAxisVector.X);
	GetCharacter()->AddControllerPitchInput(LookAxisVector.Y);
}

void ABKPlayerController::OnLeftClick(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Left Click!"));

	if (PlayerCameraManager)
	{
		FVector CameraLocation = PlayerCameraManager->GetCameraLocation();

		FRotator CameraRotation = PlayerCameraManager->GetCameraRotation();

		FVector startLocation = CameraLocation;

		FVector endLocation = CameraLocation + CameraRotation.Vector() * 1000;	// 1000�� �Ÿ�

		// LineTraceByChannel�� ���̸� ���� �浹�� ����
		bHit = GetWorld()->LineTraceSingleByChannel(HitResult, startLocation, endLocation, ECollisionChannel::ECC_Visibility);

		if (bHit)
		{
			// ������: line Tracing�� ���� ���� �׸���
			DrawDebugLine(GetWorld(), startLocation, HitResult.Location, FColor::Red, false, 1.0f, 0, 1.0f);

			// ������: line�� �ε��� ��ġ�� �ʷϻ� �� �׸���
			DrawDebugSphere(GetWorld(), HitResult.Location, 5.0f, 12, FColor::Green, false, 1.0f);

			UE_LOG(LogTemp, Log, TEXT("Hit!"));

			// Hit�� ���Ͱ� Chunk�ΰ�?
			ABKChunkBase* HitChunk = Cast<ABKChunkBase>(HitResult.GetActor());

			if (HitChunk)
			{
				// Hit Chunk�� �����ϴ� Chunk World�� �ҷ��´�. (World�� Hit Chunk�� ���� index�� ã�� ����)
				ABKChunkWorld* OwningWorld = ABKChunkWorld::FindOwningChunkWorld(HitChunk);
				if (OwningWorld)
				{
					UE_LOG(LogTemp, Log, TEXT("World Hit!"));

					// Block�� ���� ��ġ
					FVector BlockWorldPosition = HitResult.Location - HitResult.Normal;
					// Block �迭 �� ��ġ
					FIntVector LocalBlockPosition = UBKVoxelFunctionLibrary::WorldToLocalBlockPosition(BlockWorldPosition, HitResult.Normal, 64);

					// ������: Chunk Index �ҷ���
					int32 ChunkIndex = OwningWorld->GetChunkIndex(HitChunk, LocalBlockPosition, BKEBlock::Air);
					UE_LOG(LogTemp, Log, TEXT("chunk index: %d"), ChunkIndex);
				}
			}
		}
		else
		{
			// ������ �ƹ� �͵� ������ �ʾ��� ��, ��ü ������ ���� ������ �׸��ϴ�.
			DrawDebugLine(GetWorld(), startLocation, endLocation, FColor::Red, false, 1.0f, 0, 1.0f);
		}
	}
}
