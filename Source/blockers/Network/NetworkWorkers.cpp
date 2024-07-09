// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkWorkers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Private/BKChunkBase.h"
#include "Kismet/GameplayStatics.h"
#include "Protocol.h"
#include "ProcessQueue.h"

int8 recv_th_count = 0;

FRecvWorker::FRecvWorker(USGameInstance* Instance, AblockersCharacter* Character) : Instance(Instance), Character(Character)
{
    c_Socket = Instance->Socket;
}

FRecvWorker::~FRecvWorker()
{

}

void FRecvWorker::RecvPacket()
{

}

void FRecvWorker::MergePacket(uint8* buffer, uint16 recvPacketSize) // ��Ŷ ����
{
    uint8* ptr = buffer;
    static size_t real_packet_size = 0;
    static size_t saved_packet_size = 0;
    static uint8 packet_buffer[256];

    while (0 != recvPacketSize) {
        if (0 == real_packet_size) real_packet_size = ptr[0];
        if (recvPacketSize + saved_packet_size >= real_packet_size) { // ��Ŷ ó���� ������ ��ŭ ���ۿ� ��Ŷ�� �ִٸ�
            memcpy(packet_buffer + saved_packet_size, ptr, real_packet_size - saved_packet_size); // ptr[0] ��ŭ��(���� ó���� ��Ŷ ũ�⸸ŭ) �Ѱ� ó��
            ProcessPacket(packet_buffer);
            ptr += real_packet_size - saved_packet_size;
            recvPacketSize -= real_packet_size - saved_packet_size;
            real_packet_size = 0;
            saved_packet_size = 0;
        }
        else {
            memcpy(packet_buffer + saved_packet_size, ptr, recvPacketSize); // ���ۿ� �����Ͱ� ���� ��Ŷ ũ�⺸�� �����ϴٸ� ���ۿ� ����
            saved_packet_size += recvPacketSize;
            recvPacketSize = 0;
        }
    }
}

void FRecvWorker::ProcessPacket(uint8* packet)
{
    switch (packet[1]) {

    case SC_LOGIN_INFO: {
        SC_LOGIN_INFO_PACKET info;

        memcpy(&info, packet, sizeof(SC_LOGIN_INFO_PACKET));
        FVector NewLocation(info.x, info.y, info.z);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%dRecv_Thread Recv Login Packet id: %d, x: %f, y: %f, z: %f"), th_num, info.id, info.x, info.y, info.z));

        AsyncTask(ENamedThreads::GameThread, [this, NewLocation, info]()
            {
                //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Login Packet")));
                if (IsValid(Character)) {
                    for (auto& p : Character->Players) {
                        if (p->id < 0) {
                            p->id = info.id;
                            Character->id = info.id;
                            p->SetActorLocation(NewLocation);
                            p->IsSelf = true;
                            
                            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Set Login Info id: %d, x: %f, y: %f, z: %f"), info.id, info.x, info.y, info.z));
                            break;
                        }
                    }
                }
                else
                    recvRunning = false;
            });
        Character->loginOk = true;

        break;
    }

    case SC_MOVE_PLAYER: {
        SC_MOVE_PLAYER_PACKET new_pos;

        memcpy(&new_pos, packet, sizeof(new_pos));
        FVector NewLocation(new_pos.x, new_pos.y, new_pos.z);
        FRotator NewRotation(new_pos.pitch, new_pos.yaw, new_pos.roll);
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Move Packet id: %d, x: %f, y: %f, z: %f"), new_pos.id, new_pos.x, new_pos.y, new_pos.z));

        // AsyncTask�� ����Ͽ� ���� �����忡�� ĳ������ ��ġ ������Ʈ
        AsyncTask(ENamedThreads::GameThread, [this, NewLocation, NewRotation, new_pos]()
            {
                //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Move Packet id: %d, x: %f, y: %f, z: %f"), new_pos.id, new_pos.x, new_pos.y, new_pos.z));
                if (IsValid(Character))
                {
                    for (auto& p : Character->Players) {
                        if (p->id == new_pos.id && Character->id != new_pos.id) {
                            //p->SetActorLocation(NewLocation);
                            p->PacketLocation = NewLocation;
                            p->PacketRotation = NewRotation;
                            break;
                        }
                    }
                }
                else
                    recvRunning = false;
            });
        break;

    }
    case SC_ADD_PLAYER: {
        SC_ADD_PLAYER_PACKET new_player;

        memcpy(&new_player, packet, sizeof(new_player));
        FVector NewLocation(new_player.x, new_player.y, new_player.z);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%dRecv_Thread Recv Add Packet id: %d, x: %f, y: %f, z: %f"), th_num, new_player.id, new_player.x, new_player.y, new_player.z));

        AsyncTask(ENamedThreads::GameThread, [this, NewLocation, new_player]()
            {
                //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Move Packet")));
                if (IsValid(Character)) {
                    for (auto& p : Character->Players) {
                        if (p->id < 0) {
                            p->id = new_player.id;
                            p->SetActorLocation(NewLocation);
                            p->PacketLocation = NewLocation;
                            UCharacterMovementComponent* MovementComponent = p->GetCharacterMovement();
                            MovementComponent->GravityScale = 0.0f;  // �߷��� 0���� ����

                            break;
                        }
                    }

                }
                else
                    recvRunning = false;
            });
        break;
    }

    case SC_ADD_BLOCK: {
        SC_ADD_BLOCK_PACKET new_block;

        memcpy(&new_block, packet, sizeof(new_block));
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Add Block Packet chunk index:%d, x: %d, y: %d, z: %d"),new_block.chunk_index, new_block.ix, new_block.iy, new_block.iz));

        BlockInfo block;
        block.chunk_index = new_block.chunk_index;
        block.index = { new_block.ix, new_block.iy, new_block.iz };
        block.type = static_cast<BKEBlock>(new_block.blocktype);

        Instance->BlockQueue.EnQ(block);
        
        break;
    }

    case SC_REMOVE_BLOCK: {
        SC_REMOVE_BLOCK_PACKET remove_block;

        memcpy(&remove_block, packet, sizeof(remove_block));
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Add Block Packet x: %d, y: %d, z: %d"), new_block.ix, new_block.iy, new_block.iz));

        BlockInfo block;
        block.chunk_index = remove_block.chunk_index;
        block.index = { remove_block.ix, remove_block.iy, remove_block.iz };
        block.world_index = { remove_block.wx, remove_block.wy, remove_block.wz };
        block.normal = { remove_block.nx, remove_block.ny, remove_block.nz };
        block.type = static_cast<BKEBlock>(remove_block.blocktype);

        Instance->BlockQueue.EnQ(block);

        break;
    }

    case ANIM: {
        ANIM_PACKET new_anim;

        memcpy(&new_anim, packet, sizeof(new_anim));
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Add Block Packet x: %d, y: %d, z: %d"), new_block.ix, new_block.iy, new_block.iz));

        AnimInfo anim;
        anim.id = new_anim.id;
        anim.type = static_cast<Anim>(new_anim.anim_type);

        Instance->AnimQueue.EnQ(anim);
        UE_LOG(LogTemp, Warning, TEXT("Anim ENQ"));
        break;
    }
    case SC_CHANGE_HP:
        /*SC_CHANGE_HP_PACKET new_hp;
        
        memcpy(&new_hp, packet, sizeof(new_hp));
        AsyncTask(ENamedThreads::GameThread, [this, new_hp]()
            {
                if (IsValid(Character)) {
                    for (auto& p : Character->Players) {
                        if (p->id == new_hp.id) {
                            p->health = new_hp.hp;

                            break;
                        }
                    }

                }
            });*/
        break;
    default:
        break;
    }
}

