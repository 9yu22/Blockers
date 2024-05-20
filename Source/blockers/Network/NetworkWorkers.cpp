// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkWorkers.h"
#include "BKChunkBase.h"
#include "GameFramework/CharacterMovementComponent.h"


int8 recv_th_count = 0;

FRecvWorker::FRecvWorker(FSocket* c_Socket, AblockersCharacter* Character) : socket(c_Socket), Character(Character)
{
   
}

FRecvWorker::~FRecvWorker()
{

}

void FRecvWorker::RecvPacket()
{

}

void FRecvWorker::MergePacket(uint8* buffer, uint16 recvPacketSize) // 패킷 조립
{
    uint8* ptr = buffer;
    static size_t real_packet_size = 0;
    static size_t saved_packet_size = 0;
    static uint8 packet_buffer[256];

    while (0 != recvPacketSize) {
        if (0 == real_packet_size) real_packet_size = ptr[0];
        if (recvPacketSize + saved_packet_size >= real_packet_size) { // 패킷 처리가 가능한 만큼 버퍼에 패킷이 있다면
            memcpy(packet_buffer + saved_packet_size, ptr, real_packet_size - saved_packet_size); // ptr[0] 만큼만(실제 처리할 패킷 크기만큼) 넘겨 처리
            ProcessPacket(packet_buffer);
            ptr += real_packet_size - saved_packet_size;
            recvPacketSize -= real_packet_size - saved_packet_size;
            real_packet_size = 0;
            saved_packet_size = 0;
        }
        else {
            memcpy(packet_buffer + saved_packet_size, ptr, recvPacketSize); // 버퍼에 데이터가 실제 패킷 크기보다 부족하다면 버퍼에 저장
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

        // AsyncTask를 사용하여 메인 스레드에서 캐릭터의 위치 업데이트
        AsyncTask(ENamedThreads::GameThread, [this, NewLocation, NewRotation, new_pos]()
            {
                //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Move Packet id: %d, x: %f, y: %f, z: %f"), new_pos.id, new_pos.x, new_pos.y, new_pos.z));
                if (IsValid(Character))
                {
                    for (auto& p : Character->Players) {
                        if (p->id == new_pos.id && Character->id != new_pos.id) {
                            p->SetActorLocation(NewLocation);
                            p->SetActorRotation(NewRotation);
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
                            UCharacterMovementComponent* MovementComponent = p->GetCharacterMovement();
                            MovementComponent->GravityScale = 0.0f;  // 중력을 0으로 설정

                            break;
                        }
                    }

                }
                else
                    recvRunning = false;
            });
        break;
    }
    case SC_ADD_BLOCK:
        //SC_ADD_BLOCK_PACKET new_block;
        //memcpy(&new_block, packet, sizeof(new_block));
        //
        //AsyncTask(ENamedThreads::GameThread, [this, new_block]()
        //    {
        //        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Move Packet")));
        //        
        //        else
        //            recvRunning = false;
        //    });
        //break;
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%dRecv_Thread Recv Add_block Packet id: %d, x: %f, y: %f, z: %f"), th_num, , new_player.x, new_player.y, new_player.z));
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
        // 버퍼에 읽어올 데이터가 있는지 확인
        uint32 bHasPendingData = 0;
        socket->HasPendingData(bHasPendingData);

        if (bHasPendingData > 0) {
            int32 BytesRead = 0;
            socket->Recv(recv_buf, sizeof(recv_buf), BytesRead);
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

FSendWorker::FSendWorker(FSocket* c_Socket, AblockersCharacter* Character) : socket(c_Socket), Character(Character)
{
    
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
    CS_MOVE_PACKET new_pos;
    FVector lastLocation;



    while (!Character->loginOk) {
        // 로그인 패킷 받기 전까지 대기
    }

    int32 BytesSent = 0;
       
    double LastSendTime = FPlatformTime::Seconds();

    while (sendRunning)
    {
        if (IsValid(Character))
        {
            CurrentLocation = Character->GetActorLocation();
            CurrentRotation = Character->GetActorRotation();

            double CurrentTime = FPlatformTime::Seconds();

            if (CurrentTime - LastSendTime >= 0.03)
            {
                // 위치 및 회전 정보 패킷 구성
                new_pos.type = CS_MOVE;
                new_pos.size = sizeof(CS_MOVE_PACKET);
                new_pos.x = CurrentLocation.X;
                new_pos.y = CurrentLocation.Y;
                new_pos.z = CurrentLocation.Z;
                new_pos.pitch = CurrentRotation.Pitch;
                new_pos.yaw = CurrentRotation.Yaw;
                new_pos.roll = CurrentRotation.Roll;

                BytesSent = 0;
                socket->Send((uint8*)&new_pos, sizeof(new_pos), BytesSent);

                LastSendTime = CurrentTime;

                // 디버그 메시지 출력
                //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Send My Move Packet, x: %f, y: %f, z: %f"), new_pos.x, new_pos.y, new_pos.z));
            }
        }
        else
        {
            sendRunning = false;
        }

        //FPlatformProcess::Sleep(0.01);
    }

    return 0;
}

void FSendWorker::Stop()
{
    sendRunning = false;
}

