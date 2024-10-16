// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <mutex>
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ProcessQueue.h"
#include "NetworkWorkers.h"
//#include "S1.h"
//#include "PacketSession.h"
#include "../blockersCharacter.h"
#include "../blockersGameMode.h"
#include "../Private/BKChunkWorld.h"
#include "../Public/ResourceItem.h"
#include "../Public/Portal.h"
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
	AblockersCharacter* MyCharacter;
	TArray<AblockersCharacter*> Players;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "portal")
	TArray<APortal*> Portals;

	TArray<AResourceItem*> items;
	//FCriticalSection items_cs;

	TQueue<BlockInfo> BlockQueue;
	TQueue<AnimInfo> AnimQueue;

	FRunnableThread* RecvThread;
	//FRunnableThread* SendThread;

	AblockersGameMode* GameMode;
	ABKChunkWorld* ChunkWorld;

	UFUNCTION(BlueprintPure, Category = "server")
	bool IsServerOnline();	// 블루프린트에서 서버 켜져 있는지 확인하는 함수

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "map")
	int32 selectedMap;

	//TSharedPtr<class PacketSession> GameServerSession;
	//TSharedPtr<class FRecvWorker> makeRecvThread;
	//TSharedPtr<class FSendWorker> makeSendThread;
};
