#include <iostream>
#include "Room.h"

Room::Room()
{
    for (auto& client : clients)
    {
        client = nullptr;
    }
   
}

Room::~Room()
{
}

void Room::SetPlayerInfo(int index_num)
{
    switch (index_num) {
    case 0:
        clients[index_num]->m_player.SetWorldLocation(7000.f, 7000.f, 1100.f);
        clients[index_num]->m_player.SetRespawnLocation(7000.f, 7000.f, 1100.f);
        clients[index_num]->m_player.portal.SetWorldLocation(6500.f, 6500.f, 300.f);
        break;
    case 1:
        clients[index_num]->m_player.SetWorldLocation(-7000.f, 7000.f, 1100.f);
        clients[index_num]->m_player.SetRespawnLocation(-7000.f, 7000.f, 1100.f);
        clients[index_num]->m_player.portal.SetWorldLocation(-6500.f, 6500.f, 300.f);
        break;
    case 2:
        clients[index_num]->m_player.SetWorldLocation(7000.f, -7000.f, 1100.f);
        clients[index_num]->m_player.SetRespawnLocation(7000.f, -7000.f, 1100.f);
        clients[index_num]->m_player.portal.SetWorldLocation(6500.f, -6500.f, 300.f);
        break;
    case 3:
        clients[index_num]->m_player.SetWorldLocation(-7000.f, -7000.f, 1100.f);
        clients[index_num]->m_player.SetRespawnLocation(-7000.f, -7000.f, 1100.f);
        clients[index_num]->m_player.portal.SetWorldLocation(-6500.f, -6500.f, 300.f);
        break;
    }
            
            // 테스트용 시작 좌표 
            //switch (client_id % 4) {
            //case 0:
            //	clients[client_id].m_player.SetWorldLocation(700.f, 700.f, 1000.f);
            //	clients[client_id].m_player.SetRespawnLocation(700.f, 700.f, 1000.f);
            //	clients[client_id].m_player.portal.SetWorldLocation(950.f, 950.f, 700.f);
            //	break;
            //case 1:
            //	clients[client_id].m_player.SetWorldLocation(-700.f, -700.f, 1000.f);
            //	clients[client_id].m_player.SetRespawnLocation(-700.f, -700.f, 1000.f);
            //	clients[client_id].m_player.portal.SetWorldLocation(-950.f, -950.f, 700.f);
            //	break;
            //case 2:
            //	clients[client_id].m_player.SetWorldLocation(700.f, -700.f, 1000.f);
            //	clients[client_id].m_player.SetRespawnLocation(700.f, -700.f, 1000.f);
            //	clients[client_id].m_player.portal.SetWorldLocation(950.f, -950.f, 700.f);
            //	break;
            //case 3:
            //	clients[client_id].m_player.SetWorldLocation(-700.f, 700.f, 1000.f);
            //	clients[client_id].m_player.SetRespawnLocation(-700.f, 700.f, 1000.f);
            //	clients[client_id].m_player.portal.SetWorldLocation(-950.f, 950.f, 700.f);
            //	break;
            //}
}

void Room::StartGame()
{
	item_manager.Init();
	room_state = PLAY;
}

void Room::ProcessTimerEvent(time_t duration_time)
{
    elapsed_time += duration_time;
    item_manager.UpdateTime(duration_time);
}

