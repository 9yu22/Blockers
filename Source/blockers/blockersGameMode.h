// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/Controller.h"
#include "blockersCharacter.h"
#include "Network/ProcessQueue.h"
#include "Public/Portal.h"
#include "blockersGameMode.generated.h"

enum Item_type { STONE, AMETHYST, RUBY, DIAMOND, NONE };

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDiedSignature, ACharacter*, Character);

UCLASS(minimalapi)
class AblockersGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AblockersGameMode();

	void SpawnCharacter(SC_ADD_PLAYER_PACKET new_player);
	void SpawnPortal(SC_ADD_PORTAL_PACKET add_portal);
	void SpawnItem(SC_ADD_ITEM_PACKET add_item);

	const FOnPlayerDiedSignature& GetOnPlayerDied() const { return OnPlayerDied; }

protected:
	virtual void BeginPlay()override;

	UFUNCTION()
	virtual void PlayerDied(ACharacter* Character); //Call When Player die
	UPROPERTY()
	FOnPlayerDiedSignature OnPlayerDied; //Signature to bind delegate

};



