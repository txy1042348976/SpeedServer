#include "Message.h"

#include <stdio.h>
#include <time.h>
#include <map>
#include <string>
#include "Database.h"
#include "ShopServer.h"
#include "Item.h"
std::map<UINT, ShopItem *> ShopItems;
// 加载商城文件
void LoadShopConfig()
{
	try
	{
		const YAML::Node Config = YAML::LoadFile(".\\Config\\Shop.yml");
		for (size_t i = 0; i < Config.size(); i++)
		{
			const YAML::Node uShop = Config[i];
			ShopItem *pShopItem = new ShopItem;
			pShopItem->ID = uShop["ID"].as<UINT>();
			pShopItem->Type = uShop["Type"].as<UINT>();
			const YAML::Node Idx = uShop["Idx"];
			for (size_t i = 0; i < Idx.size(); i++)
			{
				pShopItem->Idx[i] = Idx[i].as<UINT>();
			}
			const YAML::Node Avai = uShop["AvailPeriod"];
			for (size_t x = 0; x < Avai.size(); x++)
			{
				pShopItem->Avai[x] = Avai[x].as<INT>();
			}
			if (uShop["ContainItem"] && uShop["ContainItem"].size() > 0)
			{
				UINT Size = uShop["ContainItem"].size();
				for (size_t i = 0; i < Size; i++)
				{
					UINT ItemID = uShop["ContainItem"][i]["ItemID"].as<UINT>();
					ShopItems[ItemID] = pShopItem;
				}
			}
		}
	}
	catch (const std::exception &)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		Z_ERROR("加载商城文件出错\n");
		exit(0);
	}
	Z_INFO("商城文件加载完成[%d]!\n", ShopItems.size());
}

void ResponseBuyCommodity(ClientNode *Client, UINT Uin, UINT Time, const char *Reason, int BuyCount, ShoppingCar *aShoppingCar);

