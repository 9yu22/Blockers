// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkWorker.h"
#include "Sockets.h"
#include "Serialization/ArrayWriter.h"
#include "PacketSession.h"

RecvWorker::RecvWorker(FSocket* Socket, TSharedPtr<class PacketSession> Session) : Socket(Socket), SessionRef(Session)
{
	Thread = FRunnableThread::Create(this, TEXT("RecvWorkerThread"));
}

RecvWorker::~RecvWorker()
{
}

bool RecvWorker::Init()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Recv Thread Init")));
	return true;
}

uint32 RecvWorker::Run() // ��Ŷ ����
{
	while(Running) {
		TArray<uint8> Packet;

		// �t�� ��Ŷ�� ť�� �־���� ���� �����忡�� �������� ������.
		if (ReceivePacket(OUT Packet)) {

			// WeakPtr�� �ٷ� ��� �Ұ�->SharedPtr�� ��ȯ �ʿ�(lock->�𸮾��� pin)
			if (TSharedPtr<PacketSession> Session = SessionRef.Pin()) {
				Session->RecvPacketQueue.Enqueue(Packet);
			}
			
		}
	}

	return 0;
}

void RecvWorker::Exit()
{
}

// Run ����
void RecvWorker::Destroy()
{
	Running = false;
}

bool RecvWorker::ReceivePacket(TArray<uint8>& OutPacket)
{
	// �ϴ� ��� ũ�⸸ŭ ���� �� �ִ��� Ȯ��
	const int32 HeaderSize = sizeof(FPacketHeader);
	TArray<uint8> HeaderBuffer;
	HeaderBuffer.AddZeroed(HeaderSize);

	if (ReceiveDesiredBytes(HeaderBuffer.GetData(), HeaderSize) == false)
		return false;

	FPacketHeader Header;
	{
		FMemoryReader Reader(HeaderBuffer);
		Reader << Header;
		UE_LOG(LogTemp, Log, TEXT("Recv PacketID: %d, PacketSize: %d"), Header.PacketID, Header.PacketSize)
	}

	// ��Ŷ ��� ����
	OutPacket = HeaderBuffer;

	TArray<uint8> PayloadBuffer;
	// ��Ŷ ũ�� = ��ü ũ�� - ��� ũ��
	const int32 PayloadSize = Header.PacketSize - HeaderSize;
	// ��� �ڿ� �����͸�ŭ �޸� �߰� �Ҵ�
	OutPacket.AddZeroed(PayloadSize);

	if (ReceiveDesiredBytes(&OutPacket[HeaderSize], PayloadSize))
		return true;

	return false;
}

// ���ϴ� ũ���� �����Ͱ� ���� �� ���� ���
bool RecvWorker::ReceiveDesiredBytes(uint8* Results, int32 Size) 
{
	uint32 PendingDAtaSIze;
	// ���� ���� ���� �� ��Ŷ ���ú� ũ�Ⱑ 0���� �´�.
	if (Socket->HasPendingData({ PendingDAtaSIze }) == false || PendingDAtaSIze <= 0) {
		return false;
	}
	int32 Offset = 0;

	while (Size > 0) {
		int32 NumRead = 0;
		Socket->Recv(Results + Offset, Size, OUT NumRead);
		check(NumRead <= Size);

		if (NumRead <= 0)
			return false;
		Size -= NumRead;
	}
	return false;
}
