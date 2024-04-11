#pragma once
#include <string>
class Map
{
public:
    UINT Head;
    UINT End;
    UINT Round;
    bool Loop;
    std::string Name;
    bool MapCompatibilityMode;
};

std::string GetMapName(UINT ID);
void LoadMapConfig();
Map *GetMap(UINT ID);
bool IsRandomMap(UINT ID);
UINT RoomMapID2GameMapID(UINT MapID);