ShopItem *GetShopItem(UINT ID)
{
	ShopItem *pShopItem = ShopItems[ID];
	return pShopItem;
}
void RequestBuyCommodity(ClientNode *Client, BYTE *Body, size_t BodyLen)
{

	/*char* pMsg = EByteToHexUA2((const unsigned char*)Body, BodyLen);
	printf("Request_20000	Hex:%s\n", pMsg);*/
	std::string Reason = "";
	char buffer[16] = "";

	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	// NickName[]
	Body += MaxNickName;

	int ReqBuySeq = Read32(Body);
	UINT DstUin = Read32(Body);
	UINT PlayerExp = Read32(Body);
	USHORT CommodityType = Read16(Body);

	// printf("CommodityType: %d\n", CommodityType);

	int PayType = Read32(Body);
	int BuyCount = Read32(Body);
	if (BuyCount == 0)
	{
		return;
	}
	// printf("Uin: %d Time: %d ReqBuySeq: %d DstUin: %d PlayerExp: %d CommodityType: %d PayType: %d BuyCount: %d\n", Uin, Time, ReqBuySeq, DstUin, PlayerExp, CommodityType, PayType, BuyCount);

	// ShopServer
	ShoppingCar *aShoppingCar = (ShoppingCar *)malloc(sizeof(ShoppingCar) * BuyCount);
	if (!aShoppingCar)
	{
		return;
	}
	ItemInfo Items[100];
	for (int i = 0; i < BuyCount; i++)
	{ // ShoppingCar
		BYTE *pShoppingCar = Body;
		size_t len = Read16(pShoppingCar);

		aShoppingCar[i].CommodityID = Read32(pShoppingCar);
		aShoppingCar[i].PriceIdx = Read32(pShoppingCar);
		aShoppingCar[i].DiscountCardID = Read32(pShoppingCar);

		ShopItem *pShopItem = GetShopItem(aShoppingCar[i].CommodityID);
		DWORD ItemNum = 1;
		int AvailPeriod = 0; // 小时 -1永久
		if (pShopItem)
		{
			UINT uType = pShopItem->Type;
			UINT PriceIdx = aShoppingCar[i].PriceIdx;
			ItemNum = pShopItem->Idx[PriceIdx];
			AvailPeriod = pShopItem->Avai[PriceIdx];
			if (AvailPeriod >= 1)
			{
				AvailPeriod *= 24;
			}
		}

		// printf("CommodityID: %d PriceIdx: %d\n", aShoppingCar[i].CommodityID, aShoppingCar[i].PriceIdx);

		/*_itoa_s(aShoppingCar[i].CommodityID, buffer, 10);
		Reason = Reason + "CommodityID:" + buffer;
		_itoa_s(aShoppingCar[i].PriceIdx, buffer, 10);
		Reason = Reason + ", PriceIdx:" + buffer;
		_itoa_s(aShoppingCar[i].PriceIdx, buffer, 10);
		Reason = Reason + ", DiscountCardID:" + buffer;
		Reason += "\n";*/

		ItemInfo Item = PlayerDB_AddItem(DstUin, aShoppingCar[i].CommodityID, ItemNum, AvailPeriod, false);

		Items[i].ItemID = aShoppingCar[i].CommodityID;
		Items[i].ItemNum = Item.ItemNum;
		Items[i].AvailPeriod = Item.AvailPeriod;
		Items[i].Status = Item.Status;
		Items[i].ObtainTime = Item.ObtainTime;
		Items[i].OtherAttribute = Item.OtherAttribute;
		Items[i].ItemType = Item.ItemType;
		if (Items[i].Status && GetItemType(Items[i].ItemID) == EAIT_CAR)
		{
			Client->KartID = Items[i].ItemID;
			// printf("KartID:%d\n", Client->KartID);
		}
		Body += len;
	}

	Reason = "购买成功";

	NotifyClientAddItem(Client, BuyCount, Items);
	/*ItemStatus NewStatus;
	for (int i = 0; i < BuyCount; i++)
	{
		NewStatus.NewStatus = Items[i].Status;
		NewStatus.ItemID = Items[i].ItemID;
		ResponseChangeItemStatus(Client, 1, &NewStatus);
	}*/

	short AttachInfoLen = Read16(Body);
	UINT OtherInfo = Read32(Body);
	UINT FirstLoginTime = Read32(Body);
	UINT BuyNum = Read32(Body);
	UINT GuildID = Read32(Body);
	int ReduceCouponsID = Read32(Body);

	// #ifdef ZingSpeed
	//	Reason += u8"/C03不支持此操作";
	// #else
	//	Reason += "/C03不支持此操作";
	// #endif

	ResponseBuyCommodity(Client, Uin, Time, Reason.c_str(), BuyCount, aShoppingCar);
	free(aShoppingCar);

	/*for (size_t i = 0; i < BuyCount; i++)
	{
		UINT ItemID = Items[i].ItemID;
		UINT ItemNum = Items[i].ItemNum;
		NotifyUseItemResult(Client, ItemID, 1, ItemNum);

		printf("NotifyUseItemResult	\n");

	}*/
}
void ResponseBuyCommodity(ClientNode *Client, UINT Uin, UINT Time, const char *Reason, int BuyCount, ShoppingCar *aShoppingCar)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);	  // ResultID
	Write32(p, Uin);  // Uin
	Write32(p, Time); // Time

	memset(p, 0, MaxNickName); // NickName[]
	p += MaxNickName;

	Write32(p, 0); // ReqBuySeq
	Write32(p, 0); // DstUin
	Write32(p, 0); // PlayerExp

	Write16(p, 0);		  // CommodityType
	Write32(p, 0);		  // PayType
	Write32(p, BuyCount); // BuyCount
	for (int i = 0; i < BuyCount; i++)
	{ // ShoppingCar
		BYTE *pShoppingCar = p;
		Write16(pShoppingCar, 0); // len

		Write32(pShoppingCar, aShoppingCar[i].CommodityID);	   // CommodityID
		Write32(pShoppingCar, aShoppingCar[i].PriceIdx);	   // PriceIdx
		Write32(pShoppingCar, aShoppingCar[i].DiscountCardID); // DiscountCardID

		Write16(pShoppingCar, 0); // DisCount
		Write32(pShoppingCar, 0); // SpeedCoinNum
		Write16(pShoppingCar, 0); // BuyActivityType
		Write32(pShoppingCar, 0); // BuyActivityPara
		Write8(pShoppingCar, 0);  // ActivityNameLen

		len = pShoppingCar - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write16(p, 0); // AttachInfoLen
	Write32(p, 0); // OtherInfo
	Write32(p, 0); // FirstLoginTime

	len = strlen(Reason);
	Write16(p, (WORD)len); // ReasonLen
	memcpy(p, Reason, len);
	p += len;

	Write16(p, 0); // QuerySigLen
	Write8(p, 0);  // CommMbItem
	Write8(p, 0);  // MbItemNum
	/*
m_stItem[].m_cMbItemId
m_stItem[].m_cUse
m_stItem[].m_wContentLen
	*/
	/*
	for (size_t i = 0; i < n; i++)
	{ //Item
		BYTE* pItem = p;
		Write16(pItem, 0); //len

		Write8(pItem, 0); //MbItemId
		Write8(pItem, 0); //Use
		Write16(pItem, 0); //ContentLen

		len = pItem - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, 0); // ReduceCouponsID

	{ // payMoneyInfo
		BYTE *pMoneyInfo = p;
		Write16(pMoneyInfo, 0); // len

		Write32(pMoneyInfo, 0); // money
		Write32(pMoneyInfo, 0); // superMoney
		Write32(pMoneyInfo, 0); // coupons
		Write32(pMoneyInfo, 0); // luckMoney
		Write32(pMoneyInfo, 0); // qbqd

		len = pMoneyInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // CashCouponsID
	Write8(p, 0);  // OtherType
	Write32(p, 0); // QBCouponsNum
	Write16(p, 0); // OrderIDLen
	Write32(p, 0); // SpeedCoinNum

	len = p - buf;
	SendToClient(Client, 20000, buf, len, -1, FE_SHOPSVRD, Client->ConnID, Response);
}
void ResponseGetCfgFile(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	// char* pMsg = EByteToHexUA2((const unsigned char*)Body, BodyLen);
	// printf("ResponseGetCfgFile	Hex:%s\n", pMsg);

	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	len = Read16(Body);
	UINT FileID = Read32(Body);
	UINT FileVersion = Read32(Body);
	UINT CRC = Read32(Body);

	// printf("ResponseGetCfgFile Uin=%d FileID=%d FileVersion=%d CRC=%d\n", Uin, FileID, FileVersion, CRC);
	Write32(p, Uin);  // Uin
	Write32(p, Time); // Time
	Write16(p, 0);	  // ResultID
	{				  // CfgFileInfos
		BYTE *pCfgFileInfos = p;
		Write16(pCfgFileInfos, 0); // len

		Write32(pCfgFileInfos, FileID);		 // FileID
		Write32(pCfgFileInfos, FileVersion); // FileVersion
		Write32(pCfgFileInfos, CRC);		 // CRC
		Write32(pCfgFileInfos, 0);			 // FileLen
		Write32(pCfgFileInfos, 0);			 // FileZipLen
		Write32(pCfgFileInfos, 0);			 // CurFileZipLen

		len = pCfgFileInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0);  // CommDirectionLen
	Write32(p, 0); // DiscountCardVersion
	Write32(p, 0); // ReduceCouponsVersion

	len = p - buf;
	SendToClient(Client, 20002, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseGetPurchaseBroadcast(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write8(p, 0); // BroadcastNum
	for (size_t i = 0; i < 0; i++)
	{ // Broadcast
		BYTE *pBroadcast = p;
		Write16(pBroadcast, 0); // len

		// WriteString(pBroadcast, 0); //Name[]
		/*memset(pBroadcast, 0, MaxNickName);
		strncpy_s((char*)pBroadcast, MaxNickName, "茉莉测试", 9);
		pBroadcast += MaxNickName;*/

		Write32(pBroadcast, (DWORD)time(nullptr)); // tTime
		Write8(pBroadcast, 1);					   // Discount
		Write32(pBroadcast, 126532);			   // CommodityID
		Write32(pBroadcast, 100);				   // SavedMoney

		len = pBroadcast - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 20007, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseGetDiscountCardInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT CurVersion = Read32(Body);

	Write32(p, CurVersion); // Version
	Write8(p, 0);			// DiscountCardNum
	for (size_t i = 0; i < 0; i++)
	{ // DiscountCardInfo
		BYTE *pDiscountCardInfo = p;
		Write16(pDiscountCardInfo, 0); // len

		Write32(pDiscountCardInfo, 0); // ItemID
		Write8(pDiscountCardInfo, 0);  // Discount
		Write8(pDiscountCardInfo, 0);  // ValidCommodityNum
		Write32(pDiscountCardInfo, 0); // ValidCommodityID[]
		Write32(pDiscountCardInfo, 0); // PayTypeFlag
		Write8(pDiscountCardInfo, 0);  // LimitTypeNum
		Write32(pDiscountCardInfo, 0); // LimitTypeList[]
		Write32(pDiscountCardInfo, 0); // OverdueTime

		len = pDiscountCardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 20008, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}