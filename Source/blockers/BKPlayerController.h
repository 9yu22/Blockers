// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BKPlayerController.generated.h"

/**
 *
 */
UCLASS()
class BLOCKERS_API ABKPlayerController : public APlayerController
{
	GENERATED_BODY()

	//     �б� ����           ���� ����            �б� ����
	// BlueprintReadOnly / BlueprintWriteOnly / BlueprintReadWrite
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

protected:
	// ������ �����ϴ� BeginPlay (��Ʈ�ѷ��� ù ��° ������Ʈ�� �ϱ� ���� ȣ��)
	virtual void BeginPlay() override;
	// �Է��� �غ��ϴ� �Լ�
	virtual void SetupInputComponent() override;

	// �����̶�� ����� ����
	UFUNCTION()
	virtual void InputMove(const FInputActionValue& value);
	UFUNCTION()
	void Jump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJumping(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
};
