// Fill out your copyright notice in the Description page of Project Settings.



#include "NetworkWorkers.h"

FRecvWorker::FRecvWorker(FSocket* c_Socket, AblockersCharacter* Character) : socket(c_Socket), Character(Character)
{
    //recvThread = FRunnableThread::Create(this, TEXT("recvThread"));
}

FRecvWorker::~FRecvWorker()
{

}

void FRecvWorker::RecvPacket()
{

}

bool FRecvWorker::Init()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Thread Init")));
    return true;
}

uint32 FRecvWorker::Run()
{
    //LocationPacket locationPacket;
    uint8 recv_buf[256];

    //TestPacket t;
    //int32 tempread = 0;
    //socket->Recv((uint8*)&t, sizeof(TestPacket), tempread);

    while (recvRunning) {
        // ���ۿ� �о�� �����Ͱ� �ִ��� Ȯ��
        uint32 bHasPendingData = 0;
        socket->HasPendingData(bHasPendingData);

        if (bHasPendingData > 0) {

            int32 BytesRead = 0;
            socket->Recv(recv_buf, sizeof(recv_buf), BytesRead);
            switch (recv_buf[0] + 1){

            case SC_LOGIN_INFO: {
                SC_LOGIN_INFO_PACKET info;
                memcpy(&info, recv_buf, sizeof(SC_LOGIN_INFO_PACKET));
                FVector NewLocation(info.x, info.y, info.z);
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Login Info id: %f, x: %f, y: %f, z: %f"), info.id, info.x, info.y, info.z));
                break;
            }
               
            case SC_MOVE_PLAYER:{
                CS_MOVE_PACKET new_pos;
                memcpy(&new_pos, recv_buf, sizeof(CS_MOVE_PACKET));
                FVector NewLocation(new_pos.x, new_pos.y, new_pos.z);
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv New Pos x: %f, y: %f, z: %f"), new_pos.x, new_pos.y, new_pos.z));

                // AsyncTask�� ����Ͽ� ���� �����忡�� ĳ������ ��ġ ������Ʈ
                AsyncTask(ENamedThreads::GameThread, [this, NewLocation]()
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Packet")));
                        if (IsValid(Character))
                        {
                            Character->SetActorLocation(NewLocation);
                        }
                        else
                            recvRunning = false;
                    });
                break;
            }
            case TEST: {
                TestPacket test;
                memcpy(&test, recv_buf, sizeof(TestPacket));

                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Test Packet Recv: %d"), test.number));

            }
               
            }
        }

        //// ���μ����� �ſ� ������ �ݺ����� �ʵ��� �ణ�� ������ �߰�
        //FPlatformProcess::Sleep(0.01);
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
    CS_LOGIN_PACKET login;
    login.size = sizeof(CS_LOGIN_PACKET);
    login.type = CS_LOGIN;

    int32 BytesSent = 0;
    socket->Send((uint8*)&login, sizeof(CS_MOVE_PACKET), BytesSent);
    if (BytesSent > 0)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Login Packet Send")));
    else
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Login Packet Send Fail...")));
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
    //UPacketHeader Header;
    CS_MOVE_PACKET new_pos;
    FVector lastLocation;

    while (sendRunning)
    {
        // ĳ���� ��ġ ��������
        CurrentLocation = Character->GetActorLocation();

        if (!CurrentLocation.Equals(lastLocation, KINDA_SMALL_NUMBER))
        {
            if (Character)
            {
                // ��ġ ���� ��Ŷ ����
                new_pos.type = CS_MOVE;
                new_pos.size = sizeof(CS_MOVE_PACKET);
                new_pos.x = CurrentLocation.X;
                new_pos.y = CurrentLocation.Y;
                new_pos.z = CurrentLocation.Z;

                // ��Ŷ ����
                int32 BytesSent = 0;
                socket->Send((uint8*)&new_pos, sizeof(CS_MOVE_PACKET), BytesSent);
                lastLocation = CurrentLocation;
            }

            FPlatformProcess::Sleep(0.01);
        }
    }

    return 0;
}

void FSendWorker::Stop()
{
    sendRunning = false;
}

