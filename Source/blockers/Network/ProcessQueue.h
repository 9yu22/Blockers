#pragma once

#include <mutex>
#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "../Private/Voxel/BKEnum.h"
#include "Protocol.h"
//#include "../Private/BKNaiveChunk.h"

//struct BlockInfo {
//	FIntVector index;
//	BKEBlock type;
//};
//
//class BLOCKERS_API ProcessQueueBlock
//{
//	TQueue<BlockInfo> BlockQ;
//	std::mutex queue_mutex; // ����ִ� ť�� �ִٰ� ������ �� enqueue�� ������ ���� �ʾƵ� empty�� ����ٰ� ��ȯ�ϴ����� ���� ������ ��� ��ü ���ؽ� ���(dequeue�� empty�� �ƴϸ� ����)
//public:
//	ProcessQueueBlock();
//	~ProcessQueueBlock();
//
//	bool CheckEmpty();
//	void EnQ(BlockInfo& block);
//	BlockInfo DeQ();
//};

struct BlockInfo {
    int chunk_index;
    FIntVector index;
    FVector world_index; // ��� ���ſ��� ���
    FVector normal; // ��� ���ſ��� ���
    BKEBlock type;
};
// ��� ť�� �߰��� ���ŷ� �и��ϴ°��� ��ȿ�����̶� �����ؼ� �ϳ��� �־���� Ÿ���� �Ǻ��ؼ� ����

struct AnimInfo {
    int8 id;
    Anim type;
};

template <typename T>
class ProcessQueue
{
private:
    TQueue<T> BlockQ;
    std::mutex queue_mutex; // ����ִ� ť�� �ִٰ� ������ �� enqueue�� ������ ���� �ʾƵ� empty�� ����ٰ� ��ȯ�ϴ����� ���� ������ ��� ��ü ���ؽ� ���(dequeue�� empty�� �ƴϸ� ����)

public:
    ProcessQueue();
    ~ProcessQueue();

    bool CheckEmpty();
    void EnQ(const T& block);
    T DeQ();
};

template <typename T>
ProcessQueue<T>::ProcessQueue()
{
}

template <typename T>
ProcessQueue<T>::~ProcessQueue()
{
}

template <typename T>
bool ProcessQueue<T>::CheckEmpty()
{
    queue_mutex.lock();
    bool ret = BlockQ.IsEmpty();
    queue_mutex.unlock();
    return ret;
}

template <typename T>
void ProcessQueue<T>::EnQ(const T& block)
{
    queue_mutex.lock();
    BlockQ.Enqueue(block);
    queue_mutex.unlock();
}

template <typename T>
T ProcessQueue<T>::DeQ()
{
    queue_mutex.lock();
    T block;
    BlockQ.Dequeue(block);
    queue_mutex.unlock();
    return block;
}