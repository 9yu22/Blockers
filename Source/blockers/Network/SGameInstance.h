// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ProcessQueue.h"
#include "NetworkWorkers.h"
//#include "S1.h"
//#include "PacketSession.h"
#include "../blockersCharacter.h"
#include "../blockersGameMode.h"
#include "Protocol.h"
#include "SGameInstance.generated.h"

/**
 *
 */

UCLASS()
class BLOCKERS_API USGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USGameInstance();
	//virtual ~USGameInstance();

	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	//UFUNCTION(BlueprintCallable)
	//void HandleRecvPackets();

	//void SendPacket(SendBufferRef SendBuffer);

	static USGameInstance* GetMyInstance(UObject* WorldContextObject); // static 함수는 객체 존재 없이 호출가능

	bool SetIpAddress();

protected:
	virtual void Shutdown() override;
	
public:
	FSocket* Socket;
	//FString IpAddress = TEXT("127.0.0.1");
	FString IpAddress;
	int16 Port = 4040;

	//ProcessQueueBlock Blocks;
	TArray<AblockersCharacter*> Players;
	AblockersCharacter* MyCharacter;
	TQueue<BlockInfo> BlockQueue;
	TQueue<AnimInfo> AnimQueue;

	FRunnableThread* RecvThread;
	FRunnableThread* SendThread;

	AblockersGameMode* GameMode = nullptr;

	//TSharedPtr<class PacketSession> GameServerSession;
	//TSharedPtr<class FRecvWorker> makeRecvThread;
	//TSharedPtr<class FSendWorker> makeSendThread;
};
