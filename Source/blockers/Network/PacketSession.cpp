// Fill out your copyright notice in the Description page of Project Settings.


#include "PacketSession.h"


PacketSession::PacketSession(class FSocket* Socket) : Socket(Socket)
{
}

PacketSession::~PacketSession()
{
	Disconnect();
}

void PacketSession::Run()
{
	// AsShared()�� Weak�����͸� Shared�����ͷ� �ٲ���
	RecvWorkerThread = MakeShared<RecvWorker>(Socket, AsShared());
}

void PacketSession::Recv()
{
}

void PacketSession::Disconnect()
{
}

void PacketSession::HandleRecvPackets()
{
	while (true) {
		TArray<uint8> Packet;
		if (RecvPacketQueue.Dequeue(OUT Packet) == false)
			break;

		// Switch�� �ްԵǸ� ���� �ʹ� ������ ������ �����.
		// TODO
		// ClientPacketHandler::HandlePacket();
	}
}
//
//void PacketSession::SendOnePacket(TCHAR * buf, int16 p_size)
//{
//	TArray<uint8> Packet;
//
//	//memcpy(buf, Packet, p_size);
//
//	//SendPacket(IN Packet);
//}
