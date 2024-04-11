#pragma once

struct ShoppingCar
{
	int CommodityID;
	int PriceIdx;
	int DiscountCardID;
};

struct ShopItem
{
	UINT ID;
	UINT Idx[10];
	UINT Type;
	INT Avai[10];
};

void LoadShopConfig();
void ResponseBuyCommodity(ClientNode *Client, UINT Uin, UINT Time, const char *Reason, int BuyCount, ShoppingCar *aShoppingCar);
