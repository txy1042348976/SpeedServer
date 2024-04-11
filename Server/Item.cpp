#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Item.h"

#include <map>
#include "yaml-cpp/yaml.h"
#include "z_debug.h"
std::map<UINT, Item *> Items;	  // 全部
std::map<UINT, Item *> SkinItems; // 皮肤
std::map<UINT, Item *> PetItems;  // 宠物
std::map<UINT, Item *> KartItems; // 车
std::vector<KartAltas*> KartAltasList; // 车库列表


void LoadKartAltasListConfig()
{
	try
	{
		YAML::Node Config = YAML::LoadFile(".\\Config\\KartAltasList.yml");
		for (size_t i = 0; i < Config.size(); i++)
		{
			YAML::Node item = Config[i];
			KartAltas*pKartAltas = new KartAltas;
			pKartAltas->ID = item["ID"].as<UINT>();
			pKartAltas->KartJetNum = item["KartJetNum"].as<UINT>();
			KartAltasList.insert(KartAltasList.end(), pKartAltas);
		}
	}
	catch (const std::exception &)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		Z_INFO("加载物品文件出错\n");
		exit(0);
	}
	Z_INFO("车库文件加载完成[%d]!\n", KartAltasList.size());
}
void LoadItemConfig()
{
	try
	{
		YAML::Node Config = YAML::LoadFile(".\\Config\\Item.yml");
		for (size_t i = 0; i < Config.size(); i++)
		{
			YAML::Node item = Config[i];
			Item *pItem = new Item;
			pItem->ID = item["ID"].as<UINT>();
			std::string Type = item["Type"].as<std::string>();
			if (item["GameType"])
			{
				pItem->GameType = item["GameType"].as<UINT>();
			}
			else
			{
				pItem->GameType = 0;
			}
			pItem->Mess = item["Mess"].as<std::string>();
			if (item["Name"])
			{
				std::string Name = item["Name"].as<std::string>();
				pItem->Name = Name;
			}
			if (item["Level"])
			{
				pItem->Level = item["Level"].as<UINT>();
			}
			else
			{
				pItem->Level = 0;
			}
			pItem->Type = EAIT_Unknown;
			if (Type == "EAIT_CAR")
			{
				pItem->Type = EAIT_CAR;
				KartItems[pItem->ID] = pItem;
			}
			else if (Type == "EAIT_PET")
			{
				pItem->Type = EAIT_PET;
				PetItems[pItem->ID] = pItem;
			}
			else if (Type == "EAIT_SKIN")
			{
				pItem->Type = EAIT_SKIN;
				SkinItems[pItem->ID] = pItem;
			}
			else if (Type == "EAIT_PROP")
			{
				pItem->Type = EAIT_PROP;
			}
			else if (Type == "EAIT_CLOTHES")
			{
				pItem->Type = EAIT_CLOTHES;
			}
			else if (Type == "EAIT_BABY")
			{
				pItem->Type = EAIT_BABY;
			}
			Items[pItem->ID] = pItem;
		}
	}
	catch (const std::exception &)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		Z_INFO("加载物品文件出错\n");
		exit(0);
	}
	Z_INFO("物品文件加载完成[%d]!\n", Items.size());
}
void LoadKartSkinPetConfig()
{
	try
	{
		YAML::Node Config = YAML::LoadFile(".\\Config\\CarSkinPet.yml");
		for (size_t i = 0; i < Config.size(); i++)
		{
			YAML::Node item = Config[i];
			Item *pItem = new Item;
			pItem->ID = item["ID"].as<UINT>();
			std::string Type = item["Type"].as<std::string>();
			if (item["GameType"])
			{
				pItem->GameType = item["GameType"].as<UINT>();
			}
			else
			{
				pItem->GameType = 0;
			}
			std::string Mess = item["Mess"].as<std::string>();
			pItem->Mess = Mess;
			if (item["Name"])
			{
				std::string Name = item["Name"].as<std::string>();
				pItem->Name = Name;
			}

			if (item["Level"])
			{
				pItem->Level = item["Level"].as<UINT>();
			}
			else
			{
				pItem->Level = 0;
			}
			pItem->Type = EAIT_Unknown;
			if (Type == "EAIT_CAR")
			{
				pItem->Type = EAIT_CAR;
				KartItems[pItem->ID] = pItem;
			}
			else if (Type == "EAIT_PET")
			{
				pItem->Type = EAIT_PET;
				PetItems[pItem->ID] = pItem;
			}
			else if (Type == "EAIT_SKIN")
			{
				pItem->Type = EAIT_SKIN;
				SkinItems[pItem->ID] = pItem;
			}
			else if (Type == "EAIT_PROP")
			{
				pItem->Type = EAIT_PROP;
			}
			else if (Type == "EAIT_CLOTHES")
			{
				pItem->Type = EAIT_CLOTHES;
			}
			else if (Type == "EAIT_BABY")
			{
				pItem->Type = EAIT_BABY;
			}
			Items[pItem->ID] = pItem;
		}
	}
	catch (const std::exception &)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		Z_INFO("加载赛车皮肤宠物文件出错\n");
		exit(0);
	}
	Z_INFO("赛车皮肤宠物文件加载完成[%d]!\n", Items.size());
}
UINT GetItemType(UINT ID)
{
	Item *pItem = PetItems[ID];
	if (!pItem)
	{
		PetItems.erase(ID);
		pItem = SkinItems[ID];
	}
	if (!pItem)
	{
		SkinItems.erase(ID);
		pItem = KartItems[ID];
	}
	if (!pItem)
	{
		KartItems.erase(ID);
		pItem = Items[ID];
	}
	UINT Type = EAIT_Unknown;
	if (ID == 96462 || ID == 95405)
	{
		return Type;
	}
	if (pItem)
	{
		Type = pItem->Type;
	}
	else {
		Items.erase(ID);
	}
	return Type;
}
UINT GetItemLevel(UINT ID)
{
	Item *pItem = PetItems[ID];
	if (!pItem)
	{
		PetItems.erase(ID);
		pItem = SkinItems[ID];
	}
	if (!pItem)
	{
		SkinItems.erase(ID);
		pItem = KartItems[ID];
	}
	if (!pItem)
	{
		KartItems.erase(ID);
		pItem = Items[ID];
	}
	UINT Level = 0;
	if (pItem)
	{
		Level = pItem->Level;
	}
	else {
		Items.erase(ID);
	}
	return Level;
}
UINT GetItemGameType(UINT ID)
{
	Item *pItem = PetItems[ID];
	if (!pItem)
	{
		PetItems.erase(ID);
		pItem = SkinItems[ID];
	}
	if (!pItem)
	{
		SkinItems.erase(ID);
		pItem = KartItems[ID];
	}
	if (!pItem)
	{
		KartItems.erase(ID);
		pItem = Items[ID];
	}
	UINT GameType = 0;
	if (pItem)
	{
		GameType = pItem->GameType;
	}
	else {
		Items.erase(ID);
	}
	return GameType;
}
std::string GetItemName(UINT ID)
{
	Item *pItem = PetItems[ID];
	if (!pItem)
	{
		PetItems.erase(ID);
		pItem = SkinItems[ID];
	}
	if (!pItem)
	{
		SkinItems.erase(ID);
		pItem = KartItems[ID];
	}
	if (!pItem)
	{
		KartItems.erase(ID);
		pItem = Items[ID];
	}
	std::string Name = "";
	if (pItem)
	{
		Name = pItem->Name;
	}
	else {
		Items.erase(ID);
	}
	return Name;
}
std::string GetItemMess(UINT ID)
{
	Item* pItem = PetItems[ID];
	if (!pItem)
	{
		PetItems.erase(ID);
		pItem = SkinItems[ID];
	}
	if (!pItem)
	{
		SkinItems.erase(ID);
		pItem = KartItems[ID];
	}
	if (!pItem)
	{
		KartItems.erase(ID);
		pItem = Items[ID];
	}
	std::string Mess = "";
	if (pItem)
	{
		Mess = pItem->Mess;
	}
	else {
		Items.erase(ID);
	}
	return Mess;
}

INT GetItemSize(UINT type)
{
	if (type == EAIT_CAR)
	{
		return KartItems.size();
	}
	else if (type == EAIT_SKIN)
	{
		return SkinItems.size();
	}
	else if (type == EAIT_PET)
	{
		return PetItems.size();
	}
	else
	{
		return Items.size();
	}
}
