#pragma once
#include <iostream>
enum ItemType
{
	EAIT_Unknown,
	EAIT_CAR,
	EAIT_SKIN,
	EAIT_PET,
	EAIT_PROP,
	EAIT_CLOTHES,
	EAIT_BABY
};
struct KartAltas
{
	UINT ID;
	UINT KartJetNum;
	
};

struct AchieveInfo
{
	UINT ID;
	UINT ShowType;
	UINT LogicType;
	UINT Status;
	UINT Progress;
	UINT TotalProgress;
	INT FinTime;
	INT AchieveValue;
	double AchieveFlagType;
	UINT zhanwei;
};
class Item
{
public:
	UINT ID;
	UINT Type;
	UINT Level;	   // 赛车等级 d c b a s t1-3（暂未使用）
	UINT GameType; // 游戏模式（0普通 1 滑板 2 轮滑）
	std::string Mess;	   // 详细类型
	std::string Name;	   // 物品名称
};

void LoadKartAltasListConfig();
void LoadItemConfig();
void LoadAchieveInfo();

void LoadKartSkinPetConfig();
UINT GetItemType(UINT ID);
UINT GetItemGameType(UINT ID);
UINT GetItemLevel(UINT ID);
std::string GetItemName(UINT ID);
std::string  GetItemMess(UINT ID);
INT GetItemSize(UINT type);
