// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Network/SGameInstance.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Controller.h"
#include "blockersCharacter.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftClickAction;

protected:
	// ������ �����ϴ� BeginPlay (��Ʈ�ѷ��� ù ��° ������Ʈ�� �ϱ� ���� ȣ��)
	virtual void BeginPlay() override;
	// �Է��� �غ��ϴ� �Լ�
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaTime) override;

	// �����̶�� ����� ����
	UFUNCTION()
	virtual void InputMove(const FInputActionValue& value);

	UFUNCTION()
	void Jump(const FInputActionValue& Value);

	UFUNCTION()
	void StopJumping(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void OnLeftClick(const FInputActionValue& Value);

	UPROPERTY()
	UInputMappingContext* DefaultMappingContext;

	uint8 Anim_type = 0;

	//// ��Ʈ���ϴ� ĳ���͸� ��ȯ
	AblockersCharacter* GetMyCharacter() const;

	//// �Է� Ű�� �����ϴ� �Լ�
	void SendAnimation(int8 anim_type);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult HitResult;
};
