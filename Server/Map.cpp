#include "z_debug.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Map.h"

#include <map>
#include "yaml-cpp/yaml.h"

std::map<UINT, Map *> Maps;
std::map<UINT, std::vector<UINT>> RandomMaps;

void LoadMapConfig()//��ͼ����
{
    try
    {
        YAML::Node Config = YAML::LoadFile("Map\\Map.yml");
        for (size_t i = 0; i < Config.size(); i++)
        {
            YAML::Node map = Config[i];
            Map *pMap = new Map;
            Maps[map["ID"].as<UINT>()] = pMap;
            pMap->Head = map["Head"].as<UINT>();
            pMap->End = map["End"].as<UINT>();
            pMap->Loop = map["Loop"].as<bool>();
            pMap->Round = map["Round"].as<UINT>();
            pMap->Name = map["Name"].as<std::string>();
            pMap->MapCompatibilityMode = true;
        }
        Config = YAML::LoadFile("Map\\RandomMaps.yml");
        for (size_t i = 0; i < Config.size(); i++)
        {
            auto v = Config[i];
            UINT RandomMapID = v["ID"].as<UINT>();
            RandomMaps[RandomMapID] = std::vector<UINT>();
            auto &maps = RandomMaps[RandomMapID];
            for (size_t j = 0; j < v["Maps"].size(); j++)
            {
                maps.push_back(v["Maps"][j].as<UINT>());
            }
        }
    }
    catch (const std::exception &)
    {
        Z_ERROR("���ص�ͼ�ļ�����\n");
        exit(0);
    }
}

std::string GetMapName(UINT ID)
{
    if (Maps[ID])
    {
        return Maps[ID]->Name;
    }
    return "δ֪ͼ��" + std::to_string(ID);
}

Map *GetMap(UINT ID)
{
    return Maps[ID];
}

bool IsRandomMap(UINT ID)
{
    return ID <= 20;
}

UINT RoomMapID2GameMapID(UINT MapID)
{
    if (IsRandomMap(MapID))
    {
        
        auto &maps = RandomMaps[MapID];
        if (maps.size()<=0) {
            return 104;
        }
        return maps[rand() % maps.size()];
    }
    else
    {
        return MapID;
    }
}