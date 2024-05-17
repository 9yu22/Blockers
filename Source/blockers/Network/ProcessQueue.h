#pragma once

#include <mutex>
#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "../Private/BKNaiveChunk.h"

struct BlockInfo {
	FIntVector index;
	BKEBlock type;
};

class BLOCKERS_API ProcessQueueBlock
{
	TQueue<BlockInfo> BlockQ;
	std::mutex queue_mutex; // ����ִ� ť�� �ִٰ� ������ �� enqueue�� ������ ���� �ʾƵ� empty�� ����ٰ� ��ȯ�ϴ����� ���� ������ ��� ��ü ���ؽ� ���(dequeue�� empty�� �ƴϸ� ����)
public:
	ProcessQueueBlock();
	~ProcessQueueBlock();

	bool CheckEmpty();
	void InQ(BlockInfo& block);
	BlockInfo DeQ();
};
