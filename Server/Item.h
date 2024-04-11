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
	UINT Level;	   // �����ȼ� d c b a s t1-3����δʹ�ã�
	UINT GameType; // ��Ϸģʽ��0��ͨ 1 ���� 2 �ֻ���
	std::string Mess;	   // ��ϸ����
	std::string Name;	   // ��Ʒ����
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
