#include "Map.h"
#include "protocol.h"

Map::Map()
{
    block_map.resize(MAX_ARRAY_SIZE);
    ReadMapData();
}

Map::~Map()
{
}

bool Map::AddBlockToMap(CS_ADD_BLOCK_PACKET* packet)
{
    if (packet->blocktype != static_cast<int>(BKEBlock::Air)) {
        int index = calculateIndex(packet->chunk_index, packet->ix, packet->iy, packet->iz);
        if (block_map[index].blockType == static_cast<int>(BKEBlock::Air)) {
            block_map[index].blockType = packet->blocktype;
            return true;
        }
        return false;
    }
    return false;
}

bool Map::RemoveBlockToMap(CS_REMOVE_BLOCK_PACKET* packet)
{
    if (packet->blocktype == static_cast<int>(BKEBlock::Air)) {
        int index = calculateIndex(packet->chunk_index, packet->ix, packet->iy, packet->iz);
        if (block_map[index].blockType != static_cast<int>(BKEBlock::Air)) {
            block_map[index].blockType = packet->blocktype;
            std::cout << "x: " << packet->ix << " y: " << packet->iy << " z: " << packet->iz << "��ġ�� ��� �߰�" << std::endl;
            return true;
        }
        return false;
    }
    return false;
}

void Map::ReadMapData()
{
    std::ifstream file("Blockers_map.txt");
    if (!file) {
        std::cerr << "������ �� �� �����ϴ�.\n";
        return;
    }

    int chunk_index; // char�� ���ڸ� �о���� �ƽ�Ű �ڵ尡 �����
    int x, y, z;
    int blockType;
    int blockCount = 0;
    int crossIndex = 0;

    while (file >> chunk_index >> x >> y >> z >> blockType) {
        int index = calculateIndex(chunk_index, x, y, z);

        Block temp;
        temp.blockType = blockType;

        if (block_map[index].blockType == static_cast<int>(BKEBlock::Air)) {
            block_map[index] = temp;
            blockCount++;
        }
        else
           // std::cout << "�ߺ� ������ �ε��� " << "chunkindex: " << chunk_index << "x: " << x << "y: " << y << "z: " << z << std::endl;
            crossIndex++;
    }
    //std::cout << "������ �ʱ� �� ��� ��: " << blockCount << std::endl;
    //std::cout << "�ߺ� ������ �ʱ� �� ��� ��: " << crossIndex << std::endl; // �ߺ��� �ణ �����ϴµ� Ŭ�󿡼� �ݺ����� �ߺ� ��ȸ�ϴ°� �ֳ�..? �ƴϸ� ���� �߸� �Ѱǰ�..
    file.close();
}

int Map::calculateIndex(int chunk_index, int x, int y, int z)
{
    int index = (x + (y * SMALL_CHUNK_WIDTH) + (z * SMALL_CHUNK_WIDTH * SMALL_CHUNK_WIDTH));

    if (chunk_index > 0)
        return chunk_index * index;

    else if (chunk_index == 0)
        return BIG_CHUNK_SIZE * index;
}

Block::Block()
{
    blockType = static_cast<int>(BKEBlock::Air);
}
