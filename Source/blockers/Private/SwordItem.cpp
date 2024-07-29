// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordItem.h"
#include "../blockersCharacter.h"
#include "BKNaiveChunk.h"
#include "../Network/SGameInstance.h"


void ASwordItem::UseItem()
{
	APlayerCameraManager* PlayerCameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	if (PlayerCameraManager)
	{
		FVector CameraLocation = PlayerCameraManager->GetCameraLocation();

		FRotator CameraRotation = PlayerCameraManager->GetCameraRotation();

		FVector startLocation = CameraLocation;

		FVector endLocation = CameraLocation + CameraRotation.Vector() * 200;	// �Ÿ�: 200


		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (PlayerPawn)
		{
			// FCollisionQueryParams�� ����Ͽ� Ʈ���̽� �Ķ���� ����
			FCollisionQueryParams TraceParams(FName(TEXT("SwordTrace")), true, PlayerPawn);
			TraceParams.bTraceComplex = true;
			TraceParams.bReturnPhysicalMaterial = false;

			// LineTraceByChannel�� ���̸� ���� �浹�� ����
			bHit = GetWorld()->LineTraceSingleByChannel(HitResult, startLocation, endLocation, ECollisionChannel::ECC_GameTraceChannel6, TraceParams);

			if (bHit)
			{
				// ������: line Tracing�� ���� ���� �׸���
				DrawDebugLine(GetWorld(), startLocation, HitResult.Location, FColor::Red, false, 1.0f, 0, 1.0f);

				// ������: line�� �ε��� ��ġ�� �ʷϻ� �� �׸���
				DrawDebugSphere(GetWorld(), HitResult.Location, 5.0f, 12, FColor::Green, false, 1.0f);

				AblockersCharacter* TargetCharacter = Cast<AblockersCharacter>(HitResult.GetActor());

				if (TargetCharacter)
				{
					// ������ ����
					Durability -= Usage;

					USGameInstance* GameInstance = USGameInstance::GetMyInstance(this);
					if (GameInstance)
					{
						for (AblockersCharacter* Player : GameInstance->Players)
						{
							if (Player == TargetCharacter)
							{
								//Player->health -= 10;

								if (GameInstance && GameInstance->Socket != nullptr) {
									CS_CHANGE_PLAYER_HP_PACKET p;
									p.size = sizeof(p);
									p.type = CS_CHANGE_PLAYER_HP;
									p.hit_id = Player->id;

									int BytesSent = 0;
									GameInstance->Socket->Send((uint8*)&p, sizeof(p), BytesSent);
								}

								break;
							}
						}
					}
				}

				ABKNaiveChunk* TargetChunk = Cast<ABKNaiveChunk>(HitResult.GetActor());

				if (TargetChunk)
				{
					// ������ ����
					Durability -= Usage;
				}
			}
			else
			{
				// ������: line Tracing�� ���� ���� �׸���
				DrawDebugLine(GetWorld(), startLocation, endLocation, FColor::Red, false, 1.0f, 0, 1.0f);
			}
		}
	}
}

float ASwordItem::GetBlockDamage()
{
	if (IsCastToDiamond(this))
		return 100.f;
	return 20.0f;
}

float ASwordItem::GetPlayerDamage()
{
	return 0.0f;
}

bool ASwordItem::IsCastToDiamond(ASwordItem* MyActor) const
{
	const FString& BlueprintPath = "Blueprint'Blockers/Blueprints/Item/BP_PickaxItem_Diamond.BP_PickaxItem_Diamond_C'";

	if (MyActor == nullptr || BlueprintPath.IsEmpty())
	{
		return false;
	}

	// �������Ʈ Ŭ������ �ε�
	UBlueprint* LoadedBlueprint = Cast<UBlueprint>(StaticLoadObject(UBlueprint::StaticClass(), nullptr, *BlueprintPath));
	if (LoadedBlueprint == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("�������Ʈ�� �ε��� �� �����ϴ�: %s"), *BlueprintPath);
		return false;
	}

	UClass* BlueprintClass = LoadedBlueprint->GeneratedClass;
	if (BlueprintClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("�������Ʈ Ŭ������ ��ȿ���� �ʽ��ϴ�: %s"), *BlueprintPath);
		return false;
	}

	// MyActor�� �ε�� �������Ʈ Ŭ������ �ν��Ͻ����� Ȯ��
	return MyActor->IsA(BlueprintClass);
}
