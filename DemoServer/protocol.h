#pragma once

constexpr int PORT_NUM = 4040;
constexpr int BUF_SIZE = 4096; 
constexpr int NAME_SIZE = 20;

constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;

constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_ADD_PLAYER = 3;
constexpr char SC_REMOVE_PLAYER = 4;
constexpr char SC_MOVE_PLAYER = 5;
constexpr char SC_ADD_BLOCK = 6;
constexpr char CS_ADD_BLOCK = 7;
constexpr char ANIM = 8;
constexpr char SC_REMOVE_BLOCK = 9;
constexpr char CS_REMOVE_BLOCK = 10;

constexpr char SC_CHANGE_PLAYER_HP = 11;
constexpr char CS_CHANGE_PLAYER_HP = 12;

constexpr char SC_ADD_PORTAL = 13;
constexpr char SC_CHANGE_PORTAL_HP = 14;
constexpr char CS_CHANGE_PORTAL_HP = 15;

constexpr char SC_ADD_ITEM = 16;
constexpr char SC_REMOVE_ITEM = 17;
constexpr char CS_REMOVE_ITEM = 18;

constexpr char SC_RESPAWN = 19;

constexpr char SC_DESTROY_PORTAL = 20;

constexpr char CS_DISCONNECT = 50;
//constexpr char SC_DISCONNECT = 51;

constexpr char CS_STRESS_TEST = 100;
constexpr char SC_STRESS_TEST = 101;

enum OP_TYPE { OP_ACCEPT, OP_RECV, OP_SEND };

enum class Anim : char { IDLE, MOVE, JUMP };

enum class BKEBlock : char
{
    Null, Air, Stone, Dirt, Grass, Amethyst, Stair
};

typedef unsigned char u_char;

#pragma pack (push, 1)
struct CS_STRESS_TEST_PACKET{
    u_char size;
    char type;
    int id;
    float x, y, z;
    float pitch, yaw, roll;
    unsigned move_time;
};

struct SC_STRESS_TEST_PACKET {
    u_char size;
    char type;
    int id;
    float x, y, z;
    float pitch, yaw, roll;
    unsigned move_time;
};

struct CS_LOGIN_PACKET {
    u_char size;
    char type;
    //char name[NAME_SIZE];
};

struct CS_MOVE_PACKET {
    u_char size;
    char type;
    float x, y, z;
    float pitch, yaw, roll;
};

struct SC_LOGIN_INFO_PACKET {
    u_char size;
    char type;
    int id;
    float x, y, z;
};

struct SC_ADD_PLAYER_PACKET {
    u_char size;
    char type;
    int id;
    float x, y, z;
};

struct SC_ADD_PORTAL_PACKET {
    u_char size;
    char type;
    int id;
    float hp;
    float x, y, z;
};

struct SC_REMOVE_PLAYER_PACKET {
    u_char size;
    char type;
    int id;
};

struct SC_MOVE_PLAYER_PACKET {
    u_char size;
    char type;
    int id;
    float x, y, z;
    float pitch, yaw, roll;
};

struct SC_ADD_BLOCK_PACKET {
    u_char size;
    char type;
    char hp;
    int chunk_index;
    int ix, iy, iz;
    char blocktype;
};

struct CS_ADD_BLOCK_PACKET {
    u_char size;
    char type;
    char hp;
    int chunk_index;
    int ix, iy, iz;
    char blocktype;
};

struct SC_REMOVE_BLOCK_PACKET {
    u_char size;
    char type;
    int chunk_index;
    int ix, iy, iz;
    float wx, wy, wz; 
    float nx, ny, nz; 
    int block_hp;
    char blocktype;
    char collapsibleType;
};

struct CS_REMOVE_BLOCK_PACKET {
    u_char size;
    char type;
    int chunk_index;
    int ix, iy, iz;
    float wx, wy, wz;
    float nx, ny, nz; 
    int block_hp;
    char blocktype;
    char collapsibleType;
};

struct ANIM_PACKET {
    u_char size;
    char type;
    int id;
    char anim_type;
};

struct CS_CHANGE_PLAYER_HP_PACKET {
    u_char size;
    char type;
    //int8 id;
    int hit_id;
};

struct SC_CHANGE_PLAYER_HP_PACKET {
    u_char size;
    char type;
    int id;
    float hp;
};

struct CS_CHANGE_PORTAL_HP_PACKET {
    u_char size;
    char type;
    //int8 id;
    int hit_id;
};

struct SC_CHANGE_PORTAL_HP_PACKET {
    u_char size;
    char type;
    int id;
    float hp;
};

struct CS_DISCONNECT_PACKET {
    u_char size;
    char type;
    int id;
};

/*struct SC_DISCONNECT_PACKET {
    u_char size;
    char type;
    int id;
}*/;

struct SC_ADD_ITEM_PACKET {
    u_char size;
    char type;
    int id;
    float x, y, z;
    char item_type;
};

struct SC_REMOVE_ITEM_PACKET {
    u_char size;
    char type;
    int id;
};

struct CS_REMOVE_ITEM_PACKET {
    u_char size;
    char type;
    int id;
};

struct SC_RESPAWN_PACKET {
    u_char size;
    char type;
    int id;
    float hp;
    float x, y, z;
};

struct SC_DESTROY_PORTAL_PACKET {
    u_char size;
    char type;
    int id;
};
#pragma pack (pop)