void Room::ProcessPacket(int c_id, char* packet)
{
    static CS_ADD_BLOCK_PACKET prev_add;
    static CS_REMOVE_BLOCK_PACKET prev_remove;

    if (!is_timer) {
        item_manager.Init();
        is_timer = true;
    }

    // 임시
    //UpdateTimer();
    //Item_type type = item_manager.CheckCanSpawn();

    //if (type != NONE) {
    //    for (int i = 0; i < 4; i++) {
    //        Item item(++set_item_id, type, i);

    //        for (auto& pl : clients) {
    //            if (false == pl->b_use) continue;
    //            pl->send_add_item_packet(item);
    //        }
    //    }
    //}

    switch (packet[1]) {
    case CS_LOGIN: {
        std::cout << "클라이언트 " << c_id << " 로그인 패킷 도착" << std::endl;
        clients[c_id]->send_login_player_packet();

        clients[c_id]->send_add_portal_packet(clients[c_id]->m_player.portal);

        for (auto& pl : clients) {
            if (false == pl->b_use || pl->m_player.m_id == c_id) continue;
            pl->send_add_player_packet(clients[c_id]->m_player);
            pl->send_add_portal_packet(clients[c_id]->m_player.portal);
        }
        for (auto& pl : clients) {
            if (false == pl->b_use || pl->m_player.m_id == c_id) continue;
            clients[c_id]->send_add_player_packet(pl->m_player);
            clients[c_id]->send_add_portal_packet(pl->m_player.portal);
        }
        break;
    }
    case CS_MOVE: {
        CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);
        clients[c_id]->m_player.SetWorldLocation(p->x, p->y, p->z);
        clients[c_id]->m_player.SetWorldRotation(p->pitch, p->yaw, p->roll);

        if (clients[c_id]->m_player.location.z < -100.0f) {
            for (auto& pl : clients) {
                if (pl->b_use)
                    pl->send_respawn_packet(clients[c_id]->m_player);
            }
        }
        else {
            for (auto& pl : clients) {
                if (pl->b_use && pl->m_player.m_id != c_id)
                    pl->send_move_player_packet(clients[c_id]->m_player);
            }
        }
        break;
    }
    case CS_ADD_BLOCK: {
        CS_ADD_BLOCK_PACKET* p = reinterpret_cast<CS_ADD_BLOCK_PACKET*>(packet);

        if (memcmp(&prev_add, p, sizeof(CS_ADD_BLOCK_PACKET)) == 0) {
            break;
        }

        std::memcpy(&prev_add, p, sizeof(CS_ADD_BLOCK_PACKET));
        for (auto& pl : clients) {
            if (pl->b_use)
                pl->send_add_block_packet(packet);
        }

        break;
    }
    case CS_REMOVE_BLOCK: {
        CS_REMOVE_BLOCK_PACKET* p = reinterpret_cast<CS_REMOVE_BLOCK_PACKET*>(packet);

        if (memcmp(&prev_remove, p, sizeof(CS_REMOVE_BLOCK_PACKET)) == 0)
            break;

        std::memcpy(&prev_add, p, sizeof(CS_REMOVE_BLOCK_PACKET));
        for (auto& pl : clients) {
            if (pl->b_use)
                pl->send_remove_block_packet(packet);
        }

        break;
    }
    case CS_CHANGE_PLAYER_HP: {
        CS_CHANGE_PLAYER_HP_PACKET* p = reinterpret_cast<CS_CHANGE_PLAYER_HP_PACKET*>(packet);

        if (clients[p->hit_id]->m_player.m_hp > 0.f) {
            clients[p->hit_id]->m_player.m_hp -= 20.f;

            if (clients[p->hit_id]->m_player.m_hp > 0.f) {
                clients[p->hit_id]->send_player_hp_packet(clients[p->hit_id]->m_player);
            }
            else {
                if (clients[p->hit_id]->m_player.portal.m_hp > 0.f) {
                    for (auto& pl : clients) {
                        if (pl->b_use)
                            pl->send_respawn_packet(clients[p->hit_id]->m_player);
                    }
                    clients[p->hit_id]->m_player.m_hp = 100.f;
                }
                else {
                    for (auto& pl : clients) {
                        if (pl->b_use) {
                            clients[p->hit_id]->m_player.m_hp = -10.f;
                            clients[p->hit_id]->send_player_hp_packet(clients[p->hit_id]->m_player);
                        }
                    }
                }
            }
        }
        break;
    }
    case CS_CHANGE_PORTAL_HP: {
        CS_CHANGE_PORTAL_HP_PACKET* p = reinterpret_cast<CS_CHANGE_PORTAL_HP_PACKET*>(packet);
        if (clients[p->hit_id]->m_player.portal.m_hp > 0.f) {
            clients[p->hit_id]->m_player.portal.m_hp -= 300.f;

            if (clients[p->hit_id]->m_player.portal.m_hp > 0.f) {
                clients[p->hit_id]->send_portal_hp_packet(clients[p->hit_id]->m_player.portal);
            }
            else {
                for (auto& pl : clients) {
                    if (pl->b_use)
                        pl->send_destroy_portal_packet(clients[p->hit_id]->m_player.portal);
                }
            }
        }
        break;
    }
    case ANIM: {
        ANIM_PACKET* anim = reinterpret_cast<ANIM_PACKET*>(packet);
        for (auto& pl : clients) {
            if (pl->b_use && c_id == anim->id)
                pl->send_anim_packet(anim);
        }
        break;
    }
    case CS_DISCONNECT: {
        CS_DISCONNECT_PACKET* p = reinterpret_cast<CS_DISCONNECT_PACKET*>(packet);
        DisconnectClient(p->id);
        break;
    }
    case CS_STRESS_TEST: {
        CS_STRESS_TEST_PACKET* p = reinterpret_cast<CS_STRESS_TEST_PACKET*>(packet);
        clients[c_id]->m_player.last_move_time = p->move_time;
        for (auto& pl : clients)
            if (pl->b_use)
                pl->send_stress_test_packet(clients[c_id]->m_player);
        break;
    }
    default:
        std::cout << "잘못된 패킷 수신, PacketType: " << packet[1] << std::endl;
        break;
    }
}


void Room::EndGame()
{
    for (auto& client : clients)
    {
        client = nullptr;
    }
}

void Room::DisconnectClient(int c_id) // 급한대로 그대로 가져옴
{
    for (auto& pl : clients) {
        {
            std::lock_guard<std::mutex> mx(clients[c_id]->session_mx);
            if (true != pl->b_use) continue;
        }
        pl->send_remove_player_packet(clients[c_id]->m_player);
    }
    closesocket(clients[c_id]->m_socket);
    std::cout << "클라이언트 " << c_id << " 연결 끊김" << std::endl;

    {
        std::lock_guard<std::mutex> mx(clients[c_id]->session_mx);
        clients[c_id]->b_use = false;
    }
}
