// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordItem.h"
#include "../blockersCharacter.h"
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

		// LineTraceByChannel�� ���̸� ���� �浹�� ����
		bHit = GetWorld()->LineTraceSingleByChannel(HitResult, startLocation, endLocation, ECollisionChannel::ECC_Visibility);

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
		}
		else
		{
			// ������: line Tracing�� ���� ���� �׸���
			DrawDebugLine(GetWorld(), startLocation, endLocation, FColor::Red, false, 1.0f, 0, 1.0f);
		}
	}
}