// Fill out your copyright notice in the Description page of Project Settings.


#include "BKPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"

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