bool FRecvWorker::Init()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Thread Init")));
    return true;
}

uint32 FRecvWorker::Run()
{
    uint8 recv_buf[256];

    th_num = ++recv_th_count;

    while (recvRunning) {
        // ���ۿ� �о�� �����Ͱ� �ִ��� Ȯ��
        uint32 bHasPendingData = 0;
        c_Socket->HasPendingData(bHasPendingData);

        if (bHasPendingData > 0) {
            int32 BytesRead = 0;
            c_Socket->Recv(recv_buf, sizeof(recv_buf), BytesRead);
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Pavket Recv")));
            MergePacket(recv_buf, BytesRead);     
        }

        FPlatformProcess::Sleep(0.01);
    }
    return 0;
}

void FRecvWorker::Stop()
{
    recvRunning = false;
}

// SendWorker

FSendWorker::FSendWorker(USGameInstance* Instance, AblockersCharacter* Character) : Instance(Instance), Character(Character)
{
    c_Socket = Instance->Socket;
}

FSendWorker::~FSendWorker()
{
}

bool FSendWorker::Init()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Send Thread Init")));
    return true;
}

uint32 FSendWorker::Run()
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("SendWorker Running Start")));
    //UPacketHeader Header;

    while (!Character->loginOk) {
        // �α��� ��Ŷ �ޱ� ������ ���
    }

    int32 BytesSent = 0;
       
    double LastSendTime = FPlatformTime::Seconds();

    //while (sendRunning)
    //{
    //    if (IsValid(Character))
    //    {
    //        CurrentLocation = Character->GetActorLocation();
    //        CurrentRotation = Character->GetActorRotation();

    //        double CurrentTime = FPlatformTime::Seconds();

    //        if (CurrentTime - LastSendTime >= 0.1)
    //        {
    //            CS_MOVE_PACKET new_pos;
    //            // ��ġ �� ȸ�� ���� ��Ŷ ����
    //            new_pos.type = CS_MOVE;
    //            new_pos.size = sizeof(CS_MOVE_PACKET);
    //            new_pos.x = CurrentLocation.X;
    //            new_pos.y = CurrentLocation.Y;
    //            new_pos.z = CurrentLocation.Z;
    //            new_pos.pitch = CurrentRotation.Pitch;
    //            new_pos.yaw = CurrentRotation.Yaw;
    //            new_pos.roll = CurrentRotation.Roll;

    //            BytesSent = 0;
    //            c_Socket->Send((uint8*)&new_pos, sizeof(new_pos), BytesSent);

    //            LastSendTime = CurrentTime;

    //            // ����� �޽��� ���
    //            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Send My Move Packet, x: %f, y: %f, z: %f"), new_pos.x, new_pos.y, new_pos.z));
    //        }
    //    }
    //    else
    //    {
    //        sendRunning = false;
    //    }

    //    //FPlatformProcess::Sleep(0.01);
    //}

    return 0;
}

void FSendWorker::Stop()
{
    sendRunning = false;
}

