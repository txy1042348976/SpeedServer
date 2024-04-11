#include "Message.h"
#include "Database.h"
#include "LegendShape.h"

#include <stdio.h>
#include <malloc.h>
#include "Item.h"
#include <random>
#include "oicq/tea.h"
#include <mutex>
#include "sqlite/sqlite3.h"
#include "SkillStone.h"
#include "GameServer.h"
#include "Map.h"

extern DWORD WINAPI OnSingleJump(PVOID Client);
extern DWORD WINAPI OnSingleOver(PVOID Client);
extern DWORD WINAPI OnSkill5(PVOID Client);
extern DWORD WINAPI SetGameTime(PVOID Client);
extern sqlite3 *PlayerDB;
extern std::map<UINT, Item*> Items;	  // 全部
extern std::map<UINT, Item*> SkinItems; // 皮肤
extern std::map<UINT, Item*> PetItems;  // 宠物
extern std::map<UINT, Item*> KartItems; // 车
extern std::vector<KartAltas*> KartAltasList; // 车库列表
std::map<UINT, ClientNode *> Clients;
std::mutex Mutex2;
std::map<UINT, AppellationInfo *> Appellations; // 全部
YAML::Node BaseGameConfig = YAML::LoadFile(".\\Config\\GameConfig.yml");
std::map<UINT, LimitMsgInfo*> LimitMsgs;
BYTE StartTimerChallenge[1024] = { 0 };
BYTE StartTimerChallenge2[1024] = { 0 };
int IPClientkey[] = { 1,3,2,1,1,7,2,1,8,3,7,6 };//客户端KEY 预输入登录信息key
int IPServerkey[] = { 3,1,2,8,5,3,7,7,3,0,5,6 };//服务端KEY 登录信息key
int Mysqlkey[] = { 7,9,4,1,7,3,2,7,2,5,5,7 };//数据库Key
std::map<UINT, NpcBaseInfo> NpcList;
void LoadNpcListConfig()
{
	try
	{
		YAML::Node Config = YAML::LoadFile(".\\Npc\\NPCListInfo.yml");
		for (size_t i = 0; i < Config.size(); i++)
		{
			YAML::Node NpcBaseInfos = Config[i];
			NpcBaseInfo* pNpcBaseInfo = new NpcBaseInfo;
			UINT NpcID = NpcBaseInfos["NpcID"].as<UINT>();
			pNpcBaseInfo->NpcID = NpcID;
			pNpcBaseInfo->MinLevel = NpcBaseInfos["MinLevel"].as<UINT>();
			pNpcBaseInfo->MaxLevel = NpcBaseInfos["MaxLevel"].as<UINT>();
			pNpcBaseInfo->RoomName = NpcBaseInfos["RoomName"].as<std::string>();
			pNpcBaseInfo->RoleID = NpcBaseInfos["RoleID"].as<UINT>();
			pNpcBaseInfo->HairID = NpcBaseInfos["HairID"].as<UINT>();
			pNpcBaseInfo->FaceID = NpcBaseInfos["FaceID"].as<UINT>();
			pNpcBaseInfo->GlassID = NpcBaseInfos["GlassID"].as<UINT>();
			pNpcBaseInfo->CoatID = NpcBaseInfos["CoatID"].as<UINT>();
			pNpcBaseInfo->GloveID = NpcBaseInfos["GloveID"].as<UINT>();
			pNpcBaseInfo->TrousersID = NpcBaseInfos["TrousersID"].as<UINT>();
			pNpcBaseInfo->PersonarID = NpcBaseInfos["PersonarID"].as<UINT>();
			pNpcBaseInfo->TattooID = NpcBaseInfos["TattooID"].as<UINT>();
			pNpcBaseInfo->WingID = NpcBaseInfos["WingID"].as<UINT>();
			pNpcBaseInfo->DecoratorID = NpcBaseInfos["DecoratorID"].as<UINT>();
			pNpcBaseInfo->KartID = NpcBaseInfos["KartID"].as<UINT>();
			pNpcBaseInfo->ColorID = NpcBaseInfos["ColorID"].as<UINT>();
			pNpcBaseInfo->TrackID = NpcBaseInfos["TrackID"].as<UINT>();
			pNpcBaseInfo->EffectID = NpcBaseInfos["EffectID"].as<UINT>();
			pNpcBaseInfo->BrandID = NpcBaseInfos["BrandID"].as<UINT>();
			pNpcBaseInfo->TailID = NpcBaseInfos["TailID"].as<UINT>();
			NpcList[NpcID] =*pNpcBaseInfo;
		}
	}
	catch (const std::exception&)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		Z_INFO("加载物品文件出错\n");
		exit(0);
	}
	Z_INFO("NPC文件加载完成[%d]!\n", NpcList.size());
}
NpcBaseInfo* RoomNcpInfo() {
	int randomID;
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<int> dis(1, NpcList.size());
	randomID = dis(eng);
	return &NpcList[randomID];
}

std::string EncryptionIP(std::string IP,UINT Type) {//加密ip函数 //1Client 2 Server
	int len = IP.size();

	for (int i = 0; i < len; i++) {
		if (Type == 1) {
			IP[i] = IP[i] ^ IPClientkey[i % 12];
		}
		else if (Type == 2) {
			IP[i] = IP[i] ^ IPServerkey[i % 12];
		}
		else if (Type == 3) {
			IP[i] = IP[i] ^ Mysqlkey[i % 12];
		}
		
	}
	return IP;
}
std::string DecodeIP(std::string IP, UINT Type) {//解密ip函数
	int len = IP.size();
	for (int i = 0; i < len; i++) {
		if (Type == 1) {
			IP[i] = IP[i] ^ IPClientkey[i % 12];
		}else if (Type == 2) {
			IP[i] = IP[i] ^ IPServerkey[i % 12];
		}
		else if (Type == 3) {
			IP[i] = IP[i] ^ Mysqlkey[i % 12];
		}

		
	}
	return IP;
}
YAML::Node getGameConfig()
{
	return BaseGameConfig;
}
void initListConfig()//初始话限制msg
{
	if (getGameConfig()["LimitMsgID"])
	{
		YAML::Node LimitList = getGameConfig()["LimitMsgID"];
		for (size_t i = 0; i < LimitList.size(); i++)
		{
			YAML::Node Node = LimitList[i];
			LimitMsgInfo* MsgInfo = new LimitMsgInfo;
			MsgInfo->MSGID = Node["MsgID"].as<UINT>();
			MsgInfo->isInfo = Node["INFO"].as<UINT>();
			LimitMsgs[Node["MsgID"].as<UINT>()] = MsgInfo;
		}
	}
}
void LoadAppellationConfig()
{
	try
	{
		YAML::Node Config = YAML::LoadFile(".\\Config\\Appellations.yml");
		for (size_t i = 0; i < Config.size(); i++)
		{
			YAML::Node item = Config[i];
			AppellationInfo *pAppellation = new AppellationInfo;
			pAppellation->Type = item["Type"].as<UINT>();
			pAppellation->Level = item["Level"].as<UINT>();
			pAppellation->Difficulty = item["Difficulty"].as<UINT>();
			pAppellation->MapId = item["MapId"].as<UINT>();
			pAppellation->Value = item["Value"].as<UINT>();
			Appellations[pAppellation->Type] = pAppellation;
		}
	}
	catch (const std::exception &)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		Z_ERROR("加载称号出错\n");
		exit(0);
	}
	Z_INFO("称号文件加载完成[%d]!\n", Appellations.size());
}
UINT GetAppellationLevel(UINT ID)
{
	AppellationInfo *p = Appellations[ID];
	UINT Level = 0;
	if (p)
	{
		Level = p->Level;
	}
	return Level;
}
void ClientNode::InitForRace(UINT GameMapID)
{
	this->MapID = GameMapID;
	auto pMap = GetMap(GameMapID);
	if (pMap)
	{
		this->EndCheckPoint = pMap->End;
		this->TotalRound = pMap->Round;
		this->Loop = pMap->Loop;
	}
	else
	{
		this->EndCheckPoint = 0;
		this->TotalRound = 1;
		this->Loop = false;
	}
	this->MapCompatibilityMode = true;
	this->FinishTime = 0;
	this->Round = 0;
	this->PassedCheckPoint = 0;

	this->useDQ = 0;
	this->kartPow = 0;
	this->NumPropsInHand = 0;
	this->N2OCountRequiredForReborn = 0;
	this->Fuel = 0;
	this->TeamFuel = 0;
	this->CollectedN2O = 0;
	this->KartSkillID = 0;
	this->isRaceBegin = false;
	this->taskID = 0;
	this->SubGameMode = 0;
	this->BaseGameMode = 0;
	RoomNode *Room = GetRoom(this->RoomID);
	if (Room)
	{
		this->SubGameMode = Room->SubGameMode;
		this->BaseGameMode = Room->BaseGameMode;
	}
}
void ClientNode::InitForDance()
{
	this->GameID = 2;
	auto Room = GetRoom(this->RoomID);
	if (!Room)
		return;
	// assert: BaseGameMode == 4
	if (Room->SceneID == 0)
	{
		Room->SceneID = 17;
	}
	if (Room->MusicID < 10)
	{
		Room->MusicID = 100;
	}
	this->IsReady = false;
	this->Scores = 0;
}

const static USHORT GameServerShortCut[64] = {
	0x0074, // 小橘子神灯
	0x007B, // 魔力圣杯
	0x0081, // 终极魔法阵
	0x0089, // 每日一抽
	0x0078, // 糖果屋
	0x007A, // 占卜屋
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
};
void MyRequestGetUin(ClientNode *Client, BYTE *Body, size_t BodyLen);
void MyRequestRegister(ClientNode *Client, BYTE *Body, size_t BodyLen);
void AddClient(ClientNode *Client, UINT Uin)
{
	Clients[Uin] = Client;
}

void DelClient(UINT Uin)
{
	Mutex2.lock();
	Clients.erase(Uin);
	Mutex2.unlock();
}
ClientNode getBakClient(ClientNode* Client) {
	ClientNode temp;
	temp.NobleID = Client->NobleID;
	temp.GameTime = Client->NobleID;
	temp.KartStatus = Client->NobleID;
	temp.BaseGameMode = Client->NobleID;
	temp.SubGameMode = Client->NobleID;
	temp.Server = Client->Server;
	temp.BufferPtr = Client->BufferPtr;
	temp.BufferSize = Client->BufferSize;
	temp.BufferOffset = Client->BufferOffset;
	temp.ConnID = Client->ConnID;
	temp.IsLogin = Client->IsLogin;
	temp.IsFinish = Client->IsFinish;
	temp.IsExit = Client->IsExit;
	temp.Uin = Client->Uin;
	temp.ServerID = Client->ServerID;
	temp.KartID = Client->KartID;
	temp.SkinID = Client->SkinID;
	temp.OrgKartId = Client->OrgKartId;
	temp.TeamID = Client->TeamID;
	temp.useDQ = Client->useDQ;
	temp.getPropNum = Client->getPropNum;
	temp.kartPow = Client->kartPow;
	temp.Fuel = Client->Fuel;
	temp.TeamFuel = Client->TeamFuel;
	temp.CollectedN2O = Client->CollectedN2O;
	temp.AppellationType = Client->AppellationType;
	temp.matchQueueType = Client->matchQueueType;
	temp.isRaceBegin = Client->isRaceBegin;
	temp.taskID = Client->taskID;
	temp.RoomID = Client->RoomID;
	temp.GameID = Client->GameID;
	temp.IsReady = Client->IsReady;
	temp.MapID = Client->MapID;
	temp.FinishTime = Client->FinishTime;
	temp.TotalRound = Client->TotalRound;
	temp.Round = Client->Round;
	temp.PassedCheckPoint = Client->PassedCheckPoint;
	temp.EndCheckPoint = Client->EndCheckPoint;
	temp.Loop = Client->Loop;
	temp.MapCompatibilityMode = Client->MapCompatibilityMode;
	temp.Scores = Client->Scores;
	temp.TaskID = Client->TaskID;
	temp.ChatRoomID = Client->ChatRoomID;
	strcpy_s(temp.NickName, Client->NickName);
	temp.Gender = Client->Gender;
	temp.Country = Client->Country;
	temp.License = Client->License;
	temp.Experience = Client->Experience;
	temp.SuperMoney = Client->SuperMoney;
	temp.Money = Client->Money;
	temp.WinNum = Client->WinNum;
	temp.SecondNum = Client->SecondNum;
	temp.ThirdNum = Client->ThirdNum;
	temp.TotalNum = Client->TotalNum;
	temp.CurHonor = Client->CurHonor;
	temp.TotalHonor = Client->TotalHonor;
	temp.TodayHonor = Client->TodayHonor;
	temp.RelaxTime = Client->RelaxTime;
	temp.MonthDurationBefor = Client->MonthDurationBefor;
	temp.MonthDurationCur = Client->MonthDurationCur;
	temp.Charm = Client->Charm;
	temp.DurationGame = Client->DurationGame;
	temp.DanceExp = Client->DanceExp;
	 temp.Coupons = Client->Coupons;
	temp.Admiration = Client->Admiration;
	temp.LuckMoney = Client->LuckMoney;
	temp.TeamWorkExp = Client->TeamWorkExp;
	temp.AchievePoint = Client->AchievePoint;
	temp.RegTime = Client->RegTime;

	temp.NewModel = Client->NewModel;

	temp.Pet = Client->Pet;
	temp.Engage = Client->Engage;

	temp.NumPropsInHand = Client->NumPropsInHand;
	temp.KartSkillID = Client->KartSkillID;
	temp.PetSkillID = Client->PetSkillID;
	temp.UseCountUpperlimit = Client->UseCountUpperlimit;
	temp.N2OCountRequiredForReborn = Client->N2OCountRequiredForReborn;
	temp.BorderRoomID = Client->BorderRoomID;

	temp.OptNum = Client->OptNum;
	temp.OptIndex = Client->OptIndex;
	temp.JumpThread = Client->JumpThread;
	temp.Timer = Client->Timer;
	temp.Callback = Client->Callback;
	temp.ClientVersion = Client->ClientVersion;
	return temp;

}
ClientNode *GetClient(UINT Uin)
{
	ClientNode *Client = NULL;
	Mutex2.lock();
	auto iter = Clients.find(Uin);
	if (iter != Clients.end())
	{
		Client = iter->second;
	}
	else
	{
		Z_INFO("用户ID:%d 离线\n", Uin);
	}
	Mutex2.unlock();
	return Client;
}
std::map<UINT, ClientNode *> GetClients()
{
	return Clients;
}
// void EeloadRoomList() {
//	std::map<UINT, ClientNode*> Clients = GetClients();
//	std::mutex Mutex2;
//	Mutex2.lock();
//	auto iter = Clients.begin();
//	for (iter = Clients.begin(); iter != Clients.end(); iter++)
//	{
//		ClientNode* Client = iter->second;
//		ResponseGetRoomList(Client, 0, 0);
//	}
//	Mutex2.unlock();
//
// }
void OnRecvFromClient(ClientNode *Client, USHORT MsgID, BYTE *Body, size_t BodyLen)
{
	try
	{
		if (!Client) return;
		if (LimitMsgs[MsgID])
		{
			if (LimitMsgs[MsgID]->isInfo==1) {
	#ifdef DEBUG
				Z_WARN("禁止运行的事件:%d\n", MsgID);
	#endif
			}

			return;
		}
		char uinstr[10];
		std::string propstr1;
		UINT randitem;
		UINT temp;
		BYTE* BodyHead;
	#ifdef DEBUG
		//#include "z_debug.h"
		Z_INFO("[游戏服务][连接ID:%d]请求消息:%d\n", Client->ConnID, MsgID);
	#endif
		if (!Client->IsLogin)
		{
			if (MsgID > 100)
			{
				return; // 未登录则不能执行太多操作
			}
		}
		if (Client->IsExit == true)
		{
			if (MsgID == 112 || MsgID == 102 || MsgID == 538)
				return;
			NotifyKickPlayer(Client, "你的账号可以在另一个地方登录或丢失服务器连接！", 1);
			// ResponseStartGame(Client, "/<customlink=UI.children.AppEventHandle.OnKickPlayerMsg('2','你的账号在另一个设备登录,已经被踢出服务状态')>你的账号在另一个设备登录,请点击这里退出");
			return;
		}
	

		switch (MsgID)
		{
			// MyServer
		case 1: // 登录
			MyRequestGetUin(Client, Body, BodyLen);
			break;
		case 2: // 注册
			MyRequestRegister(Client, Body, BodyLen);
			break;
			// DirServer
		case 90: // 服务器信息
			RequestGetWorldInfo(Client, Body, BodyLen);
			break;
		case 91:// 休闲区列表
			RequestGetChatRoomList(Client, Body, BodyLen);
			break;
		case 100:// 请求登录
			RequestLogin(Client, Body, BodyLen);
			break;
		case 101:// 退出登录
			RequestLogout(Client, Body, BodyLen);
			break;
		case 102:// 房间列表
			RequestGetRoomList(Client, Body, BodyLen);
			break;
		case 103:// 创建房间
			RequestCreateRoom(Client, Body, BodyLen);
			// EeloadRoomList();
			break;
		case 104:// 进入房间
			RequestEnterRoom(Client, Body, BodyLen);
			// EeloadRoomList();
			break;
		case 105:// 离开房间
			if (Client->matchQueueType>0) {//判断匹配模式不调用离开房间
				break;
			}
			RequestLeaveRoom(Client, Body, BodyLen);
			// EeloadRoomList();
			break;
		case 106:// 快速进入房间
			RequestQuickEnterRoom(Client, Body, BodyLen);
			break;
		case 107:// 更改座位状态
			RequestChangeSeatState(Client, Body, BodyLen);
			// EeloadRoomList();
			break;
		case 108:// 踢出房间
			RequestKickFromRoom(Client, Body, BodyLen);
			// EeloadRoomList();
			break;
		case 109:// 更改准备状态
			RequestChangeReadyState(Client, Body, BodyLen);
			break;
		case 110:// 开始游戏
			RequestStartGame(Client, Body, BodyLen);
			// EeloadRoomList();
			break;
		case 111:// 发言、喇叭
			RequestTalk(Client, Body, BodyLen);
			break;
		case 112:// 常规事件（目前用来配置自动喊话）
			RequestHello(Client, Body, BodyLen);
			break;
		case 114:// 添加好友
			RequestAddFriend(Client, Body, BodyLen);
			break;
		case 117:// 添加好友
			RequestDeleteFriendGroup(Client, Body, BodyLen);
			break;
		case 119:// 请求漂移（配置赛车特性）
			RequestReportDrift(Client, Body, BodyLen);
			break;
		case 120:// 准备进入游戏
			if (Client->TaskID == 0)//紫钻练习场房间也会请求这个消息 需要过滤
				RequestPrepareReady(Client, Body, BodyLen);
			break;
			/*RequestPrepareReady(Client, Body, BodyLen);
			break;*/
		case 121:// 更改地图
			RequestChangeMap(Client, Body, BodyLen);
			break;
		case 122:// 根据Uin查找用户
			RequestFindPlayerByQQ(Client, Body, BodyLen);
			break;
		case 123:// 实时报告 四秒调用一次
			//对局实时报告 频繁适度减少读取优化
			RequestReportCurrentInfo(Client, Body, BodyLen);
			if(Client->GameTime==0&& Client->StartGameTime==false){//游戏时间 //特性区
				Client->StartGameTime = true;
				HANDLE GameTime = CreateThread(NULL, 0, SetGameTime, Client, 0, NULL);			
				if (GameTime)
				{
					CloseHandle(GameTime);
				}
				if (Client->GetSkillID(5)) {//手游飞跃特性加强版
					HANDLE Skill = CreateThread(NULL, 0, OnSkill5, Client, 0, NULL);
					if (Skill)
					{
						CloseHandle(Skill);
					}

				}
		
			}
			break;
		case 124:// 请求获得道具 道具模式用的多
			RequestGetProp(Client, Body, BodyLen);
			break;
		case 126:// 请求获得道具 道具模式用的多
			RequestPropEffectResult(Client, Body, BodyLen);
			break;
		case 127:// 请求获得道具 道具模式用的多
			RequestChangeAimState(Client, Body, BodyLen);
			break;
		case 128:// 更改组队队伍
			RequestChangeTeam(Client, Body, BodyLen);
			break;
		case 130:// 穿戴装备
			RequestChangeItemStatus(Client, Body, BodyLen);
			break;
		case 138:// 紫钻练习场、剧情、任务、教程
			RequestStartSingleTask(Client, Body, BodyLen); // 紫钻练习场
			break;
		case 139:// 退出紫钻练习场、剧情、任务、教程
			RequestReportCurrentOpt(Client, Body, BodyLen); // 退出紫钻练习场撒
			break;
		case 135:// 任务列表
			RequestGetTaskList(Client, Body, BodyLen);
			break;
		case 145:// 获取许可证（暂无用处）
			RequestGetLicenseInfo(Client, Body, BodyLen);
			break;
		case 148:// 获取许可证（暂无用处）
			RequestGetGroupFriendBaseInfo(Client, Body, BodyLen);
			break;
		case 151:// 未知
			RequestSwitchInterface(Client, Body, BodyLen);
			break;
		case 152:// 获取系统任务列表（暂无用处）
			RequestGetSystemTaskList(Client, Body, BodyLen);
			break;
		case 153://多人调用函数
			RequestTransferByTCP(Client, Body, BodyLen);
			break;
		case 158: // 增值类道具
			RequestUseItem(Client, Body, BodyLen);
			break;
		case 1652: // 获取随机皇族ID
			RequestRandomOneNobleID(Client, Body, BodyLen);
			break;
		case 1653: // 取消替换皇族ID
			RequestDropRandomNobleID(Client, Body, BodyLen);
			break;
		case 1654: // 替换皇族ID
			RequestReplaceNobleID(Client, Body, BodyLen);
			break;
		case 159: //邀请玩家
			RequestInvitePlayer(Client, Body, BodyLen);
			break;
		case 160:// 幸运币抽奖列表
			RequestExchange(Client, Body, BodyLen);
			break;
		case 162:// 更换性别
			RequestChangeGender(Client, Body, BodyLen);
			break;
		case 713:// 转让房主
			RequestChangeRoomOwner(Client, Body, BodyLen);
			break;
		case 163://加载好友列表（现在还可以用来加载切频道不消失的效果）
			RequestNewGetFriendList(Client, Body, BodyLen);
			break;
		case 167://加载好友列表（现在还可以用来加载切频道不消失的效果）
			RequestCancelEngage(Client, Body, BodyLen);
			break;
		case 116://加载好友列表（现在还可以用来加载切频道不消失的效果）
			RequestAddFriendGroup(Client);
			break;
		case 165://使用氮气道具触发
			if (Client->IsFinish == true) {
				break;
			}
			RequestUseProp2(Client, Body, BodyLen);
			break;
		case 166://请求结婚
			RequestEngage(Client, Body, BodyLen);
			break;
		case 168: // 打开物品
			RequestOpenBox(Client, Body, BodyLen);
			break;
		case 349: // 打开物品
			RequestKartPetDoAction(Client, Body, BodyLen);
			break;
		case 169: // Lupin 休闲区道具配置
			RequestOutDoorEggInfo(Client, Body, BodyLen);
			break;
		case 170: //Lupin 打开休闲区道具
			RequestOutDoorSmashEgg(Client, Body, BodyLen);
			break;
		case 24028://通用函数（暂无用处）
			RequestReportCommonEvent(Client, Body, BodyLen);
			break;
		case 1214:// 改装转移配置
			RequestGetKartRefitAttrTransferInfo(Client, Body, BodyLen);
			break;
		case 1208:// 请求改装转移
			RequestKartRefitAttrTransfer(Client, Body, BodyLen);
			break;
		case 20125://请求非卖品等（未完善）
			RequestGetPlayerNonSaleItemInfo(Client, Body, BodyLen);
			break;
		case 171://商城幸运币
			RequestGetCommodityExchangeItems(Client, Body, BodyLen);
			break;
		case 172://幸运币物品列表
			RequestGetLuckAwardList(Client, Body, BodyLen);
			break;
		case 174://请求改装
			RequestRefitKart(Client, Body, BodyLen);
			break;
		case 175://禁止改装列表（跟赛车源计划有联系）
			RequestGetForbidRefitKartList(Client, Body, BodyLen);
			break;
		case 179://更改Udp状态
			RequestChangeUdpState(Client, Body, BodyLen);
			break;
		case 183://跳过任务（暂无用处）
			RequestSkipFreshTask(Client, Body, BodyLen);
			break;
		case 187:// 获取随机任务（未完善）
			RequestGetRandomTask(Client, Body, BodyLen);
			break;
		case 189:// 打开宝箱
			RequestGetKeyOfBox(Client, Body, BodyLen);
			break;
		case 190://获取宝箱物品
			RequestGetBoxItems(Client, Body, BodyLen);
			break;
		case 195: // 打开荣耀商店
			ResponseGetTeamList(Client, Body, BodyLen);
			break;
		case 255: // 房间  极限挑战
			ResponseGetInviteList(Client, Body, BodyLen);
			break;
		case 233: // 活跃中心（未完善）
			//ResponseC2GGetSignInfo(Client, Body, BodyLen);
			break;
		case 237: // 沙漏信息?
			//ResponseC2GSandglassInfo(Client, Body, BodyLen);
			break;
		case 253://更改舞蹈音乐
			RequestDanceChangeMusic(Client, Body, BodyLen);
			break;
		case 254:// 更改舞蹈场景
			RequestChangeScene(Client, Body, BodyLen);
			break;
		case 257:// 更改舞蹈模式
			RequestDanceChangeMode(Client, Body, BodyLen);
			break;
		case 258:// 更改舞蹈Flag
			RequestDanceChangeRandKeyFlag(Client, Body, BodyLen);
			break;
		case 259:// 进入舞蹈模式
			RequestDancePrepareReady(Client, Body, BodyLen);
			break;
		case 251: // 响应舞蹈获取房间列表（会报错 不影响运行）
			// ResponseDanceGetRoomList(Client, Body, BodyLen);
			break;
		case 260: //舞蹈按键请求
			RequestDanceStepStatistics(Client, Body, BodyLen);
			break;
		case 261://删除物品
			RequestDeleteOneItem(Client, Body, BodyLen);
			break;
		case 262://情侣舞蹈
			RequestLoverDanceChangeOneMode(Client, Body, BodyLen);
			break;
		case 263://情侣舞蹈按键
			RequestLoverDanceStepStatistics(Client, Body, BodyLen);
			break;
		case 271://请求改装信息
			RequestKartRefitInfo(Client, Body, BodyLen);
			break;
		case 277://更换模型
			RequestChangeModel(Client, Body, BodyLen);
			break;
		case 313://获取夺宝物品列表
			RequestGetNewHandCritBoxAwardCfg(Client, Body, BodyLen);
			break;
		case 316://外形改装 装备皮肤
			RequestSaveShapeRefit(Client, Body, BodyLen);
			break;
		case 351://休闲区移动函数
			RequestHeroMove(Client, Body, BodyLen);
			break;
		case 352://进入休闲区
			RequestEnterOutdoor(Client, Body, BodyLen, 0);
			break;
		case 353://退出休闲区
			if (Client->RoomID != 0)//退出3d房间
			{
				LeaveRoom(Client, 0);
			}
			RequestLeaveOutdoor(Client, Body, BodyLen);
			break;
		case 355://休闲区动作函数
			RequestHeroDoAction(Client, Body, BodyLen);
			break;
		case 350://休闲区位置函数
			RequestHeroPos(Client, Body, BodyLen);
			break;
		case 358://休闲区PK
			RequestUseHammer(Client, Body, BodyLen);
			break;
		case 357://新手注册物品
			RequestRegister2(Client, Body, BodyLen);
			break;
		case 359://PK状态函数
			RequestChangeHammerStatus(Client, Body, BodyLen);
			break;
		case 360://未知状态
			RequestChangeChairState(Client, Body, BodyLen);
			break;
		case 362://未知状态
			RequestGetSpecialBroadcasts(Client, Body, BodyLen);
			break;
		case 363://未知
			RequestGetMaster(Client, Body, BodyLen);
			break;
		case 373://获取成就列表
			RequestGetAchieveList(Client, Body, BodyLen);
			break;
		case 376://更改签名
			RequestModifyPlayerSignature(Client, Body, BodyLen);
			break;
		case 389://开始航海
			RequestBeginSailTreasure(Client, Body, BodyLen);
			break;
		case 395://结束航海
			RequestOverSailTreasure(Client, Body, BodyLen);
			break;
		case 415: // 响应获取收藏夹任务
			ResponseGetFavoriteTask(Client, Body, BodyLen);
			break;
		case 440: // 响应获取收藏夹任务
			//RequestGetPetSkillList(Client, Body, BodyLen);
			break;
		case 24426: // 响应获取收藏夹任务
			//RequestGetPetShowInfo(Client, Body, BodyLen);
			break;
		case 24528: // 响应获取收藏夹任务
			//RequestGetPetInfo(Client, Body, BodyLen);
			break;
		case 452:// 获取商品车列表（未完成）
			RequestGetShoppingCarItemList(Client, Body, BodyLen);
			break;
		case 461://获取顶部列表
			RequestTopList(Client, Body, BodyLen);
			break;
		case 499://未知
			RequestTransferUDPOK(Client, Body, BodyLen);
			break;
		case 538://防作弊
			AckAntiBot(Client, Body, BodyLen);
			break;
		case 881://创建城邦
			RequestC2GCreateWonderLand(Client, Body, BodyLen);
			break;
		case 895://检测城邦名字
			RequestC2GCheckWonderLandName(Client, Body, BodyLen);
			break;
		case 901://镶嵌宝石
			RequestInlaySkillStone(Client, Body, BodyLen);
			break;
		case 905://合成宝石
			ResponseCompositeSkillStone(Client, Body, BodyLen);
			break;
		case 934://复位请求
			RequestReportFeedBack(Client, Body, BodyLen);
			break;
		case 936://未知
			RequestGetExchangeDesc(Client, Body, BodyLen);
			break;
		case 944://未知
			RequestFizzInfo(Client, Body, BodyLen);
			break;
		case 1400://获取世界在线信息
			// RequestWorldOnlineInfo(Client, Body, BodyLen);
			break;
		case 20137://进入花园（未完成）
			RequestEnterPersonalGarden(Client, Body, BodyLen);
			break;
		case 1500://影子挑战
			RequestStartShadowChallenge(Client, Body, BodyLen);
			break;
		case 507://添加好友
			AckBeAddedAsFriend(Client, Body, BodyLen);
			break;
		case 572://添加请求
			AckBeEngaged(Client, Body, BodyLen);
			break;
		case 1501://结束影子挑战
			RequestEndShadowChallenge(Client, Body, BodyLen);
			break;
		case 10501://鱼登录
			RequestFishLogin(Client, Body, BodyLen);
			break;
		case 11451://获取玩家事件配置（未配置）
			RequestGetPlayerEventCfg(Client, Body, BodyLen);
			break;
		case 11452://获取玩家配置（未配置）
			RequestGetPlayerEvent(Client, Body, BodyLen);
			break;
		case 11506://获取玩家配置（未配置）
			RequestChangeCarryWizard(Client, Body, BodyLen);
			break;
		case 13710://获取车库配置
			RequestC2SGetKartAltasConfig(Client, Body, BodyLen);
			break;
		case 13705://查询车库
			RequestC2SSearchKartAltas(Client, Body, BodyLen);
			break;
		case 11356: // 获取小橘子桌面信息
			ResponseGetShortCutSet(Client, Body, BodyLen);
			break;
		case 11357: // 保存小橘子桌面信息
			ResponseSetShortCutSet(Client, Body, BodyLen);
			break;
		case 15017:// 检车车队名字
			RequestCheckGuildName(Client, Body, BodyLen);
			break;
		case 15000:// 创建车队
			RequestCreateGuild(Client, Body, BodyLen);
			break;
		case 15003:// 车队信息
			RequestGuildDetail(Client, Body, BodyLen);
			break;
		case 636:// 未知
			RequestGameLogicCommonReport(Client, Body, BodyLen);
			break;
		case 420:// 主题屋状态
			RequestGetWareHouseStatus(Client, Body, BodyLen);
			break;
		case 421:// 主题曲物品列表
			RequestGetWareHouseItemList(Client, Body, BodyLen);
			break;
		case 422:// 主题屋设置
			RequestDoWareHouseOperation(Client, Body, BodyLen);
			break;
		case 11216:
			RequestUpdateCustomMapList(Client, Body, BodyLen);
			break;
		case 11211:
			RequestC2GGetGuildChallengeInfo(Client, Body, BodyLen);
			break;
		case 28412:
			RequestDoBagItemShowOperate(Client, Body, BodyLen);
			break;
		case 131:
			RequestChangeIdentity(Client, Body, BodyLen);
			break;
		case 133:
			RequestCheckName(Client, Body, BodyLen);
			break;
		case 154:
			RequestModifyNickname(Client, Body, BodyLen);
			break;
		case 275://暂无用处
			//RequestChangeRecommandStatus(Client, Body, BodyLen);
			break;
		case 903:
			RequestAddStoneGroove(Client, Body, BodyLen);
			break;
		case 697:
			RequestPVPLogin(Client, Body, BodyLen);
			break;
		case 909:
			// 宝石发动通知其他玩家
			NotifyMatchSkillStoneInfo(Client, Body, BodyLen);
			break;
		case 912:
			RequestRemoveSkillStone(Client, Body, BodyLen);
			break;
		case 10927:
			RequestSkateSkillUpgrade(Client, Body, BodyLen);
			break;
		case 178:
			RequestUpdatePetName(Client, Body, BodyLen);
			break;
		case 317:
			RequestGetShapeRefitCfg(Client, Body, BodyLen);

			break;
		case 319: //
			//ResponseShapeLegendOperate(Client, Body, BodyLen);
			RequestShapeLegendOperate(Client, Body, BodyLen);
			break;
		case 424:
			RequestLeaveGame(Client, Body, BodyLen);
			Client->matchQueueType = 0;
			break;
		case 1401:
			RequestC2GSign3Operate(Client, Body, BodyLen);
			break;
		case 16055:
			RequestGetPrivilegeIntroInfo(Client, Body, BodyLen);
			break;
		case 20079:
			RequestAddToMatchQueue(Client, Body, BodyLen);
			break;
		case 20080:
			RequestCancelMatchQueue(Client, Body, BodyLen);
			break;
		case 20082:
			RequestEnterMatchRoom(Client, Body, BodyLen);
			break;
		case 20097:
			RequestReportCommonAction(Client, Body, BodyLen);
			break;
		case 21110:
			RequestSecondRefitKart(Client, Body, BodyLen);
			break;
		case 21111:
			RequestSecondGetKartRefitLimitCfg(Client, Body, BodyLen);
			break;
		case 24029:
			RequestGetActivityCenterInfo(Client, Body, BodyLen);
			break;
		case 24095: // 全人民送车大狂欢
			ResponseCarLottoryGetInfo(Client, Body, BodyLen);
			break;
		case 24724: // 全人民送车大狂欢
			RequestGetPwdRedEnvelopeContent(Client, Body, BodyLen);
			break;
		case 24723: // 全人民送车大狂欢
			RequestSendPwdRedEnvelope(Client, Body, BodyLen);
			break;
		case 15015: // 全人民送车大狂欢
			RequestPlayerGuildInfo(Client, Body, BodyLen);
			break;
		case 24053:
			RequestC2GGetPlayerStatus(Client, Body, BodyLen);
			break;
		case 24201:
			RequestGetItemTimeLimtCfg(Client, Body, BodyLen);
			break;
		case 24202:
			RequestHyperSpaceJumboGetCfg(Client, Body, BodyLen);
			break;
		case 24215:
			RequestGetKeyTranslateInfoNew(Client, Body, BodyLen);
			break;
		case 24251:
			RequestQuickRPC(Client, Body, BodyLen);
			break;
		case 24258:
			RequestTimerChallenge2ndMoreHallInfo(Client, Body, BodyLen);
			break;
		case 24391:
			RequestWeRelayCommonCfg(Client, Body, BodyLen);
			break;
		case 24465:
			RequestPreGetCommonBoxAward(Client, Body, BodyLen);
			break;
		case 24470:
			RequestGetMagicHouseInfo(Client, Body, BodyLen);
			break;
		case 24721:
			RequestSSCOpenStatus(Client, Body, BodyLen);
			break;
		case 25000:
			RequestGetRankedMatchShowInfo(Client, Body, BodyLen);
			break;
		case 26114:
			RequestReportAntiCollisionDataBuff(Client, Body, BodyLen);
			break;
		case 28331:
			RequestGetKartTransformersInfo(Client, Body, BodyLen);
			break;
		case 28421:
			RequestReplayOperate(Client, Body, BodyLen);
			break;
		case 11043:
			RequestC2GGetHRoomPhyInfo(Client, Body, BodyLen);
			break;
			// ShopServer
		case 20000:
			RequestBuyCommodity(Client, Body, BodyLen);
			break;
		case 15398:
			RequestChangePetEquipStatus(Client, Body, BodyLen);
			break;
		case 10929:
			RequestGetBattleModeCarSkillInfoList(Client, Body, BodyLen);
			break;
		case 16068:
			RequestGetAllWakeKartInfo(Client, Body, BodyLen);
			break;
		case 24310:
			RequestDoActiveKartInfo(Client, Body, BodyLen);
			break;
		case 24311:
			RequestGetActivedKartInfo(Client, Body, BodyLen);
			break;
		case 20128:
			RequestGetNonSaleItemConsumeItemList(Client, Body, BodyLen);
			break;
		case 28379:
			RequestGetQuickRaceTimeRandMap(Client, Body, BodyLen);
			break;
		case 11229:
			RequestC2GLoadFile(Client, Body, BodyLen);
			break;
		case 25010:
			RequestRankedMatchDoBanMap(Client, Body, BodyLen);
			break;
		case 24756:
			RequestGetGameEndBoxAward(Client, Body, BodyLen);
			break;
		case 21036:
			RequestDoWareHouseOperationBatch(Client, Body, BodyLen);
			break;
		case 21017:
			RequestOpSecondPass(Client, Body, BodyLen);
			break;
		case 28413:
			RequestDressInfoOperate(Client, Body, BodyLen);
			break;
		case 25110:
			RequestInternationalSeriesGetInfo(Client, Body, BodyLen);
			break;
		case 20083:
			NotifyBorderGameCondition(Client);
			// RequestBuyCommodity(Client, Body, BodyLen);
			break;
		case 24640:
			RequestPlayerAction(Client, Body, BodyLen);
			break;
		case 28419:
			RequestQsLabCommittalTaskInfo(Client, Body, BodyLen);
			break;
		case 11526:
			RequestC2GGetWizardBaseInfo(Client, Body, BodyLen);
			break;
		case 20126:
			RequestGetShopNonSaleInfo(Client, Body, BodyLen);
			break;
		case 1618:
			RequestWearAppellation(Client, Body, BodyLen);
			break;
		case 1619:
			RequestUnwearAppellation(Client, Body, BodyLen);
			break;
		case 25012:
			RequestRankedMatchPersonalPanelInfo(Client, Body, BodyLen);
			break;
		case 13711:
			RequestPointCfg(Client, Body, BodyLen);
			break;
		case 25013:
			RequestRankedMatchPannelOperate(Client, Body, BodyLen);
			break;
		case 11474:
			RequestGetWizardHomeBaseInfo(Client, Body, BodyLen);
			break;
		case 24360:
			RequestRaiseLoverBabyDirectlyInfo(Client, Body, BodyLen);
			break;
		case 24352:
			RequestGetLoveBabyInfo(Client, Body, BodyLen);
			break;
		case 24043:
			//RequestGetDropInfo(Client, Body, BodyLen);
			break;
		case 11471:
			//RequestGetConsumeScoreConfUrl(Client, Body, BodyLen);
			break;
		case 24412:
			RequestSkatePropRoomZan(Client, Body, BodyLen);
			break;
		case 20002:
			ResponseGetCfgFile(Client, Body, BodyLen);
			break;
		case 20007:
			//ResponseGetPurchaseBroadcast(Client, Body, BodyLen);
			break;
		case 20008:
			//ResponseGetDiscountCardInfo(Client, Body, BodyLen);
			break;
		case 20040: // 天天爱升级 领取经验
			//ResponseC2SReceiveTimesExperience(Client, Body, BodyLen);
			break;
		case 20041: // 天天爱升级 冻结经验
			//ResponseC2SFrostThawTimesExperience(Client, Body, BodyLen);
			break;
		case 20042: // 天天爱升级
			//ResponseC2SGetTimesExperienceInfo(Client, Body, BodyLen);
			break;
		case 24507: // 响应获小橘子神灯配置
			ResponseGetMagicLampCfg(Client, Body, BodyLen);
			break;
		case 24508: // 响应获小橘子神灯 点灯
			ResponseMagicLampLightOn(Client, Body, BodyLen);
			break;
		case 24726: // 魔力圣杯
			ResponseGetMagicCupCfg(Client, Body, BodyLen);
			break;
		case 24727: // 魔力圣杯 魔法盒
			ResponseGetTempBoxItemInfo(Client, Body, BodyLen);
			break;
		case 24728: // 魔力圣杯 施法 抽奖
			ResponseMagicCupGetAward(Client, Body, BodyLen);
			break;
		case 24729: // 魔力圣杯 魔法盒 分解成魔法碎片
			ResponseMagicCupExchangePiece(Client, Body, BodyLen);
			break;
		case 24471: // 魔法阵抽奖
			ResponseMagicHouseCallOn(Client, Body, BodyLen);
			break;
		case 25130: // 每日一抽 打开
			ResponseGetDailyExtractShowInfo(Client, Body, BodyLen);
			break;
		case 25131: // 每日一抽 试试手气 抽奖
			ResponseDailyExtractInfo(Client, Body, BodyLen);
			break;
		case 25132: // 每日一抽 选定奖励
			ResponseChooseAwardForDailyExtract(Client, Body, BodyLen);
			break;
		case 24308: // 小橘子糖果屋品尝糖果
			ResponseGetAward(Client, Body, BodyLen);
			break;
		case 24642: // 小橘子的糖果屋
			ResponseGetMagicCubesInfo(Client, Body, BodyLen);
			break;
		case 24786: // 魔法阵抽奖 商店
			ResponseGetExchangeShopInfo(Client, Body, BodyLen);
			break;
		case 24787: // 魔法阵抽奖 商店 兑换道具
			ResponseDoShopExchange(Client, Body, BodyLen);
			break;
		case 24501:
			RequestGetThemeHouseShowInfo(Client, Body, BodyLen);
			break;
		case 24754:
			ResponsePointChallengeOpenMainBoard(Client);//极限挑战
			break;
		case 24236:
			ResponseStartTimerChallenge(Client);
			break;
		case 24254:
			//PrintfBody(Client, MsgID, Body, BodyLen);
			RequestReportTimerChallenge2ndEvent(Client, Body, BodyLen);
			break;
		case 21133:
			//PrintfBody(Client, MsgID, Body, BodyLen);
			//RequestGetPKPetInfo(Client,Body, BodyLen);
			break;
		case 21140://响应获取PK宠物配置
			//ResponseGetPKPetCfg(Client, Body, BodyLen);
			break;
		case 21150:
			//PrintfBody(Client, MsgID, Body, BodyLen);
			RequestPetTrialInfo(Client,Body, BodyLen);
			break;
		case 24784:
			RequestGetLoveCrystalInfo(Client,Body, BodyLen);
			break;
		case 24793:
			RequestGetBabyDeleteAwardInfo(Client,Body, BodyLen);
			break;
		case 24386:
			RequestChangeLoverBabyEquipStatus(Client,Body, BodyLen);
			break;
		default:
	#ifdef DEBUG
			Z_DEBUG("未配置的事件 % d\n", MsgID);
	#endif
			break;
		}
	}
	catch (const std::exception&)
	{
		Z_DEBUG("发生异常 % d\n", MsgID);
	}
	catch (...)
	{
		Z_DEBUG("发生异常 % d\n", MsgID);
	}
}

void SendToClient(ClientNode *Client, short MsgID, const BYTE *Data, DWORD Length, short SrcID, char SrcFE, short DstID, short MsgType, char Encrypt)
{
	if (!Client) { return; }
	if (Client->ConnID <= 0) return;
	CSHEAD *ProtocolHead;
	MSGHEAD *MsgHead;
	void *MsgBody;
	int TotalLength;

	int MsgLen = sizeof(MSGHEAD) + Length;
	MsgHead = (MSGHEAD *)malloc(MsgLen);
	if (!MsgHead)
	{
		return;
	}
	MsgHead->MsgID = htons(MsgID);
	MsgHead->MsgType = htons(MsgType);
	MsgHead->MsgSeq = htonl(-1);
	MsgHead->SrcFE = SrcFE;
	MsgHead->DstFE = FE_CLIENT;
	MsgHead->SrcID = htons(SrcID);
	MsgHead->DstID = htons(DstID);
	MsgHead->BodyLen = htons(sizeof(USHORT) + USHORT(Length));
	MsgBody = (void *)(MsgHead + 1);

	memcpy(MsgBody, Data, Length);
	if (Encrypt)
	{
		int EncryptLen = oi_symmetry_encrypt2_len(MsgLen);
		TotalLength = sizeof(CSHEAD) + EncryptLen;
		if (!(ProtocolHead = (CSHEAD *)malloc(TotalLength)))
		{
			return;
		}
		oi_symmetry_encrypt2((BYTE *)MsgHead, MsgLen, Client->Key, (BYTE *)(ProtocolHead + 1), &EncryptLen);
		TotalLength = sizeof(CSHEAD) + EncryptLen;
	}
	else
	{
		TotalLength = sizeof(CSHEAD) + MsgLen;
		if (!(ProtocolHead = (CSHEAD *)malloc(TotalLength)))
		{
			return;
		}
		memcpy((MSGHEAD *)(ProtocolHead + 1), MsgHead, MsgLen);
	}
	free(MsgHead);

	ProtocolHead->TotalLength = htonl(TotalLength);
	ProtocolHead->Ver = htons(116);
	ProtocolHead->DialogID = htons(SrcID);
	ProtocolHead->Seq = htonl(-1);
	ProtocolHead->Uin = htonl(Client->Uin);
	ProtocolHead->BodyFlag = Encrypt;
	ProtocolHead->OptLength = 0;

	Client->Server->Send(Client->ConnID, (BYTE *)ProtocolHead, TotalLength);
#ifdef DEBUG
	Z_YELLOW("[连接ID:%d]回复消息:%d\n", Client->ConnID, MsgID);
#endif 

	free(ProtocolHead);
}

void OnFullReceive(ClientNode *Client, BYTE *Data, size_t Length)
{
	if (Length < sizeof(CSHEAD))
	{
		Client->Server->Disconnect(Client->ConnID);
		return;
	}
	CSHEAD *ProtocolHead = (CSHEAD *)Data;
	BYTE *Body;
	DWORD HeadLen = sizeof(CSHEAD) + ProtocolHead->OptLength;
	if (Length < HeadLen)
	{
		Client->Server->Disconnect(Client->ConnID);
		return;
	}

	if (ProtocolHead->OptLength)
	{
		Body = (BYTE *)(ProtocolHead + 1);
		// 官方服务器应该根据这个进行查表获取key
		// 而我们偷懒,把它作为登录密码,这样其实不安全,并且它是明文传输的

		char *Password = new char[ProtocolHead->OptLength + 1];
		memcpy(Password, Body, ProtocolHead->OptLength);
		Password[ProtocolHead->OptLength] = 0;
		// printf("CmpPassword:%s\n", Password);
#ifdef ZingSpeed
		if (strcmp("!", Password) == 0) // TODO: 这个密码是固定的,不安全.
		{
			Client->IsLogin = TRUE;
		}
#else
		if (UserDB_CmpPassword(ntohl(ProtocolHead->Uin), Password))
		{
			Client->IsLogin = TRUE;
		}
#endif
		delete[] Password;
	}

	DWORD BodyLen = Length - HeadLen;
	if (BodyLen >= sizeof(MSGHEAD))
	{
		MSGHEAD *MsgHead = (MSGHEAD *)((DWORD)Data + HeadLen);
		if (ProtocolHead->BodyFlag)
		{
			Body = (BYTE *)malloc(BodyLen);
			if (!Body)
			{
				return;
			}
			if (oi_symmetry_decrypt2((BYTE *)MsgHead, BodyLen, Client->Key, Body, (int *)&BodyLen) == false)
			{
				Z_ERROR("[连接ID:%d]解密失败\n", Client->ConnID);
				free(Body);
				return;
			}
		}
		else
		{
			Body = (BYTE *)malloc(BodyLen);
			if (!Body)
			{
				return;
			}
			memcpy(Body, MsgHead, BodyLen);
		}
		MsgHead = (MSGHEAD *)Body;
		Body = (BYTE *)Body + sizeof(MSGHEAD);
		BodyLen = BodyLen - sizeof(MSGHEAD);

		if (ProtocolHead->OptLength)
		{
			Client->ServerID = ntohs(MsgHead->DstID);
		}
		OnRecvFromClient(Client, ntohs(MsgHead->MsgID), Body, ntohs(MsgHead->BodyLen));

		free(MsgHead);
	}
	else
	{
		Z_ERROR("[连接ID:%d]不支持请求检查更新\n", Client->ConnID);
		Client->Server->Disconnect(Client->ConnID);
	}
}

void RequestFishLogin(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseFishLogin(Client);
}
void ResponseFishLogin(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // Result
	Write32(p, Client->Uin); // Uin

	{ // BaseFishInfo
		BYTE *pBaseFishInfo = p;
		Write16(pBaseFishInfo, 0); // len

		Write32(pBaseFishInfo, 0); // Experience
		Write32(pBaseFishInfo, 0); // Money
		Write8(pBaseFishInfo, 0);  // FishStockNum
		Write8(pBaseFishInfo, 0);  // RubbishCount

		len = pBaseFishInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ // TodayInfo
		BYTE *pTodayInfo = p;
		Write16(pTodayInfo, 0); // len

		Write8(pTodayInfo, 0); // RestStealCount
		Write8(pTodayInfo, 0); // RestVisitStrangerCount
		Write8(pTodayInfo, 0); // RestClearRubbishCount

		len = pTodayInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); // FishStockNum
	/*
m_astFishStockInfo[].m_uchFishStockID
m_astFishStockInfo[].m_uchMaturityCount
m_astFishStockInfo[].m_uchTotalGrowStage
m_astFishStockInfo[].m_uchCurrentGrowStage
m_astFishStockInfo[].m_uchFishNum
m_astFishStockInfo[].m_uchStealedCount
m_astFishStockInfo[].m_uchFeedInfo
m_astFishStockInfo[].m_iFishItemID
m_astFishStockInfo[].m_uiFishStartTime
m_astFishStockInfo[].m_uiMaturityTime
m_astFishStockInfo[].m_uiCurGrowStageEndTime
	*/

	Write8(p, 0); // ReasonLen

	len = p - buf;
	SendToClient(Client, 10501, buf, len, 0, 32, Client->ConnID, Response);
}

void RequestGetPlayerEventCfg(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseGetPlayerEventCfg(Client);
}
void ResponseGetPlayerEventCfg(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write32(p, 0); // GetFriendsEventInterval
	Write32(p, 0); // GetOtherEventInterval
	Write32(p, 0); // FilterTimeLimit

	len = p - buf;
	SendToClient(Client, 11451, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetPlayerEvent(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE TargetType = Read8(Body);
	BYTE TimeType = Read8(Body);

	// m_ushTarGetNum
	// m_astTarGetInfo[].m_uiUin
	// m_astTarGetInfo[].m_uiEventID
	ResponseGetPlayerEvent(Client, TargetType, TimeType);
}
void ResponseGetPlayerEvent(ClientNode *Client, BYTE TargetType, BYTE TimeType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);		   // ResultID
	Write8(p, TargetType); //
	Write8(p, TimeType);   //
	Write16(p, 0);		   // Num
	/*
m_astEvent[].m_uiUin
m_astEvent[].m_ucType
m_astEvent[].m_uiEventID
m_astEvent[].m_ucIntNum
m_astEvent[].m_auiPara[]
m_astEvent[].m_ucShortNum
m_astEvent[].m_aushPara[]
m_astEvent[].m_uStrNum
m_astEvent[].m_astPara[].m_szStr[]
m_astEvent[].m_uiTime
	*/

	len = p - buf;
	SendToClient(Client, 11452, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestChangeUdpState(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	BYTE State = Read8(Body);
	Z_INFO("Udp状态:%d\n", State);
}

void RequestTransferUDPOK(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseTransferUDPOK(Client);
}
void ResponseTransferUDPOK(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID

	len = p - buf;
	SendToClient(Client, 499, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
/*
999
--------------------CNotifyUDPOKPara--------------------
m_unUin
m_unTime
m_nSrcDlg
m_unSrcUin
m_nInfoLength
--------------------------------------------------------
*/

void RequestFizzInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseFizzInfo(Client);
}
void ResponseFizzInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	{			   // FizzBaseInfo
		BYTE *pFizzBaseInfo = p;
		Write16(pFizzBaseInfo, 0); // len

		Write16(pFizzBaseInfo, 0); // FizzPointDaily
		Write16(pFizzBaseInfo, 0); // FizzPointWeekly
		Write32(pFizzBaseInfo, 0); // FizzPointTotal
		Write8(pFizzBaseInfo, 0);  // FizzEmblem

		len = pFizzBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // FizzTaskNum
	/*
	for (size_t i = 0; i < 0; i++)
	{ //FizzTaskInfo
		BYTE* pFizzTaskInfo = p;
		Write16(pFizzTaskInfo, 0); //len

		Write8(pFizzTaskInfo, 0); //TaskID
		Write8(pFizzTaskInfo, 0); //Status
		Write32(pFizzTaskInfo, 0); //TotalProgress
		Write32(pFizzTaskInfo, 0); //CurrentProgress
		Write16(pFizzTaskInfo, 0); //FizzPoint
		Write8(pFizzTaskInfo, 0); //DisplayPri
		Write8(pFizzTaskInfo, 0); //SpecialFlag

		len = pFizzTaskInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); // FizzLotteryNum
	/*
	for (size_t i = 0; i < 0; i++)
	{ //FizzLotteryInfo
		BYTE* pFizzLotteryInfo = p;
		Write16(pFizzLotteryInfo, 0); //len

		Write8(pFizzLotteryInfo, 0); //LotteryType
		Write8(pFizzLotteryInfo, 0); //Status
		Write16(pFizzLotteryInfo, 0); //FizzPoint
		Write8(pFizzLotteryInfo, 0); //ControlledItemNum
		Write32(pFizzLotteryInfo, 0); //ControlledItem[]

		len = pFizzLotteryInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	len = p - buf;
	SendToClient(Client, 944, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetShoppingCarItemList(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	// ResponseGetShoppingCarItemList(Client);
}
void ResponseGetShoppingCarItemList(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	{			   // ShoppingCarItemList
		BYTE *pShoppingCarItemList = p;
		Write16(pShoppingCarItemList, 0); // len

		Write8(pShoppingCarItemList, 1);	  // ItemNum
		Write32(pShoppingCarItemList, 10020); // Item[]

		len = pShoppingCarItemList - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 452, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetKeyTranslateInfoNew(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseGetKeyTranslateInfoNew(Client);
}
void ResponseGetKeyTranslateInfoNew(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Uin
	Write16(p, 0); // ResultID
	Write8(p, 0);  // ReasonLen
	{			   // KeyTranslateInfo
		BYTE *pKeyTranslateInfo = p;
		Write16(pKeyTranslateInfo, 0); // len

		Write8(pKeyTranslateInfo, 0); // KeyTranslateDefaultKeyNum1
		Write8(pKeyTranslateInfo, 0); // KeyTranslateNewKeyNum1
		Write8(pKeyTranslateInfo, 0); // KeyTranslateDefaultKeyNum2
		Write8(pKeyTranslateInfo, 0); // KeyTranslateNewKeyNum2

		len = pKeyTranslateInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24215, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestC2SGetKartAltasConfig(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT CfgVersion = Read32(Body);

	ResponseC2SGetKartAltasConfig(Client, CfgVersion);
}
void ResponseC2SGetKartAltasConfig(ClientNode *Client, UINT CfgVersion)
{
	BYTE buf[8192*5];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write32(p, CfgVersion);	 // CfgVersion
	Write8(p, 0);			 // Result
	Write16(p, KartAltasList.size()); // KartAltasCfgNum
	for (size_t i = 0; i < KartAltasList.size(); i++)
	{ //KartAltasConfig
		BYTE* pKartAltasConfig = p;
		Write16(pKartAltasConfig, 0); //len
		Write32(pKartAltasConfig, KartAltasList[i]->ID); //ItemID
		Write32(pKartAltasConfig, 0); //KartAltasUrlLen
		Write32(pKartAltasConfig, KartAltasList[i]->KartJetNum); //KartJetNum

		len = pKartAltasConfig - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 13710, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetMaster(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseGetMaster(Client);
}
void ResponseGetMaster(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	Write8(p, 0);			 // AccStatus
	Write32(p, 0);			 // MasterUin
	Write32(p, 0);			 // AccTime
	Write32(p, 0);			 // GraduateTime
	Write16(p, 0);			 // RecentPMAwardNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //RecentPMAwards
		BYTE* pRecentPMAwards = p;
		Write16(pRecentPMAwards, 0); //len

		Write16(pRecentPMAwards, 0); //RecentPMAwardDesLen

		len = pRecentPMAwards - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); // ReasonLen

	len = p - buf;
	SendToClient(Client, 363, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestSSCOpenStatus(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseSSCOpenStatus(Client);
}
void ResponseSSCOpenStatus(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write8(p, 0);  // Satus
	Write8(p, 0);  // TipsLen

	len = p - buf;
	SendToClient(Client, 24721, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestSwitchInterface(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	bool InterfaceType = Read8(Body);

	ResponseSwitchInterface(Client);
}
void ResponseSwitchInterface(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin

	len = p - buf;
	SendToClient(Client, 151, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestSecondGetKartRefitLimitCfg(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	const static UINT OpenKartTypeListCfg[] = {
		// 可以进阶改装的赛车类型列表
		EAIT_CAR_NEW_A,
		EAIT_CAR_NEW_B,
		EAIT_CAR_NEW_D,
		EAIT_CAR_NEW_S,
		EAIT_CAR_NEW_M,
		EAIT_CAR_NEW_M0,
		EAIT_CAR_NEW_L3,
		EAIT_CAR_NEW_M3,
		EAIT_CAR_NEW_M4,
		EAIT_CAR_NEW_L2,
		EAIT_CAR_NEW_L3,
		EAIT_CAR_NEW_L4,
		EAIT_CAR_NEW_M2,
		EAIT_CAR_NEW_C,
		EAIT_CAR_NEW_L0,
		EAIT_CAR_NEW_L1,
		EAIT_CAR_NEW_M1,
		// 1565,
		EAIT_CAR_NEW_T1,
		EAIT_CAR_NEW_T2,

		EAIT_CAR_NEW_T3,

	};
	// const static UINT ExcludeKartTypeListCfg[];
	typedef struct REFITITEMWEIGHTTYPE // 库定义数据类型结构
	{
		UCHAR ItemWeightLevel;
		WORD MinWeight;
		WORD MaxWeight;
		WORD ExpAward;
	} *LPREFITITEMWEIGHTTYPE;
	const static REFITITEMWEIGHTTYPE RefitItemWeightTypeCfg[] =
		{
			// ItemWeightLevel
			// MinWeight
			// MaxWeight
			// ExpAward
			{0, 0, 500, 500},
			{1, 0, 500, 500},
			{2, 0, 500, 800},
			{3, 0, 500, 1000},
			{0, 0, 0, 0},
		};
	typedef struct CREFITITEMCFG //
	{
		UINT ItemID;
		UCHAR ItemType;
		UCHAR ItemWeightLevel;
		WORD Value;
		UINT FragmentItemID;
		UINT FragmentNumToOneItem;
		WORD IsItemOpen;
	} *LPCREFITITEMCFG;
	const static CREFITITEMCFG RefitItemCfg[] =
		{
			/*
				ItemID 进阶改装配件ID
				ItemType	改装道具类型 5=推进 6=离合 7=平衡
				ItemWeightLevel	道具重量值	1=普通 2=精良	3=极致
				Value			改装增加值
				FragmentItemID	碎片道具ID
				FragmentNumToOneItem	碎片数量
				IsItemOpen 是否启用
			*/
			{65025, 5, 1, 1, 65037, 100, 1},
			{65026, 5, 2, 1, 65038, 100, 1},
			{65027, 5, 3, 1, 65039, 100, 1},
			{65028, 6, 1, 1, 65040, 100, 1},
			{65029, 6, 2, 1, 65041, 100, 1},
			{65030, 6, 3, 1, 65042, 100, 1},
			{65031, 7, 1, 1, 65043, 100, 1},
			{65032, 7, 2, 1, 65044, 100, 1},
			{65033, 7, 3, 1, 65045, 100, 1},
			{65034, 5, 1, -1, 0, 0, 1},
			{65035, 6, 1, -1, 0, 0, 1},
			{65036, 7, 1, -1, 0, 0, 1},
		};

	Write16(p, 0);	 // ResultID
	Write32(p, Uin); // Uin
	{				 // KartRefitLimitCfg
		BYTE *pKartRefitLimitCfg = p;
		Write16(pKartRefitLimitCfg, 0); // len

		Write16(pKartRefitLimitCfg, sizeof(OpenKartTypeListCfg) / sizeof(OpenKartTypeListCfg[0])); // OpenKartTypeNum
		for (size_t i = 0; i < sizeof(OpenKartTypeListCfg) / sizeof(OpenKartTypeListCfg[0]); i++)
		{
			Write32(pKartRefitLimitCfg, OpenKartTypeListCfg[i]); // OpenKartTypeList[]
		}
		Write32(pKartRefitLimitCfg, 0); // MinFormerRefitTime 要求最低改装次数
		Write16(pKartRefitLimitCfg, 0); // ExcludeKartTypeNum
		// for (size_t i = 0; i < sizeof(ExcludeKartTypeListCfg) / sizeof(ExcludeKartTypeListCfg[0]); i++) {
		//	Write32(pKartRefitLimitCfg, ExcludeKartTypeListCfg[i]); //ExcludeKartTypeList[]
		// }
		Write16(pKartRefitLimitCfg, 1);		// ForbidListNum
		Write32(pKartRefitLimitCfg, 11190); // ForbidRefitKartID[]

		len = pKartRefitLimitCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // RefitItemWeightTypeCfg
		BYTE *pRefitItemWeightTypeCfg = p;
		Write16(pRefitItemWeightTypeCfg, 0); // len

		Write8(pRefitItemWeightTypeCfg, sizeof(RefitItemWeightTypeCfg) / sizeof(RefitItemWeightTypeCfg[0])); // TypeNum
		for (size_t i = 0; i < sizeof(RefitItemWeightTypeCfg) / sizeof(RefitItemWeightTypeCfg[0]); i++)
		{ // ItemWeightLevel
			BYTE *pItemWeightLevel = pRefitItemWeightTypeCfg;
			Write16(pItemWeightLevel, 0); // len

			Write8(pItemWeightLevel, RefitItemWeightTypeCfg[i].ItemWeightLevel); // ItemWeightLevel
			Write16(pItemWeightLevel, RefitItemWeightTypeCfg[i].MinWeight);		 // MinWeight
			Write16(pItemWeightLevel, RefitItemWeightTypeCfg[i].MaxWeight);		 // MaxWeight
			Write16(pItemWeightLevel, RefitItemWeightTypeCfg[i].ExpAward);		 // ExpAward

			len = pItemWeightLevel - pRefitItemWeightTypeCfg;
			Set16(pRefitItemWeightTypeCfg, (WORD)len);
			pRefitItemWeightTypeCfg += len;
		}

		len = pRefitItemWeightTypeCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // RefitItemCfg
		BYTE *pRefitItemCfg = p;
		Write16(pRefitItemCfg, 0); // len

		Write16(pRefitItemCfg, sizeof(RefitItemCfg) / sizeof(RefitItemCfg[0])); // ItemNum
		for (size_t i = 0; i < sizeof(RefitItemCfg) / sizeof(RefitItemCfg[0]); i++)
		{ // RefitItem
			BYTE *pRefitItem = pRefitItemCfg;
			Write16(pRefitItem, 0); // len

			Write32(pRefitItem, RefitItemCfg[i].ItemID);			   // ItemID
			Write8(pRefitItem, RefitItemCfg[i].ItemType);			   // ItemType
			Write8(pRefitItem, RefitItemCfg[i].ItemWeightLevel);	   // ItemWeightLevel
			Write16(pRefitItem, RefitItemCfg[i].Value);				   // Value
			Write32(pRefitItem, RefitItemCfg[i].FragmentItemID);	   // FragmentItemID
			Write32(pRefitItem, RefitItemCfg[i].FragmentNumToOneItem); // FragmentNumToOneItem
			Write16(pRefitItem, RefitItemCfg[i].IsItemOpen);		   // IsItemOpen

			len = pRefitItem - pRefitItemCfg;
			Set16(pRefitItemCfg, (WORD)len);
			pRefitItemCfg += len;
		}

		len = pRefitItemCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write16(p, 0);

	len = p - buf;
	SendToClient(Client, 21111, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseSecondGetKartRefitLimitCfg(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	int n = 100;
	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	{						 // KartRefitLimitCfg
		BYTE *pKartRefitLimitCfg = p;
		Write16(pKartRefitLimitCfg, 0); // len

		Write16(pKartRefitLimitCfg, n); // OpenKartTypeNum
		// Write32(pKartRefitLimitCfg, 0); //OpenKartTypeList[]
		for (size_t i = 0; i < n; i++)
		{ // OpenKartTypeList
			BYTE *OpenKartTypeList = pKartRefitLimitCfg;
			Write16(OpenKartTypeList, 0);	  // len
			Write32(OpenKartTypeList, 12720); // ItemID

			len = OpenKartTypeList - pKartRefitLimitCfg;
			Set16(pKartRefitLimitCfg, (WORD)len);
			pKartRefitLimitCfg += len;
		}
		Write32(pKartRefitLimitCfg, 0); // MinFormerRefitTime
		Write16(pKartRefitLimitCfg, n); // ExcludeKartTypeNum
		Write32(pKartRefitLimitCfg, 0); // ExcludeKartTypeList[]
		Write16(pKartRefitLimitCfg, n); // ForbidListNum
		Write32(pKartRefitLimitCfg, 0); // ForbidRefitKartID[]

		len = pKartRefitLimitCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // RefitItemWeightTypeCfg
		BYTE *pRefitItemWeightTypeCfg = p;
		Write16(pRefitItemWeightTypeCfg, 0); // len

		Write8(pRefitItemWeightTypeCfg, 0); // TypeNum
		for (size_t i = 0; i < n; i++)
		{ // ItemWeightLevel
			BYTE *pItemWeightLevel = pRefitItemWeightTypeCfg;
			Write16(pItemWeightLevel, 0); // len

			Write8(pItemWeightLevel, 0);  // ItemWeightLevel
			Write16(pItemWeightLevel, 0); // MinWeight
			Write16(pItemWeightLevel, 0); // MaxWeight
			Write16(pItemWeightLevel, 0); // ExpAward

			len = pItemWeightLevel - pRefitItemWeightTypeCfg;
			Set16(pRefitItemWeightTypeCfg, (WORD)len);
			pRefitItemWeightTypeCfg += len;
		}

		len = pRefitItemWeightTypeCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // RefitItemCfg
		BYTE *pRefitItemCfg = p;
		Write16(pRefitItemCfg, 0); // len

		Write16(pRefitItemCfg, n); // ItemNum
		for (size_t i = 0; i < n; i++)
		{ // RefitItem
			BYTE *pRefitItem = pRefitItemCfg;
			Write16(pRefitItem, 0); // len

			Write32(pRefitItem, 12720); // ItemID
			Write8(pRefitItem, 0);		// ItemType
			Write8(pRefitItem, 0);		// ItemWeightLevel
			Write16(pRefitItem, 0);		// Value
			Write32(pRefitItem, 0);		// FragmentItemID
			Write32(pRefitItem, 0);		// FragmentNumToOneItem
			Write16(pRefitItem, 1);		// IsItemOpen

			len = pRefitItem - pRefitItemCfg;
			Set16(pRefitItemCfg, (WORD)len);
			pRefitItemCfg += len;
		}

		len = pRefitItemCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 21111, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseCarLottoryGetInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	// std::string_view szUserToken("MOL");

	Write16(p, 0);			// ResultID
	Write16(p, 0);			// ReasonLen
	Write32(p, Uin);		// Uin
	Write32(p, 1683347967); // BeginTime
	Write32(p, 1693975167); // EndTime
	Write16(p, 30);			// LeftDays
	Write8(p, 1);			// IsSigned   是否已签名
	Write32(p, 500);		// Fortune		财富
	Write32(p, 1);			// TotalSignNum
	Write16(p, 0);			// ActCntLen
	Write8(p, 1);			// TotalSignAwardNum
	for (size_t i = 0; i < 1; i++)
	{ // TotalSign
		BYTE *pTotalSign = p;
		Write16(pTotalSign, 0); // len

		Write32(pTotalSign, 1); // SignNum
		{						// DisplayAward
			BYTE *pDisplayAward = pTotalSign;
			Write16(pDisplayAward, 0); // len

			Write32(pDisplayAward, 0); // Exp
			Write32(pDisplayAward, 0); // Money
			Write32(pDisplayAward, 0); // SuperMoney
			Write16(pDisplayAward, 1); // ItemNum
			for (size_t i = 0; i < 1; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pDisplayAward;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, 119076); // ItemID 119076
				Write32(pItemInfo, 1);		// ItemNum
				Write32(pItemInfo, -1);		// AvailPeriod
				Write8(pItemInfo, 0);		// Status
				Write32(pItemInfo, 0);		// ObtainTime
				Write32(pItemInfo, 0);		// OtherAttribute
				Write16(pItemInfo, 0);		// ItemType

				len = pItemInfo - pDisplayAward;
				Set16(pDisplayAward, (WORD)len);
				pDisplayAward += len;
			}
			Write32(pDisplayAward, 0); // Coupons
			Write32(pDisplayAward, 0); // GuildPoint
			Write32(pDisplayAward, 0); // LuckMoney
			Write8(pDisplayAward, 0);  // ExtendInfoNum
			for (size_t i = 0; i < 0; i++)
			{ // ExtendInfoAward
				BYTE *pExtendInfoAward = pDisplayAward;
				Write16(pExtendInfoAward, 0); // len

				Write16(pExtendInfoAward, 0); // Key
				Write32(pExtendInfoAward, 0); // AddValue
				Write32(pExtendInfoAward, 0); // TotalValue
				Write8(pExtendInfoAward, 0);  // ShowPriority
				Write16(pExtendInfoAward, 0); // BuyNeedScore
				Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

				len = pExtendInfoAward - pDisplayAward;
				Set16(pDisplayAward, (WORD)len);
				pDisplayAward += len;
			}
			Write32(pDisplayAward, 0); // SpeedCoin

			len = pDisplayAward - pTotalSign;
			Set16(pTotalSign, (WORD)len);
			pTotalSign += len;
		}
		Write8(pTotalSign, 0); // AwardState

		len = pTotalSign - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 1); // TurnAwardNum 转向奖励数量
	for (size_t i = 0; i < 1; i++)
	{ // TurnAward 119076
		BYTE *pTurnAward = p;
		Write16(pTurnAward, 0); // len

		Write32(pTurnAward, 100);	 // SuperMoney
		Write32(pTurnAward, 200);	 // Coupons
		Write32(pTurnAward, 300);	 // Fortune
		Write32(pTurnAward, 119075); // ItemID
		Write32(pTurnAward, 1);		 // ItemNum
		Write32(pTurnAward, -1);	 // ItemPeriod
		Write8(pTurnAward, 1);		 // IsBest  是最好的

		len = pTurnAward - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // StageNum
	for (size_t i = 0; i < 0; i++)
	{ // Stage
		BYTE *pStage = p;
		Write16(pStage, 0); // len

		Write32(pStage, 0); // LottoryBeginTime
		Write8(pStage, 0);	// BestAwardNum
		for (size_t i = 0; i < 0; i++)
		{ // BestAward
			BYTE *pBestAward = pStage;
			Write16(pBestAward, 0); // len

			Write32(pBestAward, 0); // ItemID
			Write8(pBestAward, 0);	// NameLen

			len = pBestAward - pStage;
			Set16(pStage, (WORD)len);
			pStage += len;
		}
		Write8(pStage, 0);	// StageState
		Write32(pStage, 0); // LottoryEndTime

		len = pStage - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // ShowBestItemNum
	for (size_t i = 0; i < 0; i++)
	{ // ShowBestItem
		BYTE *pShowBestItem = p;
		Write16(pShowBestItem, 0); // len

		Write32(pShowBestItem, 0); // ItemID
		Write8(pShowBestItem, 0);  // NameLen

		len = pShowBestItem - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // LeftTime
	Write8(p, 0);  // NextStage
	Write16(p, 0); // AwardCntLen
	Write8(p, 0);  // RecordNum
	for (size_t i = 0; i < 0; i++)
	{ // Record
		BYTE *pRecord = p;
		Write16(pRecord, 0); // len

		Write8(pRecord, 0); // RecordLen

		len = pRecord - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // TurnCostID
	Write32(p, 0); // CurrentSCarNum
	Write32(p, 0); // CurrentTCarNum
	Write32(p, 0); // CurrentACarNum

	len = p - buf;
	SendToClient(Client, 24095, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetPrivilegeIntroInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);

	ResponseGetPrivilegeIntroInfo(Client);
}
void ResponseGetPrivilegeIntroInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Uin
	Write32(p, 0); // CharmValueOfMonth
	Write32(p, 0); // VipLevel
	Write16(p, 0); // PrivilegeInfoNums
	/*
	for (size_t i = 0; i < n; i++)
	{ //PrivilegeInfos
		BYTE* pPrivilegeInfos = p;
		Write16(pPrivilegeInfos, 0); //len

		WriteUn(pPrivilegeInfos, 0); //NextLevelCharmVlaue
		WriteUn(pPrivilegeInfos, 0); //SearchTreasureNums
		WriteUn(pPrivilegeInfos, 0); //GetTreasureNums
		WriteUn(pPrivilegeInfos, 0); //CritCardNums
		WriteUn(pPrivilegeInfos, 0); //MechLargerCardNums
		WriteUn(pPrivilegeInfos, 0); //MallBuyLevel
		WriteUn(pPrivilegeInfos, 0); //HasAlreadyReceived
		WriteUn(pPrivilegeInfos, 0); //AwardItemNums
		for (size_t i = 0; i < n; i++)
		{ //FirstReachLevelItemInfos
			BYTE* pFirstReachLevelItemInfos = pPrivilegeInfos;
			Write16(pFirstReachLevelItemInfos, 0); //len

			WriteUn(pFirstReachLevelItemInfos, 0); //itemID
			WriteUn(pFirstReachLevelItemInfos, 0); //itemNums
			Write32(pFirstReachLevelItemInfos, 0); //AvailPeriod
			WriteUn(pFirstReachLevelItemInfos, 0); //isBestQuality

			len = pFirstReachLevelItemInfos - pPrivilegeInfos;
			Set16(pPrivilegeInfos, (WORD)len);
			pPrivilegeInfos += len;
		}

		len = pPrivilegeInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	// WriteUn(p, 0); //AnouncementInfo[]

	len = p - buf;
	SendToClient(Client, 16055, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetItemTimeLimtCfg(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT UpdateTime = Read32(Body);

	ResponsetGetItemTimeLimtCfg(Client, UpdateTime);
}
void ResponsetGetItemTimeLimtCfg(ClientNode *Client, UINT UpdateTime)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID

	Write32(p, 0); // ItemNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //ItemTimeLimit
		BYTE* pItemTimeLimit = p;
		Write16(pItemTimeLimit, 0); //len

		Write32(pItemTimeLimit, 0); //ItemID
		Write32(pItemTimeLimit, 0); //ExpireTime

		len = pItemTimeLimit - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, UpdateTime); // UpdateTime

	len = p - buf;
	SendToClient(Client, 24201, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}


void RequestGetShapeRefitCfgNew(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	const static unsigned char* pMsg317 = EHexToByte("00000022009A0001C1D80BD6C1D7F02DD3F1F7E8F7EB000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E696600000004000A00C800000001F28104000000000000000000000000000000000E74756E69616E7869616E64696E670100120000000000001130000018380000183801001200000309000000B400001A9000000CE400A90001A0B30DD6D5BCABD6DAC9F12DC2C0B2BC0002002F00000300000000000000001F4B6172744176617461724566666563742F4361722F4361724566662E6E6966000000340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000040005006400000001FEDA04000000000000000000000000000000001074687265656B696E67646F6D6865726F000000B10001D11B0FCCD8C2E5D2C12DB1F9CBAAC6EFCABF0002002F00000300000000000000001F4B6172744176617461724566666563742F4361722F4361724566662E6E6966000000340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000040005006400000001FED904000000000000000000000000000000000454524F59010012000017700000000000000000000000000000A200005F5A0BB7E7D6AECAB9D5DF2DC0BC0002002F00000300000000000000001F4B6172744176617461724566666563742F4361722F4361724566662E6E6966000000340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000040005006400000001F90804000000000000000000000000000000000B6D65696C616E7A68756A75000000730001F1CE08C1F9B5C0C4A7D7F0000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E696600000004000F03E800000001F1D104000000000000000000000000000000000E74756E69616E7869616E64696E670000009A0001F84B0BD6C1D7F02DD3F1F7E8F7EB000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E696600000004000A00C800000001F28004000000000000000000000000000000000E74756E69616E7869616E64696E670100120000000000001130000018380000183801001200000309000000B400001A9000000CE4006A0001ED9009B1F9BBEA2DD3A3BAAE000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000010005003200000001E70E0400000000000000000000000000000000045261726500000040000027240DD0C2CAD6C8FCB3B52DBADABEF4000000040002001400000001ED9604000000000000000000000000000000000A31357A686F756E69616E0000007800012DB411D6D5BCABC3CDCADE2DD3C4DAA4C6EFCABF000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000040005006400000001ED9404000000000000000000000000000000000A31357A686F756E69616E0000007500013D8D0DBADAD3F2B5E7C4A72DD0FEDAA4000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000040005006400000001ED9504000000000000000000000000000000000B53694A695A68695368656E00000073000167350BD2F8CCECCAB92DDDEACAD5000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000040005006400000001E70804000000000000000000000000000000000B53694A695A68695368656E0000006E000031B00DC0D7C5B52DB3B5CDF5B6DCEEF8000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000010005003200000001E2720400000000000000000000000000000000045261726500000070000153D6084D31382DD7A3C8DA000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000040005006400000001E26E04000000000000000000000000000000000B53694A695A68695368656E0000007000017FD709D6C1D7F02DBBF0C0F0000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E696600000004000A00C800000001E26D04000000000000000000000000000000000A533133636865776E616700000071000163410DD6DAC9F1D6AEC9F12DC4C4DFB8000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000040005006400000001DD5704000000000000000000000000000000000738367869796F75000000770001D7460DD5F2BAA3D6AEEAAA2DB0BDB1FB000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000040005006400000001DD5804000000000000000000000000000000000D7869796F757869616E64696E67000000750001A0B10DD6C7BBDBD6AEC9F12DBEE4C3A2000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000040005006400000001D75104000000000000000000000000000000000B53694A695A68695368656E0000003A0001820E0DB1ACCCECC0D7C5B52DBADABDF0000000010005006400000001D118040000000000000000000000000000000004526172650000003A0001820E0DB1ACCCECC0D7C5B52DE7B3BAEC000000010005006400000001D119040000000000000000000000000000000004526172650000003A0001820E0DB1ACCCECC0D7C5B52DB1F9C0B6000000010005006400000001D11A0400000000000000000000000000000000045261726500000069000163410DD6DAC9F1D6AEC9F12DD0C7D4C60001002F00000200000000000000001F4B6172744176617461724566666563742F4361722F4361724566662E6E6966000000010005006400000001CCB504000000000000000000000000000000000452617265000000A30001D11309D6C1EAD72DD2ABBBA20002002F00000300000000000000001F4B6172744176617461724566666563742F4361722F4361724566662E6E6966000000340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E696600000004000A00C800000001D11704000000000000000000000000000000000E68756E69616E7869616E64696E6700000078000128200DD5EBBCE2CDF5D5DF2DC2EDB3AC000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E696600000004000A006400000001D11604000000000000000000000000000000000E577548755368616E674A69616E67000000760001119F0BC2F3BFADC2D72DD5D4D4C6000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E696600000004000A006400000001CCAE04000000000000000000000000000000000E577548755368616E674A69616E67000000760001227A0BB1ACCCECBCD72DBBC6D6D2000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E696600000004000A006400000001C7FF04000000000000000000000000000000000E577548755368616E674A69616E670000006E000044FD0DBADAD2B9B4ABCBB52DEAD7D3B0000100340001020000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000010005003200000001C804040000000000000000000000000000000004526172650000007800013D8D0DBADAD3F2B5E7C4A72DD5C5B7C9000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E696600000004000A006400000001C1D704000000000000000000000000000000000E577548755368616E674A69616E67000000790001A0B112D6C7BBDBD6AEC9F12D6B69747479CFDEB6A8000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000040005006400000001BB0704000000000000000000000000000000000A48656C6C6F4B697474790000007B000035871053C7E0C1FAD9C8D4C2B5B62DB9D8D3F0000100340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E696600000004000A006400000001BB0504000000000000000000000000000000000E577548755368616E674A69616E67000000700001B6370FC9E4CAD6D7F92DB0ACB6EDC2DECBB9000100340001020000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E6966000000010005003200000001BB060400000000000000000000000000000000045261726500000066000175C30AB0B5BADAB2A8C8FBB6AC0001002F00000200000000000000001F4B6172744176617461724566666563742F4361722F4361724566662E6E6966000000010005003200000001B63204000000000000000000000000000000000452617265000000670001227A0BB1ACCCECBCD72DC5F3BFCB0001002F00000200000000000000001F4B6172744176617461724566666563742F4361722F4361724566662E6E6966000000010005006400000001B07504000000000000000000000000000000000452617265000000A40001ABFB09D6C1D7F02DB1F9B7EF0002002F00000300000000000000001F4B6172744176617461724566666563742F4361722F4361724566662E6E6966000000340001030000000000000000244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E696600000004000A00C800000001B07604000000000000000000000000000000000F6E69756E69616E7869616E64696E6700000069000163410DD6DAC9F1D6AEC9F12DF7E8F7EB0001002F00000200000000000000001F4B6172744176617461724566666563742F4361722F4361724566662E6E6966000000010005006400000001B077040000000000000000000000000000000004526172650000000504050001227A09B1ACCCEC2DEAD7D3B00003064C6567656E640A00200100000002000A00000001CFC701000E0001C81700000003000000000000002E0200000002000A00000001D5B702000E0001C8170000000300000000000E0001C81900000064000000000000002E0300000002000A00000001D5B802000E0001C8170000000300000000000E0001C819000000C8000000000000002E0400000005001400000001CFC802000E0001C8170000000600000000000E0001C8190000012C000000000000008C050001005E0000020000000000000000274176617461722F6B6172745F6E65772F3131383732382F4D4F44454C2F7A697368656E2E6E6966274176617461722F6B6172745F6E65772F3131383732382F4D4F44454C2F7A697368656E2E6E6966010002001400000001CFC802000E0001C8170000000600000000000E0001C8190000019000000000000000AE06000100800001020000000000000000384176617461722F6B6172745F6E65772F3131383732382F4D4F44454C2F6368657169616E6775616E677A68616F4855414E4773652E6E6966384176617461722F6B6172745F6E65772F3131383732382F4D4F44454C2F6368657169616E6775616E677A68616F4855414E4773652E6E6966010002001400000001CFC802000E0001C8170000000600000000000E0001C819000001F4000000000000002E0700000005003200000001CFC902000E0001C8170000000F00000000000E0001C81900000258000000000000008C080001005E0000030000000000000000274176617461722F6B6172745F6E65772F3131383732392F4D4F44454C2F7A697368656E2E6E6966274176617461722F6B6172745F6E65772F3131383732392F4D4F44454C2F7A697368656E2E6E6966010002003200000001CFC902000E0001C8170000000F00000000000E0001C819000002BC00000000000000AE09000100800001030000000000000000384176617461722F6B6172745F6E65772F3131383732392F4D4F44454C2F6368657169616E6775616E677A68616F4855414E4773652E6E6966384176617461722F6B6172745F6E65772F3131383732392F4D4F44454C2F6368657169616E6775616E677A68616F4855414E4773652E6E6966010002003200000001CFC902000E0001C8170000000F00000000000E0001C81900000320000000000000008C0A0001005E00040400000FA1000006D6244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E69662A4176617461722F6B6172745F6E65772F3131383732392F4D4F44454C2F737461727473686F772E6E6966010005006E00000001CFC902000E0001C8170000001E00000000000E0001C819000005780000000000000001D5B6000E0001DAF900000001000000000001A20001E2780DCEF2BFD52DC6EBCCECB4F3CAA500030738367869796F750500200100000002001400000001E16501000E0001E6B4000003E800000000000000200200000002001400000001E16501000E0001E6B4000007D000000000000000200300000002001400000001E16501000E0001E6B400000BB80000000000000088040001006800010300000000000000002C4176617461722F6B6172745F6E65772F3132333233372F4D4F44454C2F646170656E74657869616F2E6E69662C4176617461722F6B6172745F6E65772F3132333233372F4D4F44454C2F646170656E74657869616F2E6E6966010003002800000001E16501000E0001E6B400000FA00000000000000088050001006800010300000000000000002C4176617461722F6B6172745F6E65772F3132333233382F4D4F44454C2F646170656E74657869616F2E6E69662C4176617461722F6B6172745F6E65772F3132333233382F4D4F44454C2F646170656E74657869616F2E6E6966010005003C00000001E16601000E0001E6B4000013880000000000000001E279000E0001E6B500000001000000000103D5000031B009C0D7C5B52DBBC3D2B90003064C6567656E640A00200100000002000A00000001EE4501000E0001EE4A00000003000000000000002E0200000002000A00000001EE4602000E0001EE4A0000000300000000000E0001C81900000064000000000000002E0300000002000A00000001EE4702000E0001EE4A0000000300000000000E0001C819000000C8000000000000002E0400000005001400000001EE4802000E0001EE4A0000000A00000000000E0001C8190000012C000000000000008C050001005E0000020000000000000000274176617461722F6B6172745F6E65772F3132363533362F4D4F44454C2F7A697368656E2E6E6966274176617461722F6B6172745F6E65772F3132363533362F4D4F44454C2F7A697368656E2E6E6966010002001400000001EE4802000E0001EE4A0000000A00000000000E0001C819000001900000000000000096060001006800010200000000000000002C4176617461722F6B6172745F6E65772F3132363533362F4D4F44454C2F646170656E74657869616F2E6E69662C4176617461722F6B6172745F6E65772F3132363533362F4D4F44454C2F646170656E74657869616F2E6E6966010002001400000001EE4802000E0001EE4A0000000A00000000000E0001C819000001F4000000000000002E0700000005003200000001EE4902000E0001EE4A0000001400000000000E0001C81900000258000000000000008C080001005E0000030000000000000000274176617461722F6B6172745F6E65772F3132363533372F4D4F44454C2F7A697368656E2E6E6966274176617461722F6B6172745F6E65772F3132363533372F4D4F44454C2F7A697368656E2E6E6966010002003200000001EE4902000E0001EE4A0000001400000000000E0001C819000002BC0000000000000096090001006800010300000000000000002C4176617461722F6B6172745F6E65772F3132363533372F4D4F44454C2F646170656E74657869616F2E6E69662C4176617461722F6B6172745F6E65772F3132363533372F4D4F44454C2F646170656E74657869616F2E6E6966010002003200000001EE4902000E0001EE4A0000001400000000000E0001C81900000320000000000000008C0A0001005E00040400000FA1000006D6244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E69662A4176617461722F6B6172745F6E65772F3132363533372F4D4F44454C2F737461727473686F772E6E6966010005006E00000001EE4902000E0001EE4A0000002300000000000E0001C819000005780000000000000001F17F000E0001F17500000001000000000003D50001634109D6DAC9F12DC7ACC0A40003064C6567656E640A00200100000002000A00000001F18101000E0001F17600000003000000000000002E0200000002000A00000001F18202000E0001F1760000000300000000000E0001C81900000064000000000000002E0300000002000A00000001F18302000E0001F1760000000300000000000E0001C819000000C8000000000000002E0400000005001400000001F18402000E0001F1760000000600000000000E0001C8190000012C000000000000008C050001005E0000020000000000000000274176617461722F6B6172745F6E65772F3132373336342F4D4F44454C2F7A697368656E2E6E6966274176617461722F6B6172745F6E65772F3132373336342F4D4F44454C2F7A697368656E2E6E6966010002001400000001F18402000E0001F1760000000600000000000E0001C819000001900000000000000096060001006800010200000000000000002C4176617461722F6B6172745F6E65772F3132373336342F4D4F44454C2F646170656E74657869616F2E6E69662C4176617461722F6B6172745F6E65772F3132373336342F4D4F44454C2F646170656E74657869616F2E6E6966010002001400000001F18402000E0001F1760000000600000000000E0001C819000001F4000000000000002E0700000005003200000001F18502000E0001F1760000000F00000000000E0001C81900000258000000000000008C080001005E0000030000000000000000274176617461722F6B6172745F6E65772F3132373336352F4D4F44454C2F7A697368656E2E6E6966274176617461722F6B6172745F6E65772F3132373336352F4D4F44454C2F7A697368656E2E6E6966010002003200000001F18502000E0001F1760000000F00000000000E0001C819000002BC0000000000000096090001006800010300000000000000002C4176617461722F6B6172745F6E65772F3132373336352F4D4F44454C2F646170656E74657869616F2E6E69662C4176617461722F6B6172745F6E65772F3132373336352F4D4F44454C2F646170656E74657869616F2E6E6966010002003200000001F18502000E0001F1760000000F00000000000E0001C81900000320000000000000008C0A0001005E00040400000FA1000006D6244B6172744176617461724566666563742F53757065724A65742F4A65744566662E6E69662A4176617461722F6B6172745F6E65772F3132373336352F4D4F44454C2F737461727473686F772E6E6966010005006E00000001F18502000E0001F1760000001E00000000000E0001C819000005780000000000000001F180000E0001F17800000001000000000001AA0001ED890DD4B4BCAB2DB4F3BAD3C9F1CEC000040F6875616E6768657869616E64696E670500200100000002001400000001F1CC01000E0001F875000003E800000000000000200200000002001400000001F1CC01000E0001F875000007D000000000000000200300000002001400000001F1CC01000E0001F87500000BB80000000000000088040001006800010300000000000000002C4176617461722F6B6172745F6E65772F3132373433362F4D4F44454C2F646170656E74657869616F2E6E69662C4176617461722F6B6172745F6E65772F3132373433362F4D4F44454C2F646170656E74657869616F2E6E6966010003002800000001F1CC01000E0001F87500000FA00000000000000088050001006800010300000000000000002C4176617461722F6B6172745F6E65772F3132373433372F4D4F44454C2F646170656E74657869616F2E6E69662C4176617461722F6B6172745F6E65772F3132373433372F4D4F44454C2F646170656E74657869616F2E6E6966010005003C00000001F1CD01000E0001F875000013880000000000000001ED93000E0001F8760000000100000000010000");
	SendToClient(Client, 317, pMsg317, 7837, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	return;
}
void RequestGetShapeRefitCfg(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseGetShapeRefitCfgTest(Client);
}
void ResponseGetShapeRefitCfg(ClientNode *Client)
{
	int n = 1;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	YAML::Node ShapeRefitCfg = YAML::LoadFile(".\\Config\\ShapeRefitCfg.yml");
	Write16(p, 0); // ResultID
	BYTE *pKartShapeRefitSuitNum = p;
	// size_t i = 1;
	Write16(p, ShapeRefitCfg["ShapeRefitLefend"].size()); // KartShapeRefitSuitNum
	for (size_t i = 0; i < ShapeRefitCfg["ShapeRefitLefend"].size(); i++)
	{ // ShapeRefitSuit

		BYTE *pShapeRefitSuit = p;
		Write16(pShapeRefitSuit, 0); // len

		Write32(pShapeRefitSuit, ShapeRefitCfg["ShapeRefitLefend"][i]["KartID"].as<int>()); // KartID
		Write8(pShapeRefitSuit, 0);															// KarNameLen
		Write8(pShapeRefitSuit, 1);															// RefitItemNum
		Write32(pShapeRefitSuit, 0);														// RefitItem[]
		INT EffectNum = 1;
		Write8(pShapeRefitSuit, EffectNum); // EffectNum
		for (size_t i = 0; i < EffectNum; i++)
		{ // RefitEffects
			BYTE *pRefitEffects = pShapeRefitSuit;
			Write16(pRefitEffects, 0); // len

			Write16(pRefitEffects, 1); // EffectID
			Write8(pRefitEffects, 3);  // Level
			Write32(pRefitEffects, 0); // Value1
			Write32(pRefitEffects, 0); // Value2

			Write8(pRefitEffects, 36);										   // ResourceLen
			memcpy(pRefitEffects, "KartAvatarEffect/SuperJet/JetEff.nif", 36); // Resource 资源
			pRefitEffects += 36;

			Write8(pRefitEffects, 0); // EffectResLen
			Write8(pRefitEffects, 4); // Grade

			len = pRefitEffects - pShapeRefitSuit;
			Set16(pShapeRefitSuit, (WORD)len);
			pShapeRefitSuit += len;
		}

		Write16(pShapeRefitSuit, 4);   // Grade
		Write16(pShapeRefitSuit, 10);  // DressPoint
		Write16(pShapeRefitSuit, 200); // CarLibraryCollectValue
		INT RefitModifyNum = 1;
		Write8(pShapeRefitSuit, RefitModifyNum); // RefitModifyNum
		for (size_t i = 0; i < RefitModifyNum; i++)
		{ // RefitModify
			BYTE *pRefitModify = pShapeRefitSuit;
			Write16(pRefitModify, 0); // len

			Write16(pRefitModify, 1); // RefitID
			Write8(pRefitModify, 0);  // RefitModify
			Write32(pRefitModify, 0); // RefitLimitInc
			Write32(pRefitModify, 0); // RefitInc

			len = pRefitModify - pShapeRefitSuit;
			Set16(pShapeRefitSuit, (WORD)len);
			pShapeRefitSuit += len;
		}
		INT HasTurnModify = 0;
		Write8(pShapeRefitSuit, HasTurnModify); // HasTurnModify
		for (size_t i = 0; i < HasTurnModify; i++)
		{ // TurnModify
			BYTE *pTurnModify = pShapeRefitSuit;
			Write16(pTurnModify, 0); // len

			Write32(pTurnModify, 0); // BaseTurnRate
			Write32(pTurnModify, 0); // VolatileTurnRate
			Write32(pTurnModify, 0); // MaxTurnSpeed
			Write32(pTurnModify, 0); // MinTurnSpeed
			Write32(pTurnModify, 0); // MaxAccuTime
			Write32(pTurnModify, 0); // BaseAccuRate
			Write32(pTurnModify, 0); // MaxAffectSpeed

			len = pTurnModify - pShapeRefitSuit;
			Set16(pShapeRefitSuit, (WORD)len);
			pShapeRefitSuit += len;
		}

		// int ShapeSuitID = 127616;
		Write32(pShapeRefitSuit, ShapeRefitCfg["ShapeRefitLefend"][i]["ShapeSuitID"].as<int>()); // ShapeSuitID
		Write8(pShapeRefitSuit, 4);																 // RefitAddtionNum
		Write32(pShapeRefitSuit, 0);															 // RefitAddition[]
		Write32(pShapeRefitSuit, 0);															 // RefitAddition[]
		Write32(pShapeRefitSuit, 0);															 // RefitAddition[]
		Write32(pShapeRefitSuit, 0);															 // RefitAddition[]
		if (ShapeRefitCfg["ShapeRefitLefend"][i]["GradeResourceLen"])
		{
			std::string GradeResource = ShapeRefitCfg["ShapeRefitLefend"][i]["GradeResourceLen"].as<std::string>();
			UINT GradeResourceLen = strlen(GradeResource.c_str());
			Write8(pShapeRefitSuit, GradeResourceLen);						  // GradeResourceLen
			memcpy(pShapeRefitSuit, GradeResource.c_str(), GradeResourceLen); // Resource 资源 兔年限定
			pShapeRefitSuit += GradeResourceLen;
		}
		else
		{
			Write8(pShapeRefitSuit, 0); // GradeResourceLen
		}

		len = pShapeRefitSuit - p;
		Set16(p, (WORD)len);
		p += len;
	}
	// Set16(pKartShapeRefitSuitNum, (WORD)i);

	// Write16(p, 1); //KartShapeRefitLegendNum

	Write16(p, ShapeRefitCfg["ShapeRefitLegend"].size()); // KartShapeRefitLegendNum
	for (size_t i = 0; i < ShapeRefitCfg["ShapeRefitLegend"].size(); i++)
	{ // ShapeRefitLegend
		BYTE *pShapeRefitLegend = p;
		Write16(pShapeRefitLegend, 0); // len

		Write32(pShapeRefitLegend, ShapeRefitCfg["ShapeRefitLegend"][i]["KartID"].as<int>()); // KartID
		Write8(pShapeRefitLegend, 0);														  // KarNameLen
		Write16(pShapeRefitLegend, 0);														  // Grade
		if (ShapeRefitCfg["ShapeRefitLegend"][i]["GradeResourceLen"])
		{
			std::string GradeResource = ShapeRefitCfg["ShapeRefitLegend"][i]["GradeResourceLen"].as<std::string>();
			UINT GradeResourceLen = strlen(GradeResource.c_str());
			Write8(pShapeRefitLegend, GradeResourceLen);						// GradeResourceLen
			memcpy(pShapeRefitLegend, GradeResource.c_str(), GradeResourceLen); // Resource 资源 兔年限定
			pShapeRefitLegend += GradeResourceLen;
		}
		else
		{
			Write8(pShapeRefitLegend, 0); // GradeResourceLen
		}

		Write8(pShapeRefitLegend, 0); // LevelNum
		UINT ShapeSuitID = ShapeRefitCfg["ShapeRefitLegend"][i]["ShapeSuitID"].as<int>();
		for (size_t i = 0; i < 0; i++)
		{ // LevelCfg
			BYTE *pLevelCfg = pShapeRefitLegend;
			Write16(pLevelCfg, 0); // len

			Write8(pLevelCfg, 0);  // Level
			Write8(pLevelCfg, 0);  // RefitItemNum
			Write32(pLevelCfg, 0); // RefitItem[]
			Write8(pLevelCfg, 0);  // EffectNum
			for (size_t i = 0; i < 0; i++)
			{ // RefitEffects
				BYTE *pRefitEffects = pLevelCfg;
				Write16(pRefitEffects, 0); // len

				Write16(pRefitEffects, 0); // EffectID
				Write8(pRefitEffects, 0);  // Level
				Write32(pRefitEffects, 0); // Value1
				Write32(pRefitEffects, 0); // Value2
				Write8(pRefitEffects, 0);  // ResourceLen
				Write8(pRefitEffects, 0);  // EffectResLen
				Write8(pRefitEffects, 0);  // Grade

				len = pRefitEffects - pLevelCfg;
				Set16(pLevelCfg, (WORD)len);
				pLevelCfg += len;
			}
			Write16(pLevelCfg, 0);	 // DressPoint
			Write16(pLevelCfg, 400); // CarLibraryCollectValue
			Write8(pLevelCfg, 0);	 // RefitModifyNum
			for (size_t i = 0; i < 0; i++)
			{ // RefitModify
				BYTE *pRefitModify = pLevelCfg;
				Write16(pRefitModify, 0); // len

				Write16(pRefitModify, 0); // RefitID
				Write8(pRefitModify, 0);  // RefitModify
				Write32(pRefitModify, 0); // RefitLimitInc
				Write32(pRefitModify, 0); // RefitInc

				len = pRefitModify - pLevelCfg;
				Set16(pLevelCfg, (WORD)len);
				pLevelCfg += len;
			}
			Write8(pLevelCfg, 0); // HasTurnModify
			for (size_t i = 0; i < 0; i++)
			{ // TurnModify
				BYTE *pTurnModify = pLevelCfg;
				Write16(pTurnModify, 0); // len

				Write32(pTurnModify, 0); // BaseTurnRate
				Write32(pTurnModify, 0); // VolatileTurnRate
				Write32(pTurnModify, 0); // MaxTurnSpeed
				Write32(pTurnModify, 0); // MinTurnSpeed
				Write32(pTurnModify, 0); // MaxAccuTime
				Write32(pTurnModify, 0); // BaseAccuRate
				Write32(pTurnModify, 0); // MaxAffectSpeed

				len = pTurnModify - pLevelCfg;
				Set16(pLevelCfg, (WORD)len);
				pLevelCfg += len;
			}
			Write32(pLevelCfg, 0); // ShapeSuitID
			Write8(pLevelCfg, 0);  // ConsumeNum
			for (size_t i = 0; i < 0; i++)
			{ // ConsumInfo
				BYTE *pConsumInfo = pLevelCfg;
				Write16(pConsumInfo, 0); // len

				Write32(pConsumInfo, 0); // ItemID
				Write32(pConsumInfo, 0); // ItemNum
				Write32(pConsumInfo, 0); // ItemPeriod

				len = pConsumInfo - pLevelCfg;
				Set16(pLevelCfg, (WORD)len);
				pLevelCfg += len;
			}

			len = pLevelCfg - pShapeRefitLegend;
			Set16(pShapeRefitLegend, (WORD)len);
			pShapeRefitLegend += len;
		}
		Write32(pShapeRefitLegend, ShapeSuitID); // ShapeSuitID
		{										 // MaxConsumInfo
			BYTE *pMaxConsumInfo = pShapeRefitLegend;
			Write16(pMaxConsumInfo, 0); // len

			Write32(pMaxConsumInfo, 0); // ItemID
			Write32(pMaxConsumInfo, 0); // ItemNum
			Write32(pMaxConsumInfo, 0); // ItemPeriod

			len = pMaxConsumInfo - pShapeRefitLegend;
			Set16(pShapeRefitLegend, (WORD)len);
			pShapeRefitLegend += len;
		}
		Write8(pShapeRefitLegend, 0); // LegendSkinType

		len = pShapeRefitLegend - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 317, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseGetShapeRefitCfgTest(ClientNode *Client)
{
	int n = 1;
	BYTE buf[8192 * 2];
	BYTE *p = buf;
	size_t len;
	YAML::Node ShapeRefitCfg = YAML::LoadFile(".\\Config\\ShapeRefitCfg.yml");
	Write16(p, 0); // ResultID
	BYTE *pKartShapeRefitSuitNum = p;
	// size_t i = 1;
	Write16(p, ShapeRefitCfg["ShapeRefitLefend"].size()); // KartShapeRefitSuitNum
	for (size_t i = 0; i < ShapeRefitCfg["ShapeRefitLefend"].size(); i++)
	{ // ShapeRefitSuit

		BYTE *pShapeRefitSuit = p;
		Write16(pShapeRefitSuit, 0); // len

		Write32(pShapeRefitSuit, ShapeRefitCfg["ShapeRefitLefend"][i]["KartID"].as<int>()); // KartID
		if (ShapeRefitCfg["ShapeRefitLefend"][i]["KarNameLen"])
		{
			std::string GradeResource = ShapeRefitCfg["ShapeRefitLefend"][i]["KarNameLen"].as<std::string>();
			UINT GradeResourceLen = strlen(GradeResource.c_str());
			Write8(pShapeRefitSuit, GradeResourceLen);						  // KarNameLen
			memcpy(pShapeRefitSuit, GradeResource.c_str(), GradeResourceLen); // Resource 资源 兔年限定
			pShapeRefitSuit += GradeResourceLen;
		}
		else
		{
			Write8(pShapeRefitSuit, strlen("测试"));		 // KarNameLen
			memcpy(pShapeRefitSuit, "测试", strlen("测试")); // Resource 资源 兔年限定
			pShapeRefitSuit += strlen("测试");
		}
		Write8(pShapeRefitSuit, 1);	 // RefitItemNum
		Write32(pShapeRefitSuit, 0); // RefitItem[]
		INT EffectNum = 0;
		Write8(pShapeRefitSuit, EffectNum); // EffectNum
		for (size_t i = 0; i < EffectNum; i++)
		{ // RefitEffects
			BYTE *pRefitEffects = pShapeRefitSuit;
			Write16(pRefitEffects, 0); // len

			Write16(pRefitEffects, 1); // EffectID
			Write8(pRefitEffects, 3);  // Level
			Write32(pRefitEffects, 0); // Value1
			Write32(pRefitEffects, 0); // Value2

			Write8(pRefitEffects, 36);										   // ResourceLen
			memcpy(pRefitEffects, "KartAvatarEffect/SuperJet/JetEff.nif", 36); // Resource 资源
			pRefitEffects += 36;

			Write8(pRefitEffects, 0); // EffectResLen
			Write8(pRefitEffects, 4); // Grade

			len = pRefitEffects - pShapeRefitSuit;
			Set16(pShapeRefitSuit, (WORD)len);
			pShapeRefitSuit += len;
		}
		if (ShapeRefitCfg["ShapeRefitLefend"][i]["Grade"])
		{
			Write16(pShapeRefitSuit, ShapeRefitCfg["ShapeRefitLefend"][i]["Grade"].as<UINT>()); // Grade
		}
		else
		{
			Write16(pShapeRefitSuit, 0); // Grade
		}

		Write16(pShapeRefitSuit, 10);  // DressPoint
		Write16(pShapeRefitSuit, 200); // CarLibraryCollectValue
		INT RefitModifyNum = 0;
		Write8(pShapeRefitSuit, RefitModifyNum); // RefitModifyNum
		for (size_t i = 0; i < RefitModifyNum; i++)
		{ // RefitModify
			BYTE *pRefitModify = pShapeRefitSuit;
			Write16(pRefitModify, 0); // len

			Write16(pRefitModify, 1); // RefitID
			Write8(pRefitModify, 0);  // RefitModify
			Write32(pRefitModify, 0); // RefitLimitInc
			Write32(pRefitModify, 0); // RefitInc

			len = pRefitModify - pShapeRefitSuit;
			Set16(pShapeRefitSuit, (WORD)len);
			pShapeRefitSuit += len;
		}
		INT HasTurnModify = 0;
		Write8(pShapeRefitSuit, HasTurnModify); // HasTurnModify
		for (size_t i = 0; i < HasTurnModify; i++)
		{ // TurnModify
			BYTE *pTurnModify = pShapeRefitSuit;
			Write16(pTurnModify, 0); // len

			Write32(pTurnModify, 0); // BaseTurnRate
			Write32(pTurnModify, 0); // VolatileTurnRate
			Write32(pTurnModify, 0); // MaxTurnSpeed
			Write32(pTurnModify, 0); // MinTurnSpeed
			Write32(pTurnModify, 0); // MaxAccuTime
			Write32(pTurnModify, 0); // BaseAccuRate
			Write32(pTurnModify, 0); // MaxAffectSpeed

			len = pTurnModify - pShapeRefitSuit;
			Set16(pShapeRefitSuit, (WORD)len);
			pShapeRefitSuit += len;
		}

		// int ShapeSuitID = 127616;
		Write32(pShapeRefitSuit, ShapeRefitCfg["ShapeRefitLefend"][i]["ShapeSuitID"].as<int>()); // ShapeSuitID
		Write8(pShapeRefitSuit, 4);																 // RefitAddtionNum
		Write32(pShapeRefitSuit, 0);															 // RefitAddition[]
		Write32(pShapeRefitSuit, 0);															 // RefitAddition[]
		Write32(pShapeRefitSuit, 0);															 // RefitAddition[]
		Write32(pShapeRefitSuit, 0);															 // RefitAddition[]
		if (ShapeRefitCfg["ShapeRefitLefend"][i]["GradeResourceLen"])
		{
			std::string GradeResource = ShapeRefitCfg["ShapeRefitLefend"][i]["GradeResourceLen"].as<std::string>();
			UINT GradeResourceLen = strlen(GradeResource.c_str());
			Write8(pShapeRefitSuit, GradeResourceLen);						  // GradeResourceLen
			memcpy(pShapeRefitSuit, GradeResource.c_str(), GradeResourceLen); // Resource 资源 兔年限定
			pShapeRefitSuit += GradeResourceLen;
		}
		else
		{
			std::string GradeResource = "Rare";
			UINT GradeResourceLen = strlen(GradeResource.c_str());
			Write8(pShapeRefitSuit, GradeResourceLen);						  // GradeResourceLen
			memcpy(pShapeRefitSuit, GradeResource.c_str(), GradeResourceLen); // Resource 资源 兔年限定
			pShapeRefitSuit += GradeResourceLen;
		}

		len = pShapeRefitSuit - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Z_INFO("赛车皮肤加载完成[%d]!\n", ShapeRefitCfg["ShapeRefitLefend"].size());

	std::vector<KartShapeRefitLegendInfo> KartShapeRefitLegendInfoCfg;

	KartShapeRefitLegendInfo KartShapeRefitLegend{0};  // 爆天甲曜影
	KartShapeRefitLegendInfo KartShapeRefitLegend1{0}; // 雷诺幻夜
	KartShapeRefitLegendInfo KartShapeRefitLegend2{0}; // 众神乾坤
	KartShapeRefitLegendInfo KartShapeRefitLegend3{0}; // 齐天大圣
	KartShapeRefitLegendInfo KartShapeRefitLegend4{0}; // 源极
	KartShapeRefitLegendInfo KartShapeRefitLegend5{0}; // 机甲齐天大圣
	KartShapeRefitLegendInfo KartShapeRefitLegend6{0}; // 源极
	{												   // 爆天甲曜影
		KartShapeRefitLegend.KartID = 74362;		   // 爆天甲
		KartShapeRefitLegend.Grade = 3;
		KartShapeRefitLegend.KarName = "爆天-曜影";
		KartShapeRefitLegend.GradeResource = "Legend";

		// Level 1
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 1;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 118727; // 爆天·曜影-黑白	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 116759; // 曜影蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 2
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 2;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 120247; // 爆天-曜影-黑红	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 116759; // 曜影蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 116761; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 100;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
			KartShapeRefitLegendLevelInfo.ConsumInfo.clear();
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.clear();
		}

		// Level 3
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 3;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 120248; // 爆天-曜影-蓝绿	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 116759; // 曜影蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 116761; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 200;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 4
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 4;
			KartShapeRefitLegendLevelInfo.DressPoint = 5;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 20;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 118728; // 爆天·曜影-中阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 116759; // 曜影蓝图	功能道具
			ConsumInfo.ItemNum = 6;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 116761; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 300;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
			KartShapeRefitLegendLevelInfo.ConsumInfo.clear();
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.clear();
		}

		// Level 5
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 5;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 0;
			RefitEffectsCfg.Level = 2;
			RefitEffectsCfg.Value1 = 0;
			RefitEffectsCfg.Value2 = 0;
			RefitEffectsCfg.Resource = "Avatar/kart_new/118728/MODEL/zishen.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/118728/MODEL/zishen.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 20;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 118728; // 爆天·曜影-中阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 116759; // 曜影蓝图	功能道具
			ConsumInfo.ItemNum = 6;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 116761; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 400;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 6
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 6;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 1;
			RefitEffectsCfg.Level = 2;
			RefitEffectsCfg.Value1 = 0;
			RefitEffectsCfg.Value2 = 0;
			RefitEffectsCfg.Resource = "Avatar/kart_new/118728/MODEL/cheqianguangzhaoHUANGse.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/118728/MODEL/cheqianguangzhaoHUANGse.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 20;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 118728; // 爆天·曜影-中阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 116759; // 曜影蓝图	功能道具
			ConsumInfo.ItemNum = 6;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 116761; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 500;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 7
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 7;
			KartShapeRefitLegendLevelInfo.DressPoint = 5;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 50;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 118729; // 爆天·曜影-高阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 116759; // 曜影蓝图	功能道具
			ConsumInfo.ItemNum = 15;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 116761; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 600;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 8
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 8;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 0;
			RefitEffectsCfg.Level = 3;
			RefitEffectsCfg.Value1 = 0;
			RefitEffectsCfg.Value2 = 0;
			RefitEffectsCfg.Resource = "Avatar/kart_new/118729/MODEL/zishen.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/118729/MODEL/zishen.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 50;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 118729; // 爆天·曜影-高阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 116759; // 曜影蓝图	功能道具
			ConsumInfo.ItemNum = 15;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 116761; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 700;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 9
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 9;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 1;
			RefitEffectsCfg.Level = 3;
			RefitEffectsCfg.Value1 = 0;
			RefitEffectsCfg.Value2 = 0;
			RefitEffectsCfg.Resource = "Avatar/kart_new/118729/MODEL/cheqianguangzhaoHUANGse.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/118729/MODEL/cheqianguangzhaoHUANGse.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 50;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 118729; // 爆天·曜影-高阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 116759; // 曜影蓝图	功能道具
			ConsumInfo.ItemNum = 15;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 116761; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 800;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 10
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 10;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 4;
			RefitEffectsCfg.Level = 4;
			RefitEffectsCfg.Value1 = 4001;
			RefitEffectsCfg.Value2 = 1750;
			RefitEffectsCfg.Resource = "KartAvatarEffect/SuperJet/JetEff.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/118729/MODEL/startshow.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 5;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 110;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 118729; // 爆天·曜影-高阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 116759; // 曜影蓝图	功能道具
			ConsumInfo.ItemNum = 30;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 116761; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 1400;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}
		KartShapeRefitLegend.ShapeSuitID = 120246; // 爆天-曜影	赛车皮肤

		// MaxConsumInfo
		// KartShapeRefitLegend.MaxConsumInfo = new KartShapeRefitLegendMaxConsumCfg;
		KartShapeRefitLegend.MaxConsumInfo.ItemID = 121593; // 曜影结晶-直升10级	宝 箱
		KartShapeRefitLegend.MaxConsumInfo.ItemNum = 1;
		KartShapeRefitLegend.MaxConsumInfo.ItemPeriod = 0;

		KartShapeRefitLegend.LegendSkinType = 0;
	}

	{										  // 雷诺幻夜
		KartShapeRefitLegend1.KartID = 12720; // 雷诺
		KartShapeRefitLegend1.Grade = 3;
		KartShapeRefitLegend1.KarName = "雷诺-幻夜";
		KartShapeRefitLegend1.GradeResource = "Legend";

		// Level 1
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 1;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 126533; // 雷诺·幻夜-天青	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend1.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 2
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 2;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 126534; // 雷诺·幻夜-日出	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 126540; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 100;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend1.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
			KartShapeRefitLegendLevelInfo.ConsumInfo.clear();
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.clear();
		}

		// Level 3
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 3;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 126535; // 雷诺·幻夜-童梦	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 126540; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 200;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend1.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 4
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 4;
			KartShapeRefitLegendLevelInfo.DressPoint = 5;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 20;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 126536; // 雷诺·幻夜-中阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 6;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 126540; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 300;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend1.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
			KartShapeRefitLegendLevelInfo.ConsumInfo.clear();
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.clear();
		}

		// Level 5
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 5;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 0;
			RefitEffectsCfg.Level = 2;
			RefitEffectsCfg.Value1 = 0;
			RefitEffectsCfg.Value2 = 0;
			RefitEffectsCfg.Resource = "Avatar/kart_new/126536/MODEL/zishen.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/126536/MODEL/zishen.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 20;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 126536; // 雷诺·幻夜-中阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 6;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 126540; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 400;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend1.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 6
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 6;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 1;
			RefitEffectsCfg.Level = 2;
			RefitEffectsCfg.Value1 = 0;
			RefitEffectsCfg.Value2 = 0;
			RefitEffectsCfg.Resource = "Avatar/kart_new/126536/MODEL/cheqianguangzhaoHUANGse.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/126536/MODEL/cheqianguangzhaoHUANGse.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 20;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 126536; // 雷诺·幻夜-中阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 6;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 126540; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 500;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend1.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 7
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 7;
			KartShapeRefitLegendLevelInfo.DressPoint = 5;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 50;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 126537; // 雷诺·幻夜-高阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 15;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 126540; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 600;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend1.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 8
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 8;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 0;
			RefitEffectsCfg.Level = 3;
			RefitEffectsCfg.Value1 = 0;
			RefitEffectsCfg.Value2 = 0;
			RefitEffectsCfg.Resource = "Avatar/kart_new/126537/MODEL/zishen.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/126537/MODEL/zishen.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 50;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 126537; // 雷诺·幻夜-高阶
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 15;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 126540; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 700;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend1.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 9
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 9;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 1;
			RefitEffectsCfg.Level = 3;
			RefitEffectsCfg.Value1 = 0;
			RefitEffectsCfg.Value2 = 0;
			RefitEffectsCfg.Resource = "Avatar/kart_new/126537/MODEL/cheqianguangzhaoHUANGse.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/126537/MODEL/cheqianguangzhaoHUANGse.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 50;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 126537; // 雷诺·幻夜-高阶
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 15;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 126540; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 800;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend1.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 10
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 10;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 4;
			RefitEffectsCfg.Level = 4;
			RefitEffectsCfg.Value1 = 4001;
			RefitEffectsCfg.Value2 = 1750;
			RefitEffectsCfg.Resource = "KartAvatarEffect/SuperJet/JetEff.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/126537/MODEL/startshow.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 5;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 110;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 126537; // 雷诺·幻夜-高阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 30;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 126540; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 1400;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend1.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}
		KartShapeRefitLegend1.ShapeSuitID = 127359; // 雷诺·幻夜	赛车皮肤

		// MaxConsumInfo
		// KartShapeRefitLegend1.MaxConsumInfo = new KartShapeRefitLegendMaxConsumCfg;
		KartShapeRefitLegend1.MaxConsumInfo.ItemID = 127349; // 幻夜结晶-直升10级
		KartShapeRefitLegend1.MaxConsumInfo.ItemNum = 1;
		KartShapeRefitLegend1.MaxConsumInfo.ItemPeriod = 0;

		KartShapeRefitLegend1.LegendSkinType = 0;
	}
	{										  // 众神乾坤
		KartShapeRefitLegend2.KartID = 90945; // 众神之神
		KartShapeRefitLegend2.Grade = 3;
		KartShapeRefitLegend2.KarName = "众神·乾坤";
		KartShapeRefitLegend2.GradeResource = "Legend";

		// Level 1
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 1;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127361; // 众神·乾坤-炫紫	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 127350; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend2.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 2
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 2;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127362; // 众神·乾坤-魔能	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 127350; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 127353; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 100;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend2.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
			KartShapeRefitLegendLevelInfo.ConsumInfo.clear();
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.clear();
		}

		// Level 3
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 3;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127363; // 众神·乾坤-黯红	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 127350; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 127353; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 200;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend2.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 4
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 4;
			KartShapeRefitLegendLevelInfo.DressPoint = 5;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 20;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127364; // 众神·乾坤-中阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 127350; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 6;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 127353; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 300;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend2.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
			KartShapeRefitLegendLevelInfo.ConsumInfo.clear();
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.clear();
		}

		// Level 5
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 5;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 0;
			RefitEffectsCfg.Level = 2;
			RefitEffectsCfg.Value1 = 0;
			RefitEffectsCfg.Value2 = 0;
			RefitEffectsCfg.Resource = "Avatar/kart_new/127364/MODEL/zishen.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/127364/MODEL/zishen.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 20;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127364; // 众神·乾坤-中阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 127350; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 6;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 127353; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 400;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend2.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 6
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 6;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 1;
			RefitEffectsCfg.Level = 2;
			RefitEffectsCfg.Value1 = 0;
			RefitEffectsCfg.Value2 = 0;
			RefitEffectsCfg.Resource = "Avatar/kart_new/127364/MODEL/cheqianguangzhaoHUANGse.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/127364/MODEL/cheqianguangzhaoHUANGse.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 20;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127364; // 众神·乾坤-中阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 127350; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 6;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 127353; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 500;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend2.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 7
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 7;
			KartShapeRefitLegendLevelInfo.DressPoint = 5;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 50;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127365; // 众神·乾坤-高阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 127350; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 15;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 127353; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 600;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend2.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 8
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 8;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 0;
			RefitEffectsCfg.Level = 3;
			RefitEffectsCfg.Value1 = 0;
			RefitEffectsCfg.Value2 = 0;
			RefitEffectsCfg.Resource = "Avatar/kart_new/127365/MODEL/zishen.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/127365/MODEL/zishen.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 50;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127365; // 众神·乾坤-高阶
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 127350; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 15;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 127353; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 700;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend2.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 9
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 9;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 1;
			RefitEffectsCfg.Level = 3;
			RefitEffectsCfg.Value1 = 0;
			RefitEffectsCfg.Value2 = 0;
			RefitEffectsCfg.Resource = "Avatar/kart_new/127365/MODEL/cheqianguangzhaoHUANGse.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/127365/MODEL/cheqianguangzhaoHUANGse.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 50;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127365; // 众神·乾坤-高阶
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 127350; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 15;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 127353; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 800;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend2.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 10
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 10;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 4;
			RefitEffectsCfg.Level = 4;
			RefitEffectsCfg.Value1 = 4001;
			RefitEffectsCfg.Value2 = 1750;
			RefitEffectsCfg.Resource = "KartAvatarEffect/SuperJet/JetEff.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/127365/MODEL/startshow.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 5;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 110;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127365; // 众神·乾坤-高阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 127350; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 30;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 127353; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 1400;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend2.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}
		KartShapeRefitLegend2.ShapeSuitID = 127360; // 众神·乾坤

		// MaxConsumInfo
		// KartShapeRefitLegend2.MaxConsumInfo = new KartShapeRefitLegendMaxConsumCfg;
		KartShapeRefitLegend2.MaxConsumInfo.ItemID = 127352; // 幻夜结晶-直升10级
		KartShapeRefitLegend2.MaxConsumInfo.ItemNum = 1;
		KartShapeRefitLegend2.MaxConsumInfo.ItemPeriod = 0;

		KartShapeRefitLegend2.LegendSkinType = 0;
	}

	{										   // 齐天大圣
		KartShapeRefitLegend3.KartID = 123512; // 悟空
		KartShapeRefitLegend3.Grade = 3;
		KartShapeRefitLegend3.KarName = "悟空-齐天大圣";
		KartShapeRefitLegend3.GradeResource = "Legend";

		// Level 1
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 1;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 123237; // 众神·乾坤-炫紫	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend3.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 2
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 2;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 123237; // 众神·乾坤-魔能	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 126540; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 100;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend3.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
			KartShapeRefitLegendLevelInfo.ConsumInfo.clear();
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.clear();
		}

		// Level 3
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 3;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 123237; // 众神·乾坤-黯红	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 126540; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 200;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend3.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 4
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 4;
			KartShapeRefitLegendLevelInfo.DressPoint = 5;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 20;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 123237; // 众神·乾坤-中阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 6;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 126540; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 300;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend3.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
			KartShapeRefitLegendLevelInfo.ConsumInfo.clear();
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.clear();
		}

		// Level 5
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 5;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 0;
			RefitEffectsCfg.Level = 2;
			RefitEffectsCfg.Value1 = 0;
			RefitEffectsCfg.Value2 = 0;
			RefitEffectsCfg.Resource = "Avatar/kart_new/123238/MODEL/zishen.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/123238/MODEL/zishen.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 20;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 123238; // 众神·乾坤-中阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 126538; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 6;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 126540; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 400;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend3.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}
		KartShapeRefitLegend3.ShapeSuitID = 123513; // 众神·乾坤

		// MaxConsumInfo
		// KartShapeRefitLegend3.MaxConsumInfo = new KartShapeRefitLegendMaxConsumCfg;
		KartShapeRefitLegend3.MaxConsumInfo.ItemID = 127349; // 幻夜结晶-直升10级
		KartShapeRefitLegend3.MaxConsumInfo.ItemNum = 1;
		KartShapeRefitLegend3.MaxConsumInfo.ItemPeriod = 0;

		KartShapeRefitLegend3.LegendSkinType = 0;
	}
	{										   // 源极
		KartShapeRefitLegend4.KartID = 126345; // 源极之星EXA
		KartShapeRefitLegend4.Grade = 3;
		KartShapeRefitLegend4.KarName = "源极-大河神卫";
		KartShapeRefitLegend4.GradeResource = "huanghexianding";

		// Level 1
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 1;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127436; // 众神·乾坤-炫紫	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 129141; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend4.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 2
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 2;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127436; // 众神·乾坤-魔能	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 129141; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 129141; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 100;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend4.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
			KartShapeRefitLegendLevelInfo.ConsumInfo.clear();
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.clear();
		}

		// Level 3
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 3;
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 10;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127436; // 众神·乾坤-黯红	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 129141; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 3;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 129141; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 200;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend4.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}

		// Level 4
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 4;
			KartShapeRefitLegendLevelInfo.DressPoint = 5;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 20;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127436; // 众神·乾坤-中阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 129141; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 6;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 129141; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 300;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend4.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
			KartShapeRefitLegendLevelInfo.ConsumInfo.clear();
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.clear();
		}

		// Level 5
		{
			KartShapeRefitLegendLevelCfg KartShapeRefitLegendLevelInfo;
			KartShapeRefitLegendLevelInfo.Level = 5;
			// RefitEffectsCfg
			KartShapeRefitLegendRefitEffectsCfg RefitEffectsCfg;
			RefitEffectsCfg.EffectID = 0;
			RefitEffectsCfg.Level = 2;
			RefitEffectsCfg.Value1 = 0;
			RefitEffectsCfg.Value2 = 0;
			RefitEffectsCfg.Resource = "Avatar/kart_new/127437/MODEL/zishen.nif";
			RefitEffectsCfg.EffectRes = "Avatar/kart_new/127437/MODEL/zishen.nif";
			RefitEffectsCfg.Grade = 1;
			KartShapeRefitLegendLevelInfo.RefitEffectsInfo.emplace_back(RefitEffectsCfg);
			KartShapeRefitLegendLevelInfo.DressPoint = 2;
			KartShapeRefitLegendLevelInfo.CarLibraryCollectValue = 20;
			KartShapeRefitLegendLevelInfo.ShapeSuitID = 127437; // 众神·乾坤-中阶	赛车皮肤
			KartShapeRefitLegendMaxConsumCfg ConsumInfo;
			ConsumInfo.ItemID = 129141; // 幻夜蓝图	功能道具
			ConsumInfo.ItemNum = 6;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			ConsumInfo.ItemID = 129141; // 传奇能源	功能道具
			ConsumInfo.ItemNum = 400;
			ConsumInfo.ItemPeriod = 0;
			KartShapeRefitLegendLevelInfo.ConsumInfo.emplace_back(ConsumInfo);
			KartShapeRefitLegend4.LevelInfo.emplace_back(KartShapeRefitLegendLevelInfo);
		}
		KartShapeRefitLegend4.ShapeSuitID = 126355; // 众神·乾坤

		// MaxConsumInfo
		// KartShapeRefitLegend4.MaxConsumInfo = new KartShapeRefitLegendMaxConsumCfg;
		KartShapeRefitLegend4.MaxConsumInfo.ItemID = 129142; // 幻夜结晶-直升10级
		KartShapeRefitLegend4.MaxConsumInfo.ItemNum = 1;
		KartShapeRefitLegend4.MaxConsumInfo.ItemPeriod = 0;

		KartShapeRefitLegend4.LegendSkinType = 0;
	}
	KartShapeRefitLegendInfoCfg.emplace_back(KartShapeRefitLegend);
	KartShapeRefitLegendInfoCfg.emplace_back(KartShapeRefitLegend1);
	KartShapeRefitLegendInfoCfg.emplace_back(KartShapeRefitLegend2);
	KartShapeRefitLegendInfoCfg.emplace_back(KartShapeRefitLegend3);
	KartShapeRefitLegendInfoCfg.emplace_back(KartShapeRefitLegend4);

	UCHAR KartShapeRefitLegendNum = KartShapeRefitLegendInfoCfg.size();

	Write16(p, KartShapeRefitLegendNum); // KartShapeRefitLegendNum
	for (size_t Ki = 0; Ki < KartShapeRefitLegendNum; Ki++)
	{ // ShapeRefitLegend
		BYTE *pShapeRefitLegend = p;
		Write16(pShapeRefitLegend, 0); // len

		Write32(pShapeRefitLegend, KartShapeRefitLegendInfoCfg[Ki].KartID); // KartID		爆天甲
		UCHAR KarNameLen = KartShapeRefitLegendInfoCfg[Ki].KarName.length();
		Write8(pShapeRefitLegend, KarNameLen); // KarNameLen
		if (KarNameLen)
		{
			memcpy(pShapeRefitLegend, KartShapeRefitLegendInfoCfg[Ki].KarName.data(), KarNameLen); // 品级 资源 传奇
			pShapeRefitLegend += KarNameLen;
		}
		Write16(pShapeRefitLegend, KartShapeRefitLegendInfoCfg[Ki].Grade); // Grade			品级

		UCHAR GradeResourceLen = KartShapeRefitLegendInfoCfg[Ki].GradeResource.length();

		Write8(pShapeRefitLegend, GradeResourceLen); // GradeResourceLen
		if (GradeResourceLen)
		{
			memcpy(pShapeRefitLegend, KartShapeRefitLegendInfoCfg[Ki].GradeResource.data(), GradeResourceLen); // 品级 资源 传奇
			pShapeRefitLegend += GradeResourceLen;
		}

		auto &LevelInfoGroup = KartShapeRefitLegendInfoCfg[Ki].LevelInfo;
		UCHAR LevelNum = LevelInfoGroup.size();

		Write8(pShapeRefitLegend, LevelNum); // LevelNum  皮肤等级数量
		for (size_t LevelNumi = 0; LevelNumi < LevelNum; LevelNumi++)
		{ // LevelCfg
			BYTE *pLevelCfg = pShapeRefitLegend;
			Write16(pLevelCfg, 0); // len

			Write8(pLevelCfg, LevelInfoGroup[LevelNumi].Level); // Level
			Write8(pLevelCfg, 0);								// RefitItemNum
			// Write32(pLevelCfg, 0); //RefitItem[]

			auto &EffectGroup = LevelInfoGroup[LevelNumi].RefitEffectsInfo;
			UCHAR EffectNum = EffectGroup.size();

			Write8(pLevelCfg, EffectNum); // EffectNum
			for (size_t i = 0; i < EffectNum; i++)
			{ // RefitEffects
				BYTE *pRefitEffects = pLevelCfg;
				Write16(pRefitEffects, 0); // len

				Write16(pRefitEffects, EffectGroup[i].EffectID); // EffectID
				Write8(pRefitEffects, EffectGroup[i].Level);	 // Level
				Write32(pRefitEffects, EffectGroup[i].Value1);	 // Value1
				Write32(pRefitEffects, EffectGroup[i].Value2);	 // Value2

				UCHAR ResourceLen = EffectGroup[i].Resource.length();
				Write8(pRefitEffects, ResourceLen); // ResourceLen
				if (ResourceLen)
				{
					memcpy(pRefitEffects, EffectGroup[i].Resource.data(), ResourceLen); // 品级 资源 传奇
					pRefitEffects += ResourceLen;
				}
				UCHAR EffectResLen = EffectGroup[i].EffectRes.length();
				Write8(pRefitEffects, EffectResLen); // EffectResLen
				if (EffectResLen)
				{
					memcpy(pRefitEffects, EffectGroup[i].EffectRes.data(), EffectResLen); // 品级 资源 传奇
					pRefitEffects += EffectResLen;
				}

				Write8(pRefitEffects, EffectGroup[i].Grade); // Grade

				len = pRefitEffects - pLevelCfg;
				Set16(pLevelCfg, (WORD)len);
				pLevelCfg += len;
			}
			Write16(pLevelCfg, LevelInfoGroup[LevelNumi].DressPoint);			  // DressPoint
			Write16(pLevelCfg, LevelInfoGroup[LevelNumi].CarLibraryCollectValue); // CarLibraryCollectValue
			Write8(pLevelCfg, 0);												  // RefitModifyNum
			for (size_t i = 0; i < 0; i++)
			{ // RefitModify
				BYTE *pRefitModify = pLevelCfg;
				Write16(pRefitModify, 0); // len

				Write16(pRefitModify, 0); // RefitID
				Write8(pRefitModify, 0);  // RefitModify
				Write32(pRefitModify, 0); // RefitLimitInc
				Write32(pRefitModify, 0); // RefitInc

				len = pRefitModify - pLevelCfg;
				Set16(pLevelCfg, (WORD)len);
				pLevelCfg += len;
			}
			Write8(pLevelCfg, 0); // HasTurnModify
			for (size_t i = 0; i < 0; i++)
			{ // TurnModify
				BYTE *pTurnModify = pLevelCfg;
				Write16(pTurnModify, 0); // len

				Write32(pTurnModify, 0); // BaseTurnRate
				Write32(pTurnModify, 0); // VolatileTurnRate
				Write32(pTurnModify, 0); // MaxTurnSpeed
				Write32(pTurnModify, 0); // MinTurnSpeed
				Write32(pTurnModify, 0); // MaxAccuTime
				Write32(pTurnModify, 0); // BaseAccuRate
				Write32(pTurnModify, 0); // MaxAffectSpeed

				len = pTurnModify - pLevelCfg;
				Set16(pLevelCfg, (WORD)len);
				pLevelCfg += len;
			}
			Write32(pLevelCfg, LevelInfoGroup[LevelNumi].ShapeSuitID); // ShapeSuitID

			auto &ConsumInfoGroup = LevelInfoGroup[LevelNumi].ConsumInfo;
			UCHAR ConsumeNum = ConsumInfoGroup.size();

			Write8(pLevelCfg, ConsumeNum); // ConsumeNum
			for (size_t i = 0; i < ConsumeNum; i++)
			{ // ConsumInfo
				BYTE *pConsumInfo = pLevelCfg;
				Write16(pConsumInfo, 0); // len

				Write32(pConsumInfo, ConsumInfoGroup[i].ItemID);	 // ItemID
				Write32(pConsumInfo, ConsumInfoGroup[i].ItemNum);	 // ItemNum
				Write32(pConsumInfo, ConsumInfoGroup[i].ItemPeriod); // ItemPeriod

				len = pConsumInfo - pLevelCfg;
				Set16(pLevelCfg, (WORD)len);
				pLevelCfg += len;
			}

			Write16(pLevelCfg, 0); // 未知参数

			len = pLevelCfg - pShapeRefitLegend;
			Set16(pShapeRefitLegend, (WORD)len);
			pShapeRefitLegend += len;
		}
		Write32(pShapeRefitLegend, KartShapeRefitLegendInfoCfg[Ki].ShapeSuitID); // ShapeSuitID

		auto &MaxConsumInfo = KartShapeRefitLegendInfoCfg[Ki].MaxConsumInfo;

		{ // MaxConsumInfo
			BYTE *pMaxConsumInfo = pShapeRefitLegend;
			Write16(pMaxConsumInfo, 0); // len

			Write32(pMaxConsumInfo, MaxConsumInfo.ItemID);	   // ItemID
			Write32(pMaxConsumInfo, MaxConsumInfo.ItemNum);	   // ItemNum
			Write32(pMaxConsumInfo, MaxConsumInfo.ItemPeriod); // ItemPeriod

			len = pMaxConsumInfo - pShapeRefitLegend;
			Set16(pShapeRefitLegend, (WORD)len);
			pShapeRefitLegend += len;
		}
		Write8(pShapeRefitLegend, KartShapeRefitLegendInfoCfg[Ki].LegendSkinType); // LegendSkinType

		len = pShapeRefitLegend - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write16(p, 0); // 未知参数
	Z_INFO("赛车传奇皮肤加载完成[%d]!\n", KartShapeRefitLegendInfoCfg.size());
	len = p - buf;
	SendToClient(Client, 317, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestWeRelayCommonCfg(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseWeRelayCommonCfg(Client);
}
void ResponseWeRelayCommonCfg(ClientNode *Client)
{
	const unsigned char *pMsg24391 = EHexToByte("0361000000510134B17D0134B1E20000000300000051B1BEC8FCBCBEBFAAB7C5CAB1BCE4CEAA32303233C4EA3035D4C23235C8D5D6C132303233C4EA3036D4C23236C8D5A3ACC8FCBCBEC4DAC3BFD6DCCEE5B5BDD6DCC8D531303A30302D32323A3030BFAAB7C50000002E3035D4C23235C8D5D6C13036D4C23236C8D52CC3BFD6DCCEE5B5BDD6DCC8D531303A30302D32323A3030BFAAB7C5006400000005000000010012000186A000035B60000186A000035B60001200000000000000000000000000000000001200000000000000000000000000000000001200000000000000000000000000000000001200000000000000000000000000000000006400000006000000010012000186A000035B60000186A000035B60001200000000000000000000000000000000001200000000000000000000000000000000001200000000000000000000000000000000001200000000000000000000000000000000006400000007000000010012000186A000035B60000186A000035B600012000000000000000000000000000000000012000000000000000000000000000000000012000000000000000000000000000000000012000000000000000000000000000000000064000000000000000000120000000000000000000000000000000000120000000000000000000000000000000000120000000000000000000000000000000000120000000000000000000000000000000000120000000000000000000000000000000000640000000000000000001200000000000000000000000000000000001200000000000000000000000000000000001200000000000000000000000000000000001200000000000000000000000000000000001200000000000000000000000000000000006400000000000000000012000000000000000000000000000000000012000000000000000000000000000000000012000000000000000000000000000000000012000000000000000000000000000000000012000000000000000000000000000000000064000000000000000000120000000000000000000000000000000000120000000000000000000000000000000000120000000000000000000000000000000000120000000000000000000000000000000000120000000000000000000000000000000000000086000000640000000100000001019D0000000000000003007E000000000000000100000032006C00000000000000000000000000030019000078AE00000001000002D00000000000000000000000001900011A6600000001000002D00000000000000000000000001900011A6900000001000002D00000000000000000000000000000000000000000000000000000000000000000007E000000010000003300000096006C00000000000000000000000000030019000078AE00000001000001680000000000000000000000001900011A6700000001000002D00000000000000000000000001900011A6A00000001000002D00000000000000000000000000000000000000000000000000000000000000000009700000002000000970000012C00850000000000000000000000000004001900006CF300000001000002D00000000000000000000000001900006CED00000001000002D00000000000000000000000001900011A6800000001000002D00000000000000000000000001900011A6B00000001000002D000000000000000000000000000000000000000000000000000000000000000000000000500AC0000009D000F424000000001009E00000000000000000000000000050019000042D300000001000002D0000000000000000000000000190000802000000001000002D00000000000000000000000001900006C7100000001000002D00000000000000000000000001900006F9100000001000002D0000000000000000000000000190000761400000001000002D00000000000000000000000000000000000000000000000000000000000AC0000009E001E848000000001009E0000000000000000000000000005001900006E1000000001000002D0000000000000000000000000190000761400000001000002D0000000000000000000000000190000881200000001FFFFFFFF000000000000000000000000190000FE5D00000001000002D0000000000000000000000000190000F7D000000001000002D00000000000000000000000000000000000000000000000000000000000AC0000009F002DC6C000000001009E000000000000000000000000000500190000845800000001000002D000000000000000000000000019000100A100000001000002D0000000000000000000000000190000FD1200000001000002D000000000000000000000000019000075A000000001FFFFFFFF0000000000000000000000001900007E9F00000001000002D00000000000000000000000000000000000000000000000000000000000AC000000A0003D090000000001009E0000000000000000000000000005001900005E6E00000001000002D0000000000000000000000000190001033D00000001000002D00000000000000000000000001900005DB000000001FFFFFFFF0000000000000000000000001900005E8F00000001000002D0000000000000000000000000190000F88C00000001000002D00000000000000000000000000000000000000000000000000000000000AC000000A1004C4B4000000001009E000000000000000000000000000500190001035100000001000002D0000000000000000000000000190000838600000001000002D00000000000000000000000001900004E9D00000001FFFFFFFF000000000000000000000000190000865B00000001FFFFFFFF00000000000000000000000019000081EA00000001FFFFFFFF00000000000000000000000000000000000000000000000000000000000000070022000000000000000400060000FD6F0006000081E900060001171F000600010E5700220000000300000004000600005B52000600005E52000600005A170006000032940034000000040000000700060000511A000600005129000600005257000600002B8B00060000558F000600005258000600004CA50046000000050000000A00060000565700060000614300060000614400060000566600060000586E000600008403000600010E5E00060000512A00060000367100060000694A0046000000060000000A000600006BC4000600006A6900060000682E000600002BAA000600003C5E000600002E5D000600002E5F000600003C47000600004951000600003C4A001C0000000700000003000600002724000600007074000600010A60002800000009000000050006000059E8000600005DB7000600003719000600004CA70006000042D00000000000A400000000009E00000000000000000000000000050019000067D500000001000000A80000000000000000000000001900004BC400000001000000000000000000000000000000001900004C7400000001000000A80000000000000000000000001900004D2C0000000100000000000000000000000000000000190000FE05000000010000000000000000000000000000000000000000000000000000000000000000A200");

	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	// memcpy(p, pMsg24391, 2648);
	// p += 2648;
	// len = p - buf;
	// SendToClient(Client, 24391, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	// return;
	{ // SeasonCfg
		BYTE *pSeasonCfg = p;
		Write16(pSeasonCfg, 0); // len

		Write32(pSeasonCfg, 34); // SeasonId
		Write32(pSeasonCfg, 0);	 // OpenDate
		Write32(pSeasonCfg, 0);	 // CloseDate
		Write32(pSeasonCfg, 7);	 // OpenDayNum
		Write32(pSeasonCfg, 0);	 // BroadCastContentLen
		Write32(pSeasonCfg, 0);	 // OpenTimeTipsLen

		const int MAX_SPEEDRALLY_OPEN_DAY_NUM = 7;
		for (size_t i = 0; i < MAX_SPEEDRALLY_OPEN_DAY_NUM; i++)
		{ // tDayOpenCfg
			BYTE *ptDayOpenCfg = pSeasonCfg;
			Write16(ptDayOpenCfg, 0); // len

			Write32(ptDayOpenCfg, i + 1); // DayID
			Write32(ptDayOpenCfg, 0);	  // OpenTimeNum
			/*
			for (size_t i = 0; i < MAX_OPEN_TIME_NUM; i++)
			{ //tOpenTimeCfg
				BYTE* ptOpenTimeCfg = ptDayOpenCfg;
				Write16(ptOpenTimeCfg, 0); //len

				Write32(ptOpenTimeCfg, 0); //OpenTime
				Write32(ptOpenTimeCfg, 0); //CloseTime
				Write32(ptOpenTimeCfg, 0); //TiyanOpenTime
				Write32(ptOpenTimeCfg, 0); //TiyanCloseTime

				len = ptOpenTimeCfg - ptDayOpenCfg;
				Set16(ptDayOpenCfg, (WORD)len);
				ptDayOpenCfg += len;
			}
			*/
			len = ptDayOpenCfg - pSeasonCfg;
			Set16(pSeasonCfg, (WORD)len);
			pSeasonCfg += len;
		}
		Write32(pSeasonCfg, 0); // LuckyBoxIndex
		Write32(pSeasonCfg, 0); // LuckyBoxPriority
		Write32(pSeasonCfg, 0); // LuckyBoxDayLimit

		len = pSeasonCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 1); // RankNum

	for (size_t i = 0; i < 1; i++)
	{ // RankAwardInfo
		BYTE *pRankAwardInfo = p;
		Write16(pRankAwardInfo, 0); // len

		Write32(pRankAwardInfo, 0); // RankType
		Write32(pRankAwardInfo, 0); // AwardNum
		for (size_t i = 0; i < 0; i++)
		{ // BaseRankAwardToClient
			BYTE *pBaseRankAwardToClient = pRankAwardInfo;
			Write16(pBaseRankAwardToClient, 0); // len

			Write32(pBaseRankAwardToClient, 0); // Index
			Write32(pBaseRankAwardToClient, 0); // RankBegin
			Write32(pBaseRankAwardToClient, 0); // RankEnd
			{									// Award
				BYTE *pAward = pBaseRankAwardToClient;
				Write16(pAward, 0); // len

				Write32(pAward, 0); // Exp
				Write32(pAward, 0); // Money
				Write32(pAward, 0); // SuperMoney
				Write16(pAward, 0); // ItemNum
				for (size_t i = 0; i < 0; i++)
				{ // ItemInfo
					BYTE *pItemInfo = pAward;
					Write16(pItemInfo, 0); // len

					Write32(pItemInfo, 0); // ItemID
					Write32(pItemInfo, 0); // ItemNum
					Write32(pItemInfo, 0); // AvailPeriod
					Write8(pItemInfo, 0);  // Status
					WriteUn(pItemInfo, 0); // ObtainTime
					Write32(pItemInfo, 0); // OtherAttribute
					Write16(pItemInfo, 0); // ItemType

					len = pItemInfo - pAward;
					Set16(pAward, (WORD)len);
					pAward += len;
				}
				Write32(pAward, 0); // Coupons
				Write32(pAward, 0); // GuildPoint
				Write32(pAward, 0); // LuckMoney
				Write8(pAward, 0);	// ExtendInfoNum
				for (size_t i = 0; i < 0; i++)
				{ // ExtendInfoAward
					BYTE *pExtendInfoAward = pAward;
					Write16(pExtendInfoAward, 0); // len

					Write16(pExtendInfoAward, 0); // Key
					Write32(pExtendInfoAward, 0); // AddValue
					Write32(pExtendInfoAward, 0); // TotalValue
					Write8(pExtendInfoAward, 0);  // ShowPriority
					Write16(pExtendInfoAward, 0); // BuyNeedScore
					Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

					len = pExtendInfoAward - pAward;
					Set16(pAward, (WORD)len);
					pAward += len;
				}
				Write32(pAward, 0); // SpeedCoin

				len = pAward - pBaseRankAwardToClient;
				Set16(pBaseRankAwardToClient, (WORD)len);
				pBaseRankAwardToClient += len;
			}
			Write32(pBaseRankAwardToClient, 0); // Status

			len = pBaseRankAwardToClient - pRankAwardInfo;
			Set16(pRankAwardInfo, (WORD)len);
			pRankAwardInfo += len;
		}

		len = pRankAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); // ActiveAwardNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //ActiveAwardToClient
		BYTE* pActiveAwardToClient = p;
		Write16(pActiveAwardToClient, 0); //len

		Write32(pActiveAwardToClient, 0); //Index
		Write32(pActiveAwardToClient, 0); //ActiveValueLimit
		Write32(pActiveAwardToClient, 0); //RankLevLimit
		{ //Award
			BYTE* pAward = pActiveAwardToClient;
			Write16(pAward, 0); //len

			Write32(pAward, 0); //Exp
			Write32(pAward, 0); //Money
			Write32(pAward, 0); //SuperMoney
			Write16(pAward, 0); //ItemNum
			for (size_t i = 0; i < n; i++)
			{ //ItemInfo
				BYTE* pItemInfo = pAward;
				Write16(pItemInfo, 0); //len

				Write32(pItemInfo, 0); //ItemID
				Write32(pItemInfo, 0); //ItemNum
				Write32(pItemInfo, 0); //AvailPeriod
				Write8(pItemInfo, 0); //Status
				WriteUn(pItemInfo, 0); //ObtainTime
				Write32(pItemInfo, 0); //OtherAttribute
				Write16(pItemInfo, 0); //ItemType

				len = pItemInfo - pAward;
				Set16(pAward, (WORD)len);
				pAward += len;
			}
			Write32(pAward, 0); //Coupons
			Write32(pAward, 0); //GuildPoint
			Write32(pAward, 0); //LuckMoney
			Write8(pAward, 0); //ExtendInfoNum
			for (size_t i = 0; i < n; i++)
			{ //ExtendInfoAward
				BYTE* pExtendInfoAward = pAward;
				Write16(pExtendInfoAward, 0); //len

				Write16(pExtendInfoAward, 0); //Key
				Write32(pExtendInfoAward, 0); //AddValue
				Write32(pExtendInfoAward, 0); //TotalValue
				Write8(pExtendInfoAward, 0); //ShowPriority
				Write16(pExtendInfoAward, 0); //BuyNeedScore
				Write8(pExtendInfoAward, 0); //OneMatchMaxNum

				len = pExtendInfoAward - pAward;
				Set16(pAward, (WORD)len);
				pAward += len;
			}
			Write32(pAward, 0); //SpeedCoin

			len = pAward - pActiveAwardToClient;
			Set16(pActiveAwardToClient, (WORD)len);
			pActiveAwardToClient += len;
		}

		len = pActiveAwardToClient - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, 0); // CarTypeNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //CarGuide
		BYTE* pCarGuide = p;
		Write16(pCarGuide, 0); //len

		Write32(pCarGuide, 0); //CarType
		Write32(pCarGuide, 0); //CarNum
		for (size_t i = 0; i < n; i++)
		{ //GuideCarInfo
			BYTE* pGuideCarInfo = pCarGuide;
			Write16(pGuideCarInfo, 0); //len

			Write32(pGuideCarInfo, 0); //CarID

			len = pGuideCarInfo - pCarGuide;
			Set16(pCarGuide, (WORD)len);
			pCarGuide += len;
		}

		len = pCarGuide - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, 0); // TiyanCarTypeNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //TiyanCarGuide
		BYTE* pTiyanCarGuide = p;
		Write16(pTiyanCarGuide, 0); //len

		Write32(pTiyanCarGuide, 0); //CarType
		Write32(pTiyanCarGuide, 0); //CarNum
		for (size_t i = 0; i < n; i++)
		{ //GuideCarInfo
			BYTE* pGuideCarInfo = pTiyanCarGuide;
			Write16(pGuideCarInfo, 0); //len

			Write32(pGuideCarInfo, 0); //CarID

			len = pGuideCarInfo - pTiyanCarGuide;
			Set16(pTiyanCarGuide, (WORD)len);
			pTiyanCarGuide += len;
		}

		len = pTiyanCarGuide - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	{ // LuckyAwardToClient
		BYTE *pLuckyAwardToClient = p;
		Write16(pLuckyAwardToClient, 0); // len

		Write32(pLuckyAwardToClient, 0); // LuckBoxGet
		{								 // Award
			BYTE *pAward = pLuckyAwardToClient;
			Write16(pAward, 0); // len

			Write32(pAward, 0); // Exp
			Write32(pAward, 0); // Money
			Write32(pAward, 0); // SuperMoney
			Write16(pAward, 0); // ItemNum
			/*
			for (size_t i = 0; i < n; i++)
			{ //ItemInfo
				BYTE* pItemInfo = pAward;
				Write16(pItemInfo, 0); //len

				Write32(pItemInfo, 0); //ItemID
				Write32(pItemInfo, 0); //ItemNum
				Write32(pItemInfo, 0); //AvailPeriod
				Write8(pItemInfo, 0); //Status
				Write32(pItemInfo, 0); //ObtainTime
				Write32(pItemInfo, 0); //OtherAttribute
				Write16(pItemInfo, 0); //ItemType

				len = pItemInfo - pAward;
				Set16(pAward, (WORD)len);
				pAward += len;
			}
			*/
			Write32(pAward, 0); // Coupons
			Write32(pAward, 0); // GuildPoint
			Write32(pAward, 0); // LuckMoney
			Write8(pAward, 0);	// ExtendInfoNum
			/*
			for (size_t i = 0; i < n; i++)
			{ //ExtendInfoAward
				BYTE* pExtendInfoAward = pAward;
				Write16(pExtendInfoAward, 0); //len

				Write16(pExtendInfoAward, 0); //Key
				Write32(pExtendInfoAward, 0); //AddValue
				Write32(pExtendInfoAward, 0); //TotalValue
				Write8(pExtendInfoAward, 0); //ShowPriority
				Write16(pExtendInfoAward, 0); //BuyNeedScore
				Write8(pExtendInfoAward, 0); //OneMatchMaxNum

				len = pExtendInfoAward - pAward;
				Set16(pAward, (WORD)len);
				pAward += len;
			}
			*/
			Write32(pAward, 0); // SpeedCoin

			len = pAward - pLuckyAwardToClient;
			Set16(pLuckyAwardToClient, (WORD)len);
			pLuckyAwardToClient += len;
		}

		len = pLuckyAwardToClient - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24391, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyGetProp(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Uin
	Write16(p, 0); // PropID
	Write32(p, 0); // PropIndex
	Write16(p, 0); // PropPositionNO
	Write8(p, 0);  // GetPropType
	Write8(p, 0);  // PointArrayIdx
	Write32(p, 0); // LapTime
	Write8(p, 0);  // GetPropSecType
	Write32(p, 0); // SrcUin
	Write8(p, 0);  // ParaNum
	Write32(p, 0); // ParaList[]

	len = p - buf;
	SendToClient(Client, 516, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestReportAntiCollisionDataBuff(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT BuffeLen = Read16(p);
}
void RequestReportCommonAction(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	if (Client->IsFinish == true) {
		return;
	}
	BYTE *p = Body;
	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT ActionID = Read16(p);
	// 25  侧身 39 氮气 22 cw 10 小盆 37 起步 2 双喷 1 经典漂移 27 段位漂移
	//  8 空喷 9 落地喷 28 段位双喷
	if (Client->GetSkillID(1))
	{ // 技能1 手游天行者
		if (ActionID == 39)
		{
			BYTE buf[8192];
			BYTE *p = buf;
			size_t len;
			Client->kartPow += 200;
			Write16(p, 0);	 // ResultID
			Write16(p, 1);	 // PropID
			Write32(p, 0);	 // PropIndex
			Write32(p, 0);	 // CurrentAccelerateFuel 当前加速度燃料
			Write8(p, 0);	 // ReasonLen
			Write32(p, 200); // SuperJetKartFuelInc 超级喷气机车燃料
			len = p - buf;
			SendToClient(Client, 119, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
			if (Client->kartPow == 1000)
			{
				Client->kartPow = 0;
				ResponseGetPropS(Client, 34);
			}
		}
	}
	if (Client->GetSkillID(2))
	{ // 技能2 手游时序使者
		if (ActionID == 10 || ActionID == 8 || ActionID == 9 || ActionID == 2)
		{
			if (Client->useDQ > 8)
			{
				Client->kartPow += 1;
				if (Client->kartPow > 8)
				{
					Client->kartPow = 8;
				}
				BYTE buf[8192];
				BYTE *p = buf;
				size_t len;
				Write16(p, 0);	 // ResultID
				Write16(p, 1);	 // PropID
				Write32(p, 0);	 // PropIndex
				Write32(p, 0);	 // CurrentAccelerateFuel 当前加速度燃料
				Write8(p, 0);	 // ReasonLen
				Write32(p, 125); // SuperJetKartFuelInc 超级喷气机车燃料
				len = p - buf;
				SendToClient(Client, 119, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);

				/*char FilePath[MAX_PATH];
				sprintf_s(FilePath, "获得一点黑洞能量（当前黑洞能量:%d）", Client->kartPow);
				NotifyTalk(Client, Client->Uin, 1, 0, 0, MAX_PATH, FilePath);*/
			}
		}
		if (ActionID == 39)
		{
			
			if (Client->useDQ > 8)
			{
				Client->kartPow += 2;
				if (Client->kartPow > 8)
				{
					Client->kartPow = 8;
				}
				BYTE buf[8192];
				BYTE *p = buf;
				size_t len;
				Write16(p, 0);	 // ResultID
				Write16(p, 1);	 // PropID
				Write32(p, 0);	 // PropIndex
				Write32(p, 0);	 // CurrentAccelerateFuel 当前加速度燃料
				Write8(p, 0);	 // ReasonLen
				Write32(p, 250); // SuperJetKartFuelInc 超级喷气机车燃料
				len = p - buf;
				SendToClient(Client, 119, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
				/*char FilePath[MAX_PATH];
				sprintf_s(FilePath, "获得二点黑洞能量（当前黑洞能量:%d）", Client->kartPow);
				NotifyTalk(Client, Client->Uin, 1, 0, 0, MAX_PATH, FilePath);*/
			}
		}
		if (Client->useDQ <= 8)
		{
			BYTE buf[8192];
			BYTE *p = buf;
			size_t len;

			Write16(p, 0);	  // ResultID
			Write16(p, 777);  // PropID
			Write32(p, 0);	  // PropIndex
			Write32(p, 0);	  // CurrentAccelerateFuel 当前加速度燃料
			Write8(p, 0);	  // ReasonLen
			Write32(p, 1000); // SuperJetKartFuelInc 超级喷气机车燃料

			len = p - buf;
			SendToClient(Client, 119, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
		}
		if (Client->kartPow == 8)
		{
			Client->kartPow = 0;
			BYTE buf[8192];
			BYTE *p = buf;
			size_t len;

			Write16(p, 0);	  // ResultID
			Write16(p, 777);  // PropID
			Write32(p, 0);	  // PropIndex
			Write32(p, 0);	  // CurrentAccelerateFuel 当前加速度燃料
			Write8(p, 0);	  // ReasonLen
			Write32(p, 1000); // SuperJetKartFuelInc 超级喷气机车燃料

			len = p - buf;
			SendToClient(Client, 119, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
			// NotifyTalk(Client, Client->Uin, 1, 0, 0, strlen("黑洞能量发动，氮气升级"), "黑洞能量发动，氮气升级");
		}
	}
	if (ActionID == 37)
	{
		if (Client->GetSkillID(7))
		{
			NotifyUseProp(Client, Client->Uin, Client->Uin, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, 0, {}, 0, 0, 0, 0, 0);
		}
		if (Client->GetSkillID(2))
		{
			NotifyUseProp(Client, Client->Uin, Client->Uin, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, 0, {}, 0, 0, 0, 0, 0);
			ResponseGetPropS(Client, 777);
			// NotifyTalk(Client, Client->Uin, 1, 0, 0, strlen("时序使者发动技能,获得超级氮气"), "时序使者发动技能,获得氮气");
		}
	}
}
void RequestAddFriend(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT DstUin = Read32(p);
	UINT WordLength = Read32(p);
	UCHAR Type = Read8(p);
	USHORT Src = Read16(p);
	// ResponseInvitePlayer(Client, DstUin, Type);
	if (GetClient(DstUin))
	{
		NotifyBeAddedAsFriend(GetClient(DstUin), Client, Type, Src);
		ResponseAddFriend(Client, DstUin, Type, Src);
	}
	else
	{
		ResponseStartGame(Client, "该用户不在线,无法发送请求");
	}
	// NotifyAddFriendResult(Client, DstUin, Type, Src);
}
void NotifyBeAddedAsFriend(ClientNode *Client, ClientNode *DstClinet, UCHAR Type, USHORT Src)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, DstClinet->Uin); // SrcUin
	Write32(p, Client->Uin);	// DstUin
	{							// SrcPlayerInfo
		BYTE *pSrcPlayerInfo = p;
		Write16(pSrcPlayerInfo, 0); // len

		Write8(pSrcPlayerInfo, DstClinet->Gender);	// Gender
		Write8(pSrcPlayerInfo, DstClinet->Country); // Country
		//Write8(pSrcPlayerInfo, 0);					// NickName
		WriteString(pSrcPlayerInfo, DstClinet->NickName,16);
		// Write8(pSrcPlayerInfo, strlen(DstClinet->NickName)); //NickName
		// memcpy(p, DstClinet->NickName, strlen(DstClinet->NickName)); //NickName[]
		// p += strlen(DstClinet->NickName);
		len = pSrcPlayerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0);	 // WordLength
	Write8(p, Type); // Type
	Write16(p, Src); // Src

	len = p - buf;
	SendToClient(Client, 507, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void ResponseAddFriend(ClientNode *Client, UINT DstUin, UCHAR Type, USHORT Src)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	Write32(p, DstUin);		 // DstUin
	Write8(p, 0);	 // ReasonLen
	Write8(p, Type); // Type
	Write16(p, Src); // Src

	len = p - buf;
	SendToClient(Client, 114, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyAddFriendResult(ClientNode *Client, UINT SrcUin, UINT DstUin, UCHAR Type, USHORT Src)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, SrcUin); // SrcUin
	Write32(p, DstUin);		 // DstUin
	Write16(p, 0);			 // WordLength
	WriteString(p, "添加成功");
	Write8(p, Type); // Type
	Write16(p, Src); // Src

	len = p - buf;
	SendToClient(Client, 508, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void AckBeAddedAsFriend(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	short ResultID = Read16(p);
	UINT SrcUin = Read32(p);
	UINT DstUin = Read32(p);
	short WordLength = Read16(p);
	UCHAR Type = Read8(p);
	USHORT Src = Read16(p);
	NotifyAddFriendResult(Client,SrcUin, DstUin, Type, Src);
	NotifyAddFriendResult(GetClient(DstUin), SrcUin, DstUin, Type, Src);
}
void ResponseC2SReceiveTimesExperience(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	INT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT ReceiveTime = Read32(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);			// Result
	Write32(p, 1583430970); // SurplusTime
	Write32(p, 1683430970); // UseSurplusTime

	len = p - buf;
	SendToClient(Client, 20040, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseC2SFrostThawTimesExperience(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT Action = Read32(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Result

	len = p - buf;
	SendToClient(Client, 20041, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

// 经验加成
void ResponseC2SGetTimesExperienceInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	Write32(p, 0);			// Result
	Write32(p, 1683449937); // SurplusTime
	Write32(p, 1683536480); // UseSurplusTime
	Write32(p, 5);			// Multiple  几倍经验
	Write32(p, 1);			// Status
	Write32(p, 1683449937); // ExtralTime
	Write32(p, 3);			// PlayerTag
	Write32(p, 1683536480); // MaxExpTime
	Write32(p, 1683449937); // BaseTime
	Write32(p, 1683449937); // ServerTime

	len = p - buf;
	SendToClient(Client, 20042, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseGetTeamList(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT StartIdx = Read32(Body);
	UINT TeamCount = Read32(Body);
	bool Range = Read8(Body);
	int Score1 = Read32(Body);
	int Score2 = Read32(Body);

	char *pMsg = EByteToHexUA2((const unsigned char *)Body, BodyLen);
	Z_INFO("ResponseGetTeamList	Hex:%s\n", pMsg);

	Write16(p, 0);		   // ResultID
	Write32(p, 1000000);   // GuildScore
	Write32(p, StartIdx);  // StartIdx
	Write32(p, TeamCount); // TeamCount
	for (size_t i = 0; i < TeamCount; i++)
	{ // TeamInfo
		BYTE *pTeamInfo = p;
		Write16(pTeamInfo, 0); // len

		Write32(pTeamInfo, 0);	  // TeamID
		Write32(pTeamInfo, 1000); // GuildScore

		Write8(pTeamInfo, 17);

		char GuildName[] = "测试帮会";
		size_t GuildNameLength = strlen(GuildName);
		memcpy_s(pTeamInfo, 17, GuildName, GuildNameLength);
		pTeamInfo += 17;

		// WriteString(pTeamInfo, 0); //GuildName[]

		Write8(pTeamInfo, 17);

		char LeaderName[] = "测试排行榜";
		size_t LeaderNameLength = strlen(LeaderName);
		memcpy_s(pTeamInfo, 17, LeaderName, LeaderNameLength);
		pTeamInfo += 17;

		// WriteString(pTeamInfo, 0); //LeaderName[]

		len = pTeamInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 195, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseC2GGetSignInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	{ // BaseInfo
		BYTE *pBaseInfo = p;
		Write16(pBaseInfo, 0); // len

		Write8(pBaseInfo, 0);  // CurrentWeekSeq
		Write8(pBaseInfo, 0);  // CurrentWeekDaySeq
		Write32(pBaseInfo, 0); // LeftTime
		Write8(pBaseInfo, 0);  // CurrentSignNum
		Write8(pBaseInfo, 0);  // CurrentMatchNum
		Write8(pBaseInfo, 0);  // TotalMatchNum

		len = pBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // TipsInfo
		BYTE *pTipsInfo = p;
		Write16(pTipsInfo, 0); // len

		Write16(pTipsInfo, 0); // TipsLen

		len = pTipsInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // StatusEntityNum
	for (size_t i = 0; i < 0; i++)
	{ // StatusEntities
		BYTE *pStatusEntities = p;
		Write16(pStatusEntities, 0); // len

		Write8(pStatusEntities, 0); // WeekDaySeq
		Write8(pStatusEntities, 0); // SignStatus

		len = pStatusEntities - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // AwardEntityNum
	for (size_t i = 0; i < 0; i++)
	{ // AwardEntities
		BYTE *pAwardEntities = p;
		Write16(pAwardEntities, 0); // len

		Write8(pAwardEntities, 0); // AwardSeq
		Write8(pAwardEntities, 0); // AwardStatus
		Write8(pAwardEntities, 1); // TotalNum
		{						   // Award
			BYTE *pAward = pAwardEntities;
			Write16(pAward, 0); // len

			Write32(pAward, 0); // Exp
			Write32(pAward, 0); // Money
			Write32(pAward, 0); // SuperMoney
			Write16(pAward, 0); // ItemNum
			for (size_t i = 0; i < 0; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pAward;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, 127619); // ItemID
				Write32(pItemInfo, 1);		// ItemNum
				Write32(pItemInfo, 720);	// AvailPeriod
				Write8(pItemInfo, 0);		// Status
				Write32(pItemInfo, 0);		// ObtainTime
				Write32(pItemInfo, 0);		// OtherAttribute
				Write16(pItemInfo, 0);		// ItemType

				len = pItemInfo - pAward;
				Set16(pAward, (WORD)len);
				pAward += len;
			}
			Write32(pAward, 0); // Coupons
			Write32(pAward, 0); // GuildPoint
			Write32(pAward, 0); // LuckMoney
			Write8(pAward, 0);	// ExtendInfoNum
			for (size_t i = 0; i < 0; i++)
			{ // ExtendInfoAward
				BYTE *pExtendInfoAward = pAward;
				Write16(pExtendInfoAward, 0); // len

				Write16(pExtendInfoAward, 0); // Key
				Write32(pExtendInfoAward, 0); // AddValue
				Write32(pExtendInfoAward, 0); // TotalValue
				Write8(pExtendInfoAward, 0);  // ShowPriority
				Write16(pExtendInfoAward, 0); // BuyNeedScore
				Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

				len = pExtendInfoAward - pAward;
				Set16(pAward, (WORD)len);
				pAward += len;
			}
			Write32(pAward, 0); // SpeedCoin

			len = pAward - pAwardEntities;
			Set16(pAwardEntities, (WORD)len);
			pAwardEntities += len;
		}

		len = pAwardEntities - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // ControlAwardNum
	for (size_t i = 0; i < 0; i++)
	{ // ControlAwardEntities
		BYTE *pControlAwardEntities = p;
		Write16(pControlAwardEntities, 0); // len

		Write8(pControlAwardEntities, 0);		// WeekSeq
		Write32(pControlAwardEntities, 129285); // ItemID

		len = pControlAwardEntities - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 233, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseC2GSandglassInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			// ResultID
	Write32(p, 1683430970); // CurAccumulateTime
	{						// NotifyInfo
		BYTE *pNotifyInfo = p;
		Write16(pNotifyInfo, 0); // len

		Write8(pNotifyInfo, 0);	 // NotifyInGame
		Write32(pNotifyInfo, 0); // NotifyInGameTime
		Write8(pNotifyInfo, 0);	 // NotifyInQQ
		Write32(pNotifyInfo, 0); // NotifyInQQTime

		len = pNotifyInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 1); // SandglassNum  沙漏数量
	for (size_t i = 0; i < 1; i++)
	{ // SandglassCfgList
		BYTE *pSandglassCfgList = p;
		Write16(pSandglassCfgList, 0); // len

		Write32(pSandglassCfgList, 1783430970); // Time
		Write32(pSandglassCfgList, 9999);		// AwardExp

		len = pSandglassCfgList - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 237, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseGetShortCutSet(ClientNode *Client, BYTE *Body, size_t BodyLen)
{

	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	Write16(p, 0); // ResultID
	Write8(p, 64); // ShortCutSet  快捷方式集
	for (size_t i = 0; i < 64; i++)
	{									   // ShortCut[]
		Write16(p, GameServerShortCut[i]); // ShortCut[]
	}
	Write8(p, 0); // SuitToServer 适合服务器
	Write8(p, 0); // FixedShortCutNum
	// for (size_t i = 0; i < 64; i++)
	//{ //FixShortCut
	//	BYTE* pFixShortCut = p;
	//	Write16(pFixShortCut, 0); //len

	//	Write16(pFixShortCut, GameServerShortCut[i]); //ID
	//	Write8(pFixShortCut, 0); //New

	//	len = pFixShortCut - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}

	len = p - buf;
	SendToClient(Client, 11356, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseSetShortCutSet(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	/*char* pMsg = EByteToHexUA2((const unsigned char*)Body, BodyLen);
	printf("ResponseSetShortCutSet	Hex:%s\n", pMsg);*/

	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	// USHORT[] ShortCut[] = Read16(p);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID

	len = p - buf;
	SendToClient(Client, 11357, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseGetMagicLampCfg(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	const static UINT AwardItem[] = {122196, 116734, 115166, 98830, 74362, 119059};

	Write32(p, Uin); // Uin
	Write16(p, 0);	 // ResultID
	Write32(p, 1);	 // MagicLampNum 魔法灯数量
	Write16(p, 0);	 // DescribeLen
	{				 // MagicLampItemCfg
		BYTE *pMagicLampItemCfg = p;
		Write16(pMagicLampItemCfg, 0); // len

		Write16(pMagicLampItemCfg, 6); // ItemNum
		for (size_t ItemCfgi = 0; ItemCfgi < 6; ItemCfgi++)
		{ // MagicLampOneItemCfg
			BYTE *pMagicLampOneItemCfg = pMagicLampItemCfg;
			Write16(pMagicLampOneItemCfg, 0); // len

			Write16(pMagicLampOneItemCfg, 1); // RoundNum  本轮数量
			Write16(pMagicLampOneItemCfg, 1); // CurrentRound  本轮
			{								  // AwardToClient
				BYTE *pAwardToClient = pMagicLampOneItemCfg;
				Write16(pAwardToClient, 0); // len

				Write32(pAwardToClient, 0); // Exp
				Write32(pAwardToClient, 0); // Money
				Write32(pAwardToClient, 0); // SuperMoney
				Write16(pAwardToClient, 1); // ItemNum
				for (size_t i = 0; i < 1; i++)
				{ // ItemInfo
					BYTE *pItemInfo = pAwardToClient;
					Write16(pItemInfo, 0); // len

					Write32(pItemInfo, AwardItem[ItemCfgi]); // ItemID
					Write32(pItemInfo, 1);					 // ItemNum
					Write32(pItemInfo, -1);					 // AvailPeriod
					Write8(pItemInfo, 0);					 // Status
					Write32(pItemInfo, 0);					 // ObtainTime
					Write32(pItemInfo, 0);					 // OtherAttribute
					Write16(pItemInfo, 0);					 // ItemType

					len = pItemInfo - pAwardToClient;
					Set16(pAwardToClient, (WORD)len);
					pAwardToClient += len;
				}
				Write32(pAwardToClient, 0); // Coupons
				Write32(pAwardToClient, 0); // GuildPoint
				Write32(pAwardToClient, 0); // LuckMoney
				Write8(pAwardToClient, 0);	// ExtendInfoNum
				for (size_t i = 0; i < 0; i++)
				{ // ExtendInfoAward
					BYTE *pExtendInfoAward = pAwardToClient;
					Write16(pExtendInfoAward, 0); // len

					Write16(pExtendInfoAward, 0); // Key
					Write32(pExtendInfoAward, 0); // AddValue
					Write32(pExtendInfoAward, 0); // TotalValue
					Write8(pExtendInfoAward, 0);  // ShowPriority
					Write16(pExtendInfoAward, 0); // BuyNeedScore
					Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

					len = pExtendInfoAward - pAwardToClient;
					Set16(pAwardToClient, (WORD)len);
					pAwardToClient += len;
				}
				Write32(pAwardToClient, 0); // SpeedCoin

				len = pAwardToClient - pMagicLampOneItemCfg;
				Set16(pMagicLampOneItemCfg, (WORD)len);
				pMagicLampOneItemCfg += len;
			}
			Write8(pMagicLampOneItemCfg, 0); // IsCanFree 可以免费使用

			len = pMagicLampOneItemCfg - pMagicLampItemCfg;
			Set16(pMagicLampItemCfg, (WORD)len);
			pMagicLampItemCfg += len;
		}

		len = pMagicLampItemCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0);			// FreeNum 点灯免费次数
	Write32(p, 1683454550); // NextFreeTime  下次点灯免费次数刷新时间
	Write32(p, 0);			// MaxFreeNum 点灯免费最大次数

	len = p - buf;
	SendToClient(Client, 24507, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseMagicLampLightOn(ClientNode *Client, BYTE *Body, size_t BodyLen)
{

	/*char* pMsg = EByteToHexUA2((const unsigned char*)Body, BodyLen);
	printf("ResponseMagicLampLightOn	Hex:%s\n", pMsg); */

	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UCHAR LightType = Read8(Body); // 1 点灯1次 2 点灯5次  3 点灯20次
	UINT ItemID = Read32(Body);	   // 点 灯里面的奖励ID
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Uin);	  // Uin
	Write16(p, 0);		  // Result
	Write16(p, 0);		  // SuccNum 成功数量
	Write32(p, ItemID);	  // ItemID
	Write8(p, LightType); // LightType
	Write32(p, 1);		  // MagicLampNum 魔术灯数量
	Write8(p, 0);		  // HasFinalAward 有最终奖励
	Write16(p, 1);		  // AwardNum
	for (size_t i = 0; i < 1; i++)
	{ // AwardToClient
		BYTE *pAwardToClient = p;
		Write16(pAwardToClient, 0); // len

		Write32(pAwardToClient, 0); // Exp
		Write32(pAwardToClient, 0); // Money
		Write32(pAwardToClient, 0); // SuperMoney
		Write16(pAwardToClient, 1); // ItemNum
		for (size_t i = 0; i < 1; i++)
		{ // ItemInfo
			BYTE *pItemInfo = pAwardToClient;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, 66647); // ItemID
			Write32(pItemInfo, 1);	   // ItemNum
			Write32(pItemInfo, 0);	   // AvailPeriod
			Write8(pItemInfo, 0);	   // Status
			Write32(pItemInfo, 0);	   // ObtainTime
			Write32(pItemInfo, 0);	   // OtherAttribute
			Write16(pItemInfo, 0);	   // ItemType

			len = pItemInfo - pAwardToClient;
			Set16(pAwardToClient, (WORD)len);
			pAwardToClient += len;
		}
		Write32(pAwardToClient, 0); // Coupons
		Write32(pAwardToClient, 0); // GuildPoint
		Write32(pAwardToClient, 0); // LuckMoney
		Write8(pAwardToClient, 0);	// ExtendInfoNum
		for (size_t i = 0; i < 0; i++)
		{ // ExtendInfoAward
			BYTE *pExtendInfoAward = pAwardToClient;
			Write16(pExtendInfoAward, 0); // len

			Write16(pExtendInfoAward, 0); // Key
			Write32(pExtendInfoAward, 0); // AddValue
			Write32(pExtendInfoAward, 0); // TotalValue
			Write8(pExtendInfoAward, 0);  // ShowPriority
			Write16(pExtendInfoAward, 0); // BuyNeedScore
			Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

			len = pExtendInfoAward - pAwardToClient;
			Set16(pAwardToClient, (WORD)len);
			pAwardToClient += len;
		}
		Write32(pAwardToClient, 0); // SpeedCoin

		len = pAwardToClient - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 1);  // CostMagicLampNum 成本魔术灯数量
	Write16(p, 0); // FreeNum        免费次数
	Write32(p, 0); // NextFreeTime   下一次点灯时间

	len = p - buf;
	SendToClient(Client, 24508, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseGetMagicCupCfg(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	const static UINT CostItemNumGroup[] = {1, 10, 45};
	const static UINT AwardItemPollGroup[] = {126349, 123507, 119074, 113416, 109564};

	Write32(p, Uin); // Uin
	Write16(p, 0);	 // ResultID
	{				 // MagicCupSeasonCfg   魔法圣杯配置
		BYTE *pMagicCupSeasonCfg = p;
		Write16(pMagicCupSeasonCfg, 0); // len

		Write32(pMagicCupSeasonCfg, 1);			 // SeasonID
		Write32(pMagicCupSeasonCfg, 1678084039); // OpenDate
		Write32(pMagicCupSeasonCfg, 1678084039); // OpenTime
		Write32(pMagicCupSeasonCfg, 3382237639); // CloseDate
		Write32(pMagicCupSeasonCfg, 3382237639); // CloseTime

		len = pMagicCupSeasonCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 3); // LotteryDrawCfgNum  抽奖配置数量
	for (size_t i = 0; i < 3; i++)
	{ // LotteryDrawCfg
		BYTE *pLotteryDrawCfg = p;
		Write16(pLotteryDrawCfg, 0); // len

		Write32(pLotteryDrawCfg, 81011);			   // CostItemID  开启道具ID
		Write32(pLotteryDrawCfg, CostItemNumGroup[i]); // CostItemNum 开启道具数量
		Write32(pLotteryDrawCfg, (i + 1) * 66);		   // ProduceItemNum 分解零件数量
		Write32(pLotteryDrawCfg, 0);				   // AlreadyGotNum 已经得到数量

		len = pLotteryDrawCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 5); // AwardItemPoll   投票
	for (size_t i = 0; i < 5; i++)
	{ // AwardItemPollInfo   奖励项目投票
		BYTE *pAwardItemPollInfo = p;
		Write16(pAwardItemPollInfo, 0); // len

		Write32(pAwardItemPollInfo, AwardItemPollGroup[i]); // ItemID
		Write32(pAwardItemPollInfo, 1);						// ItemNum
		Write32(pAwardItemPollInfo, -1);					// AvailPeriod
		Write8(pAwardItemPollInfo, 0);						// Status
		Write32(pAwardItemPollInfo, 0);						// ObtainTime
		Write32(pAwardItemPollInfo, 0);						// OtherAttribute
		Write16(pAwardItemPollInfo, i);						// ItemType   0普通  1极品  2稀有 3稀有

		len = pAwardItemPollInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // PieceItemNum //分解零件道具数量
	Write32(p, 0); // PieceItemIDList[]  分解零件道具列表

	len = p - buf;
	SendToClient(Client, 24726, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseGetTempBoxItemInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Uin); // Uin
	Write16(p, 0);	 // ResultID
	Write16(p, 0);	 // CritItemNum
	std::map<UINT, Item*>::iterator iter;
	Write16(p, SkinItems.size()); // TempBoxItemNum
	for (iter = SkinItems.begin(); iter != SkinItems.end(); iter++)
		
	{ // MagicCupTempBoxItemInfo
		Item* pItem = iter->second;
		BYTE *pMagicCupTempBoxItemInfo = p;
		Write16(pMagicCupTempBoxItemInfo, 0); // len

		Write32(pMagicCupTempBoxItemInfo, pItem->ID); // CostItemID    开启道具ID
		Write32(pMagicCupTempBoxItemInfo, 1);	   // ItemType
		Write32(pMagicCupTempBoxItemInfo, 81012);  // ProducePieceID  产品零件ID
		Write32(pMagicCupTempBoxItemInfo, 66);	   // ProducePieceNum 产品零件数量
		Write32(pMagicCupTempBoxItemInfo, 0);	   // CostItemRecProb    成本项目回收测试

		len = pMagicCupTempBoxItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24727, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseMagicCupGetAward(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	USHORT Type = Read16(Body); // 0 抽奖1次 1抽奖10次 2抽奖50次
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	// printf("ResponseMagicCupGetAward	Type:%d\n", Type);

	Write32(p, Uin);  // Uin
	Write16(p, 0);	  // ResultID
	Write16(p, Type); // Type
	Write16(p, 1);	  // TempBoxItemNum
	{				  // AwardToClient
		BYTE *pAwardToClient = p;
		Write16(pAwardToClient, 0); // len

		Write32(pAwardToClient, 0); // Exp
		Write32(pAwardToClient, 0); // Money
		Write32(pAwardToClient, 0); // SuperMoney
		Write16(pAwardToClient, 1); // ItemNum
		for (size_t i = 0; i < 1; i++)
		{ // ItemInfo
			BYTE *pItemInfo = pAwardToClient;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, 126349); // ItemID
			Write32(pItemInfo, 1);		// ItemNum
			Write32(pItemInfo, -1);		// AvailPeriod
			Write8(pItemInfo, 0);		// Status
			Write32(pItemInfo, 0);		// ObtainTime
			Write32(pItemInfo, 0);		// OtherAttribute
			Write16(pItemInfo, 0);		// ItemType

			len = pItemInfo - pAwardToClient;
			Set16(pAwardToClient, (WORD)len);
			pAwardToClient += len;
		}
		Write32(pAwardToClient, 0); // Coupons
		Write32(pAwardToClient, 0); // GuildPoint
		Write32(pAwardToClient, 0); // LuckMoney
		Write8(pAwardToClient, 0);	// ExtendInfoNum
		for (size_t i = 0; i < 0; i++)
		{ // ExtendInfoAward
			BYTE *pExtendInfoAward = pAwardToClient;
			Write16(pExtendInfoAward, 0); // len

			Write16(pExtendInfoAward, 0); // Key
			Write32(pExtendInfoAward, 0); // AddValue
			Write32(pExtendInfoAward, 0); // TotalValue
			Write8(pExtendInfoAward, 0);  // ShowPriority
			Write16(pExtendInfoAward, 0); // BuyNeedScore
			Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

			len = pExtendInfoAward - pAwardToClient;
			Set16(pAwardToClient, (WORD)len);
			pAwardToClient += len;
		}
		Write32(pAwardToClient, 0); // SpeedCoin

		len = pAwardToClient - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24728, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseMagicCupExchangePiece(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	/*char* pMsg = EByteToHexUA2((const unsigned char*)Body, BodyLen);
	printf("ResponseMagicCupExchangePiece	Hex:%s\n", pMsg);*/

	// Hex:000027130C71682C00010001DD5B000040A0
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	USHORT ItemNum = Read16(Body);
	std::vector<UINT>ItemList;
	for(size_t i = 0;i< ItemNum;i++)
	{
		ItemList.insert(ItemList.end(), Read32(Body));
	}
	USHORT Type = Read16(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Uin); // Uin
	Write16(p, 0);	 // ResultID
	Write16(p, ItemList.size());	 // AwardItemNum
	for (size_t i = 0; i < ItemList.size(); i++)
	{ // AwardItemInfo
		ItemInfo Items[3000];
		ItemInfo Item = PlayerDB_AddItem2(Client->Uin, ItemList[i], 1, -1, false);
		Items[0].ItemID = ItemList[i];
		Items[0].ItemNum = Item.ItemNum;
		Items[0].AvailPeriod = Item.AvailPeriod;
		Items[0].Status = Item.Status;
		Items[0].ObtainTime = Item.ObtainTime;
		Items[0].OtherAttribute = Item.OtherAttribute;
		Items[0].ItemType = Item.ItemType;
		NotifyClientAddItem(Client, 1, Items);
		BYTE *pAwardItemInfo = p;
		Write16(pAwardItemInfo, 0); // len
		u8"新手赛车";
		Write32(pAwardItemInfo, ItemList[i]); // ItemID
		Write32(pAwardItemInfo, 1);	// ItemNum
		Write32(pAwardItemInfo, -1);		// AvailPeriod
		Write8(pAwardItemInfo, 0);		// Status
		Write32(pAwardItemInfo, 0);		// ObtainTime
		Write32(pAwardItemInfo, 0);		// OtherAttribute
		Write16(pAwardItemInfo, 1);		// ItemType

		len = pAwardItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, Type); // Type

	len = p - buf;
	SendToClient(Client, 24729, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseMagicHouseCallOn(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT MagicHouseID = Read32(Body);

	/*printf("ResponseMagicHouseCallOn MagicHouseID:%d\n", MagicHouseID);*/

	Write32(p, Uin); // Uin
	Write16(p, 0);	 // ResultID
	Write32(p, 0);	 // CostCallOnItemNums
	// Write32(p, 93343); //CallOnItemID  83073 76965 32867  93343
	{ // AwardInfo
		BYTE *pAwardInfo = p;
		Write16(pAwardInfo, 0); // len

		Write32(pAwardInfo, 98263); // ItemID
		Write32(pAwardInfo, 1);		// ItemNums
		Write32(pAwardInfo, -1);	// AvailPeriod
		Write16(pAwardInfo, 1);		// Received
		Write16(pAwardInfo, 0);		// AwardIndex
		Write16(pAwardInfo, 0);		// ItemType

		len = pAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0);			  // ReceiveAllAward  获得所有奖项
	Write32(p, MagicHouseID); // MagicHouseID
	Write32(p, 5);			  // CostCallOnItemNumsBeforDiscount  本次参与需要的召唤卡数量
	Write8(p, 0);			  // IsFree  是免费的

	len = p - buf;
	SendToClient(Client, 24471, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseGetDailyExtractShowInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	const static UINT AwardItemPollGroup[] = {126349, 123507, 119074, 113416, 109564, 109563};

	Write32(p, Uin); // Uin
	Write16(p, 0);	 // ResultID
	Write8(p, 1);	 // ChoosedAward  选择奖励
	Write16(p, 1);	 // ChooseAwardNums 选择奖励数量
	for (size_t i = 0; i < 1; i++)
	{ // ChooseAwardInfos
		BYTE *pChooseAwardInfos = p;
		Write16(pChooseAwardInfos, 0); // len

		Write32(pChooseAwardInfos, i); // ChooseIndex
		Write16(pChooseAwardInfos, 1); // AwardType
		{							   // AwardInfo
			BYTE *pAwardInfo = pChooseAwardInfos;
			Write16(pAwardInfo, 0); // len

			Write32(pAwardInfo, 0); // Exp
			Write32(pAwardInfo, 0); // Money
			Write32(pAwardInfo, 0); // SuperMoney
			Write16(pAwardInfo, 1); // ItemNum
			for (size_t i = 0; i < 1; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pAwardInfo;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, 126347); // ItemID  极星之翼
				Write32(pItemInfo, 1);		// ItemNum
				Write32(pItemInfo, -1);		// AvailPeriod
				Write8(pItemInfo, 0);		// Status
				Write32(pItemInfo, 0);		// ObtainTime
				Write32(pItemInfo, 0);		// OtherAttribute
				Write16(pItemInfo, 1);		// ItemType

				len = pItemInfo - pAwardInfo;
				Set16(pAwardInfo, (WORD)len);
				pAwardInfo += len;
			}
			Write32(pAwardInfo, 0); // Coupons
			Write32(pAwardInfo, 0); // GuildPoint
			Write32(pAwardInfo, 0); // LuckMoney
			Write8(pAwardInfo, 0);	// ExtendInfoNum
			for (size_t i = 0; i < 0; i++)
			{ // ExtendInfoAward
				BYTE *pExtendInfoAward = pAwardInfo;
				Write16(pExtendInfoAward, 0); // len

				Write16(pExtendInfoAward, 0); // Key
				Write32(pExtendInfoAward, 0); // AddValue
				Write32(pExtendInfoAward, 0); // TotalValue
				Write8(pExtendInfoAward, 0);  // ShowPriority
				Write16(pExtendInfoAward, 0); // BuyNeedScore
				Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

				len = pExtendInfoAward - pAwardInfo;
				Set16(pAwardInfo, (WORD)len);
				pAwardInfo += len;
			}
			Write32(pAwardInfo, 0); // SpeedCoin

			len = pAwardInfo - pChooseAwardInfos;
			Set16(pChooseAwardInfos, (WORD)len);
			pChooseAwardInfos += len;
		}
		Write8(pChooseAwardInfos, 0); // AlreayGotten
		Write8(pChooseAwardInfos, 0); // HaveAlreadyGottenPermanentItem

		len = pChooseAwardInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // ChoosedAwardInfo 选择的奖项信息
		BYTE *pChoosedAwardInfo = p;
		Write16(pChoosedAwardInfo, 0); // len

		Write32(pChoosedAwardInfo, 0); // ChooseIndex
		Write16(pChoosedAwardInfo, 0); // AwardType
		{							   // AwardInfo
			BYTE *pAwardInfo = pChoosedAwardInfo;
			Write16(pAwardInfo, 0); // len

			Write32(pAwardInfo, 0); // Exp
			Write32(pAwardInfo, 0); // Money
			Write32(pAwardInfo, 0); // SuperMoney
			Write16(pAwardInfo, 1); // ItemNum
			for (size_t i = 0; i < 1; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pAwardInfo;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, 120646); // ItemID  镇海之戟
				Write32(pItemInfo, 1);		// ItemNum
				Write32(pItemInfo, -1);		// AvailPeriod
				Write8(pItemInfo, 0);		// Status
				Write32(pItemInfo, 0);		// ObtainTime
				Write32(pItemInfo, 0);		// OtherAttribute
				Write16(pItemInfo, 1);		// ItemType

				len = pItemInfo - pAwardInfo;
				Set16(pAwardInfo, (WORD)len);
				pAwardInfo += len;
			}
			Write32(pAwardInfo, 0); // Coupons
			Write32(pAwardInfo, 0); // GuildPoint
			Write32(pAwardInfo, 0); // LuckMoney
			Write8(pAwardInfo, 0);	// ExtendInfoNum
			for (size_t i = 0; i < 0; i++)
			{ // ExtendInfoAward
				BYTE *pExtendInfoAward = pAwardInfo;
				Write16(pExtendInfoAward, 0); // len

				Write16(pExtendInfoAward, 0); // Key
				Write32(pExtendInfoAward, 0); // AddValue
				Write32(pExtendInfoAward, 0); // TotalValue
				Write8(pExtendInfoAward, 0);  // ShowPriority
				Write16(pExtendInfoAward, 0); // BuyNeedScore
				Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

				len = pExtendInfoAward - pAwardInfo;
				Set16(pAwardInfo, (WORD)len);
				pAwardInfo += len;
			}
			Write32(pAwardInfo, 0); // SpeedCoin

			len = pAwardInfo - pChoosedAwardInfo;
			Set16(pChoosedAwardInfo, (WORD)len);
			pChoosedAwardInfo += len;
		}
		Write8(pChoosedAwardInfo, 0); // AlreayGotten
		Write8(pChoosedAwardInfo, 0); // HaveAlreadyGottenPermanentItem

		len = pChoosedAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 5); // DailyExtarctAwardNums  每日Extarct奖励数量
	for (size_t Extarcti = 0; Extarcti < 5; Extarcti++)
	{ // DailyExtractAwardInfos  奖励列表
		BYTE *pDailyExtractAwardInfos = p;
		Write16(pDailyExtractAwardInfos, 0); // len

		Write32(pDailyExtractAwardInfos, 0); // ChooseIndex
		Write16(pDailyExtractAwardInfos, 0); // AwardType
		{									 // AwardInfo
			BYTE *pAwardInfo = pDailyExtractAwardInfos;
			Write16(pAwardInfo, 0); // len

			Write32(pAwardInfo, 0); // Exp
			Write32(pAwardInfo, 0); // Money
			Write32(pAwardInfo, 0); // SuperMoney
			Write16(pAwardInfo, 1); // ItemNum
			for (size_t i = 0; i < 1; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pAwardInfo;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, AwardItemPollGroup[Extarcti]); // ItemID
				Write32(pItemInfo, 1);							  // ItemNum
				Write32(pItemInfo, -1);							  // AvailPeriod
				Write8(pItemInfo, 0);							  // Status
				Write32(pItemInfo, 0);							  // ObtainTime
				Write32(pItemInfo, 0);							  // OtherAttribute
				Write16(pItemInfo, 1);							  // ItemType

				len = pItemInfo - pAwardInfo;
				Set16(pAwardInfo, (WORD)len);
				pAwardInfo += len;
			}
			Write32(pAwardInfo, 0); // Coupons
			Write32(pAwardInfo, 0); // GuildPoint
			Write32(pAwardInfo, 0); // LuckMoney
			Write8(pAwardInfo, 0);	// ExtendInfoNum
			for (size_t i = 0; i < 0; i++)
			{ // ExtendInfoAward
				BYTE *pExtendInfoAward = pAwardInfo;
				Write16(pExtendInfoAward, 0); // len

				Write16(pExtendInfoAward, 0); // Key
				Write32(pExtendInfoAward, 0); // AddValue
				Write32(pExtendInfoAward, 0); // TotalValue
				Write8(pExtendInfoAward, 0);  // ShowPriority
				Write16(pExtendInfoAward, 0); // BuyNeedScore
				Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

				len = pExtendInfoAward - pAwardInfo;
				Set16(pAwardInfo, (WORD)len);
				pAwardInfo += len;
			}
			Write32(pAwardInfo, 0); // SpeedCoin

			len = pAwardInfo - pDailyExtractAwardInfos;
			Set16(pDailyExtractAwardInfos, (WORD)len);
			pDailyExtractAwardInfos += len;
		}
		Write8(pDailyExtractAwardInfos, 0); // AlreayGotten  得到
		Write8(pDailyExtractAwardInfos, 0); // HaveAlreadyGottenPermanentItem  已经得到永久物品

		len = pDailyExtractAwardInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 1678084039); // OpenDate
	Write32(p, 3382237639); // CloseDate
	Write32(p, 1678084039); // OpenTime
	Write32(p, 3382237639); // CloseTime
	Write32(p, 7);			// CostItemNums  消耗多少张手气卡
	Write32(p, 7);			// CurrentCaiDanAwardNums
	Write8(p, 0);			// CanChangeChooseAward   可以更改选择奖项
	Write8(p, 0);			// HaveExtractedAllAward

	len = p - buf;
	SendToClient(Client, 25130, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseChooseAwardForDailyExtract(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT ChooseIndex = Read32(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Uin);		 // Uin
	Write16(p, 0);			 // ResultID
	Write32(p, ChooseIndex); // ChooseIndex

	len = p - buf;
	SendToClient(Client, 25132, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseDailyExtractInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	/*char* pMsg = EByteToHexUA2((const unsigned char*)Body, BodyLen);
	printf("ResponseDailyExtractInfo	Hex:%s\n", pMsg);*/

	Write32(p, Uin); // Uin
	Write16(p, 0);	 // ResultID
	{				 // AwardInfo
		BYTE *pAwardInfo = p;
		Write16(pAwardInfo, 0); // len

		Write32(pAwardInfo, 0); // Exp
		Write32(pAwardInfo, 0); // Money
		Write32(pAwardInfo, 0); // SuperMoney
		Write16(pAwardInfo, 1); // ItemNum
		for (size_t i = 0; i < 1; i++)
		{ // ItemInfo
			BYTE *pItemInfo = pAwardInfo;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, 119059); // ItemID
			Write32(pItemInfo, 1);		// ItemNum
			Write32(pItemInfo, -1);		// AvailPeriod
			Write8(pItemInfo, 0);		// Status
			Write32(pItemInfo, 0);		// ObtainTime
			Write32(pItemInfo, 0);		// OtherAttribute
			Write16(pItemInfo, 1);		// ItemType

			len = pItemInfo - pAwardInfo;
			Set16(pAwardInfo, (WORD)len);
			pAwardInfo += len;
		}
		Write32(pAwardInfo, 0); // Coupons
		Write32(pAwardInfo, 0); // GuildPoint
		Write32(pAwardInfo, 0); // LuckMoney
		Write8(pAwardInfo, 0);	// ExtendInfoNum
		for (size_t i = 0; i < 0; i++)
		{ // ExtendInfoAward
			BYTE *pExtendInfoAward = pAwardInfo;
			Write16(pExtendInfoAward, 0); // len

			Write16(pExtendInfoAward, 0); // Key
			Write32(pExtendInfoAward, 0); // AddValue
			Write32(pExtendInfoAward, 0); // TotalValue
			Write8(pExtendInfoAward, 0);  // ShowPriority
			Write16(pExtendInfoAward, 0); // BuyNeedScore
			Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

			len = pExtendInfoAward - pAwardInfo;
			Set16(pAwardInfo, (WORD)len);
			pAwardInfo += len;
		}
		Write32(pAwardInfo, 0); // SpeedCoin

		len = pAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 3); // CaiDanAwardNums
	Write32(p, 7); // CostItemNums
	Write8(p, 0);  // CanChangeChooseAward
	Write32(p, 0); // AwardType
	Write32(p, 0); // CurrentCaiDanAwardNums
	Write8(p, 0);  // HaveExtractedAllAward
	Write8(p, 0);  // HaveAlreadyGottenItem

	len = p - buf;
	SendToClient(Client, 25131, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseGetMagicCubesInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	const static UINT AwardList[] = {126349, 126348, 124686, 124685, 124684, 124682, 122201, 120652};

	Write32(p, Uin); // Uin
	Write16(p, 0);	 // ResultID
	{				 // OpenTime
		BYTE *pOpenTime = p;
		Write16(pOpenTime, 0); // len

		Write32(pOpenTime, 1);			// SeasonID
		Write32(pOpenTime, 1678084039); // BeginDate
		Write32(pOpenTime, 3382237639); // EndDate
		Write8(pOpenTime, 0);			// SegInDayNum
		for (size_t i = 0; i < 0; i++)
		{ // SegTimeInDay
			BYTE *pSegTimeInDay = pOpenTime;
			Write16(pSegTimeInDay, 0); // len

			Write32(pSegTimeInDay, 0); // BeginTime
			Write32(pSegTimeInDay, 0); // EndTime

			len = pSegTimeInDay - pOpenTime;
			Set16(pOpenTime, (WORD)len);
			pOpenTime += len;
		}
		Write8(pOpenTime, 0); // Mode

		len = pOpenTime - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 78623); // CandyItemID  糖果物品ID  橘子糖果
	Write32(p, 1);	   // CandyCost    糖果成本
	Write16(p, 8);	   // ItemNum
	for (size_t i = 0; i < 8; i++)
	{ // ItemInfo
		BYTE *pItemInfo = p;
		Write16(pItemInfo, 0); // len

		Write32(pItemInfo, AwardList[i]); // ItemID
		Write32(pItemInfo, 1);			  // ItemNum
		Write32(pItemInfo, -1);			  // AvailPeriod
		Write8(pItemInfo, 0);			  // Status
		Write32(pItemInfo, 0);			  // ObtainTime
		Write32(pItemInfo, 0);			  // OtherAttribute
		Write16(pItemInfo, 1);			  // ItemType

		len = pItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24642, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseGetAward(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UCHAR AwardType = Read8(Body);
	UCHAR Index = Read8(Body);
	UCHAR IndexType = Read8(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	const static UINT AwardList[] = {126349, 126348, 124686, 124685, 124684, 124682, 122201, 120652};

	Write32(p, Uin);	  // Uin
	Write16(p, 0);		  // ResultID
	Write8(p, AwardType); // AwardType
	Write8(p, Index);	  // Index
	{					  // Award
		BYTE *pAward = p;
		Write16(pAward, 0); // len

		Write32(pAward, 0); // Exp
		Write32(pAward, 0); // Money
		Write32(pAward, 0); // SuperMoney
		Write16(pAward, 1); // ItemNum
		for (size_t i = 0; i < 1; i++)
		{ // ItemInfo
			BYTE *pItemInfo = pAward;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, AwardList[Index]); // ItemID
			Write32(pItemInfo, 1);				  // ItemNum
			Write32(pItemInfo, -1);				  // AvailPeriod
			Write8(pItemInfo, 0);				  // Status
			Write32(pItemInfo, 0);				  // ObtainTime
			Write32(pItemInfo, 0);				  // OtherAttribute
			Write16(pItemInfo, 1);				  // ItemType

			len = pItemInfo - pAward;
			Set16(pAward, (WORD)len);
			pAward += len;
		}
		Write32(pAward, 0); // Coupons
		Write32(pAward, 0); // GuildPoint
		Write32(pAward, 0); // LuckMoney
		Write8(pAward, 0);	// ExtendInfoNum
		for (size_t i = 0; i < 0; i++)
		{ // ExtendInfoAward
			BYTE *pExtendInfoAward = pAward;
			Write16(pExtendInfoAward, 0); // len

			Write16(pExtendInfoAward, 0); // Key
			Write32(pExtendInfoAward, 0); // AddValue
			Write32(pExtendInfoAward, 0); // TotalValue
			Write8(pExtendInfoAward, 0);  // ShowPriority
			Write16(pExtendInfoAward, 0); // BuyNeedScore
			Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

			len = pExtendInfoAward - pAward;
			Set16(pAward, (WORD)len);
			pAward += len;
		}
		Write32(pAward, 0); // SpeedCoin

		len = pAward - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, IndexType); // IndexType

	len = p - buf;
	SendToClient(Client, 24308, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseShapeLegendOperate(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UCHAR OpType = Read8(Body);
	int ShapeID = Read32(Body);
	int KartID = 0;
	UCHAR ShapeLevel = Read8(Body);
	int NewShowChoice = Read32(Body);
	if (NewShowChoice == 0)
	{
		ResponseShapeLegendOperate2(Client, ShapeID, ShapeLevel, NewShowChoice);
		return;
	}
	// printf("ResponseShapeLegendOperate Uin:%d OpType:%d ShapeID:%d ShapeLevel:%d NewShowChoice:%d\n", Uin, OpType, ShapeID, ShapeLevel, NewShowChoice);
	UINT ShapeIDlist[] = {118727, 120247, 120248, 118728, 118729};
	UINT ShapeIDIndex = 0;
	ShapeLevel = 10;
	UINT DirectMaxLevelShapeID = 0;
	switch (ShapeID)
	{
	case 120246:
		KartID = 74362;
		DirectMaxLevelShapeID = 118729;
		switch (NewShowChoice)
		{
		case 2:
			ShapeIDIndex = 118727;
			break;
		case 4:
			ShapeIDIndex = 120247;
			break;
		case 8:
			ShapeIDIndex = 120248;
			break;
		case 112:
			ShapeIDIndex = 118728;
			break;
		case 1920:
			ShapeIDIndex = 118729;
			break;
		}
		break;
	case 127359:
		KartID = 12720;
		DirectMaxLevelShapeID = 126537;
		switch (NewShowChoice)
		{
		case 2:
			ShapeIDIndex = 126533;
			break;
		case 4:
			ShapeIDIndex = 126534;
			break;
		case 8:
			ShapeIDIndex = 126535;
			break;
		case 112:
			ShapeIDIndex = 126536;
			break;
		case 1920:
			ShapeIDIndex = 126537;
			break;
		}
		break;
	case 127360:
		KartID = 90945;
		DirectMaxLevelShapeID = 127365;
		switch (NewShowChoice)
		{
		case 2:
			ShapeIDIndex = 127361;
			break;
		case 4:
			ShapeIDIndex = 127362;
			break;
		case 8:
			ShapeIDIndex = 127363;
			break;
		case 112:
			ShapeIDIndex = 127364;
			break;
		case 1920:
			ShapeIDIndex = 127365;
			break;
		}
		break;
	case 123513:
		KartID = 123512;
		DirectMaxLevelShapeID = 123238;
		ShapeLevel = 5;
		switch (NewShowChoice)
		{
		case 2:
			ShapeIDIndex = 123237;
			break;
		case 6:
			ShapeIDIndex = 123237;
			break;
		case 14:
			ShapeIDIndex = 123237;
			break;
		case 30:
			ShapeIDIndex = 123237;
			break;
		case 32:
			ShapeIDIndex = 123238;
			break;
		}
		break;
	case 126355:
		KartID = 126345;
		DirectMaxLevelShapeID = 127437;
		ShapeLevel = 5;
		switch (NewShowChoice)
		{
		case 2:
			ShapeIDIndex = 127436;
			break;
		case 6:
			ShapeIDIndex = 127436;
			break;
		case 14:
			ShapeIDIndex = 127436;
			break;
		case 30:
			ShapeIDIndex = 127436;
			break;
		case 32:
			ShapeIDIndex = 127437;
			break;
		}
		break;
	}

	Write16(p, 0);	   // ResultID
	Write8(p, 0);	   // ReasonLen
	Write8(p, OpType); // OpType
	Write16(p, 5);	   // ShapeNum
	for (size_t i = 0; i < 5; i++)
	{ // ShapeInfo
		BYTE *pShapeInfo = p;
		Write16(pShapeInfo, 0); // len

		Write32(pShapeInfo, ShapeIDlist[i]); // ShapeID
		Write8(pShapeInfo, ShapeLevel);		 // ShapeLevel
		Write32(pShapeInfo, i);				 // ShowChoice 显示选项

		len = pShapeInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, ShapeID);   // ShapeID
	Write8(p, ShapeLevel); // ShapeLevel
	Write8(p, 0);		   // HasRefit 有改装信息
	for (size_t i = 0; i < 0; i++)
	{ // RefitClientInfo
		BYTE *pRefitClientInfo = p;
		Write16(pRefitClientInfo, 0); // len

		Write32(pRefitClientInfo, 0); // Uin
		Write32(pRefitClientInfo, 0); // KartId
		Write32(pRefitClientInfo, 0); // RefitCout
		Write16(pRefitClientInfo, 0); // MaxFlags
		Write16(pRefitClientInfo, 0); // WWeight
		Write16(pRefitClientInfo, 0); // SpeedWeight
		Write16(pRefitClientInfo, 0); // JetWeight
		Write16(pRefitClientInfo, 0); // SJetWeight
		Write16(pRefitClientInfo, 0); // AccuWeight
		Write32(pRefitClientInfo, 0); // ShapeRefitCount
		Write32(pRefitClientInfo, 0); // KartHeadRefitItemID
		Write32(pRefitClientInfo, 0); // KartTailRefitItemID
		Write32(pRefitClientInfo, 0); // KartFlankRefitItemID
		Write32(pRefitClientInfo, 0); // KartTireRefitItemID
		{							  // KartRefitExInfo
			BYTE *pKartRefitExInfo = pRefitClientInfo;
			Write16(pKartRefitExInfo, 0); // len

			Write8(pKartRefitExInfo, 0); // SpeedRefitStar
			Write8(pKartRefitExInfo, 0); // JetRefitStar
			Write8(pKartRefitExInfo, 0); // SJetRefitStar
			Write8(pKartRefitExInfo, 0); // AccuRefitStar
			Write8(pKartRefitExInfo, 0); // SpeedAddRatio
			Write8(pKartRefitExInfo, 0); // JetAddRatio
			Write8(pKartRefitExInfo, 0); // SJetAddRatio
			Write8(pKartRefitExInfo, 0); // AccuAddRatio

			len = pKartRefitExInfo - pRefitClientInfo;
			Set16(pRefitClientInfo, (WORD)len);
			pRefitClientInfo += len;
		}
		Write32(pRefitClientInfo, 0); // SecondRefitCount
		Write16(pRefitClientInfo, 0); // Speed2Weight
		Write16(pRefitClientInfo, 0); // DriftVecWeight
		Write16(pRefitClientInfo, 0); // AdditionalZSpeedWeight
		Write16(pRefitClientInfo, 0); // AntiCollisionWeight
		Write16(pRefitClientInfo, 0); // LuckyValue
		Write16(pRefitClientInfo, 0); // RefitLuckyValueMaxWeight
		Write32(pRefitClientInfo, 0); // ShapeSuitID
		Write8(pRefitClientInfo, 0);  // LegendSuitLevel
		Write32(pRefitClientInfo, 0); // LegendSuitLevelChoice
		Write32(pRefitClientInfo, 0); // ShapeLegendSuitID

		len = pRefitClientInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, NewShowChoice);		   // NewShowChoice 新展会选择
	Write32(p, DirectMaxLevelShapeID); // DirectMaxLevelShapeID   直接最大级别形状ID
	Write32(p, ShapeLevel);			   // DirectMaxShapePreLevel  直接最大形状预级别
	const char *sql = NULL;

	sqlite3_stmt *stmt = NULL;
	int result;
	sql = "UPDATE KartRefit SET LegendSuitLevel=?,LegendSuitLevelChoice=?,ShapeSuitID=?,ShapeLegendSuitID=? WHERE Uin=? AND KartID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, ShapeLevel);
		sqlite3_bind_int(stmt, 2, NewShowChoice);
		sqlite3_bind_int(stmt, 3, ShapeIDIndex);

		sqlite3_bind_int(stmt, 4, ShapeID);
		sqlite3_bind_int(stmt, 5, Client->Uin);
		sqlite3_bind_int(stmt, 6, KartID);
		result = sqlite3_step(stmt);
		Z_INFO("结果:%d", result);
	}
	else
	{
		Z_ERROR("皮肤改装错误");
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	len = p - buf;
	SendToClient(Client, 319, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	if (ShapeIDIndex > 0)
	{
		if (Client->KartID == 74362 && Client->SkinID != 116735 && Client->SkinID != 110709 && Client->SkinID > 0)
		{
			ResponseSaveShapeRefit2(Client, Client->KartID, ShapeIDIndex);
		}
		if (Client->KartID == 12720 && Client->SkinID != 123506 && Client->SkinID > 0)
		{
			ResponseSaveShapeRefit2(Client, Client->KartID, ShapeIDIndex);
		}
		if (Client->KartID == 90945 && Client->SkinID != 110711 && Client->SkinID != 117941 && Client->SkinID != 122199 && Client->SkinID > 0)
		{
			ResponseSaveShapeRefit2(Client, Client->KartID, ShapeIDIndex);
		}
		if (Client->KartID == 123513)
		{
			ResponseSaveShapeRefit2(Client, Client->KartID, ShapeIDIndex);
		}
		if (Client->KartID == 126345)
		{
			ResponseSaveShapeRefit2(Client, Client->KartID, ShapeIDIndex);
		}
	}
}
void RequestShapeLegendOperate(ClientNode* Client, BYTE* Body, size_t BodyLen) {//皮肤进阶
	BYTE buf[8192]; BYTE* p = buf; size_t len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UCHAR OpType = Read8(Body);
	int ShapeID = Read32(Body);
	UCHAR ShapeLevel = Read8(Body);
	int NewShowChoice = Read32(Body);
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	int KartID = 0;
	int SkinLevel = 0;
	int lantuid = 0;
	int nengyuanid = 0;
	YAML::Node Level;
	ItemInfo Items[1]{};
	std::string Reason = "";

	if (ShapeID == 127359) {
		KartID = 12720;
		lantuid = 126538;
		nengyuanid = 126540;
		Level = YAML::LoadFile(".\\KartSkin\\127359.yml");
	}
	else if (ShapeID == 120246) {
		KartID = 74362;
		lantuid = 116759;
		nengyuanid = 116761;
		Level = YAML::LoadFile(".\\KartSkin\\120246.yml");
	}
	else if (ShapeID == 127360) {
		KartID = 90945;
		lantuid = 127350;
		nengyuanid = 127353;
		Level = YAML::LoadFile(".\\KartSkin\\127360.yml");
	}
	/*if(OpType == 2){//更换形态
		  if(NewShowChoice == 2){
			 SkinLevel = 1;
		   }else if(NewShowChoice == 4){
			 SkinLevel = 2;
		   }else if(NewShowChoice == 8){
			 SkinLevel = 3;
		   }else if(NewShowChoice == 16){
			 SkinLevel = 4;
		   }else if(NewShowChoice == 48){
			 SkinLevel = 5;
		   }else if(NewShowChoice == 112){
			 SkinLevel = 6;
		   }else if(NewShowChoice == 128){
			 SkinLevel = 7;
		   }else if(NewShowChoice == 384){
			 SkinLevel = 8;
		   }else if(NewShowChoice == 896){
			 SkinLevel = 9;
		   }else{
			 SkinLevel = 10;
		   }*/
		   //升级
	if (OpType == 1) {
		if (PlayerDB_GetItemNum1(Uin, lantuid) < Level["ShapeSuitLevel"][ShapeLevel - 1]["lantu"].as<int>() || PlayerDB_GetItemNum(Uin, nengyuanid) < Level["ShapeSuitLevel"][ShapeLevel - 1]["nengyuan"].as<int>() || !PlayerDB_GetItemNum1(Uin, KartID)) {
			Reason = "/C01你好像还没有这个车或者材料不足！！!";
			ResponseStartGame(Client, Reason.c_str());
			return;
			ShapeLevel = 0;

		}
		else {

			if (PlayerDB_GetItemNum1(Uin, lantuid) == Level["ShapeSuitLevel"][ShapeLevel - 1]["lantu"].as<int>()) {
				PlayerDB_DelItem(Uin, lantuid);
			}
			else {
				PlayerDB_Reduce(Uin, lantuid, Level["ShapeSuitLevel"][ShapeLevel - 1]["lantu"].as<int>());
			}

			Items[0].ItemID = lantuid;
			Items[0].ItemNum = PlayerDB_GetItemNum1(Uin, lantuid) - Level["ShapeSuitLevel"][ShapeLevel - 1]["lantu"].as<int>();
			Items[0].AvailPeriod = 0;
			Items[0].ObtainTime = Time;
			Items[0].Status = 0;
			Items[0].OtherAttribute = 0;
			Items[0].ItemType = 0;
			NotifyClientAddItem(Client, 1, Items);
			if (PlayerDB_GetItemNum1(Uin, nengyuanid) == Level["ShapeSuitLevel"][ShapeLevel - 1]["nengyuan"].as<int>()) {
				PlayerDB_DelItem(Uin, nengyuanid);
			}
			else {
				PlayerDB_Reduce(Uin, nengyuanid, Level["ShapeSuitLevel"][ShapeLevel - 1]["nengyuan"].as<int>());
			}
			Items[0].ItemID = nengyuanid;
			Items[0].ItemNum = PlayerDB_GetItemNum1(Uin, nengyuanid) - Level["ShapeSuitLevel"][ShapeLevel - 1]["nengyuan"].as<int>();
			Items[0].AvailPeriod = 0;
			Items[0].ObtainTime = Time;
			Items[0].Status = 0;
			Items[0].OtherAttribute = 0;
			Items[0].ItemType = 0;
			NotifyClientAddItem(Client, 1, Items);

			if (ShapeLevel == 1) {
				PlayerDB_AddItem(Uin, ShapeID, 1, -1, false);
				Items[0].ItemID = ShapeID;
				Items[0].ItemNum = 1;
				Items[0].AvailPeriod = -1;
				Items[0].ObtainTime = Time;
				Items[0].Status = 0;
				Items[0].OtherAttribute = 0;
				Items[0].ItemType = 0;

				NotifyLoginMoreInfo(Client, 1, Items, false);
				NotifyClientAddItem(Client, 1, Items);

			}
			sql = "UPDATE KartRefit SET LegendSuitLevel=? WHERE Uin=? and KartID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK) {
				sqlite3_bind_int(stmt, 1, ShapeLevel);
				sqlite3_bind_int(stmt, 2, Uin);
				sqlite3_bind_int(stmt, 3, KartID);
				result = sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
	}
	//更换形态
	if (OpType == 2) {
		if (NewShowChoice == 2) {
			SkinLevel = 1;
		}
		else if (NewShowChoice == 4) {
			SkinLevel = 2;
		}
		else if (NewShowChoice == 8) {
			SkinLevel = 3;
		}
		else if (NewShowChoice == 16) {
			SkinLevel = 4;
		}
		else if (NewShowChoice == 48) {
			SkinLevel = 5;
		}
		else if (NewShowChoice == 112) {
			SkinLevel = 6;
		}
		else if (NewShowChoice == 128) {
			SkinLevel = 7;
		}
		else if (NewShowChoice == 384) {
			SkinLevel = 8;
		}
		else if (NewShowChoice == 896) {
			SkinLevel = 9;
		}
		else {
			SkinLevel = 10;
		}
		sql = "UPDATE KartRefit SET ShapeSuitID=?,LegendSuitLevelChoice=?,ShapeLegendSuitID=? WHERE Uin=? and KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Level["ShapeSuitLevel"][SkinLevel - 1]["SuitID"].as<int>());
			sqlite3_bind_int(stmt, 2, NewShowChoice);
			sqlite3_bind_int(stmt, 3, ShapeID);
			sqlite3_bind_int(stmt, 4, Uin);
			sqlite3_bind_int(stmt, 5, KartID);
			result = sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;

		sql = "SELECT LegendSuitLevel  FROM KartRefit WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, KartID);
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				ShapeLevel = sqlite3_column_int(stmt, 0);
			}

		}
		sqlite3_finalize(stmt);
		stmt = NULL;


		//皮肤切换处理2.0
		if (Client->KartID == KartID) {
			ItemStatus* aItemStatus = (ItemStatus*)malloc(sizeof(ItemStatus) * 2);

			if (ShapeID != 0) {
				//不为0则为切皮肤
				//切为无s标志的皮肤车
				aItemStatus[0].ItemID = KartID;
				aItemStatus[0].NewStatus = 0;
				aItemStatus[1].ItemID = Level["ShapeSuitLevel"][SkinLevel - 1]["SuitID"].as<int>();
				aItemStatus[1].NewStatus = 1;
				Client->SkinID = Level["ShapeSuitLevel"][SkinLevel - 1]["SuitID"].as<int>();
				Client->OrgKartId = KartID;
				ClientNode* temp = GetClient(Client->Uin);
				temp->SkinID = Level["ShapeSuitLevel"][SkinLevel - 1]["SuitID"].as<int>();
				temp->OrgKartId = KartID;
			}
			else {
				//为0则为切原车
				aItemStatus[0].ItemID = KartID;
				aItemStatus[0].NewStatus = 0;
				aItemStatus[1].ItemID = Client->SkinID;
				aItemStatus[1].NewStatus = 1;
				//Client->SkinID = 0;
				Client->OrgKartId = 10020;
				ClientNode* temp = GetClient(Client->Uin);
				//temp->SkinID = 0;
				//temp->OrgKartId = 0;
			}




			RoomNode* Room = GetRoom(Client->RoomID);
			if (!Room)
			{
				// NotifyChangeItemStatus(Client, Client->Uin, 2, aItemStatus);
			}
			else
			{
				for (char i = 0; i < 6; i++)
				{
					ClientNode* RoomClient = Room->Player[i];
					if (RoomClient && RoomClient != Client) {
						NotifyChangeItemStatus2(RoomClient, Client->Uin, 2, aItemStatus);
						if (ShapeID != 0) {
							//切回带皮肤带s标的原车
							aItemStatus[0].ItemID = Level["ShapeSuitLevel"][SkinLevel - 1]["SuitID"].as<int>();
							aItemStatus[0].NewStatus = 0;
							aItemStatus[1].ItemID = KartID;
							aItemStatus[1].NewStatus = 1;
						}
						else {
							aItemStatus[0].ItemID = Client->SkinID;
							aItemStatus[0].NewStatus = 0;
							aItemStatus[1].ItemID = KartID;
							aItemStatus[1].NewStatus = 1;
							Client->SkinID = 0;
							Client->OrgKartId = 0;
							ClientNode* temp = GetClient(Client->Uin);
							temp->SkinID = 0;
							temp->OrgKartId = 0;
						}
						NotifyChangeItemStatus2(RoomClient, Client->Uin, 2, aItemStatus);

					}

				}
			}
		}
		// 到这里皮肤就处理完了
	}



	Write16(p, 0); //ResultID
	Write8(p, 0); //ReasonLen
	Write8(p, OpType); //OpType
	Write16(p, 2); //ShapeNum 用户皮肤等级
	//for (size_t i = 0; i < 0; i++)
	{ //ShapeInfo
		BYTE* pShapeInfo = p;
		Write16(pShapeInfo, 0); //len
		Write32(pShapeInfo, 120246); //ShapeID
		sql = "SELECT LegendSuitLevel,LegendSuitLevelChoice  FROM KartRefit WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, 74362);
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				Write8(pShapeInfo, sqlite3_column_int(stmt, 0)); //ShapeLevel
				Write32(pShapeInfo, sqlite3_column_int(stmt, 1));
			}
			else {
				Write8(pShapeInfo, 0); //ShapeLevel
				Write32(pShapeInfo, 0);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		len = pShapeInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ //ShapeInfo
		BYTE* pShapeInfo = p;
		Write16(pShapeInfo, 0); //len
		Write32(pShapeInfo, 127359); //ShapeID
		sql = "SELECT LegendSuitLevel,LegendSuitLevelChoice  FROM KartRefit WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, 12720);
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				Write8(pShapeInfo, sqlite3_column_int(stmt, 0)); //ShapeLevel
				Write32(pShapeInfo, sqlite3_column_int(stmt, 1));
			}
			else {
				Write8(pShapeInfo, 0); //ShapeLevel
				Write32(pShapeInfo, 0);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		len = pShapeInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, ShapeID); //ShapeID外形ID
	Write8(p, ShapeLevel); //ShapeLevel


	//特殊段落(写法草懒得优化)
	if (OpType == 2) {
		Write8(p, 1); //HasRefit
		{
			BYTE* pRefitClientInfo = p;
			Write16(pRefitClientInfo, 0); //len

			Write32(pRefitClientInfo, Uin); //Uin
			Write32(pRefitClientInfo, KartID); //KartId

			Write32(pRefitClientInfo, 0); //RefitCout
			Write16(pRefitClientInfo, 0); //MaxFlags
			Write16(pRefitClientInfo, 0); //WWeight
			Write16(pRefitClientInfo, 0); //SpeedWeight
			Write16(pRefitClientInfo, 0); //JetWeight
			Write16(pRefitClientInfo, 0); //SJetWeight
			Write16(pRefitClientInfo, 0); //AccuWeight

			Write32(pRefitClientInfo, 0); //ShapeRefitCount
			Write32(pRefitClientInfo, 0); //KartHeadRefitItemID
			Write32(pRefitClientInfo, 0); //KartTailRefitItemID
			Write32(pRefitClientInfo, 0); //KartFlankRefitItemID
			Write32(pRefitClientInfo, 0); //KartTireRefitItemID
			{ //KartRefitExInfo
				BYTE* pKartRefitExInfo = pRefitClientInfo;
				Write16(pKartRefitExInfo, 0); //len

				Write8(pKartRefitExInfo, 3); //SpeedRefitStar
				Write8(pKartRefitExInfo, 3); //JetRefitStar
				Write8(pKartRefitExInfo, 3); //SJetRefitStar
				Write8(pKartRefitExInfo, 3); //AccuRefitStar
				Write8(pKartRefitExInfo, 10); //SpeedAddRatio
				Write8(pKartRefitExInfo, 10); //JetAddRatio
				Write8(pKartRefitExInfo, 10); //SJetAddRatio
				Write8(pKartRefitExInfo, 10); //AccuAddRatio

				len = pKartRefitExInfo - pRefitClientInfo;
				Set16(pRefitClientInfo, (WORD)len);
				pRefitClientInfo += len;
			}

			Write32(pRefitClientInfo, 0); //SecondRefitCount
			Write16(pRefitClientInfo, 0); //Speed2Weight
			Write16(pRefitClientInfo, 0); //DriftVecWeight
			Write16(pRefitClientInfo, 0); //AdditionalZSpeedWeight
			Write16(pRefitClientInfo, 0); //AntiCollisionWeight
			Write16(pRefitClientInfo, 0); //LuckyValue
			Write16(pRefitClientInfo, 0); //RefitLuckyValueMaxWeight
			Write32(pRefitClientInfo, Level["ShapeSuitLevel"][SkinLevel - 1]["SuitID"].as<int>()); //ShapeSuitID
			Write8(pRefitClientInfo, ShapeLevel); //LegendSuitLevel
			Write32(pRefitClientInfo, NewShowChoice); //LegendSuitLevelChoice
			Write32(pRefitClientInfo, ShapeID); //ShapeLegendSuitID.

			len = pRefitClientInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	else {
		Write8(p, 2); //HasRefit

		for (size_t i = 0; i < 2; i++)
		{//RefitClientInfo
			if (i == 1) {
				KartID = 12720;
			}
			else {
				KartID = 74362;
			}
			BYTE* pRefitClientInfo = p;
			Write16(pRefitClientInfo, 0); //len

			Write32(pRefitClientInfo, Uin); //Uin
			Write32(pRefitClientInfo, KartID); //KartId

			Write32(pRefitClientInfo, 0); //RefitCout
			Write16(pRefitClientInfo, 0); //MaxFlags
			Write16(pRefitClientInfo, 0); //WWeight
			Write16(pRefitClientInfo, 0); //SpeedWeight
			Write16(pRefitClientInfo, 0); //JetWeight
			Write16(pRefitClientInfo, 0); //SJetWeight
			Write16(pRefitClientInfo, 0); //AccuWeight

			Write32(pRefitClientInfo, 0); //ShapeRefitCount
			Write32(pRefitClientInfo, 0); //KartHeadRefitItemID
			Write32(pRefitClientInfo, 0); //KartTailRefitItemID
			Write32(pRefitClientInfo, 0); //KartFlankRefitItemID
			Write32(pRefitClientInfo, 0); //KartTireRefitItemID
			{ //KartRefitExInfo
				BYTE* pKartRefitExInfo = pRefitClientInfo;
				Write16(pKartRefitExInfo, 0); //len

				Write8(pKartRefitExInfo, 0); //SpeedRefitStar
				Write8(pKartRefitExInfo, 0); //JetRefitStar
				Write8(pKartRefitExInfo, 0); //SJetRefitStar
				Write8(pKartRefitExInfo, 0); //AccuRefitStar
				Write8(pKartRefitExInfo, 0); //SpeedAddRatio
				Write8(pKartRefitExInfo, 0); //JetAddRatio
				Write8(pKartRefitExInfo, 0); //SJetAddRatio
				Write8(pKartRefitExInfo, 0); //AccuAddRatio

				len = pKartRefitExInfo - pRefitClientInfo;
				Set16(pRefitClientInfo, (WORD)len);
				pRefitClientInfo += len;
			}

			Write32(pRefitClientInfo, 0); //SecondRefitCount
			Write16(pRefitClientInfo, 0); //Speed2Weight
			Write16(pRefitClientInfo, 0); //DriftVecWeight
			Write16(pRefitClientInfo, 0); //AdditionalZSpeedWeight
			Write16(pRefitClientInfo, 0); //AntiCollisionWeight
			Write16(pRefitClientInfo, 0); //LuckyValue
			Write16(pRefitClientInfo, 0); //RefitLuckyValueMaxWeight
			Write32(pRefitClientInfo, 0); //ShapeSuitID
			Write8(pRefitClientInfo, 0); //LegendSuitLevel
			Write32(pRefitClientInfo, 0); //LegendSuitLevelChoice
			Write32(pRefitClientInfo, 0); //ShapeLegendSuitID.

			len = pRefitClientInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}




	Write32(p, NewShowChoice); //NewShowChoice
	Write32(p, 0); //DirectMaxLevelShapeID 直接最大级别形状ID
	Write32(p, 0); //DirectMaxShapePreLevel 直接最大形状预级别

	len = p - buf;
	SendToClient(Client, 319, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseSystemMSG(ClientNode* Client, const char* Reason)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, -1); //ResultID

	len = strlen(Reason);
	Write8(p, (BYTE)len); //ReasonLen
	memcpy(p, Reason, len);
	p += len;

	len = p - buf;
	SendToClient(Client, 110, buf, len, Client->RoomID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseShapeLegendOperate2(ClientNode *Client, int ShapeID, UCHAR ShapeLevel, int NewShowChoice)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UCHAR OpType = 3;
	int KartID = 0;

	// printf("ResponseShapeLegendOperate Uin:%d OpType:%d ShapeID:%d ShapeLevel:%d NewShowChoice:%d\n", Uin, OpType, ShapeID, ShapeLevel, NewShowChoice);
	UINT ShapeIDlist[] = {118727, 120247, 120248, 118728, 118729};
	UINT ShapeIDIndex = 0;
	UINT DirectMaxLevelShapeID = 0;
	switch (ShapeID)
	{
	case 120246:
		KartID = 74362;
		DirectMaxLevelShapeID = 118729;
		switch (NewShowChoice)
		{
		case 2:
			ShapeIDIndex = 118727;
			break;
		case 4:
			ShapeIDIndex = 120247;
			break;
		case 8:
			ShapeIDIndex = 120248;
			break;
		case 112:
			ShapeIDIndex = 118728;
			break;
		case 1920:
			ShapeIDIndex = 118729;
			break;
		}
		break;
	case 127359:
		KartID = 12720;
		DirectMaxLevelShapeID = 126537;
		switch (NewShowChoice)
		{
		case 2:
			ShapeIDIndex = 126533;
			break;
		case 4:
			ShapeIDIndex = 126534;
			break;
		case 8:
			ShapeIDIndex = 126535;
			break;
		case 112:
			ShapeIDIndex = 126536;
			break;
		case 1920:
			ShapeIDIndex = 126537;
			break;
		}
		break;
	case 127360:
		KartID = 90945;
		DirectMaxLevelShapeID = 127365;
		switch (NewShowChoice)
		{
		case 2:
			ShapeIDIndex = 127361;
			break;
		case 4:
			ShapeIDIndex = 127362;
			break;
		case 8:
			ShapeIDIndex = 127363;
			break;
		case 112:
			ShapeIDIndex = 127364;
			break;
		case 1920:
			ShapeIDIndex = 127365;
			break;
		}
		break;
	case 123513:
		KartID = 123512;
		DirectMaxLevelShapeID = 123238;
		ShapeLevel = 5;
		switch (NewShowChoice)
		{
		case 2:
			ShapeIDIndex = 123237;
			break;
		case 6:
			ShapeIDIndex = 123237;
			break;
		case 14:
			ShapeIDIndex = 123237;
			break;
		case 30:
			ShapeIDIndex = 123237;
			break;
		case 32:
			ShapeIDIndex = 123238;
			break;
		}
		break;
	case 126355:
		KartID = 126345;
		DirectMaxLevelShapeID = 127437;
		ShapeLevel = 5;
		switch (NewShowChoice)
		{
		case 2:
			ShapeIDIndex = 127436;
			break;
		case 6:
			ShapeIDIndex = 127436;
			break;
		case 14:
			ShapeIDIndex = 127436;
			break;
		case 30:
			ShapeIDIndex = 127436;
			break;
		case 32:
			ShapeIDIndex = 127437;
			break;
		}
		break;
	}

	Write16(p, 0);	   // ResultID
	Write8(p, 0);	   // ReasonLen
	Write8(p, OpType); // OpType
	Write16(p, 5);	   // ShapeNum
	for (size_t i = 0; i < 5; i++)
	{ // ShapeInfo
		BYTE *pShapeInfo = p;
		Write16(pShapeInfo, 0); // len

		Write32(pShapeInfo, ShapeIDlist[i]); // ShapeID
		Write8(pShapeInfo, ShapeLevel);		 // ShapeLevel
		Write32(pShapeInfo, i);				 // ShowChoice 显示选项

		len = pShapeInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, ShapeID);   // ShapeID
	Write8(p, ShapeLevel); // ShapeLevel
	Write8(p, 0);		   // HasRefit 有改装信息
	for (size_t i = 0; i < 0; i++)
	{ // RefitClientInfo
		BYTE *pRefitClientInfo = p;
		Write16(pRefitClientInfo, 0); // len

		Write32(pRefitClientInfo, 0); // Uin
		Write32(pRefitClientInfo, 0); // KartId
		Write32(pRefitClientInfo, 0); // RefitCout
		Write16(pRefitClientInfo, 0); // MaxFlags
		Write16(pRefitClientInfo, 0); // WWeight
		Write16(pRefitClientInfo, 0); // SpeedWeight
		Write16(pRefitClientInfo, 0); // JetWeight
		Write16(pRefitClientInfo, 0); // SJetWeight
		Write16(pRefitClientInfo, 0); // AccuWeight
		Write32(pRefitClientInfo, 0); // ShapeRefitCount
		Write32(pRefitClientInfo, 0); // KartHeadRefitItemID
		Write32(pRefitClientInfo, 0); // KartTailRefitItemID
		Write32(pRefitClientInfo, 0); // KartFlankRefitItemID
		Write32(pRefitClientInfo, 0); // KartTireRefitItemID
		{							  // KartRefitExInfo
			BYTE *pKartRefitExInfo = pRefitClientInfo;
			Write16(pKartRefitExInfo, 0); // len

			Write8(pKartRefitExInfo, 0); // SpeedRefitStar
			Write8(pKartRefitExInfo, 0); // JetRefitStar
			Write8(pKartRefitExInfo, 0); // SJetRefitStar
			Write8(pKartRefitExInfo, 0); // AccuRefitStar
			Write8(pKartRefitExInfo, 0); // SpeedAddRatio
			Write8(pKartRefitExInfo, 0); // JetAddRatio
			Write8(pKartRefitExInfo, 0); // SJetAddRatio
			Write8(pKartRefitExInfo, 0); // AccuAddRatio

			len = pKartRefitExInfo - pRefitClientInfo;
			Set16(pRefitClientInfo, (WORD)len);
			pRefitClientInfo += len;
		}
		Write32(pRefitClientInfo, 0); // SecondRefitCount
		Write16(pRefitClientInfo, 0); // Speed2Weight
		Write16(pRefitClientInfo, 0); // DriftVecWeight
		Write16(pRefitClientInfo, 0); // AdditionalZSpeedWeight
		Write16(pRefitClientInfo, 0); // AntiCollisionWeight
		Write16(pRefitClientInfo, 0); // LuckyValue
		Write16(pRefitClientInfo, 0); // RefitLuckyValueMaxWeight
		Write32(pRefitClientInfo, 0); // ShapeSuitID
		Write8(pRefitClientInfo, 0);  // LegendSuitLevel
		Write32(pRefitClientInfo, 0); // LegendSuitLevelChoice
		Write32(pRefitClientInfo, 0); // ShapeLegendSuitID

		len = pRefitClientInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, NewShowChoice);		   // NewShowChoice 新展会选择
	Write32(p, DirectMaxLevelShapeID); // DirectMaxLevelShapeID   直接最大级别形状ID
	Write32(p, 10);					   // DirectMaxShapePreLevel  直接最大形状预级别
	const char *sql = NULL;

	sqlite3_stmt *stmt = NULL;
	int result;
	sql = "UPDATE KartRefit SET LegendSuitLevel=?,LegendSuitLevelChoice=?,ShapeSuitID=?,ShapeLegendSuitID=? WHERE Uin=? AND KartID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, ShapeLevel);
		sqlite3_bind_int(stmt, 2, NewShowChoice);
		sqlite3_bind_int(stmt, 3, ShapeIDIndex);

		sqlite3_bind_int(stmt, 4, ShapeID);
		sqlite3_bind_int(stmt, 5, Client->Uin);
		sqlite3_bind_int(stmt, 6, KartID);
		result = sqlite3_step(stmt);
		Z_INFO("结果:%d", result);
	}
	else
	{
		Z_ERROR("皮肤改装错误");
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	len = p - buf;
	SendToClient(Client, 319, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	if (ShapeIDIndex > 0)
	{
		if (Client->KartID == 74362 && Client->SkinID != 116735 && Client->SkinID != 110709 && Client->SkinID > 0)
		{
			ResponseSaveShapeRefit2(Client, Client->KartID, ShapeIDIndex);
		}
		if (Client->KartID == 12720 && Client->SkinID != 123506 && Client->SkinID > 0)
		{
			ResponseSaveShapeRefit2(Client, Client->KartID, ShapeIDIndex);
		}
		if (Client->KartID == 90945 && Client->SkinID != 110711 && Client->SkinID != 117941 && Client->SkinID != 122199 && Client->SkinID > 0)
		{
			ResponseSaveShapeRefit2(Client, Client->KartID, ShapeIDIndex);
		}
		if (Client->KartID == 123513)
		{
			ResponseSaveShapeRefit2(Client, Client->KartID, ShapeIDIndex);
		}
		if (Client->KartID == 126345)
		{
			ResponseSaveShapeRefit2(Client, Client->KartID, ShapeIDIndex);
		}
	}
}
void ResponseGetExchangeShopInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	/*char* pMsg = EByteToHexUA2((const unsigned char*)Body, BodyLen);
	printf("ResponseGetExchangeShopInfo	Hex:%s\n", pMsg);*/

	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	Write16(p, 0); // ResultID
	for (size_t i = 0; i < 3; i++)
	{ // ExchangeItemList
		BYTE *pExchangeItemList = p;
		Write16(pExchangeItemList, 0);	   // len
		Write32(pExchangeItemList, 83074); // ExchangeItemID
		Write16(pExchangeItemList, 20);	   // Num
		for (size_t i = 0; i < 20; i++)
		{ // ExchangeItem
			BYTE *pExchangeItem = pExchangeItemList;
			Write16(pExchangeItem, 0); // len

			Write32(pExchangeItem, 123505); // ItemID
			Write32(pExchangeItem, 1);		// ItemNum
			Write32(pExchangeItem, -1);		// AvailPeriod
			Write32(pExchangeItem, 0);		// CostExchangeItemNum 兑换需要数量
			Write8(pExchangeItem, 0);		// Flag  是否已经兑换

			len = pExchangeItem - pExchangeItemList;
			Set16(pExchangeItemList, (WORD)len);
			pExchangeItemList += len;
		}

		len = pExchangeItemList - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 1678084039); // OpenStartTime
	Write32(p, 1698084039); // OpenEndTime

	len = p - buf;
	SendToClient(Client, 24786, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseDoShopExchange(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	/*char* pMsg = EByteToHexUA2((const unsigned char*)Body, BodyLen);
	printf("ResponseDoShopExchange	Hex:%s\n", pMsg);*/

	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	// ExchangeInfo
	len = Read16(Body);
	UINT ItemID = Read32(Body);
	UINT ItemNum = Read32(Body);
	UINT AvailPeriod = Read32(Body);
	UINT CostExchangeItemNum = Read32(Body);
	BYTE Flag = Read8(Body);
	BYTE ExchangeShopType = Read8(Body);

	Write16(p, 0); // ResultID
	{			   // AwardItemInfo
		BYTE *pAwardItemInfo = p;
		Write16(pAwardItemInfo, 0); // len

		Write32(pAwardItemInfo, 0); // Exp
		Write32(pAwardItemInfo, 0); // Money
		Write32(pAwardItemInfo, 0); // SuperMoney
		Write16(pAwardItemInfo, 0); // ItemNum
		for (size_t i = 0; i < 0; i++)
		{ // ItemInfo
			BYTE *pItemInfo = pAwardItemInfo;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, 0); // ItemID
			Write32(pItemInfo, 0); // ItemNum
			Write32(pItemInfo, 0); // AvailPeriod
			Write8(pItemInfo, 0);  // Status
			Write32(pItemInfo, 0); // ObtainTime
			Write32(pItemInfo, 0); // OtherAttribute
			Write16(pItemInfo, 0); // ItemType

			len = pItemInfo - pAwardItemInfo;
			Set16(pAwardItemInfo, (WORD)len);
			pAwardItemInfo += len;
		}
		Write32(pAwardItemInfo, 0); // Coupons
		Write32(pAwardItemInfo, 0); // GuildPoint
		Write32(pAwardItemInfo, 0); // LuckMoney
		Write8(pAwardItemInfo, 0);	// ExtendInfoNum
		for (size_t i = 0; i < 0; i++)
		{ // ExtendInfoAward
			BYTE *pExtendInfoAward = pAwardItemInfo;
			Write16(pExtendInfoAward, 0); // len

			Write16(pExtendInfoAward, 0); // Key
			Write32(pExtendInfoAward, 0); // AddValue
			Write32(pExtendInfoAward, 0); // TotalValue
			Write8(pExtendInfoAward, 0);  // ShowPriority
			Write16(pExtendInfoAward, 0); // BuyNeedScore
			Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

			len = pExtendInfoAward - pAwardItemInfo;
			Set16(pAwardItemInfo, (WORD)len);
			pAwardItemInfo += len;
		}
		Write32(pAwardItemInfo, 0); // SpeedCoin

		len = pAwardItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // ExchangeInfo
		BYTE *pExchangeInfo = p;
		Write16(pExchangeInfo, 0); // len

		Write32(pExchangeInfo, ItemID);				 // ItemID
		Write32(pExchangeInfo, ItemNum);			 // ItemNum
		Write32(pExchangeInfo, AvailPeriod);		 // AvailPeriod
		Write32(pExchangeInfo, CostExchangeItemNum); // CostExchangeItemNum
		Write8(pExchangeInfo, Flag);				 // Flag

		len = pExchangeInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, ExchangeShopType); // ExchangeShopType

	len = p - buf;
	SendToClient(Client, 24787, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void ResponseDanceGetRoomList(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	short StartRoomIdx = Read16(Body);
	short GetRoomNum = Read16(Body);
	short BaseGameMode = Read16(Body);
	short SubGameMode = Read16(Body);
	bool RoomFlag = Read8(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			  // ResultID
	Write32(p, Uin);		  // Uin
	Write16(p, 1);			  // CurTotalRoomNum 当前总房间数
	Write16(p, StartRoomIdx); // StartIdx
	Write16(p, 1);			  // RoomNum
	for (size_t i = 0; i < 1; i++)
	{ // DanceRooms
		BYTE *pDanceRooms = p;
		Write16(pDanceRooms, 0); // len

		Write16(pDanceRooms, Client->RoomID); // RoomID
		Write16(pDanceRooms, BaseGameMode);	  // BaseGameMode
		Write16(pDanceRooms, SubGameMode);	  // SubGameMode
		Write8(pDanceRooms, 0);				  // CurrentPlayerNum
		Write8(pDanceRooms, 0);				  // TotalSeatNum
		Write8(pDanceRooms, 0);				  // Status
		Write32(pDanceRooms, 0);			  // Mode
		Write32(pDanceRooms, 17);			  // SceneID
		Write32(pDanceRooms, 303);			  // MusicID
		Write8(pDanceRooms, 0);				  // RandKeyFlag
		Write8(pDanceRooms, 0);				  // Flag

		memcpy(pDanceRooms, "茉莉测试1", MaxRoomName); // RoomName
		pDanceRooms += MaxRoomName;

		// WriteString(pDanceRooms, 0); //RoomName[]
		Write16(pDanceRooms, 0); // SvrId
		Write16(pDanceRooms, 0); // SceneID
		Write8(pDanceRooms, 0);	 // CurrentOBNum
		Write8(pDanceRooms, 0);	 // TotalOBNum
		Write8(pDanceRooms, 0);	 // AllowAutoStart

		len = pDanceRooms - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0);			  // ReasonLen
	Write16(p, BaseGameMode); // BaseGameMode
	Write16(p, SubGameMode);  // SubGameMode
	Write8(p, RoomFlag);	  // RoomFlag

	len = p - buf;
	SendToClient(Client, 251, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyKickPlayer(ClientNode *Client, const char *Reason, UINT KickType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	Write16(p, 0);			 // ReasonID
	Write32(p, Client->Uin); // SrcUin
	len = strlen(Reason);
	Write8(p, len);			// ResourceLen
	memcpy(p, Reason, len); // Resource 资源
	p += len;
	Write8(p, KickType); // AntiCheatMode
	Write8(p, KickType); // KickType

	len = p - buf;
	SendToClient(Client, 900, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyKickFromGame(ClientNode *Client, const char *Reason, UINT KickType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ReasonID
	Write32(p, Client->Uin); // SrcUin
	len = strlen(Reason);
	Write8(p, len);			// ResourceLen
	memcpy(p, Reason, len); // Resource 资源
	p += len;
	Write8(p, KickType); // KickType

	len = p - buf;
	SendToClient(Client, 460, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void ResponseGetFavoriteTask(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	Write16(p, 0); // Result
	Write8(p, 0);  // FavoriteTaskNum
	for (size_t i = 0; i < 0; i++)
	{ // FavoriteTask
		BYTE *pFavoriteTask = p;
		Write16(pFavoriteTask, 0); // len

		Write32(pFavoriteTask, 0); // TaskID
		Write32(pFavoriteTask, 0); // AddTime

		len = pFavoriteTask - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 415, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestG2DMagicCardGetCardAward(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int CardID = Read32(p);
}

void ResponseG2DMagicCardGetCardAward(ClientNode *Client, int CardID)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);		// ResultID
	Write32(p, CardID); // CardID

	len = p - buf;
	SendToClient(Client, 12007, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyG2CUpdateSkateTaskLevel(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write8(p, 3); // SkateTaskLevel

	len = p - buf;
	SendToClient(Client, 778, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestSkateSkillUpgrade(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT SkillId = Read8(p);
	ResponseSkateSkillUpgrade(Client, SkillId);
}

void ResponseSkateSkillUpgrade(ClientNode *Client, UINT SkillId)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);		// ResultID
	Write8(p, SkillId); // SkillId
	Write8(p, 1);		// SkillLevel
	Write32(p, 1000);	// SkateCoin

	len = p - buf;
	SendToClient(Client, 10927, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyG2CSkateCfg(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	INT n = 1;
	Write32(p, 0); // SkateSkillCfgNum
	for (size_t i = 0; i < n; i++)
	{ // SkateSkillCfg
		BYTE *pSkateSkillCfg = p;
		Write16(pSkateSkillCfg, 0); // len

		Write32(pSkateSkillCfg, 0); // SkilllLevel
		Write32(pSkateSkillCfg, 0); // UpgradeCostCoin
		Write32(pSkateSkillCfg, 0); // ExpAward

		len = pSkateSkillCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // ComboAwardCoinCfg[]
	Write32(p, 0); // SkateChannelCoinThreshold
	Write8(p, 0);  // SkateChannelNum
	for (size_t i = 0; i < n; i++)
	{ // SkateChannelInfo
		BYTE *pSkateChannelInfo = p;
		Write16(pSkateChannelInfo, 0); // len

		Write32(pSkateChannelInfo, 0); // ChannelId
		Write32(pSkateChannelInfo, 0); // LowerLevel
		Write32(pSkateChannelInfo, 0); // HigerLevel
		Write32(pSkateChannelInfo, 0); // LowerCoin
		Write32(pSkateChannelInfo, 0); // HigerCoin

		len = pSkateChannelInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // MaxSkateCoinCountDaily

	len = p - buf;
	SendToClient(Client, 10928, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifySkateLoveAdd(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // Value

	len = p - buf;
	SendToClient(Client, 20117, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestSkatePropRoomZan(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseSkatePropRoomZan(Client);
}

void ResponseSkatePropRoomZan(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	Write8(p, 1);			 // StatusLeft

	len = p - buf;
	SendToClient(Client, 24412, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifySkatePropRoomZanInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Uin
	Write32(p, 0); // UinSrc
	Write32(p, 0); // ScoreNew
	Write32(p, 0); // ScoreAdd

	len = p - buf;
	SendToClient(Client, 24413, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void NotifySkatePropRoomZanStatus(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write8(p, 0); // StatusLeft

	len = p - buf;
	SendToClient(Client, 24414, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void NotifySkatePropCoinChangeInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write8(p, 0);  // Type
	Write16(p, 0); // ResultID
	Write32(p, 0); // UinSrc
	Write32(p, 0); // CoinSrc
	Write32(p, 0); // UinDst
	Write32(p, 0); // CoinDst
	Write32(p, 0); // CoinChanged
	Write16(p, 0); // PropID

	len = p - buf;
	SendToClient(Client, 24415, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestPVPLogin(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT RandKey = Read32(Body);

	ResponsePVPLogin(Client);
}

void ResponsePVPLogin(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID

	len = p - buf;

	SendToClient(Client, 697, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyBorderGameCondition(ClientNode *Client)
{
	ConditionInfo *ConditionInfos = (ConditionInfo *)malloc(sizeof(ConditionInfo) * 6);
	ConditionInfo *TeamConditionInfos = (ConditionInfo *)malloc(sizeof(ConditionInfo) * 6);
	{ // 0

		ConditionInfos[0].BorderGameType = 0;
		ConditionInfos[0].NeededMoney = 10;
		ConditionInfos[0].NeededLevel = 30;
		ConditionInfos[0].OpenTime = 0;
		ConditionInfos[0].CloseTime = 235959;
		ConditionInfos[0].NeedHonor = 0;

		TeamConditionInfos[0].BorderGameType = 0;
		TeamConditionInfos[0].NeededMoney = 10;
		TeamConditionInfos[0].NeededLevel = 30;
		TeamConditionInfos[0].NeedHonor = 0;
	}
	{ // 1

		ConditionInfos[1].BorderGameType = 1;
		ConditionInfos[1].NeededMoney = 100;
		ConditionInfos[1].NeededLevel = 65;
		ConditionInfos[1].OpenTime = 0;
		ConditionInfos[1].CloseTime = 235959;
		ConditionInfos[1].NeedHonor = 1000;

		TeamConditionInfos[1].BorderGameType = 1;
		TeamConditionInfos[1].NeededMoney = 100;
		TeamConditionInfos[1].NeededLevel = 65;
		TeamConditionInfos[1].NeedHonor = 1000;
	}
	{ // 2

		ConditionInfos[2].BorderGameType = 2;
		ConditionInfos[2].NeededMoney = 10;
		ConditionInfos[2].NeededLevel = 30;
		ConditionInfos[2].OpenTime = 0;
		ConditionInfos[2].CloseTime = 235959;
		ConditionInfos[2].NeedHonor = 0;

		TeamConditionInfos[2].BorderGameType = 2;
		TeamConditionInfos[2].NeededMoney = 10;
		TeamConditionInfos[2].NeededLevel = 30;
		TeamConditionInfos[2].NeedHonor = 0;
	}
	{ // 3

		ConditionInfos[3].BorderGameType = 3;
		ConditionInfos[3].NeededMoney = 100;
		ConditionInfos[3].NeededLevel = 65;
		ConditionInfos[3].OpenTime = 0;
		ConditionInfos[3].CloseTime = 235959;
		ConditionInfos[3].NeedHonor = 1000;

		TeamConditionInfos[3].BorderGameType = 3;
		TeamConditionInfos[3].NeededMoney = 100;
		TeamConditionInfos[3].NeededLevel = 65;
		TeamConditionInfos[3].NeedHonor = 1000;
	}
	{ // 4

		ConditionInfos[4].BorderGameType = 4;
		ConditionInfos[4].NeededMoney = 300;
		ConditionInfos[4].NeededLevel = 90;
		ConditionInfos[4].OpenTime = 140000;
		ConditionInfos[4].CloseTime = 210000;
		ConditionInfos[4].NeedHonor = 20000;

		TeamConditionInfos[4].BorderGameType = 4;
		TeamConditionInfos[4].NeededMoney = 300;
		TeamConditionInfos[4].NeededLevel = 90;
		TeamConditionInfos[4].NeedHonor = 20000;
	}
	{ // 5

		ConditionInfos[5].BorderGameType = 5;
		ConditionInfos[5].NeededMoney = 300;
		ConditionInfos[5].NeededLevel = 90;
		ConditionInfos[5].OpenTime = 140000;
		ConditionInfos[5].CloseTime = 210000;
		ConditionInfos[5].NeedHonor = 20000;

		TeamConditionInfos[5].BorderGameType = 5;
		TeamConditionInfos[5].NeededMoney = 300;
		TeamConditionInfos[5].NeededLevel = 90;
		TeamConditionInfos[5].NeedHonor = 20000;
	}
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	int Condition = 6;
	int TeamCondition = 6;
	for (size_t i = 0; i < Condition; i++)
	{ // Condition
		BYTE *pCondition = p;
		Write16(pCondition, 0); // len

		Write32(pCondition, ConditionInfos[i].BorderGameType); // BorderGameType
		Write16(pCondition, ConditionInfos[i].NeededMoney);	   // NeededMoney
		Write16(pCondition, ConditionInfos[i].NeededLevel);	   // NeededLevel
		{													   // TimeLimit
			BYTE *pTimeLimit = pCondition;
			Write16(pTimeLimit, 0); // len

			Write32(pTimeLimit, ConditionInfos[i].OpenTime);  // OpenTime
			Write32(pTimeLimit, ConditionInfos[i].CloseTime); // CloseTime

			len = pTimeLimit - pCondition;
			Set16(pCondition, (WORD)len);
			pCondition += len;
		}
		Write32(pCondition, ConditionInfos[i].NeedHonor); // NeedHonor

		len = pCondition - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0);  // NeedMapNum
	Write32(p, 0); // MapID[]
	Write8(p, 0);  // LeftVipFreeTime[]
	for (size_t i = 0; i < TeamCondition; i++)
	{ // TeamCondition
		BYTE *pTeamCondition = p;
		Write16(pTeamCondition, 0); // len

		Write32(pTeamCondition, TeamConditionInfos[i].BorderGameType); // BorderGameType
		Write16(pTeamCondition, TeamConditionInfos[i].NeededMoney);	   // NeededMoney
		Write16(pTeamCondition, TeamConditionInfos[i].NeededLevel);	   // NeededLevel
		Write32(pTeamCondition, TeamConditionInfos[i].NeedHonor);	   // NeedHonor

		len = pTeamCondition - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // SeasonTime
		BYTE *pSeasonTime = p;
		Write16(pSeasonTime, 0); // len

		Write32(pSeasonTime, 1683820800); // OpenTime
		Write32(pSeasonTime, 1689695999); // CloseTime

		len = pSeasonTime - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 1); // TimeNum
	for (size_t i = 0; i < 1; i++)
	{ // TimeLimit
		BYTE *pTimeLimit = p;
		Write16(pTimeLimit, 0); // len

		Write32(pTimeLimit, 123000); // OpenTime
		Write32(pTimeLimit, 223000); // CloseTime

		len = pTimeLimit - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0);	// IsBorderTeamAwardMultiple
	Write32(p, 60); // WaitPrimaryRaceMatchTimeout
	Write32(p, 60); // WaitHighRaceMatchTimeout
	Write32(p, 60); // WaitHigherRaceMatchTimeout
	Write32(p, 60); // WaitPrimaryPropMatchTimeout
	Write32(p, 60); // WaitHighPropMatchTimeout
	Write32(p, 60); // WaitHigherPropMatchTimeout
	Write32(p, 60); // WaitPrimaryRaceTeamMatchTimeout
	Write32(p, 60); // WaitHighRaceTeamMatchTimeout
	Write32(p, 60); // WaitHigherRaceTeamMatchTimeout
	Write16(p, 1);	// PropTimeNum
	for (size_t i = 0; i < 1; i++)
	{ // PropTimeLimit
		BYTE *pPropTimeLimit = p;
		Write16(pPropTimeLimit, 0); // len

		Write32(pPropTimeLimit, 123000); // OpenTime
		Write32(pPropTimeLimit, 223000); // CloseTime

		len = pPropTimeLimit - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // AllAreaBorderCfg
		BYTE *pAllAreaBorderCfg = p;
		Write16(pAllAreaBorderCfg, 0); // len

		Write32(pAllAreaBorderCfg, 16800768); // Open
		{									  // AllAreaNormalBorderCfg
			BYTE *pAllAreaNormalBorderCfg = pAllAreaBorderCfg;
			Write16(pAllAreaNormalBorderCfg, 0); // len

			for (size_t i = 0; i < 0; i++)
			{ // TypeList
				BYTE *pTypeList = pAllAreaNormalBorderCfg;
				Write16(pTypeList, 0); // len

				Write16(pTypeList, 0); // TimeNum
				for (size_t i = 0; i < 0; i++)
				{ // TimeLimit
					BYTE *pTimeLimit = pTypeList;
					Write16(pTimeLimit, 0); // len

					Write32(pTimeLimit, 0); // OpenTime
					Write32(pTimeLimit, 0); // CloseTime

					len = pTimeLimit - pTypeList;
					Set16(pTypeList, (WORD)len);
					pTypeList += len;
				}
				Write8(pTypeList, 0); // IsModeOpen

				len = pTypeList - pAllAreaNormalBorderCfg;
				Set16(pAllAreaNormalBorderCfg, (WORD)len);
				pAllAreaNormalBorderCfg += len;
			}

			len = pAllAreaNormalBorderCfg - pAllAreaBorderCfg;
			Set16(pAllAreaBorderCfg, (WORD)len);
			pAllAreaBorderCfg += len;
		}
		{ // AllAreaTeamBorderCfg
			BYTE *pAllAreaTeamBorderCfg = pAllAreaBorderCfg;
			Write16(pAllAreaTeamBorderCfg, 0); // len

			for (size_t i = 0; i < 0; i++)
			{ // TypeList
				BYTE *pTypeList = pAllAreaTeamBorderCfg;
				Write16(pTypeList, 0); // len

				Write16(pTypeList, 0); // TimeNum
				for (size_t i = 0; i < 0; i++)
				{ // TimeLimit
					BYTE *pTimeLimit = pTypeList;
					Write16(pTimeLimit, 0); // len

					Write32(pTimeLimit, 0); // OpenTime
					Write32(pTimeLimit, 0); // CloseTime

					len = pTimeLimit - pTypeList;
					Set16(pTypeList, (WORD)len);
					pTypeList += len;
				}
				Write8(pTypeList, 0); // IsModeOpen

				len = pTypeList - pAllAreaTeamBorderCfg;
				Set16(pAllAreaTeamBorderCfg, (WORD)len);
				pAllAreaTeamBorderCfg += len;
			}

			len = pAllAreaTeamBorderCfg - pAllAreaBorderCfg;
			Set16(pAllAreaBorderCfg, (WORD)len);
			pAllAreaBorderCfg += len;
		}

		len = pAllAreaBorderCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	for (size_t i = 0; i < 0; i++)
	{ // BorderTimeLimit
		BYTE *pBorderTimeLimit = p;
		Write16(pBorderTimeLimit, 0); // len

		Write16(pBorderTimeLimit, 0); // TimeNum
		for (size_t i = 0; i < 0; i++)
		{ // TimeLimit
			BYTE *pTimeLimit = pBorderTimeLimit;
			Write16(pTimeLimit, 0); // len

			Write32(pTimeLimit, 0); // OpenTime
			Write32(pTimeLimit, 0); // CloseTime

			len = pTimeLimit - pBorderTimeLimit;
			Set16(pBorderTimeLimit, (WORD)len);
			pBorderTimeLimit += len;
		}

		len = pBorderTimeLimit - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // MultiHonorInfo
		BYTE *pMultiHonorInfo = p;
		Write16(pMultiHonorInfo, 0); // len

		{ // OpenDateTime
			BYTE *pOpenDateTime = pMultiHonorInfo;
			Write16(pOpenDateTime, 0); // len

			Write32(pOpenDateTime, 0); // OpenDate
			Write32(pOpenDateTime, 0); // OpenTime
			Write32(pOpenDateTime, 0); // CloseDate
			Write32(pOpenDateTime, 0); // CloseTime
			Write32(pOpenDateTime, 0); // Open
			Write32(pOpenDateTime, 0); // Close

			len = pOpenDateTime - pMultiHonorInfo;
			Set16(pMultiHonorInfo, (WORD)len);
			pMultiHonorInfo += len;
		}
		Write8(pMultiHonorInfo, 0); // BeforeOpenLen
		Write8(pMultiHonorInfo, 0); // OpenLen

		len = pMultiHonorInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0);  // ForbidCarNum
	Write32(p, 0); // ForbidCar[]

	len = p - buf;
	SendToClient(Client, 544, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestGetThemeHouseShowInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UCHAR PlayerGender = Read8(p);
	ResponseGetThemeHouseShowInfo(Client);
}
void ResponseGetThemeHouseShowInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write32(p, 10);			 // CollectionValue
	Write16(p, 0);			 // ResultID
	Write16(p, 1);			 // RootThemeNums
	for (size_t i = 0; i < 0; i++)
	{ // RootThemeInfos
		BYTE *pRootThemeInfos = p;
		Write16(pRootThemeInfos, 0); // len

		Write8(pRootThemeInfos, 1);	 // RootThemeID
		Write8(pRootThemeInfos, 0);	 // ThemeName[]
		Write32(pRootThemeInfos, 1); // ChildThemeNums
		for (size_t i = 0; i < 1; i++)
		{ // ChildThemeInfos
			BYTE *pChildThemeInfos = pRootThemeInfos;
			Write16(pChildThemeInfos, 0); // len

			Write8(pChildThemeInfos, 0);  // ThemeType
			Write8(pChildThemeInfos, 0);  // ThemeID
			Write32(pChildThemeInfos, 0); // OpenTime
			Write32(pChildThemeInfos, 0); // CloseTime
			Write8(pChildThemeInfos, 0);  // ThemeName[]
			Write16(pChildThemeInfos, 0); // CollectdtThemeItemNums
			Write16(pChildThemeInfos, 1); // ThemeItemNums
			for (size_t i = 0; i < 1; i++)
			{ // ThemeItemInfos
				BYTE *pThemeItemInfos = pChildThemeInfos;
				Write16(pThemeItemInfos, 0); // len

				Write8(pThemeItemInfos, 0);		 // ItemType
				Write8(pThemeItemInfos, 0);		 // ReceStat
				Write32(pThemeItemInfos, 10020); // ItemID[]
				Write32(pThemeItemInfos, 1);	 // ItemNums
				Write32(pThemeItemInfos, -1);	 // AvailPeroid
				Write32(pThemeItemInfos, 0);	 // Index
				Write32(pThemeItemInfos, 0);	 // CostTreasureFragment
				Write8(pThemeItemInfos, 0);		 // ExclusiveItem
				Write32(pThemeItemInfos, 0);	 // CollectValue
				Write8(pThemeItemInfos, 0);		 // BestQuality
				Write8(pThemeItemInfos, 0);		 // UrlLen

				len = pThemeItemInfos - pChildThemeInfos;
				Set16(pChildThemeInfos, (WORD)len);
				pChildThemeInfos += len;
			}
			Write8(pChildThemeInfos, 0);  // ThemeStat
			Write16(pChildThemeInfos, 0); // ExpAddCfgNums
			for (size_t i = 0; i < 0; i++)
			{ // ThemeItemExpAddInfo
				BYTE *pThemeItemExpAddInfo = pChildThemeInfos;
				Write16(pThemeItemExpAddInfo, 0); // len

				Write16(pThemeItemExpAddInfo, 0); // EquippedNums
				Write16(pThemeItemExpAddInfo, 0); // ExpAddValue

				len = pThemeItemExpAddInfo - pChildThemeInfos;
				Set16(pChildThemeInfos, (WORD)len);
				pChildThemeInfos += len;
			}
			Write8(pChildThemeInfos, 0);  // CanRecExclusiveAward
			Write16(pChildThemeInfos, 0); // ThemeAppelationNums
			for (size_t i = 0; i < 0; i++)
			{ // ThemeAppelationInfo
				BYTE *pThemeAppelationInfo = pChildThemeInfos;
				Write16(pThemeAppelationInfo, 0); // len

				Write16(pThemeAppelationInfo, 0); // EquippedNums
				Write8(pThemeAppelationInfo, 0);  // AppelationName[]

				len = pThemeAppelationInfo - pChildThemeInfos;
				Set16(pChildThemeInfos, (WORD)len);
				pChildThemeInfos += len;
			}

			len = pChildThemeInfos - pRootThemeInfos;
			Set16(pRootThemeInfos, (WORD)len);
			pRootThemeInfos += len;
		}

		len = pRootThemeInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // HasMoreInfo

	len = p - buf;
	SendToClient(Client, 24501, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetConsumeScoreConfUrl(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	int Time = Read32(p);
	UCHAR Md5StrLen = Read8(p);
	ResponseGetConsumeScoreConfUrl(Client, Md5StrLen);
}

void ResponseGetConsumeScoreConfUrl(ClientNode *Client, UCHAR Md5StrLen)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write8(p, Md5StrLen); // Md5StrLen
	Write16(p, 1);		  // BroadcastLen
	Write16(p, 1);		  // ExchangeNumber
	for (size_t i = 0; i < 1; i++)
	{ // Exchange
		BYTE *pExchange = p;
		Write16(pExchange, 0); // len

		Write32(pExchange, 0);	   // PackId
		Write32(pExchange, 10020); // ItemId
		Write32(pExchange, -1);	   // ItemAvailPeriod
		Write32(pExchange, 1);	   // ItemNumber
		Write32(pExchange, 0);	   // Score
		Write8(pExchange, 0);	   // HasTimeLimit
		for (size_t i = 0; i < 0; i++)
		{ // TimeLimit
			BYTE *pTimeLimit = pExchange;
			Write16(pTimeLimit, 0); // len

			Write32(pTimeLimit, 0); // StartTime
			Write32(pTimeLimit, 0); // EndTime

			len = pTimeLimit - pExchange;
			Set16(pExchange, (WORD)len);
			pExchange += len;
		}
		Write8(pExchange, 0); // HasDiscount
		for (size_t i = 0; i < 0; i++)
		{ // Discount
			BYTE *pDiscount = pExchange;
			Write16(pDiscount, 0); // len

			Write32(pDiscount, 0); // OrigScore
			Write32(pDiscount, 0); // DiscountValue

			len = pDiscount - pExchange;
			Set16(pExchange, (WORD)len);
			pExchange += len;
		}

		len = pExchange - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 11471, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetDropInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseGetDropInfo(Client);
}

void ResponseGetDropInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);	   // ResultID
	Write8(p, 0);	   // ReasonLen
	Write32(p, 10020); // ItemId
	Write32(p, 5);	   // TotalDropCount
	Write32(p, 10);	   // ParticipatedPlayerNum
	Write32(p, 10);	   // TimeLeftToOpenLottery
	Write32(p, 10);	   // LotteryNum
	for (size_t i = 0; i < 10; i++)
	{ // DropLotteryAwardInfo
		BYTE *pDropLotteryAwardInfo = p;
		Write16(pDropLotteryAwardInfo, 0); // len

		{ // Award
			BYTE *pAward = pDropLotteryAwardInfo;
			Write16(pAward, 0); // len

			Write32(pAward, 0); // Exp
			Write32(pAward, 0); // Money
			Write32(pAward, 0); // SuperMoney
			Write16(pAward, 1); // ItemNum
			for (size_t i = 0; i < 1; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pAward;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, 10020); // ItemID
				Write32(pItemInfo, 1);	   // ItemNum
				Write32(pItemInfo, -1);	   // AvailPeriod
				Write8(pItemInfo, 0);	   // Status
				Write32(pItemInfo, 0);	   // ObtainTime
				Write32(pItemInfo, 0);	   // OtherAttribute
				Write16(pItemInfo, 0);	   // ItemType

				len = pItemInfo - pAward;
				Set16(pAward, (WORD)len);
				pAward += len;
			}
			Write32(pAward, 0); // Coupons
			Write32(pAward, 0); // GuildPoint
			Write32(pAward, 0); // LuckMoney
			Write8(pAward, 0);	// ExtendInfoNum
			for (size_t i = 0; i < 0; i++)
			{ // ExtendInfoAward
				BYTE *pExtendInfoAward = pAward;
				Write16(pExtendInfoAward, 0); // len

				Write16(pExtendInfoAward, 0); // Key
				Write32(pExtendInfoAward, 0); // AddValue
				Write32(pExtendInfoAward, 0); // TotalValue
				Write8(pExtendInfoAward, 0);  // ShowPriority
				Write16(pExtendInfoAward, 0); // BuyNeedScore
				Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

				len = pExtendInfoAward - pAward;
				Set16(pAward, (WORD)len);
				pAward += len;
			}
			Write32(pAward, 0); // SpeedCoin

			len = pAward - pDropLotteryAwardInfo;
			Set16(pDropLotteryAwardInfo, (WORD)len);
			pDropLotteryAwardInfo += len;
		}
		Write32(pDropLotteryAwardInfo, 0); // LotteryPlayerCount
		Write32(pDropLotteryAwardInfo, 0); // LotteryTime
		Write8(pDropLotteryAwardInfo, 0);  // LotteryStatus
		Write8(pDropLotteryAwardInfo, 0);  // SelfLotteryStatus
		Write32(pDropLotteryAwardInfo, 0); // DemandOfDropsToOpenLottery
		Write32(pDropLotteryAwardInfo, 0); // DropsInWishingWell

		len = pDropLotteryAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // BroadCastLen

	len = p - buf;
	SendToClient(Client, 24043, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestEngage(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT SrcUin = Read32(p);
	UINT Time = Read32(p);
	int EngageType = Read32(p);
	int RingID = Read32(p);
	UINT DstUin = Read32(p);
	// char[] NickName[] = ReadString(p);
	short WordLength = Read16(p);

	if (GetClient(DstUin))
	{
		NotifyBeEngaged(Client, GetClient(DstUin), RingID, EngageType);
		ResponseEngage(Client, DstUin, EngageType);
	}
	else
	{
		ResponseStartGame(Client, "该用户不在线,无法发送请求");
	}
}

void ResponseEngage(ClientNode *Client, UINT DstUin, int EngageType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	Write32(p, EngageType);	 // EngageType
	Write32(p, DstUin);		 // DstUin
	Write8(p, 0);			 // ReasonLen

	len = p - buf;
	SendToClient(Client, 166, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyBeEngaged(ClientNode *SrcClient, ClientNode *Client, int RingID, int EngageType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, SrcClient->Uin); // SrcUin
	Write32(p, Client->Uin);	// DstUin
	Write32(p, EngageType);		// EngageType
	{							// SrcPlayerInfo
		BYTE *pSrcPlayerInfo = p;
		Write16(pSrcPlayerInfo, 0); // len

		Write8(pSrcPlayerInfo, SrcClient->Gender);	// Gender
		Write8(pSrcPlayerInfo, SrcClient->Country); // Country
		Write8(pSrcPlayerInfo, MaxNickName);		// NickName[]
		memcpy(pSrcPlayerInfo, SrcClient->NickName, MaxNickName);
		pSrcPlayerInfo += MaxNickName;

		len = pSrcPlayerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0);		// WordLength
	Write32(p, RingID); // RingId

	len = p - buf;
	SendToClient(Client, 572, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void AckBeEngaged(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	short ResultID = Read16(p); // 0同意 1 拒绝
	UINT SrcUin = Read32(p);
	UINT DstUin = Read32(p);
	int EngageType = Read32(p);
	short WordLength = Read16(p);
	if (ResultID == 1)
	{
		// ResponseStartGame(Client, "对方拒绝了你的请求");
		ResponseStartGame(GetClient(DstUin), "对方拒绝了你的请求");
	}
	else
	{
		const char *sql = NULL;
		sqlite3_stmt *stmt = NULL;
		int result;
		sql = "insert into EngageInfo(EngageUin1,EngageUin2,NickName1,NickName2,BabyName1,BabyName2) VALUES(?,?,?,?,?,?)";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, DstUin);
			sqlite3_bind_int(stmt, 2, Client->Uin);
			sqlite3_bind_text(stmt, 3, GetClient(DstUin)->NickName, strlen(GetClient(DstUin)->NickName), SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 4, Client->NickName, strlen(Client->NickName), SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 5, "一号宝宝", strlen("一号宝宝"), SQLITE_TRANSIENT);
			sqlite3_bind_text(stmt, 6, "二号宝宝", strlen("二号宝宝"), SQLITE_TRANSIENT);
			result = sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		NotifyEngageResult(Client, ResultID, GetClient(DstUin), EngageType);
		NotifyEngageResult(GetClient(DstUin), ResultID, Client, EngageType);
	}
}
void NotifyEngageResult(ClientNode *Client, UINT ResultID, ClientNode *DstUin, int EngageType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, ResultID);				 // ResultID
	Write32(p, Client->Uin);			 // Uin
	Write32(p, DstUin->Uin);			 // DstUin
	Write8(p, strlen(DstUin->NickName)); // RelNickName[]
	memcpy(p, DstUin->NickName, strlen(DstUin->NickName));
	p += strlen(DstUin->NickName);
	Write32(p, EngageType); // EngageType
	Write16(p, 0);			// WordLength
	Write8(p, 0);			// ReasonLen
	Write32(p, 0);			// RingId
	Write32(p, 0);			// EngageTime
	Write8(p, 0);			// EngageFlag

	len = p - buf;
	SendToClient(Client, 573, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyEngageResult2(ClientNode *Client, UINT ResultID, int EngageType)
{
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	sql = "SELECT EngageUin1,EngageUin2,NickName1,NickName2 FROM EngageInfo WHERE EngageUin1 = ? or EngageUin2 = ?";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, Client->Uin);
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			BYTE buf[8192];
			BYTE *p = buf;
			size_t len;

			Write16(p, ResultID);	 // ResultID
			Write32(p, Client->Uin); // Uin
			if (sqlite3_column_int(stmt, 0) == Client->Uin)
			{
				Client->Engage->UIN = sqlite3_column_int(stmt, 1);
				char szCharName[MaxNickName] = "";
				memcpy(szCharName, sqlite3_column_text(stmt, 3), MaxNickName);
				strcpy_s(Client->Engage->NickName, szCharName);
				Write32(p, sqlite3_column_int(stmt, 1));				 // DstUin
				Write8(p, strlen((char *)sqlite3_column_text(stmt, 3))); // RelNickName[]
				memcpy(p, (char *)sqlite3_column_text(stmt, 3), 16);
				p += 16;
			}
			else
			{
				Client->Engage->UIN = sqlite3_column_int(stmt, 0);
				char szCharName[MaxNickName] = "";
				memcpy(szCharName, sqlite3_column_text(stmt, 2), MaxNickName);
				strcpy_s(Client->Engage->NickName, szCharName);
				Write32(p, sqlite3_column_int(stmt, 0));				 // DstUin
				Write8(p, strlen((char *)sqlite3_column_text(stmt, 2))); // RelNickName[]
				memcpy(p, (char *)sqlite3_column_text(stmt, 2), 16);
				p += 16;
			}

			Write32(p, EngageType); // EngageType
			Write16(p, 0);			// WordLength
			Write8(p, 0);			// ReasonLen
			Write32(p, 0);			// RingId
			Write32(p, 100000);		// EngageTime
			Write8(p, 4);			// EngageFlag

			len = p - buf;
			SendToClient(Client, 573, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
}
void NotifyEngageResult3(ClientNode* Client)
{
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	sql = "SELECT EngageUin1,EngageUin2,NickName1,NickName2,BabyOneID,BabyOneItemID,BabyOneEquipStat,BabyOneGender,BabyTwoID,BabyTwoItemID,BabyTwoEquipStat,BabyTwoGender,BackGroundID,BabyName1,BabyName2,EngageTime FROM EngageInfo WHERE EngageUin1 = ? or EngageUin2 = ?";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, Client->Uin);
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			if (sqlite3_column_int(stmt, 0) == Client->Uin)
			{
				Client->Engage->UIN = sqlite3_column_int(stmt, 1);
				char szCharName[MaxNickName] = "";
				memcpy(szCharName, sqlite3_column_text(stmt, 3), MaxNickName);
				strcpy_s(Client->Engage->NickName, szCharName);
			}
			else {
				Client->Engage->UIN = sqlite3_column_int(stmt, 0);
				char szCharName[MaxNickName] = "";
				memcpy(szCharName, sqlite3_column_text(stmt, 2), MaxNickName);
				strcpy_s(Client->Engage->NickName, szCharName);

			}
			Client->Engage->BabyOneID = sqlite3_column_int(stmt, 4);
			Client->Engage->BabyOneItemID = sqlite3_column_int(stmt, 5);
			Client->Engage->BabyOneEquipStat = sqlite3_column_int(stmt, 6);
			Client->Engage->BabyOneGender = sqlite3_column_int(stmt, 7);
			Client->Engage->BabyTwoID = sqlite3_column_int(stmt, 8);
			Client->Engage->BabyTwoItemID = sqlite3_column_int(stmt, 9);
			Client->Engage->BabyTwoEquipStat = sqlite3_column_int(stmt, 10);
			Client->Engage->BabyTwoGender = sqlite3_column_int(stmt, 11);
			Client->Engage->BackGroundID = sqlite3_column_int(stmt, 12);
			{
				char szCharName[MaxNickName] = "";
				memcpy(szCharName, sqlite3_column_text(stmt, 13), MaxNickName);
				strcpy_s(Client->Engage->BabyName1, szCharName); 
			} 
			{
				char szCharName[MaxNickName] = "";
				memcpy(szCharName, sqlite3_column_text(stmt, 14), MaxNickName);
				strcpy_s(Client->Engage->BabyName2, szCharName);
			}
			Client->Engage->EngageTime = sqlite3_column_int(stmt, 15);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
}
void NotifyEngageResult4(ClientNode* Client, BYTE*& p)
{

	size_t len;
	if (Client&&Client->Engage->UIN > 0)
	{ // PlayerRelationInfo
		BYTE* pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); // len

		Write32(pPlayerRelationInfo, Client->Uin); // SrcUin
		Write32(pPlayerRelationInfo, 3); // RelationFlag
		Write32(pPlayerRelationInfo, Client->Engage->UIN); // RelationUin
		memcpy(pPlayerRelationInfo, Client->Engage->NickName, MaxNickName);
		pPlayerRelationInfo += MaxNickName;
		//WriteString(pPlayerRelationInfo, Client->Engage->NickName, maxnick);	 // NickName

		Write32(pPlayerRelationInfo, Client->Engage->EngageTime); // EngageTime
		Write32(pPlayerRelationInfo, 10); // NextCanBookingTimeLimit
		Write32(pPlayerRelationInfo, Client->Engage->EngageTime); // BeginHoneyMoonTime
		Write32(pPlayerRelationInfo, Client->Engage->EngageTime); // EndHoneyMoonTime
		Write8(pPlayerRelationInfo, 4);	 // EngageFlag

		len = pPlayerRelationInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	else
	{ //PlayerRelationInfo玩家关系信息
		BYTE* pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); //len


		Write32(pPlayerRelationInfo, 0); //SrcUin
		Write32(pPlayerRelationInfo, 0); //RelationFlag
		Write32(pPlayerRelationInfo, 0); //RelationUin

		char RelationNickName[MaxNickName] = "";
		memcpy(pPlayerRelationInfo, RelationNickName, MaxNickName);
		pPlayerRelationInfo += MaxNickName;

		Write32(pPlayerRelationInfo, 0); //EngageTime启动时间参与时间
		Write32(pPlayerRelationInfo, 0); //NextCanBookingTimeLimit
		Write32(pPlayerRelationInfo, 0); //BeginHoneyMoonTime
		Write32(pPlayerRelationInfo, 0); //EndHoneyMoonTime
		Write8(pPlayerRelationInfo, 0); //EngageFlag


		len = pPlayerRelationInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
}
void NotifyEngageResult5(UINT Uin,BYTE*& p)
{
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	ClientNode* Client =new ClientNode;
	Client->Uin = Uin;
	Client->Engage = new EngageInfo{};
	size_t len;
	int result;
	sql = "SELECT EngageUin1,EngageUin2,NickName1,NickName2,BabyOneID,BabyOneItemID,BabyOneEquipStat,BabyOneGender,BabyTwoID,BabyTwoItemID,BabyTwoEquipStat,BabyTwoGender,BackGroundID,BabyName1,BabyName2,EngageTime FROM EngageInfo WHERE EngageUin1 = ? or EngageUin2 = ?";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, Client->Uin);
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			if (sqlite3_column_int(stmt, 0) == Client->Uin)
			{
				Client->Engage->UIN = sqlite3_column_int(stmt, 1);
				char szCharName[MaxNickName] = "";
				memcpy(szCharName, sqlite3_column_text(stmt, 3), MaxNickName);
				strcpy_s(Client->Engage->NickName, szCharName);
			}
			else {
				Client->Engage->UIN = sqlite3_column_int(stmt, 0);
				char szCharName[MaxNickName] = "";
				memcpy(szCharName, sqlite3_column_text(stmt, 2), MaxNickName);
				strcpy_s(Client->Engage->NickName, szCharName);

			}
			Client->Engage->BabyOneID = sqlite3_column_int(stmt, 4);
			Client->Engage->BabyOneItemID = sqlite3_column_int(stmt, 5);
			Client->Engage->BabyOneEquipStat = sqlite3_column_int(stmt, 6);
			Client->Engage->BabyOneGender = sqlite3_column_int(stmt, 7);
			Client->Engage->BabyTwoID = sqlite3_column_int(stmt, 8);
			Client->Engage->BabyTwoItemID = sqlite3_column_int(stmt, 9);
			Client->Engage->BabyTwoEquipStat = sqlite3_column_int(stmt, 10);
			Client->Engage->BabyTwoGender = sqlite3_column_int(stmt, 11);
			Client->Engage->BackGroundID = sqlite3_column_int(stmt, 12);
			{
				char szCharName[MaxNickName] = "";
				memcpy(szCharName, sqlite3_column_text(stmt, 13), MaxNickName);
				strcpy_s(Client->Engage->BabyName1, szCharName);
			}
			{
				char szCharName[MaxNickName] = "";
				memcpy(szCharName, sqlite3_column_text(stmt, 14), MaxNickName);
				strcpy_s(Client->Engage->BabyName2, szCharName);
			}
			Client->Engage->EngageTime = sqlite3_column_int(stmt, 15);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (Client && Client->Engage->UIN > 0)
	{ // PlayerRelationInfo
		BYTE* pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); // len

		Write32(pPlayerRelationInfo, Client->Uin); // SrcUin
		Write32(pPlayerRelationInfo, 3); // RelationFlag
		Write32(pPlayerRelationInfo, 0); // RelationUin
		WriteString(pPlayerRelationInfo, Client->Engage->NickName, MaxNickName);	 // NickName
		Write32(pPlayerRelationInfo, Client->Engage->EngageTime); // EngageTime
		Write32(pPlayerRelationInfo, 0); // NextCanBookingTimeLimit
		Write32(pPlayerRelationInfo, Client->Engage->EngageTime); // BeginHoneyMoonTime
		Write32(pPlayerRelationInfo, Client->Engage->EngageTime); // EndHoneyMoonTime
		Write8(pPlayerRelationInfo, 4);	 // EngageFlag

		len = pPlayerRelationInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	else
	{ //PlayerRelationInfo玩家关系信息
		BYTE* pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); //len


		Write32(pPlayerRelationInfo, 0); //SrcUin
		Write32(pPlayerRelationInfo, 0); //RelationFlag
		Write32(pPlayerRelationInfo, 0); //RelationUin

		char RelationNickName[MaxNickName] = "";
		memcpy(pPlayerRelationInfo, RelationNickName, MaxNickName);
		pPlayerRelationInfo += MaxNickName;

		Write32(pPlayerRelationInfo, 0); //EngageTime启动时间参与时间
		Write32(pPlayerRelationInfo, 0); //NextCanBookingTimeLimit
		Write32(pPlayerRelationInfo, 0); //BeginHoneyMoonTime
		Write32(pPlayerRelationInfo, 0); //EndHoneyMoonTime
		Write8(pPlayerRelationInfo, 0); //EngageFlag


		len = pPlayerRelationInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
}
void AckBeInvited(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	short ResultID = Read16(p);
	UINT SrcUin = Read32(p);
	UINT DstUin = Read32(p);
	bool Type = Read8(p);
}
void NotifyInviteResult(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write32(p, 0); // DstUin
	Write8(p, 0);  // ReasonLen
	Write8(p, 0);  // Type

	len = p - buf;
	SendToClient(Client, 568, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyBeInvited(ClientNode *Client, ClientNode *Dest)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	RoomNode *Room = GetRoom(Dest->RoomID);
	Write32(p, Client->Uin); // SrcUin
	Write8(p, MaxNickName);	 // SrcNickName[]
	memcpy(p, Dest->NickName, MaxNickName);
	p += MaxNickName;
	Write32(p, Dest->Uin); // DstUin
	{					   // PositionInfo
		BYTE *pPositionInfo = p;
		Write16(pPositionInfo, 0); // len

		Write8(pPositionInfo, 0);				// ServerType
		Write16(pPositionInfo, 0);				// Status
		Write32(pPositionInfo, Dest->ServerID); // ServerID
		{										// RoomInfo
			BYTE *pRoomInfo = pPositionInfo;
			Write16(pRoomInfo, 0); // len

			Write16(pRoomInfo, Room->ID);			   // RoomID
			Write16(pRoomInfo, Room->BaseGameMode);	   // BaseGameMode
			Write16(pRoomInfo, Room->SubGameMode);	   // SubGameMode
			Write8(pRoomInfo, Room->CurrentPlayerNum); // CurrentPlayerNum
			Write8(pRoomInfo, Room->SeatNum);		   // TotalSeatNum
			Write8(pRoomInfo, Room->startStatus);	   // Status
			Write32(pRoomInfo, Room->MapID);		   // MapID
			Write8(pRoomInfo, Room->Flag);			   // Flag
			Write8(pRoomInfo, MaxRoomName);			   // RoomName[]
			memcpy(pRoomInfo, Room->Name, MaxRoomName);
			pRoomInfo += MaxRoomName;
			Write8(pRoomInfo, 0); // HasFBInfo
			// for (size_t i = 0; i < 0; i++)
			//{ //FBInfo
			//	BYTE* pFBInfo = pRoomInfo;
			//	Write16(pFBInfo, 0); //len

			//	Write8(pFBInfo, 0); //AvailableLev
			//	Write8(pFBInfo, 0); //Lev
			//	Write8(pFBInfo, 0); //SubLev
			//	Write32(pFBInfo, 0); //BossID
			//	Write8(pFBInfo, 0); //FBSeason

			//	len = pFBInfo - pRoomInfo;
			//	Set16(pRoomInfo, (WORD)len);
			//	pRoomInfo += len;
			//}
			Write8(pRoomInfo, 0);  // SpeFlag
			Write32(pRoomInfo, 0); // RoomNo
			Write16(pRoomInfo, 0); // SvrId
			Write16(pRoomInfo, 0); // SceneID
			Write8(pRoomInfo, 0);  // CurrentOBNum
			Write8(pRoomInfo, 0);  // TotalOBNum
			Write8(pRoomInfo, 0);  // SpecialActivityStatus
			Write8(pRoomInfo, 0);  // AllowAutoStart
			Write32(pRoomInfo, 0); // Gender
			Write32(pRoomInfo, 0); // LocaleCode
			Write8(pRoomInfo, 0);  // TalkRoomBGID
			Write8(pRoomInfo, 0);  // SpecialActivityIDNum
			Write8(pRoomInfo, 0);  // SpecialActivityInfoNum
			for (size_t i = 0; i < 0; i++)
			{ // SpecialActivityInfo
				BYTE *pSpecialActivityInfo = pRoomInfo;
				Write16(pSpecialActivityInfo, 0); // len

				Write8(pSpecialActivityInfo, 0); // ProfessionLicenseInfoNum
				for (size_t i = 0; i < 0; i++)
				{ // ProfessionLicenseInfo
					BYTE *pProfessionLicenseInfo = pSpecialActivityInfo;
					Write16(pProfessionLicenseInfo, 0); // len

					Write8(pProfessionLicenseInfo, 6); // BigLevel
					Write8(pProfessionLicenseInfo, 5); // SmallLevel

					len = pProfessionLicenseInfo - pSpecialActivityInfo;
					Set16(pSpecialActivityInfo, (WORD)len);
					pSpecialActivityInfo += len;
				}

				len = pSpecialActivityInfo - pRoomInfo;
				Set16(pRoomInfo, (WORD)len);
				pRoomInfo += len;
			}
			Write8(pRoomInfo, 0);  // ValueNum
			Write32(pRoomInfo, 0); // EnterRoomValue[]
			Write8(pRoomInfo, 0);  // ParaNum
			Write32(pRoomInfo, 0); // ParaList[]
			Write32(pRoomInfo, 0); // GSvrRoomNo
			Write8(pRoomInfo, 0);  // OwnerName[]

			len = pRoomInfo - pPositionInfo;
			Set16(pPositionInfo, (WORD)len);
			pPositionInfo += len;
		}
		Write8(pPositionInfo, 0); // StatusLen
		Write8(pPositionInfo, 0); // CanJoin

		len = pPositionInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0);  // Type
	Write32(p, 0); // Para0
	Write32(p, 0); // Para1
	Write8(p, 0);  // GVoiceRoomNameLen
	Write32(p, 0); // EquipedItemNum
	for (size_t i = 0; i < 0; i++)
	{ // PlayerItemInfo
		BYTE *pPlayerItemInfo = p;
		Write16(pPlayerItemInfo, 0); // len

		Write32(pPlayerItemInfo, 0); // ItemID
		Write32(pPlayerItemInfo, 0); // ItemNum
		Write32(pPlayerItemInfo, 0); // AvailPeriod
		Write8(pPlayerItemInfo, 0);	 // Status
		Write32(pPlayerItemInfo, 0); // ObtainTime
		Write32(pPlayerItemInfo, 0); // OtherAttribute
		Write16(pPlayerItemInfo, 0); // ItemType

		len = pPlayerItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 1); // ReserveFlag
	Write8(p, 0); // Gender

	len = p - buf;
	SendToClient(Client, 569, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestRaiseLoverBabyDirectlyInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	short LoveBabyGender = Read16(p);
	UCHAR BabySeq = Read8(p);
	ResponseRaiseLoverBabyInfo(Client, BabySeq, LoveBabyGender, 10001, 10002);
}
void NotifyClientInviteRaiseLoveBabyFailedInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Uin
	Write32(p, 0); // MasterLoveUin
	Write32(p, 0); // ViceLoveUin
	Write16(p, 0); // LoveBabyGender
	Write16(p, 0); // ResultID

	len = p - buf;
	SendToClient(Client, 24361, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyClientReceiveRaiseLoveBabyInvition(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 10001); // Uin
	Write32(p, 10001); // MasterLoveUin
	Write32(p, 10002); // ViceLoveUin
	Write16(p, 0);	   // LoveBabyGender

	len = p - buf;
	SendToClient(Client, 24358, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestRaiseLoveBabyInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT InviteUin = Read32(p);
	UINT ViceUin = Read32(p);
	short LoveBabyGender = Read16(p);
	UCHAR BabySeq = Read8(p);
	ResponseRaiseLoverBabyInfo(Client, BabySeq, LoveBabyGender, InviteUin, ViceUin);
}

void ResponseRaiseLoverBabyInfo(ClientNode *Client, UCHAR BabySeq, short LoveBabyGender, UINT InviteUin, UINT ViceUin)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin);	// Uin
	Write32(p, InviteUin);		// InviteUin
	Write32(p, ViceUin);		// ViceUin
	Write16(p, LoveBabyGender); // LoveBabyGender
	Write8(p, BabySeq);			// BabySeq

	len = p - buf;
	SendToClient(Client, 24350, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestRankedMatchPersonalPanelInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT DstUin = Read32(p);
	UINT Time = Read32(p);
	ResponseRankedMatchPersonalPanelInfo(Client);
}

void ResponseRankedMatchPersonalPanelInfo(ClientNode *Client)
{
	BYTE buf[8192 * 2];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResltID
	Write16(p, 37);			 // SeasonRecordInfoNums
	for (size_t i = 0; i < 37; i++)
	{ // SeasonRecordInfos
		BYTE *pSeasonRecordInfos = p;
		Write16(pSeasonRecordInfos, 0); // len

		Write32(pSeasonRecordInfos, i + 1);	 // SeasonID
		Write32(pSeasonRecordInfos, 137);	 // MapID
		Write32(pSeasonRecordInfos, 999);	 // RaceNums
		Write32(pSeasonRecordInfos, 999);	 // WinNums
		Write32(pSeasonRecordInfos, 999);	 // FinishNums
		Write32(pSeasonRecordInfos, 999);	 // ArenaWinNums
		Write32(pSeasonRecordInfos, 999);	 // ArenaRaceNums
		Write32(pSeasonRecordInfos, 999);	 // ArenaFiveWinNums
		Write32(pSeasonRecordInfos, 119058); // CommonlyUsedKartID
		Write32(pSeasonRecordInfos, 999);	 // MaxContinuousWinNums
		Write32(pSeasonRecordInfos, 999);	 // CurrentContinuousWinNums
		Write32(pSeasonRecordInfos, 999);	 // SeasonRankTop1Nums
		Write32(pSeasonRecordInfos, 999);	 // SeasonFinishNums
		Write32(pSeasonRecordInfos, 119058); // BenmingKartID

		len = pSeasonRecordInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 31); // FeishInfoNums
	UINT FeishiInfos[] =
		{
			94372, 107121, 107122, 109443, 109444, 111296, 111297, 113485, 113486, 113762, 113763, 115493, 115494, 118366, 118367, 118859, 118860, 120356, 120357,
			121988, 121989, 123296, 123297, 124741, 124742, 126526, 126527, 127502, 127503, 129237, 129238};
	for (size_t i = 0; i < 31; i++)
	{ // FeishiInfos
		BYTE *pFeishiInfos = p;
		Write16(pFeishiInfos, 0); // len

		Write32(pFeishiInfos, FeishiInfos[i]); // FeishItemID
		Write32(pFeishiInfos, 0);			   // FeiShiDescLen
		Write8(pFeishiInfos, 1);			   // HaveOrNot

		len = pFeishiInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // SeasonDesInfoNums
	for (size_t i = 0; i < 0; i++)
	{ // RankedMatchSeasonDesInfos
		BYTE *pRankedMatchSeasonDesInfos = p;
		Write16(pRankedMatchSeasonDesInfos, 0); // len

		Write32(pRankedMatchSeasonDesInfos, i + 1);						// SeasonID
		WriteString(pRankedMatchSeasonDesInfos, "杀了你", MaxNickName); // SeasonName[]

		len = pRankedMatchSeasonDesInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 37); // SeasonSuperRaceInfoNums
	for (size_t i = 0; i < 37; i++)
	{ // SuperRaceInfo
		BYTE *pSuperRaceInfo = p;
		Write16(pSuperRaceInfo, 0); // len

		Write32(pSuperRaceInfo, i + 1); // SeasonID
		Write32(pSuperRaceInfo, 999);	// SuperRaceTotalRank
		Write32(pSuperRaceInfo, 999);	// SuperRaceTotalRaceNums
		Write32(pSuperRaceInfo, 999);	// SuperRaceTotalFinishNums
		Write32(pSuperRaceInfo, 0);		// Para1
		Write32(pSuperRaceInfo, 0);		// Para2
		Write32(pSuperRaceInfo, 0);		// Para3
		Write32(pSuperRaceInfo, 0);		// Para4
		Write32(pSuperRaceInfo, 0);		// Para5

		len = pSuperRaceInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 10);	 // BestSpeedGodShowStep
	Write32(p, 999); // MaxSeasonRankTop1Nums
	Write32(p, 999); // MaxSeasonFinishNums

	len = p - buf;
	SendToClient(Client, 25012, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestRankedMatchPannelOperate(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT OperateType = Read32(p);
	UINT Time = Read32(p);
	ResponseRankedMatchPannelOperate(Client, OperateType);
}

void ResponseRankedMatchPannelOperate(ClientNode *Client, UINT OperateType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	Write32(p, OperateType); // CurrentStat

	len = p - buf;
	SendToClient(Client, 25013, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyAppellationList(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	Write8(p, Appellations.size()); // AppellationNum
	Mutex2.lock();
	std::map<UINT, AppellationInfo *>::iterator iter;
	for (iter = Appellations.begin(); iter != Appellations.end(); iter++)
	{ // Achieves
		AppellationInfo *AppellationInfo = iter->second;
		if (!AppellationInfo)
		{
			continue;
		}
		BYTE *pRecord = p;
		Write16(pRecord, 0); // len

		Write8(pRecord, AppellationInfo->Type);		  // Type
		Write8(pRecord, AppellationInfo->Level);	  // Level
		Write8(pRecord, 0);							  // Status
		Write8(pRecord, AppellationInfo->Difficulty); // Difficulty
		Write32(pRecord, AppellationInfo->MapId);	  // MapId
		Write32(pRecord, AppellationInfo->Value);	  // Value

		len = pRecord - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Mutex2.unlock();
	len = p - buf;
	SendToClient(Client, 1616, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestWearAppellation(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UCHAR Type = Read8(p);
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	sql = "update BaseInfo set AppellationType = ? where Uin = ?";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Type);
		sqlite3_bind_int(stmt, 2, Client->Uin);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	ResponseWearAppellation(Client, Type);
}

void ResponseWearAppellation(ClientNode *Client, UCHAR Type)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);	 // ResultId
	Write8(p, Type); // Type

	len = p - buf;
	SendToClient(Client, 1618, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestUnwearAppellation(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UCHAR Type = Read8(p);
	ResponseUnwearAppellation(Client, Type);
}

void ResponseUnwearAppellation(ClientNode *Client, UCHAR Type)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);	 // ResultId
	Write8(p, Type); // Type

	len = p - buf;
	SendToClient(Client, 1619, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetShopNonSaleInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseGetShopNonSaleInfo(Client);
}

void ResponseGetShopNonSaleInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultId
	Write32(p, Client->Uin); // Uin
	Write32(p, 5);			 // TotalShopNonSaleItemCount
	Write32(p, 5);			 // ItemNum
	for (size_t i = 0; i < 5; i++)
	{ // ShopNonSaleItemInfo
		BYTE *pShopNonSaleItemInfo = p;
		Write16(pShopNonSaleItemInfo, 0); // len

		Write32(pShopNonSaleItemInfo, 10020); // ItemID
		Write32(pShopNonSaleItemInfo, 100);	  // FocusCount

		len = pShopNonSaleItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // Seq
	Write8(p, 5);  // HasClientPlayerNonSaleItemInfoList
	for (size_t i = 0; i < 5; i++)
	{ // ClientPlayerNonSaleItemInfoList
		BYTE *pClientPlayerNonSaleItemInfoList = p;
		Write16(pClientPlayerNonSaleItemInfoList, 0); // len

		Write32(pClientPlayerNonSaleItemInfoList, 5); // ItemNum
		for (size_t i = 0; i < 5; i++)
		{ // PlayerNonSaleItemInfo
			BYTE *pPlayerNonSaleItemInfo = pClientPlayerNonSaleItemInfoList;
			Write16(pPlayerNonSaleItemInfo, 0); // len

			Write32(pPlayerNonSaleItemInfo, 10020); // ItemID
			Write32(pPlayerNonSaleItemInfo, 100);	// FocusCount
			Write32(pPlayerNonSaleItemInfo, 0);		// FocusTime

			len = pPlayerNonSaleItemInfo - pClientPlayerNonSaleItemInfoList;
			Set16(pClientPlayerNonSaleItemInfoList, (WORD)len);
			pClientPlayerNonSaleItemInfoList += len;
		}

		len = pClientPlayerNonSaleItemInfoList - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 20126, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestQsLabCommittalTaskInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT SeasonId = Read32(p);
	UCHAR Type = Read8(p);
	ResponseQsLabCommittalTaskInfo(Client);
}

void ResponseQsLabCommittalTaskInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	Write8(p, 1);			 // TaskNum
	for (size_t i = 0; i < 1; i++)
	{ // QsLabCommittalTaskInfo
		BYTE *pQsLabCommittalTaskInfo = p;
		Write16(pQsLabCommittalTaskInfo, 0); // len

		Write32(pQsLabCommittalTaskInfo, 0); // TaskID
		Write32(pQsLabCommittalTaskInfo, 0); // UnLockLeftTime
		Write8(pQsLabCommittalTaskInfo, 0);	 // Status

		len = pQsLabCommittalTaskInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 1); // SeasonNum
	for (size_t i = 0; i < 1; i++)
	{ // QsLabSeasonTaskCfg
		BYTE *pQsLabSeasonTaskCfg = p;
		Write16(pQsLabSeasonTaskCfg, 0); // len

		Write16(pQsLabSeasonTaskCfg, 0); // SeasonID
		Write8(pQsLabSeasonTaskCfg, 0);	 // TaskNum
		for (size_t i = 0; i < 0; i++)
		{ // TaskAwardStatus
			BYTE *pTaskAwardStatus = pQsLabSeasonTaskCfg;
			Write16(pTaskAwardStatus, 0); // len

			Write32(pTaskAwardStatus, 0); // TaskId
			Write8(pTaskAwardStatus, 0);  // ReceiveStatus
			Write8(pTaskAwardStatus, 0);  // AwardStatus

			len = pTaskAwardStatus - pQsLabSeasonTaskCfg;
			Set16(pQsLabSeasonTaskCfg, (WORD)len);
			pQsLabSeasonTaskCfg += len;
		}

		len = pQsLabSeasonTaskCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 1); // Type

	len = p - buf;
	SendToClient(Client, 28419, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyBugletTalkALL(ClientNode *SrcClient, USHORT ContentLength, char *Content, BYTE BugletType)
{
	std::map<UINT, ClientNode *> Clients = GetClients();
	std::mutex Mutex2;
	Mutex2.lock();
	auto iter = Clients.begin();
	for (iter = Clients.begin(); iter != Clients.end(); iter++)
	{
		ClientNode *Client = iter->second;
		if (SrcClient == Client)
		{
			continue;
		}
		NotifyBugletTalk(SrcClient, Client, ContentLength, Content, BugletType);
	}
	Mutex2.unlock();
}
void RequestDeleteFriendGroup(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	std::map<UINT, ClientNode*> Clients = GetClients();
	std::mutex Mutex2;
	Mutex2.lock();
	auto iter = Clients.begin();
	for (iter = Clients.begin(); iter != Clients.end(); iter++)
	{
		ClientNode* Node = iter->second;
		if (Node&&Node != Client)
		{
			ResponseFindPlayerByQQ(Client, Node->Uin, 0, 0);
		}
	}
	Mutex2.unlock();
}
void NotifyBugletTalk(ClientNode *SrcClient, ClientNode *Client, USHORT ContentLength, char *Content, BYTE BugletType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	int BugletNum = 1;
	Write8(p, BugletNum); // BugletNum
	for (size_t i = 0; i < BugletNum; i++)
	{ // BugletInfo
		BYTE *pBugletInfo = p;
		Write16(pBugletInfo, 0);									   // len
		Write32(pBugletInfo, SrcClient->Uin);						   // SrcUin
		WriteString(pBugletInfo, SrcClient->NickName, MaxBaseNameNew); // SrcNickName[]
		WriteString(pBugletInfo, "MC骑士团", MaxBaseNameNew);		   // SrcGuildName[]
		Write8(pBugletInfo, SrcClient->Gender);						   // Gender
		Write16(pBugletInfo, ContentLength);						   // ContentLength
		memcpy(pBugletInfo, Content, ContentLength);
		pBugletInfo += ContentLength;
		Write8(pBugletInfo, BugletType); // BugletType
		Write32(pBugletInfo, 0);		 // Identify
		Write32(pBugletInfo, 0);		 // VipFlag
		Write32(pBugletInfo, 0);		 // Contentid[]
		{								 // NobleInfo
			BYTE *pNobleInfo = pBugletInfo;
			Write16(pNobleInfo, 0); // len

			Write32(pNobleInfo, 0); // NobleID
			Write8(pNobleInfo, 0);	// NobleLevel
			Write32(pNobleInfo, 0); // NoblePoint
			Write32(pNobleInfo, 0); // NobleLeftDays

			len = pNobleInfo - pBugletInfo;
			Set16(pBugletInfo, (WORD)len);
			pBugletInfo += len;
		}
		{ // GuildVipBaseInfo
			BYTE *pGuildVipBaseInfo = pBugletInfo;
			Write16(pGuildVipBaseInfo, 0); // len

			Write8(pGuildVipBaseInfo, 0);  // GuildVipLevel
			Write32(pGuildVipBaseInfo, 0); // GuildVipPoint

			len = pGuildVipBaseInfo - pBugletInfo;
			Set16(pBugletInfo, (WORD)len);
			pBugletInfo += len;
		}
		Write8(pBugletInfo, 0); // HasLoverVip
		for (size_t i = 0; i < 0; i++)
		{ // LoverVipInfo
			BYTE *pLoverVipInfo = pBugletInfo;
			Write16(pLoverVipInfo, 0); // len

			Write8(pLoverVipInfo, 0);  // LoverVipLevel
			Write32(pLoverVipInfo, 0); // LoverVipPoint
			Write8(pLoverVipInfo, 0);  // GrowRate

			len = pLoverVipInfo - pBugletInfo;
			Set16(pBugletInfo, (WORD)len);
			pBugletInfo += len;
		}
		Write8(pBugletInfo, 0);	 // GardenLevel
		WriteUn(pBugletInfo, 0); // ConsumeVipLevel
		{						 // EmperorInfo
			BYTE *pEmperorInfo = pBugletInfo;
			Write16(pEmperorInfo, 0); // len

			Write8(pEmperorInfo, 0);  // EmperorLevel
			Write32(pEmperorInfo, 0); // EmperorPoint
			Write32(pEmperorInfo, 0); // EmperorLeftDays
			Write8(pEmperorInfo, 0);  // EmperorGrowRate

			len = pEmperorInfo - pBugletInfo;
			Set16(pBugletInfo, (WORD)len);
			pBugletInfo += len;
		}
		Write8(pBugletInfo, 1); // HasCarLottoryRecord
		for (size_t i = 0; i < 1; i++)
		{ // CarLottoryRecord
			BYTE *pCarLottoryRecord = pBugletInfo;
			Write16(pCarLottoryRecord, 0); // len

			Write32(pCarLottoryRecord, 10);	   // AwardTime
			Write32(pCarLottoryRecord, 10020); // ItemID
			Write32(pCarLottoryRecord, 1);	   // ItemNum
			Write32(pCarLottoryRecord, -1);	   // AvailPeriod

			len = pCarLottoryRecord - pBugletInfo;
			Set16(pBugletInfo, (WORD)len);
			pBugletInfo += len;
		}

		len = pBugletInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 555, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestGetPwdRedEnvelopeContent(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseGetPwdRedEnvelopeContent(Client);
}

void ResponseGetPwdRedEnvelopeContent(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	Write16(p, 0);			 // ContentLen

	len = p - buf;
	SendToClient(Client, 24724, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestSendPwdRedEnvelope(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT ContentLen = Read16(p);
	char *Content = (char *)p;
	p += ContentLen;
	int c;
	c = atof(Content);
		ItemInfo Items[3000];
		ItemInfo Item = PlayerDB_AddItem2(Client->Uin, c, 1, -1, false);
		Items[0].ItemID = c;
		Items[0].ItemNum = Item.ItemNum;
		Items[0].AvailPeriod = Item.AvailPeriod;
		Items[0].Status = Item.Status;
		Items[0].ObtainTime = Item.ObtainTime;
		Items[0].OtherAttribute = Item.OtherAttribute;
		Items[0].ItemType = Item.ItemType;
		NotifyClientAddItem(Client, 1, Items);
		int itemNum = 1;
		ItemInfo *aItemStatus = (ItemInfo *)malloc(sizeof(ItemInfo) * itemNum);
		aItemStatus[0].ItemID = c;
		aItemStatus[0].ItemNum = Item.ItemNum;
		aItemStatus[0].AvailPeriod = Item.AvailPeriod;
		ResponseOpenBox(Client, 22107, aItemStatus, itemNum, 1);
}

void ResponseSendPwdRedEnvelope(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID

	len = p - buf;
	SendToClient(Client, 24723, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void delay(int seconds) //  参数必须为整型，表示延时多少秒
{
	clock_t start = clock();
	clock_t lay = (clock_t)seconds * CLOCKS_PER_SEC;
	while ((clock() - start) < lay)
		;
}

void delay(double seconds) //  参数为双精度浮点型。这个函数是我修改的上面那个函数，重载一下。
{
	double start = clock();
	double lay = (double)seconds * CLOCKS_PER_SEC;
	while ((clock() - start) < lay)
		;
}
void RequestInternationalSeriesGetInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseInternationalSeriesGetInfo(Client);
}

void ResponseInternationalSeriesGetInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	Write8(p, 0);			 // ReasonLen
	Write32(p, 5);			 // CoinNum
	Write16(p, 99999);		 // Score
	Write16(p, 5);			 // CanGetCoinNum
	Write16(p, 0);			 // NextRefreshCoinTime
	Write32(p, 0);			 // BeginDate
	Write32(p, 0);			 // EndDate
	Write32(p, 0);			 // CoinClearDate
	for (size_t i = 0; i < 0; i++)
	{ // MapIDList
		BYTE *pMapIDList = p;
		Write16(pMapIDList, 0); // len

		Write32(pMapIDList, 0); // MapIDList[]

		len = pMapIDList - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // HasGotAward
	for (size_t i = 0; i < 0; i++)
	{ // BoxInfo
		BYTE *pBoxInfo = p;
		Write16(pBoxInfo, 0); // len

		Write16(pBoxInfo, 0); // BoxNeedCoins
		{					  // Award
			BYTE *pAward = pBoxInfo;
			Write16(pAward, 0); // len

			Write32(pAward, 0); // Exp
			Write32(pAward, 0); // Money
			Write32(pAward, 0); // SuperMoney
			Write16(pAward, 0); // ItemNum
			for (size_t i = 0; i < 0; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pAward;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, 0); // ItemID
				Write32(pItemInfo, 0); // ItemNum
				Write32(pItemInfo, 0); // AvailPeriod
				Write8(pItemInfo, 0);  // Status
				WriteUn(pItemInfo, 0); // ObtainTime
				Write32(pItemInfo, 0); // OtherAttribute
				Write16(pItemInfo, 0); // ItemType

				len = pItemInfo - pAward;
				Set16(pAward, (WORD)len);
				pAward += len;
			}
			Write32(pAward, 0); // Coupons
			Write32(pAward, 0); // GuildPoint
			Write32(pAward, 0); // LuckMoney
			Write8(pAward, 0);	// ExtendInfoNum
			for (size_t i = 0; i < 0; i++)
			{ // ExtendInfoAward
				BYTE *pExtendInfoAward = pAward;
				Write16(pExtendInfoAward, 0); // len

				Write16(pExtendInfoAward, 0); // Key
				Write32(pExtendInfoAward, 0); // AddValue
				Write32(pExtendInfoAward, 0); // TotalValue
				Write8(pExtendInfoAward, 0);  // ShowPriority
				Write16(pExtendInfoAward, 0); // BuyNeedScore
				Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

				len = pExtendInfoAward - pAward;
				Set16(pAward, (WORD)len);
				pAward += len;
			}
			Write32(pAward, 0); // SpeedCoin

			len = pAward - pBoxInfo;
			Set16(pBoxInfo, (WORD)len);
			pBoxInfo += len;
		}

		len = pBoxInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 99999); // CurScoreCanGetCoinNum

	len = p - buf;
	SendToClient(Client, 25110, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestDressInfoOperate(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT OptType = Read16(p);//5穿戴 //1 保存 // 0查询 //2 清空
	USHORT SuitIdx = Read16(p);
	short NameLen = Read16(p);
	short ItemNum = Read16(p);
	int ItemID[100] = {};
	for (int i = 0; i < ItemNum; i++)
	{
		ItemID[i] = Read32(p);
	}
	UCHAR FileMd5Len = Read8(p);
	UINT FileSize = Read32(p);
	{//重新查询性别
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;
		sql = "SELECT Gender FROM BaseInfo WHERE Uin = ?";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				Client->Gender = sqlite3_column_int(stmt, 0);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	
	if (OptType==0) {//打开背包查询
		ResponseDressInfoOperate(Client, OptType, ItemNum, ItemID, 0, FileMd5Len);
	}
	else if (OptType == 1){//保存
		ResponseDressInfoOperate(Client, OptType, ItemNum, ItemID, SuitIdx, FileMd5Len);
		
	}
	else if (OptType == 2) {//清空
		ResponseDressInfoOperate(Client, OptType, ItemNum, ItemID, SuitIdx, FileMd5Len);
	}
	else if (OptType == 5) {//穿戴
		 {//穿戴逻辑
			ItemStatus* aItemStatus = (ItemStatus*)malloc(sizeof(ItemStatus) * 50);
			for (int j = 0; j < ItemNum; j++)
			{//取消穿戴
				aItemStatus[j].ItemID = ItemID[j];
				aItemStatus[j].NewStatus = 0;
				PlayDB_UpdateItemStatus(Client->Uin, aItemStatus[j].ItemID, 0);

			}
			
			const char* sql = NULL;
			sqlite3_stmt* stmt = NULL;
			int result;
			sql = "SELECT ItemID FROM DressInfo WHERE Uin = ? And GenderType = ? AND SuitIdx=?";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);
				sqlite3_bind_int(stmt, 2, Client->Gender);
				sqlite3_bind_int(stmt, 3, SuitIdx);
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{
					aItemStatus[ItemNum].ItemID = sqlite3_column_int(stmt, 0);
					aItemStatus[ItemNum].NewStatus = 1;
					PlayDB_UpdateItemStatus(Client->Uin, aItemStatus[ItemNum].ItemID, 1);
					ItemNum++;
				}
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			ResponseChangeItemStatus(Client, ItemNum, aItemStatus);
			// ResponseSaveShapeRefit4(Client,Client->KartID);
			do
			{ // 普通房间
				RoomNode* Room = GetRoom(Client->RoomID);
				if (!Room)
				{
					break;
				}
				for (char i = 0; i < 6; i++)
				{
					ClientNode* RoomClient = Room->Player[i];
					if (RoomClient && RoomClient != Client)
					{
						NotifyChangeItemStatus(RoomClient, Client->Uin, ItemNum, aItemStatus);
					}
				}
			} while (false);
			do
			{ // 休闲区
				ChatRoomNode* ChatRoom = GetChatRoom(Client->ChatRoomID);
				if (!ChatRoom)
				{
					return;
				}
				for (char i = 0; i < 99; i++)
				{
					ClientNode* RoomClient = ChatRoom->Player[i];
					if (RoomClient && RoomClient != Client)
					{
						NotifyChangeItemStatus(RoomClient, Client->Uin, ItemNum, aItemStatus);
					}
				}
			} while (false);
			free(aItemStatus);
		}
		
		
	}
}

void ResponseDressInfoOperate(ClientNode *Client, USHORT OptType, short ItemNum, int ItemID[100], USHORT SuitIdx, UCHAR FileMd5Len)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	Write8(p, 0);			 // ReasonLen
	Write16(p, OptType);	 // OptType
	if (OptType==0) {
		UINT DressSuitNum = 3;
		Write8(p, DressSuitNum);			 // DressSuitNum
		for (size_t i = 0; i < DressSuitNum; i++)
		{ 
			ItemNum = 0;
			const char* sql = NULL;
			sqlite3_stmt* stmt = NULL;
			int result;
			sql = "SELECT ItemID FROM DressInfo WHERE Uin = ? And GenderType = ? AND SuitIdx=?";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);
				sqlite3_bind_int(stmt, 2, Client->Gender);
				sqlite3_bind_int(stmt, 3, i);
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{
					ItemID[ItemNum] = sqlite3_column_int(stmt, 0);
					ItemNum++;
				}
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			// DressSuitInfo
			BYTE* pDressSuitInfo = p;
			Write16(pDressSuitInfo, 0); // len

			Write16(pDressSuitInfo, i); // SuitIdx
			Write16(pDressSuitInfo, ItemNum); // ItemNum
			for (int j = 0; j < ItemNum; j++)
			{
				Write32(pDressSuitInfo, ItemID[j]); // ItemID[]
			}
			Write8(pDressSuitInfo, 0);				// DressModel
			Write8(pDressSuitInfo, Client->Gender); // GenderType
			Write16(pDressSuitInfo, 0);				// NameLen
			Write8(pDressSuitInfo, FileMd5Len);		// FileMd5Len
			Write16(pDressSuitInfo, 0);				// UuidLen
			Write32(pDressSuitInfo, 0);				// LastUpdateTime

			len = pDressSuitInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	else if(OptType==1){
		for (size_t i = 0; i < 1; i++)
		{ // DressSuitInfo
			PlayDB_ClearDress(Client->Uin, SuitIdx, Client->Gender);
			for (int j = 0; j < ItemNum; j++)
			{
				PlayDB_SaveDress(Client->Uin, SuitIdx, ItemID[j], Client->Gender);
			}

		}
		ResponseDressInfoOperate(Client, 0, ItemNum, ItemID, 0, FileMd5Len);
		return;
	}
	else if (OptType == 2) {
		PlayDB_ClearDress(Client->Uin, SuitIdx, Client->Gender);
		ResponseDressInfoOperate(Client, 0, ItemNum, ItemID, 0, FileMd5Len);
		return;
	}
	
	Write32(p, 0); // InvalidNum
	Write32(p, 0); // InvalidItemID[]
	Write8(p, 1);  // HasCfg
	for (size_t i = 0; i < 1; i++)
	{ // Cfg
		BYTE *pCfg = p;
		Write16(pCfg, 0); // len

		Write32(pCfg, 6);  // DressSuitNumMax
		Write32(pCfg, 99999); // UploadSuitPicMaxSize

		len = pCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 28413, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void InitClientGameArrt(ClientNode *Client)
{
	Client->useDQ = 0;
	Client->DriftCount = 0;
	Client->kartPow = 0;
	Client->NumPropsInHand = 0;
	Client->N2OCountRequiredForReborn = 0;
	Client->Fuel = 0;
	Client->TeamFuel = 0;
	Client->CollectedN2O = 0;
	Client->KartSkillID = 0;
	Client->PetSkillID = 0;
	Client->isRaceBegin = false;
	Client->IsFinish = false;
	//Client->IsFinish == FALSE;
	Client->taskID = 0;
	Client->SubGameMode = 0;
	Client->BaseGameMode = 0;
	Client->BorderRoomID = 0;
	Client->TaskID = 0;
	Client->JumpThread = 0;
	Client->getPropNum = 0;
	//Client->PropList[0] = 0;
	//Client->PropList[1] = 0;
	Client->SkillStoneList[0] = 0;
	Client->SkillStoneList[1] = 0;
	Client->SkillStoneList[2] = 0;
	Client->SkillStoneList[3] = 0;
	Client->SkillStoneList[4] = 0;

	Client->GameTime = 0;
	Client->KartStatus = 0;
	Client->StartGameTime = false;
	Client->StoneMode = false;
	Client->UseCountUpperlimit = 0;
	RoomNode *Room = GetRoom(Client->RoomID);
	if (Room)
	{
		Client->SubGameMode = Room->SubGameMode;
		Client->BaseGameMode = Room->BaseGameMode;
	}
}
const static int GetShapeLevel(UINT ShowChoice)
{
	int NewShapeLevel = 0;
	switch (ShowChoice)
	{
	case 2:
		NewShapeLevel = 1;
		break;
	case 4:
		NewShapeLevel = 2;
		break;
	case 8:
		NewShapeLevel = 3;
		break;
	case 16:
		NewShapeLevel = 4;
		break;
	case 48:
		NewShapeLevel = 5;
		break;
	case 112:
		NewShapeLevel = 6;
		break;
	case 128:
		NewShapeLevel = 7;
		break;
	case 384:
		NewShapeLevel = 8;
		break;
	case 896:
		NewShapeLevel = 9;
		break;
	case 1920:
		NewShapeLevel = 10;
		break;
	default:
		break;
	}

	return NewShapeLevel;
}
const static int GetLegendShapeID(UINT LegendShapeID, UINT ShapeLevel)
{
	UINT NewLegendShapeID = 0;
	if (LegendShapeID == 120246) // 爆天-曜影	赛车皮肤
	{
		switch (ShapeLevel)
		{
		case 1:
			NewLegendShapeID = 118727;
			break; // 爆天-曜影-黑白	赛车皮肤
		case 2:
			NewLegendShapeID = 120247;
			break; // 爆天-曜影-黑红	赛车皮肤
		case 3:
			NewLegendShapeID = 120248;
			break; // 爆天-曜影-蓝绿	赛车皮肤
		case 4:
			NewLegendShapeID = 118728;
			break; // 爆天-曜影-中阶	赛车皮肤
		case 5:
			NewLegendShapeID = 118728;
			break; // 爆天-曜影-中阶	赛车皮肤
		case 6:
			NewLegendShapeID = 118728;
			break; // 爆天-曜影-中阶	赛车皮肤
		case 7:
			NewLegendShapeID = 118729;
			break; // 爆天-曜影-高阶	赛车皮肤
		case 8:
			NewLegendShapeID = 118729;
			break; // 爆天-曜影-高阶	赛车皮肤
		case 9:
			NewLegendShapeID = 118729;
			break; // 爆天-曜影-高阶	赛车皮肤
		case 10:
			NewLegendShapeID = 118729;
			break; // 爆天-曜影-高阶	赛车皮肤
		default:
			break;
		}
	}
	else if (LegendShapeID == 127359) // 雷诺·幻夜 赛车皮肤
	{
		switch (ShapeLevel)
		{
		case 1:
			NewLegendShapeID = 126533;
			break; // 雷诺-幻夜-天青	赛车皮肤
		case 2:
			NewLegendShapeID = 126534;
			break; // 雷诺-幻夜-日出	赛车皮肤
		case 3:
			NewLegendShapeID = 126535;
			break; // 雷诺-幻夜-童梦	赛车皮肤
		case 4:
			NewLegendShapeID = 126536;
			break; // 雷诺-幻夜-中阶	赛车皮肤
		case 5:
			NewLegendShapeID = 126536;
			break; // 雷诺-幻夜-中阶	赛车皮肤
		case 6:
			NewLegendShapeID = 126536;
			break; // 雷诺-幻夜-中阶	赛车皮肤
		case 7:
			NewLegendShapeID = 126537;
			break; // 雷诺-幻夜-高阶	赛车皮肤
		case 8:
			NewLegendShapeID = 126537;
			break; // 雷诺-幻夜-高阶	赛车皮肤
		case 9:
			NewLegendShapeID = 126537;
			break; // 雷诺-幻夜-高阶	赛车皮肤
		case 10:
			NewLegendShapeID = 126537;
			break; // 雷诺-幻夜-高阶	赛车皮肤
		default:
			break;
		}
	}
	else if (LegendShapeID == 127360) // 众神·乾坤 赛车皮肤
	{
		switch (ShapeLevel)
		{
		case 1:
			NewLegendShapeID = 127361;
			break; // 众神·乾坤-炫紫	赛车皮肤
		case 2:
			NewLegendShapeID = 127362;
			break; // 众神·乾坤-魔能	赛车皮肤
		case 3:
			NewLegendShapeID = 127363;
			break; // 众神·乾坤-黯红	赛车皮肤
		case 4:
			NewLegendShapeID = 127364;
			break; // 众神·乾坤-中阶	赛车皮肤
		case 5:
			NewLegendShapeID = 127364;
			break; // 众神·乾坤-中阶	赛车皮肤
		case 6:
			NewLegendShapeID = 127364;
			break; // 众神·乾坤-中阶	赛车皮肤
		case 7:
			NewLegendShapeID = 127365;
			break; // 众神·乾坤-高阶	赛车皮肤
		case 8:
			NewLegendShapeID = 127365;
			break; // 众神·乾坤-高阶	赛车皮肤
		case 9:
			NewLegendShapeID = 127365;
			break; // 众神·乾坤-高阶	赛车皮肤
		case 10:
			NewLegendShapeID = 127365;
			break; // 众神·乾坤-高阶	赛车皮肤
		default:
			break;
		}
	}
	return NewLegendShapeID;
}
void ResponseShapeLegendOperateNew(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	/*char* pMsg = EByteToHexUA2((const unsigned char*)Body, BodyLen);
	printf("ResponseShapeLegendOperate	Hex:%s\n", pMsg);*/

	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UCHAR OpType = Read8(Body);
	UINT ShapeID = Read32(Body);
	UCHAR ShapeLevel = Read8(Body);
	UINT NewShowChoice = Read32(Body);
	int NewShapeLevel = GetShapeLevel(NewShowChoice);
	UINT ShapeSuitID = GetLegendShapeID(ShapeID, NewShapeLevel);
	UINT KartID = 0;
	if (ShapeID == 120246)
	{
		KartID = 74362;
	}
	else if (ShapeID == 127359)
	{
		KartID = 12720;
	}
	else if (ShapeID == 127360)
	{
		KartID = 90945;
	}

	Z_INFO("ResponseShapeLegendOperate Uin:%d OpType:%d ShapeID:%d ShapeLevel:%d NewShowChoice:%d\n", Uin, OpType, ShapeID, NewShapeLevel, NewShowChoice);
	// UINT ShapeIDlist[] = { 126537 ,118729,127365 };

	const char *sql = nullptr;
	sqlite3_stmt *stmt = nullptr;

	// 判断是否有改装数据
	sql = "SELECT KartID  FROM KartRefit WHERE Uin=? AND KartID=?;";
	int RefitKartID = 0;
	if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, KartID);
		if (sqlite3_step(stmt) == SQLITE_ROW)
			RefitKartID = sqlite3_column_int(stmt, 0);
	}
	sqlite3_finalize(stmt);
	stmt = nullptr;
	if (RefitKartID <= 0)
	{
		sql = "INSERT INTO KartRefit(Uin,KartID) VALUES (?,?);";
		if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, KartID);
			sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);
	}
	stmt = nullptr;
	sql = "UPDATE KartRefit SET ShapeSuitID=?,LegendSuitLevel=?,LegendSuitLevelChoice=?,ShapeLegendSuitID=? WHERE Uin=? AND KartID=?;";
	if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, ShapeSuitID);
		sqlite3_bind_int(stmt, 2, NewShapeLevel);
		sqlite3_bind_int(stmt, 3, NewShowChoice);
		sqlite3_bind_int(stmt, 4, ShapeID);

		sqlite3_bind_int(stmt, 5, Uin);
		sqlite3_bind_int(stmt, 6, KartID);
		sqlite3_step(stmt);
	}
	else
	{
		Z_ERROR("ShapeRefit error");
	}
	sqlite3_finalize(stmt);
	stmt = nullptr;

	const UINT ShapeIDlist[] = {120246, 127359, 127360};

	Write16(p, 0);	   // ResultID
	Write8(p, 0);	   // ReasonLen
	Write8(p, OpType); // OpType
	Write16(p, 3);	   // ShapeNum  皮肤数量
	for (size_t i = 0; i < 3; i++)
	{ // ShapeInfo
		BYTE *pShapeInfo = p;
		Write16(pShapeInfo, 0); // len

		Write32(pShapeInfo, ShapeIDlist[i]); // ShapeID
		Write8(pShapeInfo, 10);				 // ShapeLevel
		Write32(pShapeInfo, 4);				 // ShowChoice 显示选项

		len = pShapeInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, ShapeID);	  // ShapeID
	Write8(p, NewShapeLevel); // ShapeLevel
	Write8(p, 1);			  // HasRefit 有改装信息
	for (size_t i = 0; i < 1; i++)
	{ // RefitClientInfo
		BYTE *pRefitClientInfo = p;
		Write16(pRefitClientInfo, 0); // len

		Write32(pRefitClientInfo, Uin);	   // Uin
		Write32(pRefitClientInfo, KartID); // KartId
		Write32(pRefitClientInfo, 0);	   // RefitCout
		Write16(pRefitClientInfo, 0);	   // MaxFlags
		Write16(pRefitClientInfo, 0);	   // WWeight
		Write16(pRefitClientInfo, 0);	   // SpeedWeight
		Write16(pRefitClientInfo, 0);	   // JetWeight
		Write16(pRefitClientInfo, 0);	   // SJetWeight
		Write16(pRefitClientInfo, 0);	   // AccuWeight
		Write32(pRefitClientInfo, 0);	   // ShapeRefitCount
		Write32(pRefitClientInfo, 0);	   // KartHeadRefitItemID
		Write32(pRefitClientInfo, 0);	   // KartTailRefitItemID
		Write32(pRefitClientInfo, 0);	   // KartFlankRefitItemID
		Write32(pRefitClientInfo, 0);	   // KartTireRefitItemID
		{								   // KartRefitExInfo
			BYTE *pKartRefitExInfo = pRefitClientInfo;
			Write16(pKartRefitExInfo, 0); // len

			Write8(pKartRefitExInfo, 0); // SpeedRefitStar
			Write8(pKartRefitExInfo, 0); // JetRefitStar
			Write8(pKartRefitExInfo, 0); // SJetRefitStar
			Write8(pKartRefitExInfo, 0); // AccuRefitStar
			Write8(pKartRefitExInfo, 0); // SpeedAddRatio
			Write8(pKartRefitExInfo, 0); // JetAddRatio
			Write8(pKartRefitExInfo, 0); // SJetAddRatio
			Write8(pKartRefitExInfo, 0); // AccuAddRatio

			len = pKartRefitExInfo - pRefitClientInfo;
			Set16(pRefitClientInfo, (WORD)len);
			pRefitClientInfo += len;
		}
		Write32(pRefitClientInfo, 0);			  // SecondRefitCount
		Write16(pRefitClientInfo, 0);			  // Speed2Weight
		Write16(pRefitClientInfo, 0);			  // DriftVecWeight
		Write16(pRefitClientInfo, 0);			  // AdditionalZSpeedWeight
		Write16(pRefitClientInfo, 0);			  // AntiCollisionWeight
		Write16(pRefitClientInfo, 0);			  // LuckyValue
		Write16(pRefitClientInfo, 0);			  // RefitLuckyValueMaxWeight
		Write32(pRefitClientInfo, ShapeSuitID);	  // ShapeSuitID
		Write8(pRefitClientInfo, NewShapeLevel);  // LegendSuitLevel
		Write32(pRefitClientInfo, NewShowChoice); // LegendSuitLevelChoice
		Write32(pRefitClientInfo, ShapeID);		  // ShapeLegendSuitID

		len = pRefitClientInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, NewShowChoice); // NewShowChoice 新展会选择
	Write32(p, ShapeID);	   // DirectMaxLevelShapeID   直接最大级别形状ID
	Write32(p, 10);			   // DirectMaxShapePreLevel  直接最大形状预级别

	len = p - buf;
	SendToClient(Client, 319, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void ResponseStartTimerChallenge(ClientNode* Client)
{
	Client->TaskID = 0xFFFE;
	Client->GameID = 2;
	Client->TotalRound = 1;
	Client->Loop = false;
	Client->FinishTime = 0;
	Client->Round = 0;
	Client->PassedCheckPoint = 0;
	Client->EndCheckPoint = 225;
	BYTE* p = StartTimerChallenge;
	p += 57;
	Write32(p, Client->Uin);
	p += 82;
	Write32(p, Client->Uin);
	Write32(p, Client->KartID);
	p += 104;
	Write32(p, Client->Uin);

	SendToClient(Client, 509, StartTimerChallenge, 589, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
	NotifyKartPhysParam(Client, NULL);
	//NotifyOtherKartStoneInfo(Client);

	//NotifyTimerChallengeCommonInfo
	const char* buf1 = "\x01\xe5\x00\x00\x00\x03\x00\x0a\x00\x00\x02\x4a\x00\x00\x00\x63\x00\x0a\x00\x00\x02\x4c\x00\x00\x00\x00\x00\x0a\x00\x00\x02\x4e\x00\x00\x00\x00\x00\x00\x13\x88\x00\x00\x00\x03\x00\x26\x05\x00\x0c\x00\x0c\x00\x15\x00\x17\x00\x1b\x00\x31\x00\x32\x00\x33\x00\x3a\x00\x3b\x00\x4b\x00\x4c\x00\x4d\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x92\x0c\x00\x42\x00\x01\x00\x02\x00\x03\x00\x04\x00\x06\x00\x07\x00\x08\x00\x09\x00\x0e\x00\x0f\x00\x11\x00\x12\x00\x1d\x00\x1e\x00\x1f\x00\x20\x00\x25\x00\x26\x00\x27\x00\x29\x00\x2a\x00\x2b\x00\x2c\x00\x2d\x00\x2e\x00\x30\x00\x34\x00\x35\x00\x36\x00\x37\x00\x38\x00\x39\x00\x3c\x00\x3d\x00\x3e\x00\x40\x00\x41\x00\x42\x00\x43\x00\x45\x00\x47\x00\x48\x00\x49\x00\x4a\x00\x4e\x00\x4f\x00\x50\x00\x51\x00\x52\x00\x53\x00\x54\x00\x55\x00\x56\x00\x59\x00\x5a\x00\x5b\x00\x5c\x00\x5d\x00\x5e\x00\x5f\x00\x60\x00\x61\x00\x62\x00\x63\x00\x64\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x88\x0d\x00\x3d\x00\x05\x00\x06\x00\x07\x00\x08\x00\x0a\x00\x0b\x00\x0c\x00\x0d\x00\x0e\x00\x0f\x00\x10\x00\x11\x00\x13\x00\x14\x00\x15\x00\x18\x00\x1a\x00\x1b\x00\x1d\x00\x1f\x00\x20\x00\x21\x00\x22\x00\x23\x00\x24\x00\x25\x00\x26\x00\x27\x00\x28\x00\x2a\x00\x2b\x00\x2c\x00\x2e\x00\x30\x00\x31\x00\x34\x00\x35\x00\x38\x00\x39\x00\x3e\x00\x40\x00\x42\x00\x49\x00\x4a\x00\x53\x00\x56\x00\x59\x00\x5d\x00\x5f\x00\x67\x00\x68\x00\x6a\x00\x73\x00\x7a\x00\x7c\x00\x83\x00\x8c\x00\x92\x00\x93\x00\x9c\x00\xaf\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x6d\x01\x00\x6a\x01\x00\x00\x00\x00\x00\x51\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03\x00\x10\x02\x4e\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x10\x02\x4c\x00\x00\x00\x01\x00\x00\x00\x00\x01\x00\x00\x00\x00\x10\x02\x4a\x00\x00\x00\x63\x00\x00\x00\x63\x01\x00\x00\x63\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x03\x00\x00\x00\x03\x00\x00\x00\x00\x6c\x6f";

	SendToClient(Client, 24239, (BYTE*)buf1, 487, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
	//NotifyTimerChallengeEvent 08

	const char* buf2 = "\x00\x00\x27\x11\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x27\x10\x00\x0F\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	SendToClient(Client, 24238, (BYTE*)buf2, 42, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);

	NotifyRaceBegin(Client);

	const char* buf3 = "\x00\x00\x27\x11\x00\x00\x00\x07\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x0F\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	//NotifyTimerChallengeEvent 07
	SendToClient(Client, 24238, (BYTE*)buf3, 42, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
	//通关全部
	if (Client->JumpThread == 0)
	{
		HANDLE jump = CreateThread(NULL, 0, OnSingleJump, Client, 0, NULL);
		if (jump)
		{
			Client->JumpThread = 1;
			CloseHandle(jump);
		}
	}
}
void ResponsePointChallengeOpenMainBoard(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 0); //ResultID
	Write32(p, 0); //TodayHighestTongGuanNum
	Write32(p, 0); //TodayHighestScore
	Write32(p, 0); //TotayTotalScore
	Write32(p, 5); //TiaoZhanNum
	Write32(p, 5); //TiaoZhanMaxNum
	Write32(p, 0); //GetTiaoZhanLeftSeconds
	Write32(p, 0); //HistoryHighestTongGuanNum
	Write32(p, 0); //HistoryHighestScore
	{ //AwardArray
		BYTE* pAwardArray = p;
		Write16(pAwardArray, 0); //len
		//道具key：586复活卡 588增时卡 590最后一搏
		Write8(pAwardArray, 3); //ExtendInfoNum
		{ //ExtendInfoAward
			BYTE* pExtendInfoAward = pAwardArray;
			Write16(pExtendInfoAward, 0); //len

			Write16(pExtendInfoAward, 586); //Key
			Write32(pExtendInfoAward, 90); //AddValue
			Write32(pExtendInfoAward, 90); //TotalValue
			Write8(pExtendInfoAward, 1); //ShowPriority
			Write16(pExtendInfoAward, 0); //BuyNeedScore
			Write8(pExtendInfoAward, 90); //OneMatchMaxNum

			len = pExtendInfoAward - pAwardArray;
			Set16(pAwardArray, (WORD)len);
			pAwardArray += len;
		}
		{ //ExtendInfoAward
			BYTE* pExtendInfoAward = pAwardArray;
			Write16(pExtendInfoAward, 0); //len

			Write16(pExtendInfoAward, 588); //Key
			Write32(pExtendInfoAward, 0); //AddValue
			Write32(pExtendInfoAward, 0); //TotalValue
			Write8(pExtendInfoAward, 0); //ShowPriority
			Write16(pExtendInfoAward, 0); //BuyNeedScore
			Write8(pExtendInfoAward, 0); //OneMatchMaxNum

			len = pExtendInfoAward - pAwardArray;
			Set16(pAwardArray, (WORD)len);
			pAwardArray += len;
		}
		{ //ExtendInfoAward
			BYTE* pExtendInfoAward = pAwardArray;
			Write16(pExtendInfoAward, 0); //len

			Write16(pExtendInfoAward, 590); //Key
			Write32(pExtendInfoAward, 0); //AddValue
			Write32(pExtendInfoAward, 0); //TotalValue
			Write8(pExtendInfoAward, 0); //ShowPriority
			Write16(pExtendInfoAward, 0); //BuyNeedScore
			Write8(pExtendInfoAward, 0); //OneMatchMaxNum

			len = pExtendInfoAward - pAwardArray;
			Set16(pAwardArray, (WORD)len);
			pAwardArray += len;
		}
		len = pAwardArray - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); //WeekTotalScore
	Write32(p, 0); //TiaoZhanNum_TodayRecover
	Write32(p, 0); //TiaoZhanNum_MaxTodayRecoverNum
	Write32(p, 0); //GetTiaoZhanTotalSeconds

	len = p - buf;
	SendToClient(Client, 24754, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseStartTimerChallenge2(ClientNode* Client)
{
	Client->TaskID = 0xFFFF;
	Client->GameID = 2;
	Client->TotalRound = 1;
	Client->Loop = false;
	Client->FinishTime = 0;
	Client->Round = 0;
	Client->PassedCheckPoint = 0;
	Client->MapCompatibilityMode = true;
	Client->EndCheckPoint = 0xFFFF;//结束点暂未定义 可以根据24256消息
	BYTE* p = StartTimerChallenge2;
	p += 57;
	Write32(p, Client->Uin);
	p += 82;
	Write32(p, Client->Uin);
	Write32(p, Client->KartID);
	p += 104;
	Write32(p, Client->Uin);
	p += 196;
	Write32(p, Client->Uin);
	p += 20;
	Write32(p, Client->Uin);
	//ResponseAddToMatchQueue(Client, 0x8);
	//NotifyMatchResult(Client, 0x8);
	//NotifyTimerChallenge2ndCommonInfo
	const char* buf1 = "\x01\xc1\x00\x00\x00\x02\x00\xd4\x13\x00\x63\x00\x01\x00\x02\x00\x03\x00\x05\x00\x06\x00\x07\x00\x09\x00\x0a\x00\x0b\x00\x0d\x00\x0e\x00\x0f\x00\x11\x00\x12\x00\x13\x00\x16\x00\x17\x00\x18\x00\x19\x00\x1a\x00\x1b\x00\x1d\x00\x1e\x00\x1f\x00\x21\x00\x22\x00\x23\x00\x25\x00\x26\x00\x27\x00\x2a\x00\x2b\x00\x2c\x00\x2e\x00\x2f\x00\x30\x00\x32\x00\x33\x00\x34\x00\x37\x00\x38\x00\x39\x00\x3a\x00\x3b\x00\x3c\x00\x3e\x00\x3f\x00\x40\x00\x43\x00\x44\x00\x45\x00\x46\x00\x47\x00\x48\x00\x49\x00\x4a\x00\x4b\x00\x4c\x00\x4d\x00\x4e\x00\x4f\x00\x50\x00\x51\x00\x5e\x00\x5f\x00\x60\x00\x61\x00\x62\x00\x63\x00\x64\x00\x65\x00\x66\x00\x67\x00\x68\x00\x69\x00\x6a\x00\x6b\x00\x6c\x00\x6d\x00\x6e\x00\x6f\x00\x70\x00\x71\x00\x72\x00\x73\x00\x74\x00\x75\x00\x82\x00\x83\x00\x84\x00\x85\x00\x86\x00\x87\x00\x8e\x00\x8f\x00\x90\x00\x91\x00\x92\x00\x93\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x6e\x11\x00\x30\x00\x04\x00\x08\x00\x0c\x00\x10\x00\x14\x00\x15\x00\x1c\x00\x20\x00\x24\x00\x28\x00\x29\x00\x2d\x00\x31\x00\x35\x00\x36\x00\x3d\x00\x41\x00\x42\x00\x52\x00\x53\x00\x54\x00\x55\x00\x56\x00\x57\x00\x58\x00\x59\x00\x5a\x00\x5b\x00\x5c\x00\x5d\x00\x76\x00\x77\x00\x78\x00\x79\x00\x7a\x00\x7b\x00\x7c\x00\x7d\x00\x7e\x00\x7f\x00\x80\x00\x81\x00\x88\x00\x89\x00\x8a\x00\x8b\x00\x8c\x00\x8d\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x6d\x01\x00\x6a\x00\x00\x00\x00\x00\x00\x51\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03\x00\x10\x02\x4d\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x02\x4d\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x02\x4e\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x1b\x58\x00\x00\x00\x01\x50\x0f\x01\x00\x00\x4e\x20\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x23\x28\x01\x00\x53\x00\x00\x27\x11\x01\x00\x4c\x00\x12\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00\x05\x00\x00\x00\x06\x00\x00\x00\x07\x00\x00\x00\x09\x00\x00\x00\x0a\x00\x00\x00\x0b\x00\x00\x00\x0d\x00\x00\x00\x0e\x00\x00\x00\x0f\x00\x00\x00\x12\x00\x00\x00\x13\x00\x00\x00\x11\x00\x00\x00\x16\x00\x00\x00\x17\x00\x00\x00\x18\x01\x00\x01\x00\x00";

	//NotifyTimerChallenge2ndEvent
	const char* buf2 = "\x00\x00\x00\x07\x00\x00\x00\x06\x00\x00\x00\x00\x00\x01\x38\x80\x00\x01\x38\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00";
	//开始计时
	const char* buf3 = "\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x38\x80\x00\x01\x38\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x61\x6c";
	//技能显示在notifygamebegin
	SendToClient(Client, 509, StartTimerChallenge2, 816, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
	NotifyKartPhysParam(Client, NULL);
	SendToClient(Client, 24256, (BYTE*)buf1, 565, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
	SendToClient(Client, 24255, (BYTE*)buf2, 91, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
	//NotifyOtherKartStoneInfo(Client);

	NotifyRaceBegin(Client);

	SendToClient(Client, 24255, (BYTE*)buf3, 91, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
	if (Client->JumpThread == 0)
	{
		HANDLE jump = CreateThread(NULL, 0, OnSingleJump, Client, 0, NULL);
		if (jump)
		{
			Client->JumpThread = 1;
			CloseHandle(jump);
		}
	}
}
void RequestReportTimerChallenge2ndEvent(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT EventID = Read32(p);
	//printf("Challenge2ndEvent:%d \n", EventID);
	if (EventID == 19)//退出挑战
	{
		Client->matchQueueType = 0;
		Client->IsFinish = TRUE;
		//结束
		HANDLE over = CreateThread(NULL, 0, OnSingleOver, Client, 0, NULL);
		if (over)
		{
			CloseHandle(over);
		}
	}
	if (EventID == 17)//使用飞行技能
	{
		const char* buf = "\x00\x00\x27\x11\x00\x00\x00\x11\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
		SendToClient(Client, 24254, (BYTE*)buf, 20, Client->ConnID, FE_PLAYER, Client->ConnID, Response);

	}


}
void RequestChangeRecommandStatus(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UCHAR Idle = Read8(p);
	NotifyRecommandResult(Client);
	ResponseChangeRecommandStatus(Client);
}

void ResponseChangeRecommandStatus(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID

	len = p - buf;
	SendToClient(Client, 275, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyRecommandResult(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write8(p, 0); //RcmdType
	{ //PlayerSNSInfo
		BYTE* pPlayerSNSInfo = p;
		Write16(pPlayerSNSInfo, 0); //len

		WriteString(pPlayerSNSInfo, "Test"); //Name[]
		{ //BaseInfo
			BYTE* pBaseInfo = pPlayerSNSInfo;
			Write16(pBaseInfo, 0); //len

			Write32(pBaseInfo, Client->Uin); //Uin
			Write32(pBaseInfo, Client->RegTime); //RegisterTime
			Write8(pBaseInfo, Client->Gender); //Gender
			Write16(pBaseInfo, Client->RegTime); //BornYear
			Write8(pBaseInfo, 0); //Zodiac
			Write32(pBaseInfo, 0); //LocaleCode
			Write32(pBaseInfo, 0); //Flag
			WriteUn(pBaseInfo, 0); //tLastReportTime

			len = pBaseInfo - pPlayerSNSInfo;
			Set16(pPlayerSNSInfo, (WORD)len);
			pPlayerSNSInfo += len;
		}
		Write32(pPlayerSNSInfo, 0); //Exp
		Write32(pPlayerSNSInfo, 0); //RightFlag

		len = pPlayerSNSInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 276, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestC2GLoadFile(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UCHAR Cmd = Read8(p);
	USHORT UuidLen = Read16(p);
	ResponseC2GLoadFile(Client, Cmd, UuidLen);

}
void ResponseC2GLoadFile(ClientNode* Client, UCHAR Cmd, UCHAR UuidLen)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, 0); //Uin
	Write8(p, Cmd); //Cmd
	Write16(p, UuidLen); //UrlLen

	len = p - buf;
	SendToClient(Client, 11229, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetNonSaleItemConsumeItemList(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int SrcItemID = Read32(p);
	ItemInfo Items[3000];
	ItemInfo Item = PlayerDB_AddItem2(Client->Uin, SrcItemID, 1, -1, false);
	Items[0].ItemID = SrcItemID;
	Items[0].ItemNum = Item.ItemNum;
	Items[0].AvailPeriod = Item.AvailPeriod;
	Items[0].Status = Item.Status;
	Items[0].ObtainTime = Item.ObtainTime;
	Items[0].OtherAttribute = Item.OtherAttribute;
	Items[0].ItemType = Item.ItemType;
	NotifyClientAddItem(Client, 1, Items);
	int itemNum = 1;
	ItemInfo* aItemStatus = (ItemInfo*)malloc(sizeof(ItemInfo) * itemNum);
	aItemStatus[0].ItemID = SrcItemID;
	aItemStatus[0].ItemNum = Item.ItemNum;
	aItemStatus[0].AvailPeriod = Item.AvailPeriod;
	ResponseOpenBox(Client, 22107, aItemStatus, itemNum, 1);
}

void ResponseGetNonSaleItemConsumeItemList(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultId
	Write32(p, 0); //Uin
	Write32(p, 0); //SrcItemID
	Write32(p, 0); //ConsumNum
	Write32(p, 0); //ConsumeItem[]

	len = p - buf;
	SendToClient(Client, 20128, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGameLogicCommonReport(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT ReportType = Read16(p);
	UCHAR ParaNum = Read8(p);
	UINT ParaList[50];
	for (size_t i = 0; i < ParaNum; i++) {
		ParaList[i] = Read32(p);
	}
	UCHAR HasScenePosition = Read8(p);
	for (size_t i = 0; i < 1; i++)
	{ //ScenePositionInfo
		BYTE* pScenePositionInfo = p;
		len = Read16(pScenePositionInfo);

		int SceneID = Read32(pScenePositionInfo);
		UCHAR PositionNum = Read8(pScenePositionInfo);
		for (size_t i = 0; i < PositionNum; i++)
		{ //Position
			BYTE* pPosition = pScenePositionInfo;
			len = Read16(pPosition);

			UINT Idx = Read32(pPosition);
			int Postion = Read32(pPosition);

			pScenePositionInfo += len;
		}

		p += len;
	}
	UINT NPCUin = Read32(p);
	UINT DstUin = Read32(p);
	UCHAR OtherPropPosNum = Read8(p);
	for (size_t i = 0; i < OtherPropPosNum; i++)
	{ //OtherPropPos
		BYTE* pOtherPropPos = p;
		len = Read16(pOtherPropPos);

		UINT Idx = Read32(pOtherPropPos);
		int Postion = Read32(pOtherPropPos);

		p += len;
	}
	ResponseGameLogicCommonReport(Client, ReportType,ParaNum, ParaList, 0);
}

void ResponseGameLogicCommonReport(ClientNode* Client, USHORT ReportType, UCHAR ParaNum, UINT ParaList[], UCHAR OtherPropPosNum)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 0); //ResultID
	Write16(p, 0); //GameLogicType
	Write16(p, ReportType); //ReportType
	Write8(p, ParaNum); //ParaNum
	for (size_t i = 0; i < ParaNum; i++) {
		Write32(p, ParaList[i]); //ParaList[]
	}
	
	Write32(p, 0); //NPCUin
	Write32(p, 0); //DstUin
	Write8(p, 0); //OtherPropPosNum
	for (size_t i = 0; i < 0; i++)
	{ //OtherPropPos
		BYTE* pOtherPropPos = p;
		Write16(pOtherPropPos, 0); //len

		Write32(pOtherPropPos, 0); //Idx
		Write32(pOtherPropPos, 0); //Postion[]

		len = pOtherPropPos - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 636, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestC2SSearchKartAltas(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT DstUin = Read32(p);
	ResponseC2SearchKartAltas(Client,DstUin);
}
void ResponseC2SearchKartAltas(ClientNode* Client, UINT DstUin)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //Result
	Write32(p, DstUin); //DstUin
	Write16(p, KartAltasList.size()); //ItemNum
	for (size_t i = 0; i < 20; i++)
	{ //ItemInfoArray
		BYTE* pItemInfoArray = p;
		Write16(pItemInfoArray, 0); //len

		Write32(pItemInfoArray, KartAltasList[i]->ID); //ItemID
		Write32(pItemInfoArray, 0); //RefitCout
		{ //AltasCurKartAttr
			BYTE* pAltasCurKartAttr = pItemInfoArray;
			Write16(pAltasCurKartAttr, 0); //len

			Write32(pAltasCurKartAttr, DstUin); //Uin
			Write32(pAltasCurKartAttr, KartAltasList[i]->ID); //KartId
			Write32(pAltasCurKartAttr, 0); //RefitCout
			Write16(pAltasCurKartAttr, 0); //MaxFlags
			Write16(pAltasCurKartAttr, 0); //WWeight
			Write16(pAltasCurKartAttr, 0); //SpeedWeight
			Write16(pAltasCurKartAttr, 0); //JetWeight
			Write16(pAltasCurKartAttr, 0); //SJetWeight
			Write16(pAltasCurKartAttr, 0); //AccuWeight
			Write32(pAltasCurKartAttr, 0); //ShapeRefitCount
			Write32(pAltasCurKartAttr, 0); //KartHeadRefitItemID
			Write32(pAltasCurKartAttr, 0); //KartTailRefitItemID
			Write32(pAltasCurKartAttr, 0); //KartFlankRefitItemID
			Write32(pAltasCurKartAttr, 0); //KartTireRefitItemID
			{ //KartRefitExInfo
				BYTE* pKartRefitExInfo = pAltasCurKartAttr;
				Write16(pKartRefitExInfo, 0); //len

				Write8(pKartRefitExInfo, 0); //SpeedRefitStar
				Write8(pKartRefitExInfo, 0); //JetRefitStar
				Write8(pKartRefitExInfo, 0); //SJetRefitStar
				Write8(pKartRefitExInfo, 0); //AccuRefitStar
				Write8(pKartRefitExInfo, 0); //SpeedAddRatio
				Write8(pKartRefitExInfo, 0); //JetAddRatio
				Write8(pKartRefitExInfo, 0); //SJetAddRatio
				Write8(pKartRefitExInfo, 0); //AccuAddRatio

				len = pKartRefitExInfo - pAltasCurKartAttr;
				Set16(pAltasCurKartAttr, (WORD)len);
				pAltasCurKartAttr += len;
			}
			Write32(pAltasCurKartAttr, 0); //SecondRefitCount
			Write16(pAltasCurKartAttr, 0); //Speed2Weight
			Write16(pAltasCurKartAttr, 0); //DriftVecWeight
			Write16(pAltasCurKartAttr, 0); //AdditionalZSpeedWeight
			Write16(pAltasCurKartAttr, 0); //AntiCollisionWeight
			Write16(pAltasCurKartAttr, 0); //LuckyValue
			Write16(pAltasCurKartAttr, 0); //RefitLuckyValueMaxWeight
			Write32(pAltasCurKartAttr, 0); //ShapeSuitID
			Write8(pAltasCurKartAttr, 0); //LegendSuitLevel
			Write32(pAltasCurKartAttr, 0); //LegendSuitLevelChoice
			Write32(pAltasCurKartAttr, 0); //ShapeLegendSuitID

			len = pAltasCurKartAttr - pItemInfoArray;
			Set16(pItemInfoArray, (WORD)len);
			pItemInfoArray += len;
		}
		{ //AltasKartStoneGrooveInfo
			BYTE* pAltasKartStoneGrooveInfo = pItemInfoArray;
			Write16(pAltasKartStoneGrooveInfo, 0); //len

			Write32(pAltasKartStoneGrooveInfo, 0); //KartID
			Write32(pAltasKartStoneGrooveInfo, 0); //StoneGrooveNum
			for (size_t i = 0; i < 0; i++)
			{ //StoneGrooveInfo
				BYTE* pStoneGrooveInfo = pAltasKartStoneGrooveInfo;
				Write16(pStoneGrooveInfo, 0); //len

				Write32(pStoneGrooveInfo, 0); //StoneUseOccaType
				Write32(pStoneGrooveInfo, 0); //SkillStoneID

				len = pStoneGrooveInfo - pAltasKartStoneGrooveInfo;
				Set16(pAltasKartStoneGrooveInfo, (WORD)len);
				pAltasKartStoneGrooveInfo += len;
			}

			len = pAltasKartStoneGrooveInfo - pItemInfoArray;
			Set16(pItemInfoArray, (WORD)len);
			pItemInfoArray += len;
		}
		{ //AltasKartActiveLevelInfo
			BYTE* pAltasKartActiveLevelInfo = pItemInfoArray;
			Write16(pAltasKartActiveLevelInfo, 0); //len

			Write32(pAltasKartActiveLevelInfo, 0); //KartType
			Write32(pAltasKartActiveLevelInfo, 0); //ItemID
			Write32(pAltasKartActiveLevelInfo, 0); //ActiveLevel

			len = pAltasKartActiveLevelInfo - pItemInfoArray;
			Set16(pItemInfoArray, (WORD)len);
			pItemInfoArray += len;
		}

		len = pItemInfoArray - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //HasMoreInfo

	len = p - buf;
	SendToClient(Client, 13705, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyS2CMyKartAltas(ClientNode* Client)
{
	BYTE* buf = new BYTE[8192 * 2];
	BYTE* p = buf;
	size_t len;
	std::vector<UINT> HasKartAltasList;//拥有车列表
	if (buf == nullptr)
	{
		// 分配空间失败的处理
		return;
	}
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;

	sql = "SELECT ItemID FROM Item WHERE Uin = ? And AvailPeriod = -1";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		if (stmt == nullptr)
		{
			// 空指针处理
			delete[] buf;
			return;
		}
		sqlite3_bind_int(stmt, 1, Client->Uin);
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			int KartID = sqlite3_column_int(stmt, 0);
			if (KartID == 129295 || KartID == 129296 || KartID == 130784 || KartID == 130785) {
				continue;//新版b
			}
			if (KartID == 130774 || KartID == 130779 || KartID == 130780 || KartID == 130782 || KartID == 130783) {
				continue;//新版a
			}
			if (KartID == 130776) {
				continue;//新版s
			}
			if (KartID == 130775) {
				continue;//新版m
			}
			if (KartID == 130538 || KartID == 129285) {
				continue;//新版t
			}
			if (GetItemType(KartID) == EAIT_CAR) {
				HasKartAltasList.insert(HasKartAltasList.end(), KartID);
			}
		}
		Write32(p, Client->Uin); //Uin
		BYTE* pItemNum = p;
		int ItemNum = 0;
		bool HasMoreInfo = false;
		Write16(p, 0); //ItemNum
		for (size_t i = 0; i < HasKartAltasList.size(); i++)
		{ //ItemInfoArray
			if (i < 120) {
				ItemNum++;
				BYTE* pItemInfoArray = p;
				Write16(pItemInfoArray, 0); //len

				Write32(pItemInfoArray, HasKartAltasList[i]); //ItemID
				Write32(pItemInfoArray, 0); //RefitCout
				{ //AltasCurKartAttr
					BYTE* pAltasCurKartAttr = pItemInfoArray;
					Write16(pAltasCurKartAttr, 0); //len

					Write32(pAltasCurKartAttr, Client->Uin); //Uin
					Write32(pAltasCurKartAttr, HasKartAltasList[i]); //KartId
					Write32(pAltasCurKartAttr, 0); //RefitCout
					Write16(pAltasCurKartAttr, 0); //MaxFlags
					Write16(pAltasCurKartAttr, 0); //WWeight
					Write16(pAltasCurKartAttr, 0); //SpeedWeight
					Write16(pAltasCurKartAttr, 0); //JetWeight
					Write16(pAltasCurKartAttr, 0); //SJetWeight
					Write16(pAltasCurKartAttr, 0); //AccuWeight
					Write32(pAltasCurKartAttr, 0); //ShapeRefitCount
					Write32(pAltasCurKartAttr, 0); //KartHeadRefitItemID
					Write32(pAltasCurKartAttr, 0); //KartTailRefitItemID
					Write32(pAltasCurKartAttr, 0); //KartFlankRefitItemID
					Write32(pAltasCurKartAttr, 0); //KartTireRefitItemID
					{ //KartRefitExInfo
						BYTE* pKartRefitExInfo = pAltasCurKartAttr;
						Write16(pKartRefitExInfo, 0); //len

						Write8(pKartRefitExInfo, 0); //SpeedRefitStar
						Write8(pKartRefitExInfo, 0); //JetRefitStar
						Write8(pKartRefitExInfo, 0); //SJetRefitStar
						Write8(pKartRefitExInfo, 0); //AccuRefitStar
						Write8(pKartRefitExInfo, 0); //SpeedAddRatio
						Write8(pKartRefitExInfo, 0); //JetAddRatio
						Write8(pKartRefitExInfo, 0); //SJetAddRatio
						Write8(pKartRefitExInfo, 0); //AccuAddRatio

						len = pKartRefitExInfo - pAltasCurKartAttr;
						Set16(pAltasCurKartAttr, (WORD)len);
						pAltasCurKartAttr += len;
					}
					Write32(pAltasCurKartAttr, 0); //SecondRefitCount
					Write16(pAltasCurKartAttr, 0); //Speed2Weight
					Write16(pAltasCurKartAttr, 0); //DriftVecWeight
					Write16(pAltasCurKartAttr, 0); //AdditionalZSpeedWeight
					Write16(pAltasCurKartAttr, 0); //AntiCollisionWeight
					Write16(pAltasCurKartAttr, 0); //LuckyValue
					Write16(pAltasCurKartAttr, 0); //RefitLuckyValueMaxWeight
					Write32(pAltasCurKartAttr, 0); //ShapeSuitID
					Write8(pAltasCurKartAttr, 0); //LegendSuitLevel
					Write32(pAltasCurKartAttr, 0); //LegendSuitLevelChoice
					Write32(pAltasCurKartAttr, 0); //ShapeLegendSuitID

					len = pAltasCurKartAttr - pItemInfoArray;
					Set16(pItemInfoArray, (WORD)len);
					pItemInfoArray += len;
				}
				{ //AltasKartStoneGrooveInfo
					BYTE* pAltasKartStoneGrooveInfo = pItemInfoArray;
					Write16(pAltasKartStoneGrooveInfo, 0); //len

					Write32(pAltasKartStoneGrooveInfo, 0); //KartID
					Write32(pAltasKartStoneGrooveInfo, 0); //StoneGrooveNum
					for (size_t i = 0; i < 0; i++)
					{ //StoneGrooveInfo
						BYTE* pStoneGrooveInfo = pAltasKartStoneGrooveInfo;
						Write16(pStoneGrooveInfo, 0); //len

						Write32(pStoneGrooveInfo, 0); //StoneUseOccaType
						Write32(pStoneGrooveInfo, 0); //SkillStoneID

						len = pStoneGrooveInfo - pAltasKartStoneGrooveInfo;
						Set16(pAltasKartStoneGrooveInfo, (WORD)len);
						pAltasKartStoneGrooveInfo += len;
					}

					len = pAltasKartStoneGrooveInfo - pItemInfoArray;
					Set16(pItemInfoArray, (WORD)len);
					pItemInfoArray += len;
				}
				{ //AltasKartActiveLevelInfo
					BYTE* pAltasKartActiveLevelInfo = pItemInfoArray;
					Write16(pAltasKartActiveLevelInfo, 0); //len

					Write32(pAltasKartActiveLevelInfo, 0); //KartType
					Write32(pAltasKartActiveLevelInfo, 0); //ItemID
					Write32(pAltasKartActiveLevelInfo, 0); //ActiveLevel

					len = pAltasKartActiveLevelInfo - pItemInfoArray;
					Set16(pItemInfoArray, (WORD)len);
					pItemInfoArray += len;
				}

				len = pItemInfoArray - p;
				Set16(p, (WORD)len);
				p += len;
			}
			else
			{
				HasMoreInfo = true;
				break;
			}

		}
		Set16(pItemNum, (WORD)ItemNum);
		Write8(p, HasMoreInfo); //HasMoreInfo

		len = p - buf;
		SendToClient(Client, 13706, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
		if (HasMoreInfo)
		{
			NotifyS2CMyKartAltasMoreInfo(Client, ItemNum, HasKartAltasList);
		}
		// 释放堆空间
		delete[] buf;
	}
}
void NotifyS2CMyKartAltasMoreInfo(ClientNode* Client, UINT Index , std::vector<UINT> HasKartAltasList)
{
	BYTE* buf = new BYTE[8192 * 2];
	BYTE* p = buf;
	size_t len;
	if (buf == nullptr)
	{
		// 分配空间失败的处理
		return;
	}
	Write32(p, Client->Uin); //Uin
	BYTE* pItemNum = p;
	int ItemNum = 0;
	bool HasMoreInfo = false;
	Write16(p, 0); //ItemNum
	for (size_t i = 0; i < HasKartAltasList.size(); i++)
	{
		if (i + Index >= HasKartAltasList.size()) {
			break;
		}
		//ItemInfoArray
		if (i < 120) {
			ItemNum++;
			BYTE* pItemInfoArray = p;
			Write16(pItemInfoArray, 0); //len

			Write32(pItemInfoArray, HasKartAltasList[i + Index]); //ItemID
			Write32(pItemInfoArray, 0); //RefitCout
			{ //AltasCurKartAttr
				BYTE* pAltasCurKartAttr = pItemInfoArray;
				Write16(pAltasCurKartAttr, 0); //len

				Write32(pAltasCurKartAttr, Client->Uin); //Uin
				Write32(pAltasCurKartAttr, HasKartAltasList[i + Index]); //KartId
				Write32(pAltasCurKartAttr, 0); //RefitCout
				Write16(pAltasCurKartAttr, 0); //MaxFlags
				Write16(pAltasCurKartAttr, 0); //WWeight
				Write16(pAltasCurKartAttr, 0); //SpeedWeight
				Write16(pAltasCurKartAttr, 0); //JetWeight
				Write16(pAltasCurKartAttr, 0); //SJetWeight
				Write16(pAltasCurKartAttr, 0); //AccuWeight
				Write32(pAltasCurKartAttr, 0); //ShapeRefitCount
				Write32(pAltasCurKartAttr, 0); //KartHeadRefitItemID
				Write32(pAltasCurKartAttr, 0); //KartTailRefitItemID
				Write32(pAltasCurKartAttr, 0); //KartFlankRefitItemID
				Write32(pAltasCurKartAttr, 0); //KartTireRefitItemID
				{ //KartRefitExInfo
					BYTE* pKartRefitExInfo = pAltasCurKartAttr;
					Write16(pKartRefitExInfo, 0); //len

					Write8(pKartRefitExInfo, 0); //SpeedRefitStar
					Write8(pKartRefitExInfo, 0); //JetRefitStar
					Write8(pKartRefitExInfo, 0); //SJetRefitStar
					Write8(pKartRefitExInfo, 0); //AccuRefitStar
					Write8(pKartRefitExInfo, 0); //SpeedAddRatio
					Write8(pKartRefitExInfo, 0); //JetAddRatio
					Write8(pKartRefitExInfo, 0); //SJetAddRatio
					Write8(pKartRefitExInfo, 0); //AccuAddRatio

					len = pKartRefitExInfo - pAltasCurKartAttr;
					Set16(pAltasCurKartAttr, (WORD)len);
					pAltasCurKartAttr += len;
				}
				Write32(pAltasCurKartAttr, 0); //SecondRefitCount
				Write16(pAltasCurKartAttr, 0); //Speed2Weight
				Write16(pAltasCurKartAttr, 0); //DriftVecWeight
				Write16(pAltasCurKartAttr, 0); //AdditionalZSpeedWeight
				Write16(pAltasCurKartAttr, 0); //AntiCollisionWeight
				Write16(pAltasCurKartAttr, 0); //LuckyValue
				Write16(pAltasCurKartAttr, 0); //RefitLuckyValueMaxWeight
				Write32(pAltasCurKartAttr, 0); //ShapeSuitID
				Write8(pAltasCurKartAttr, 0); //LegendSuitLevel
				Write32(pAltasCurKartAttr, 0); //LegendSuitLevelChoice
				Write32(pAltasCurKartAttr, 0); //ShapeLegendSuitID

				len = pAltasCurKartAttr - pItemInfoArray;
				Set16(pItemInfoArray, (WORD)len);
				pItemInfoArray += len;
			}
			{ //AltasKartStoneGrooveInfo
				BYTE* pAltasKartStoneGrooveInfo = pItemInfoArray;
				Write16(pAltasKartStoneGrooveInfo, 0); //len

				Write32(pAltasKartStoneGrooveInfo, 0); //KartID
				Write32(pAltasKartStoneGrooveInfo, 0); //StoneGrooveNum
				for (size_t i = 0; i < 0; i++)
				{ //StoneGrooveInfo
					BYTE* pStoneGrooveInfo = pAltasKartStoneGrooveInfo;
					Write16(pStoneGrooveInfo, 0); //len

					Write32(pStoneGrooveInfo, 0); //StoneUseOccaType
					Write32(pStoneGrooveInfo, 0); //SkillStoneID

					len = pStoneGrooveInfo - pAltasKartStoneGrooveInfo;
					Set16(pAltasKartStoneGrooveInfo, (WORD)len);
					pAltasKartStoneGrooveInfo += len;
				}

				len = pAltasKartStoneGrooveInfo - pItemInfoArray;
				Set16(pItemInfoArray, (WORD)len);
				pItemInfoArray += len;
			}
			{ //AltasKartActiveLevelInfo
				BYTE* pAltasKartActiveLevelInfo = pItemInfoArray;
				Write16(pAltasKartActiveLevelInfo, 0); //len

				Write32(pAltasKartActiveLevelInfo, 0); //KartType
				Write32(pAltasKartActiveLevelInfo, 0); //ItemID
				Write32(pAltasKartActiveLevelInfo, 0); //ActiveLevel

				len = pAltasKartActiveLevelInfo - pItemInfoArray;
				Set16(pItemInfoArray, (WORD)len);
				pItemInfoArray += len;
			}

			len = pItemInfoArray - p;
			Set16(p, (WORD)len);
			p += len;
		}
		else
		{
			HasMoreInfo = true;
			break;
		}

	}
	Set16(pItemNum, (WORD)ItemNum);
	Write8(p, HasMoreInfo); //HasNext
	Index += ItemNum;
	len = p - buf;
	SendToClient(Client, 13704, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
	if (HasMoreInfo)
	{
		NotifyS2CMyKartAltasMoreInfo(Client, Index, HasKartAltasList);
	}
	// 释放堆空间
	delete[] buf;
}
void NotifySpecialActivityInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 0); //ActivityID
	Write8(p, 0); //Flag
	Write32(p, 0); //OpenDate
	Write32(p, 0); //CloseDate
	Write32(p, 0); //OpenTime
	Write32(p, 0); //CloseTime
	Write8(p, 0); //SpecialActivityIDNum
	Write8(p, 1); //SpecialActivityInfoNum
	for (size_t i = 0; i < 1; i++)
	{ //SpecialActivityInfo
		BYTE* pSpecialActivityInfo = p;
		Write16(pSpecialActivityInfo, 0); //len

		Write8(pSpecialActivityInfo, 1); //ProfessionLicenseInfoNum
		for (size_t i = 0; i < 1; i++)
		{ //ProfessionLicenseInfo
			BYTE* pProfessionLicenseInfo = pSpecialActivityInfo;
			Write16(pProfessionLicenseInfo, 0); //len

			Write8(pProfessionLicenseInfo, 6); //BigLevel
			Write8(pProfessionLicenseInfo, 5); //SmallLevel

			len = pProfessionLicenseInfo - pSpecialActivityInfo;
			Set16(pSpecialActivityInfo, (WORD)len);
			pSpecialActivityInfo += len;
		}

		len = pSpecialActivityInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24767, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestGetSpecialActivityInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT ActID = Read32(p);

}

void ResponseGetSpecialActivityInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //Uin
	Write16(p, 0); //ResultID
	Write32(p, 0); //ActID
	for (size_t i = 0; i < 0; i++)
	{ //RabbitPurseMoonGetInfo
		BYTE* pRabbitPurseMoonGetInfo = p;
		Write16(pRabbitPurseMoonGetInfo, 0); //len

		Write16(pRabbitPurseMoonGetInfo, 0); //AwardNums
		for (size_t i = 0; i < 0; i++)
		{ //AwardInfos
			BYTE* pAwardInfos = pRabbitPurseMoonGetInfo;
			Write16(pAwardInfos, 0); //len

			Write8(pAwardInfos, 0); //MapIndex
			Write16(pAwardInfos, 0); //AwardType
			Write16(pAwardInfos, 0); //AwardIndex
			Write16(pAwardInfos, 0); //EventAwardID
			{ //AwardItemInfo
				BYTE* pAwardItemInfo = pAwardInfos;
				Write16(pAwardItemInfo, 0); //len

				Write32(pAwardItemInfo, 0); //Exp
				Write32(pAwardItemInfo, 0); //Money
				Write32(pAwardItemInfo, 0); //SuperMoney
				Write16(pAwardItemInfo, 0); //ItemNum
				for (size_t i = 0; i < 0; i++)
				{ //ItemInfo
					BYTE* pItemInfo = pAwardItemInfo;
					Write16(pItemInfo, 0); //len

					Write32(pItemInfo, 0); //ItemID
					Write32(pItemInfo, 0); //ItemNum
					Write32(pItemInfo, 0); //AvailPeriod
					Write8(pItemInfo, 0); //Status
					WriteUn(pItemInfo, 0); //ObtainTime
					Write32(pItemInfo, 0); //OtherAttribute
					Write16(pItemInfo, 0); //ItemType

					len = pItemInfo - pAwardItemInfo;
					Set16(pAwardItemInfo, (WORD)len);
					pAwardItemInfo += len;
				}
				Write32(pAwardItemInfo, 0); //Coupons
				Write32(pAwardItemInfo, 0); //GuildPoint
				Write32(pAwardItemInfo, 0); //LuckMoney
				Write8(pAwardItemInfo, 0); //ExtendInfoNum
				for (size_t i = 0; i < 0; i++)
				{ //ExtendInfoAward
					BYTE* pExtendInfoAward = pAwardItemInfo;
					Write16(pExtendInfoAward, 0); //len

					Write16(pExtendInfoAward, 0); //Key
					Write32(pExtendInfoAward, 0); //AddValue
					Write32(pExtendInfoAward, 0); //TotalValue
					Write8(pExtendInfoAward, 0); //ShowPriority
					Write16(pExtendInfoAward, 0); //BuyNeedScore
					Write8(pExtendInfoAward, 0); //OneMatchMaxNum

					len = pExtendInfoAward - pAwardItemInfo;
					Set16(pAwardItemInfo, (WORD)len);
					pAwardItemInfo += len;
				}
				Write32(pAwardItemInfo, 0); //SpeedCoin

				len = pAwardItemInfo - pAwardInfos;
				Set16(pAwardInfos, (WORD)len);
				pAwardInfos += len;
			}
			Write32(pAwardInfos, 0); //AwardRound
			Write32(pAwardInfos, 0); //AwardMaxNums
			Write32(pAwardInfos, 0); //AwardLowNums
			Write32(pAwardInfos, 0); //CurrentProcessValue
			Write32(pAwardInfos, 0); //AwardNeedProcessValue
			Write8(pAwardInfos, 0); //GetAward
			Write8(pAwardInfos, 0); //BestItemAward
			Write16(pAwardInfos, 0); //AwardExtraType
			Write32(pAwardInfos, 0); //ReceBeginDate
			Write32(pAwardInfos, 0); //ReceEndDate
			Write16(pAwardInfos, 0); //AwardPoolID
			Write8(pAwardInfos, 0); //AwardStatus

			len = pAwardInfos - pRabbitPurseMoonGetInfo;
			Set16(pRabbitPurseMoonGetInfo, (WORD)len);
			pRabbitPurseMoonGetInfo += len;
		}
		Write16(pRabbitPurseMoonGetInfo, 0); //ChanceGeneNumsByDay
		for (size_t i = 0; i < 0; i++)
		{ //PartiChanceGeneInfosByDay
			BYTE* pPartiChanceGeneInfosByDay = pRabbitPurseMoonGetInfo;
			Write16(pPartiChanceGeneInfosByDay, 0); //len

			Write8(pPartiChanceGeneInfosByDay, 0); //TaskID
			Write8(pPartiChanceGeneInfosByDay, 0); //TaskPeriod
			Write32(pPartiChanceGeneInfosByDay, 0); //AlreadyAwardNums
			Write32(pPartiChanceGeneInfosByDay, 0); //AwardMoveNums
			Write32(pPartiChanceGeneInfosByDay, 0); //TaskFinishNums
			Write32(pPartiChanceGeneInfosByDay, 0); //TaskNeedValue

			len = pPartiChanceGeneInfosByDay - pRabbitPurseMoonGetInfo;
			Set16(pRabbitPurseMoonGetInfo, (WORD)len);
			pRabbitPurseMoonGetInfo += len;
		}
		Write16(pRabbitPurseMoonGetInfo, 0); //ChanceGeneNumsByWeek
		for (size_t i = 0; i < 0; i++)
		{ //PartiChanceGeneInfosByWeek
			BYTE* pPartiChanceGeneInfosByWeek = pRabbitPurseMoonGetInfo;
			Write16(pPartiChanceGeneInfosByWeek, 0); //len

			Write8(pPartiChanceGeneInfosByWeek, 0); //TaskID
			Write8(pPartiChanceGeneInfosByWeek, 0); //TaskPeriod
			Write32(pPartiChanceGeneInfosByWeek, 0); //AlreadyAwardNums
			Write32(pPartiChanceGeneInfosByWeek, 0); //AwardMoveNums
			Write32(pPartiChanceGeneInfosByWeek, 0); //TaskFinishNums
			Write32(pPartiChanceGeneInfosByWeek, 0); //TaskNeedValue

			len = pPartiChanceGeneInfosByWeek - pRabbitPurseMoonGetInfo;
			Set16(pRabbitPurseMoonGetInfo, (WORD)len);
			pRabbitPurseMoonGetInfo += len;
		}
		{ //OpenTimeInfo
			BYTE* pOpenTimeInfo = pRabbitPurseMoonGetInfo;
			Write16(pOpenTimeInfo, 0); //len

			Write32(pOpenTimeInfo, 0); //OpenDate
			Write32(pOpenTimeInfo, 0); //CloseDate
			Write32(pOpenTimeInfo, 0); //OpenTime
			Write32(pOpenTimeInfo, 0); //CloseTime

			len = pOpenTimeInfo - pRabbitPurseMoonGetInfo;
			Set16(pRabbitPurseMoonGetInfo, (WORD)len);
			pRabbitPurseMoonGetInfo += len;
		}
		Write32(pRabbitPurseMoonGetInfo, 0); //CostItemID
		Write32(pRabbitPurseMoonGetInfo, 0); //CostItemNums
		Write32(pRabbitPurseMoonGetInfo, 0); //CostItemNumsForOne
		Write32(pRabbitPurseMoonGetInfo, 0); //ExchangeItemID
		Write32(pRabbitPurseMoonGetInfo, 0); //ExchangeItemNums
		Write16(pRabbitPurseMoonGetInfo, 0); //ExchangeStatNum
		Write16(pRabbitPurseMoonGetInfo, 0); //ExchangeStat[]
		Write8(pRabbitPurseMoonGetInfo, 0); //CanExchangeIndex
		Write8(pRabbitPurseMoonGetInfo, 0); //IsPopWindow
		{ //LuckyTaskDate
			BYTE* pLuckyTaskDate = pRabbitPurseMoonGetInfo;
			Write16(pLuckyTaskDate, 0); //len

			Write32(pLuckyTaskDate, 0); //StartDate
			Write32(pLuckyTaskDate, 0); //EndDate

			len = pLuckyTaskDate - pRabbitPurseMoonGetInfo;
			Set16(pRabbitPurseMoonGetInfo, (WORD)len);
			pRabbitPurseMoonGetInfo += len;
		}
		Write8(pRabbitPurseMoonGetInfo, 0); //ShowDay

		len = pRabbitPurseMoonGetInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 28374, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestSpecialActivityDoLottery(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT ActID = Read32(p);

}

void ResponseSpecialActivityDoLottery(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //Uin
	Write16(p, 0); //ResultID
	Write32(p, 0); //ActID
	for (size_t i = 0; i < 0; i++)
	{ //RabbitPurseMoonDoLottery
		BYTE* pRabbitPurseMoonDoLottery = p;
		Write16(pRabbitPurseMoonDoLottery, 0); //len

		Write8(pRabbitPurseMoonDoLottery, 0); //GetMileageNow
		Write32(pRabbitPurseMoonDoLottery, 0); //ExchangeItemNums
		{ //ExtractAwardInfo
			BYTE* pExtractAwardInfo = pRabbitPurseMoonDoLottery;
			Write16(pExtractAwardInfo, 0); //len

			Write8(pExtractAwardInfo, 0); //MapIndex
			Write16(pExtractAwardInfo, 0); //AwardType
			Write16(pExtractAwardInfo, 0); //AwardIndex
			Write16(pExtractAwardInfo, 0); //EventAwardID
			{ //AwardItemInfo
				BYTE* pAwardItemInfo = pExtractAwardInfo;
				Write16(pAwardItemInfo, 0); //len

				Write32(pAwardItemInfo, 0); //Exp
				Write32(pAwardItemInfo, 0); //Money
				Write32(pAwardItemInfo, 0); //SuperMoney
				Write16(pAwardItemInfo, 0); //ItemNum
				for (size_t i = 0; i < 0; i++)
				{ //ItemInfo
					BYTE* pItemInfo = pAwardItemInfo;
					Write16(pItemInfo, 0); //len

					Write32(pItemInfo, 0); //ItemID
					Write32(pItemInfo, 0); //ItemNum
					Write32(pItemInfo, 0); //AvailPeriod
					Write8(pItemInfo, 0); //Status
					WriteUn(pItemInfo, 0); //ObtainTime
					Write32(pItemInfo, 0); //OtherAttribute
					Write16(pItemInfo, 0); //ItemType

					len = pItemInfo - pAwardItemInfo;
					Set16(pAwardItemInfo, (WORD)len);
					pAwardItemInfo += len;
				}
				Write32(pAwardItemInfo, 0); //Coupons
				Write32(pAwardItemInfo, 0); //GuildPoint
				Write32(pAwardItemInfo, 0); //LuckMoney
				Write8(pAwardItemInfo, 0); //ExtendInfoNum
				for (size_t i = 0; i < 0; i++)
				{ //ExtendInfoAward
					BYTE* pExtendInfoAward = pAwardItemInfo;
					Write16(pExtendInfoAward, 0); //len

					Write16(pExtendInfoAward, 0); //Key
					Write32(pExtendInfoAward, 0); //AddValue
					Write32(pExtendInfoAward, 0); //TotalValue
					Write8(pExtendInfoAward, 0); //ShowPriority
					Write16(pExtendInfoAward, 0); //BuyNeedScore
					Write8(pExtendInfoAward, 0); //OneMatchMaxNum

					len = pExtendInfoAward - pAwardItemInfo;
					Set16(pAwardItemInfo, (WORD)len);
					pAwardItemInfo += len;
				}
				Write32(pAwardItemInfo, 0); //SpeedCoin

				len = pAwardItemInfo - pExtractAwardInfo;
				Set16(pExtractAwardInfo, (WORD)len);
				pExtractAwardInfo += len;
			}
			Write32(pExtractAwardInfo, 0); //AwardRound
			Write32(pExtractAwardInfo, 0); //AwardMaxNums
			Write32(pExtractAwardInfo, 0); //AwardLowNums
			Write32(pExtractAwardInfo, 0); //CurrentProcessValue
			Write32(pExtractAwardInfo, 0); //AwardNeedProcessValue
			Write8(pExtractAwardInfo, 0); //GetAward
			Write8(pExtractAwardInfo, 0); //BestItemAward
			Write16(pExtractAwardInfo, 0); //AwardExtraType
			Write32(pExtractAwardInfo, 0); //ReceBeginDate
			Write32(pExtractAwardInfo, 0); //ReceEndDate
			Write16(pExtractAwardInfo, 0); //AwardPoolID
			Write8(pExtractAwardInfo, 0); //AwardStatus

			len = pExtractAwardInfo - pRabbitPurseMoonDoLottery;
			Set16(pRabbitPurseMoonDoLottery, (WORD)len);
			pRabbitPurseMoonDoLottery += len;
		}
		Write8(pRabbitPurseMoonDoLottery, 0); //NeedRefreshAward
		Write16(pRabbitPurseMoonDoLottery, 0); //RefreshAwardNums
		for (size_t i = 0; i < 0; i++)
		{ //FiveDaysExchangeCarsAwardInfos
			BYTE* pFiveDaysExchangeCarsAwardInfos = pRabbitPurseMoonDoLottery;
			Write16(pFiveDaysExchangeCarsAwardInfos, 0); //len

			Write8(pFiveDaysExchangeCarsAwardInfos, 0); //MapIndex
			Write16(pFiveDaysExchangeCarsAwardInfos, 0); //AwardType
			Write16(pFiveDaysExchangeCarsAwardInfos, 0); //AwardIndex
			Write16(pFiveDaysExchangeCarsAwardInfos, 0); //EventAwardID
			{ //AwardItemInfo
				BYTE* pAwardItemInfo = pFiveDaysExchangeCarsAwardInfos;
				Write16(pAwardItemInfo, 0); //len

				Write32(pAwardItemInfo, 0); //Exp
				Write32(pAwardItemInfo, 0); //Money
				Write32(pAwardItemInfo, 0); //SuperMoney
				Write16(pAwardItemInfo, 0); //ItemNum
				for (size_t i = 0; i < 0; i++)
				{ //ItemInfo
					BYTE* pItemInfo = pAwardItemInfo;
					Write16(pItemInfo, 0); //len

					Write32(pItemInfo, 0); //ItemID
					Write32(pItemInfo, 0); //ItemNum
					Write32(pItemInfo, 0); //AvailPeriod
					Write8(pItemInfo, 0); //Status
					WriteUn(pItemInfo, 0); //ObtainTime
					Write32(pItemInfo, 0); //OtherAttribute
					Write16(pItemInfo, 0); //ItemType

					len = pItemInfo - pAwardItemInfo;
					Set16(pAwardItemInfo, (WORD)len);
					pAwardItemInfo += len;
				}
				Write32(pAwardItemInfo, 0); //Coupons
				Write32(pAwardItemInfo, 0); //GuildPoint
				Write32(pAwardItemInfo, 0); //LuckMoney
				Write8(pAwardItemInfo, 0); //ExtendInfoNum
				for (size_t i = 0; i < 0; i++)
				{ //ExtendInfoAward
					BYTE* pExtendInfoAward = pAwardItemInfo;
					Write16(pExtendInfoAward, 0); //len

					Write16(pExtendInfoAward, 0); //Key
					Write32(pExtendInfoAward, 0); //AddValue
					Write32(pExtendInfoAward, 0); //TotalValue
					Write8(pExtendInfoAward, 0); //ShowPriority
					Write16(pExtendInfoAward, 0); //BuyNeedScore
					Write8(pExtendInfoAward, 0); //OneMatchMaxNum

					len = pExtendInfoAward - pAwardItemInfo;
					Set16(pAwardItemInfo, (WORD)len);
					pAwardItemInfo += len;
				}
				Write32(pAwardItemInfo, 0); //SpeedCoin

				len = pAwardItemInfo - pFiveDaysExchangeCarsAwardInfos;
				Set16(pFiveDaysExchangeCarsAwardInfos, (WORD)len);
				pFiveDaysExchangeCarsAwardInfos += len;
			}
			Write32(pFiveDaysExchangeCarsAwardInfos, 0); //AwardRound
			Write32(pFiveDaysExchangeCarsAwardInfos, 0); //AwardMaxNums
			Write32(pFiveDaysExchangeCarsAwardInfos, 0); //AwardLowNums
			Write32(pFiveDaysExchangeCarsAwardInfos, 0); //CurrentProcessValue
			Write32(pFiveDaysExchangeCarsAwardInfos, 0); //AwardNeedProcessValue
			Write8(pFiveDaysExchangeCarsAwardInfos, 0); //GetAward
			Write8(pFiveDaysExchangeCarsAwardInfos, 0); //BestItemAward
			Write16(pFiveDaysExchangeCarsAwardInfos, 0); //AwardExtraType
			Write32(pFiveDaysExchangeCarsAwardInfos, 0); //ReceBeginDate
			Write32(pFiveDaysExchangeCarsAwardInfos, 0); //ReceEndDate
			Write16(pFiveDaysExchangeCarsAwardInfos, 0); //AwardPoolID
			Write8(pFiveDaysExchangeCarsAwardInfos, 0); //AwardStatus

			len = pFiveDaysExchangeCarsAwardInfos - pRabbitPurseMoonDoLottery;
			Set16(pRabbitPurseMoonDoLottery, (WORD)len);
			pRabbitPurseMoonDoLottery += len;
		}
		Write32(pRabbitPurseMoonDoLottery, 0); //RemainCostItemNums

		len = pRabbitPurseMoonDoLottery - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 28375, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetForbidRefitKartList(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	if (getGameConfig()["KartActivation"] && getGameConfig()["KartActivation"].as<UINT>() == 1)
	{
		BYTE* p = Body; WORD len;

		UINT Uin = Read32(p);
		UINT Time = Read32(p);
		ResponseGetForbidRefitKartList(Client);
	}
	
}

void ResponseGetForbidRefitKartList(ClientNode* Client)
{
	
	std::map<UINT, UINT> ActiveKart; // 车
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	size_t i = 0;
	sql = "SELECT KartID  FROM KartRefit WHERE Uin = ? And ActiveLevel >0;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{ 
			ActiveKart[sqlite3_column_int(stmt, 0)] = sqlite3_column_int(stmt, 0);
		}
	}
	else
	{
		Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	BYTE buf[8192]; BYTE* p = buf; size_t len;
	UINT ForbiddenKartID[] = { 129287, 129099, 127438, 126532, 126347, 124679, 123514, 123499, 122196, 120646, 119067, 119059, 119058, 117935, 116734, 115166, 115160, 112177, 112176, 109563, 109560, 109299, 107371, 107370, 106676, 106675, 106674, 106673, 105425, 104129, 102225, 102224, 102223, 102219, 101885, 100049, 100048, 98830, 98829, 98263, 96785, 94844, 94843, 94842, 94841, 94838, 91957, 90945, 89936, 88865, 86998, 85949, 85948, 85947, 85946, 85942, 84869, 83042, 82249, 81293, 80542, 80161, 78296, 78292, 77236, 75808, 74363, 74362, 73697, 72741, 71588, 71452, 70047, 68643, 66600, 65864, 65109, 63702, 63431, 63429, 62772, 60430, 34851, 33789, 33788, 33258, 32819, 32407, 32406, 31570, 30808, 28779, 28110, 27598, 26931, 26144, 25717, 24924, 24410, 24146, 23980, 23378, 23063, 23015, 22526, 22097, 21961, 21593, 21319, 21086, 20471, 20244, 19951, 19618, 19298, 18767, 18179, 17668, 17354, 16572, 15456, 14107, 14106, 13902, 13703, 12948, 11376 };
	Write16(p, 0); //ResultID
	Write32(p, Client->Uin); //Uin
	size_t ForbidListNum = 0;
	BYTE* pForbidListNum = p;
	Write16(p, 0); //ForbidListNum
	for (size_t i = 0; i < 127; i++) {
		if (ActiveKart[ForbiddenKartID[i]]== ForbiddenKartID[i]) {
			continue;
		}
		ForbidListNum++;
		Write32(p, ForbiddenKartID[i]); //ForbiddenKartID[]
	}
	Set16(pForbidListNum, (WORD)ForbidListNum);
	len = p - buf;
	SendToClient(Client, 175, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyKartActiveCfgInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 3); //EnergySourceNums
	typedef struct EnergySource // 库定义数据类型结构
	{
		UINT SourceClass;
		UINT ActiveProbability;
		UINT ItemID;
	} *LPREFITITEMWEIGHTTYPE;
	const static EnergySource EnergySourceCfg[] =
	{
		{1,500,69938},
		{2,1000,69937},
		{3,1500,69936},
	};
	for (size_t i = 0; i < 3; i++)
	{ //EnergySourceInfo
		BYTE* pEnergySourceInfo = p;
		Write16(pEnergySourceInfo, 0); //len

		Write16(pEnergySourceInfo, EnergySourceCfg[i].SourceClass); //SourceClass
		Write16(pEnergySourceInfo, EnergySourceCfg[i].ActiveProbability); //ActiveProbability
		Write32(pEnergySourceInfo, EnergySourceCfg[i].ItemID); //ItemID

		len = pEnergySourceInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	UINT ForbiddenKartID[] = {129287, 129099, 127438, 126532, 126347, 124679, 123514, 123499, 122196, 120646, 119067, 119059, 119058, 117935, 116734, 115166, 115160, 112177, 112176, 109563, 109560, 109299, 107371, 107370, 106676, 106675, 106674, 106673, 105425, 104129, 102225, 102224, 102223, 102219, 101885, 100049, 100048, 98830, 98829, 98263, 96785, 94844, 94843, 94842, 94841, 94838, 91957, 90945, 89936, 88865, 86998, 85949, 85948, 85947, 85946, 85942, 84869, 83042, 82249, 81293, 80542, 80161, 78296, 78292, 77236, 75808, 74363, 74362, 73697, 72741, 71588, 71452, 70047, 68643, 66600, 65864, 65109, 63702, 63431, 63429, 62772, 60430, 34851, 33789, 33788, 33258, 32819, 32407, 32406, 31570, 30808, 28779, 28110, 27598, 26931, 26144, 25717, 24924, 24410, 24146, 23980, 23378, 23063, 23015, 22526, 22097, 21961, 21593, 21319, 21086, 20471, 20244, 19951, 19618, 19298, 18767, 18179, 17668, 17354, 16572, 15456, 14107, 14106, 13902, 13703, 12948, 11376 };
	Write16(p, 0); //zhanwei
	Write16(p, 127); //ForbiddenActiveKartNums
	for (size_t i = 0; i < 127;i++) {
		Write32(p, ForbiddenKartID[i]); //ForbiddenKartID[]
	}
	Write16(p, 0); //ActiveKartNums
	Write32(p, 0); //ActiveKartID[]

	len = p - buf;
	SendToClient(Client, 24313, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestDoActiveKartInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT KartID = Read32(p);
	UINT ActiveItemID = Read32(p);
	UINT ActiveNums = Read32(p);
	UINT ActiveRate =0;
	UINT ActiveSuccNums = 0;
	UINT ActiveFailedNums = 0;
	UINT CurrentActiveLevel = 0;
	if (ActiveItemID== 69938) {
		ActiveRate = 5;
	}else if (ActiveItemID == 69937) {
		ActiveRate = 10;
	} else if (ActiveItemID == 69936) {
		ActiveRate = 15;
	}
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	sql = "SELECT ActiveLevel  FROM KartRefit WHERE Uin = ? And KartID = ?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, KartID);
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			CurrentActiveLevel = sqlite3_column_int(stmt, 0);
		}
	}
	else
	{
		Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (CurrentActiveLevel >= 10) {
		ResponseStartGame(Client, "激活等级已经达到最高等级！");
		return;
	}
	for (size_t i = 0; i < ActiveNums;i++) {
		sql = "SELECT ActiveLevel  FROM KartRefit WHERE Uin = ? And KartID = ?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, KartID);
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				CurrentActiveLevel = sqlite3_column_int(stmt, 0);
			}
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		if (CurrentActiveLevel>=10) {
			ResponseStartGame(Client,"激活等级已经达到最高等级！");
			break;
		}
		long randomID;
		std::random_device rd;
		std::mt19937 eng(rd());
		std::uniform_int_distribution<int> dis(0, 100);
		randomID = dis(eng);
		//if (ActiveRate> randomID) {
		if (true) {
			ActiveSuccNums++;
			sql = "UPDATE KartRefit SET ActiveLevel=ActiveLevel+1 WHERE Uin=? AND KartID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);
				sqlite3_bind_int(stmt, 2, KartID);
				
				result = sqlite3_step(stmt);
			}
			else
			{
				Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		else {
			ActiveFailedNums++;
		}
	}
	ResponseDoActiveKartInfo(Client, KartID, ActiveSuccNums, ActiveFailedNums, CurrentActiveLevel);
	if (getGameConfig()["KartActivation"] && getGameConfig()["KartActivation"].as<UINT>() == 1)
	{
		ResponseGetForbidRefitKartList(Client);
	}
}

void ResponseDoActiveKartInfo(ClientNode* Client, UINT KartID, UINT ActiveSuccNums, UINT ActiveFailedNums, UINT CurrentActiveLevel)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write32(p, KartID); //KartID
	Write16(p, 0); //ResultID
	Write16(p, ActiveSuccNums); //ActiveSuccNums
	Write16(p, ActiveFailedNums); //ActiveFailedNums
	Write16(p, CurrentActiveLevel); //CurrentActiveLevel
	Write16(p, 0); //AwardItemNum
	for (size_t i = 0; i < 0; i++)
	{ //ActiveFailedAwardItemInfo
		BYTE* pActiveFailedAwardItemInfo = p;
		Write16(pActiveFailedAwardItemInfo, 0); //len

		WriteUn(pActiveFailedAwardItemInfo, 0); //itemID
		WriteUn(pActiveFailedAwardItemInfo, 0); //itemNums
		Write32(pActiveFailedAwardItemInfo, 0); //AvailPeriod

		len = pActiveFailedAwardItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); //LuckValue
	Write32(p, 0); //TotalLuckValue

	len = p - buf;
	SendToClient(Client, 24310, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetActivedKartInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT KartID = Read32(p);
	ResponseGetActivedKartInfo(Client,KartID);
}

void ResponseGetActivedKartInfo(ClientNode* Client,UINT KartID)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	UINT CurrentActiveLevel = 0;
	sql = "SELECT ActiveLevel  FROM KartRefit WHERE Uin = ? And KartID = ?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, KartID);
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			CurrentActiveLevel = sqlite3_column_int(stmt, 0);
		}
	}
	else
	{
		Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	Write32(p, Client->Uin); //Uin
	Write32(p, KartID); //KartID
	Write16(p, 0); //ResultID
	Write32(p, 0); //LuckyFactor
	Write16(p, CurrentActiveLevel); //CurrentActiveLevel

	len = p - buf;
	SendToClient(Client, 24311, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyPlayerAllActiveKartInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	size_t i = 0;
	typedef struct KartActiveLevel // 库定义数据类型结构
	{
		UINT KartID;
		UINT ActiveLevel;
	} *LPREFITITEMWEIGHTTYPE;
	std::vector<KartActiveLevel*> KartActiveLevelList;
	sql = "SELECT KartID,ActiveLevel  FROM KartRefit WHERE Uin = ? And ActiveLevel >0;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			KartActiveLevel* pKartActiveLevel = new KartActiveLevel;
			pKartActiveLevel->KartID = sqlite3_column_int(stmt, 0);
			pKartActiveLevel->ActiveLevel = sqlite3_column_int(stmt, 1);
			KartActiveLevelList.insert(KartActiveLevelList.end(), pKartActiveLevel);
			i++;
		}
	}
	else
	{
		Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	Write32(p, Client->Uin); //Uin
	Write16(p, KartActiveLevelList.size()); //KartNums
	for (size_t i = 0; i < KartActiveLevelList.size(); i++)
	{ //ActiveKartInfo
		BYTE* pActiveKartInfo = p;
		Write16(pActiveKartInfo, 0); //len

		Write32(pActiveKartInfo, 0); //KartType
		Write32(pActiveKartInfo, KartActiveLevelList[i]->KartID); //ItemID
		Write32(pActiveKartInfo, KartActiveLevelList[i]->ActiveLevel); //ActiveLevel

		len = pActiveKartInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24338, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestPointCfg(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT PointID = Read16(p);
	UCHAR Contex = Read8(p);
	USHORT GameType = Read16(p);
	//ResponsePointCfg(Client);
}

void ResponsePointCfg(ClientNode* Client)
{
	if (getGameConfig()["13711"])
	{
		
			const static unsigned char* pMsg13711 = EHexToByte(getGameConfig()["13711"].as<std::string>().data());
			SendToClient(Client, 13711, pMsg13711, 30000, Client->ConnID, FE_PLAYER, Client->ConnID, Response);

		

	}
	
}
void RequestKartPetDoAction(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	short ActionID = Read16(p);
	NotifyKartPetDoAction(Client, ActionID);
}

void NotifyKartPetDoAction(ClientNode* Client, short ActionID)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write32(p, 0); //Time
	Write16(p, ActionID); //ActionID

	len = p - buf;
	SendToClient(Client, 349, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
int getMaxFlag(int SJetWeight, int AccuWeight, int JetWeight, int WWeight, int SpeedWeight, int AntiCollisionWeight, int Speed2Weight, int DriftVecWeight, int AdditionalZSpeedWeight) {
	int MaxFlag = 0;
	if (SJetWeight>20) {
		MaxFlag += 4096;
	}
	if (AccuWeight > 20) {
		MaxFlag += 2048;
	}
	if (JetWeight > 20) {
		MaxFlag += 8192;
	}
	if (WWeight > 20) {
		MaxFlag += 32768;
	}
	if (SpeedWeight > 20) {
		MaxFlag += 16384;
	}
	if (AntiCollisionWeight > 20) {
		MaxFlag += 128;
	}
	if (Speed2Weight > 40) {
		MaxFlag += 1024;
	}
	if (DriftVecWeight > 40) {
		MaxFlag += 512;
	}
	if (AdditionalZSpeedWeight > 40) {
		MaxFlag += 256;
	}
	return MaxFlag;
}
void RequestGetLoveCrystalInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT Context = Read32(p);
	ResponseGetLoveCrystalInfo(Client);
}

void ResponseGetLoveCrystalInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, Client->Uin); //Uin
	Write32(p, 0); //LoveCrystalValue
	for (size_t i = 0; i < 0; i++)
	{ //Award
		BYTE* pAward = p;
		Write16(pAward, 0); //len

		{ //Condition
			BYTE* pCondition = pAward;
			Write16(pCondition, 0); //len

			Write32(pCondition, 0); //LoveCrystalValue
			Write32(pCondition, 0); //FirstLoveBabyLev
			Write32(pCondition, 0); //SecondLoveBabyLev

			len = pCondition - pAward;
			Set16(pAward, (WORD)len);
			pAward += len;
		}
		Write8(pAward, 0); //RaiseTicket
		Write16(pAward, 0); //ItemNum
		for (size_t i = 0; i < 0; i++)
		{ //ItemInfo
			BYTE* pItemInfo = pAward;
			Write16(pItemInfo, 0); //len

			Write32(pItemInfo, 0); //ItemID
			Write32(pItemInfo, 0); //ItemNum
			Write32(pItemInfo, 0); //AvailPeriod
			Write8(pItemInfo, 0); //Status
			WriteUn(pItemInfo, 0); //ObtainTime
			Write32(pItemInfo, 0); //OtherAttribute
			Write16(pItemInfo, 0); //ItemType

			len = pItemInfo - pAward;
			Set16(pAward, (WORD)len);
			pAward += len;
		}
		Write8(pAward, 0); //Status

		len = pAward - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //LeftLoveBabyRaiseTicket
	Write32(p, 0); //AwardLoveCrystalValue
	Write8(p, 0); //CanAddValue
	Write32(p, 0); //Context

	len = p - buf;
	SendToClient(Client, 24784, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetBabyDeleteAwardInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseGetBabyDeleteAwardInfo(Client);
}

void ResponseGetBabyDeleteAwardInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write8(p, 0); //HaveAward
	{ //AwardInfo
		BYTE* pAwardInfo = p;
		Write16(pAwardInfo, 0); //len

		Write32(pAwardInfo, 0); //Exp
		Write32(pAwardInfo, 0); //Money
		Write32(pAwardInfo, 0); //SuperMoney
		Write16(pAwardInfo, 0); //ItemNum
		for (size_t i = 0; i < 0; i++)
		{ //ItemInfo
			BYTE* pItemInfo = pAwardInfo;
			Write16(pItemInfo, 0); //len

			Write32(pItemInfo, 0); //ItemID
			Write32(pItemInfo, 0); //ItemNum
			Write32(pItemInfo, 0); //AvailPeriod
			Write8(pItemInfo, 0); //Status
			WriteUn(pItemInfo, 0); //ObtainTime
			Write32(pItemInfo, 0); //OtherAttribute
			Write16(pItemInfo, 0); //ItemType

			len = pItemInfo - pAwardInfo;
			Set16(pAwardInfo, (WORD)len);
			pAwardInfo += len;
		}
		Write32(pAwardInfo, 0); //Coupons
		Write32(pAwardInfo, 0); //GuildPoint
		Write32(pAwardInfo, 0); //LuckMoney
		Write8(pAwardInfo, 0); //ExtendInfoNum
		for (size_t i = 0; i < 0; i++)
		{ //ExtendInfoAward
			BYTE* pExtendInfoAward = pAwardInfo;
			Write16(pExtendInfoAward, 0); //len

			Write16(pExtendInfoAward, 0); //Key
			Write32(pExtendInfoAward, 0); //AddValue
			Write32(pExtendInfoAward, 0); //TotalValue
			Write8(pExtendInfoAward, 0); //ShowPriority
			Write16(pExtendInfoAward, 0); //BuyNeedScore
			Write8(pExtendInfoAward, 0); //OneMatchMaxNum

			len = pExtendInfoAward - pAwardInfo;
			Set16(pAwardInfo, (WORD)len);
			pAwardInfo += len;
		}
		Write32(pAwardInfo, 0); //SpeedCoin

		len = pAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //ShouldRemindRasieSecondBabyAward

	len = p - buf;
	SendToClient(Client, 24793, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestCancelEngage(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT DstUin = Read32(p);
	int EngageType = Read32(p);
	short WordLength = Read16(p);
	int UseItemId = Read32(p);
	ResponseCancelEngage(Client, DstUin);
	NotifyCancelEngage(Client, Client->Uin,Client->Engage->NickName, Client->Engage->UIN,EngageType);
	ClientNode* Dst = GetClient(DstUin);
	if (Dst) {
		ResponseCancelEngage(Dst, Uin);
		NotifyCancelEngage(Dst, Dst->Uin, Dst->Engage->NickName, Dst->Engage->UIN, EngageType);
	}
	ResponseStartGame(Client,"离婚成功");
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	sql = "DELETE FROM EngageInfo WHERE EngageUin1 = ? or EngageUin2 = ?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, Client->Uin);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
}

void ResponseCancelEngage(ClientNode* Client, UINT DstUin)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, Client->Uin); //Uin
	Write32(p, DstUin); //DstUin
	Write8(p, 0); //ReasonLen

	len = p - buf;
	SendToClient(Client, 167, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyCancelEngage(ClientNode* Client, UINT SrcUin,std::string NickName,UINT DstUin, int EngageType)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, SrcUin); //SrcUin
	WriteString(p, NickName.data(),16); //NickName[]
	Write32(p, DstUin); //DstUin
	Write32(p, 0); //EngageType
	Write16(p, 0); //WordLength

	len = p - buf;
	SendToClient(Client, 574, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestPropEffectResult(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	short PropID = Read16(p);
	int PropIndex = Read32(p);
	short EffectResult = Read16(p);
	int ItemID = Read32(p);
	UINT SrcUin = Read32(p);
	int StoneSkillType = Read32(p);
	UINT LapTime = Read32(p);
	UCHAR RemoveOwnPropNum = Read8(p);
	std::vector<UCHAR> RemoveOwnProp;
	for (size_t i = 0; i < RemoveOwnPropNum;i++) {
		RemoveOwnProp.insert(RemoveOwnProp.end(), Read32(p));
	}
	bool DstType = Read8(p);
	UCHAR SrcUinRank = Read8(p);
	UCHAR SrcUinTeammateRank = Read8(p);
	UINT NPCUin = Read32(p);
	if (EffectResult==0) {
		ClientNode* TargetClient = GetClient(SrcUin);
		if (TargetClient) {
			NotifyPropEffectResult(TargetClient,Uin, RemoveOwnProp,SrcUin,PropID,PropIndex,StoneSkillType,LapTime);
			NotifyPropPoint(TargetClient,Uin, PropID);
		}
	}
}

void NotifyPropEffectResult(ClientNode* Client,UINT Uin, std::vector<UCHAR> RemoveOwnProp,UINT SrcUin, UINT PropID, UINT PropIndex, UINT StoneSkillType, UINT LapTime)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	WriteUn(p, Uin); //Uin
	Write16(p, PropID); //PropID
	Write32(p, PropIndex); //PropIndex
	Write32(p, StoneSkillType); //StoneSkillType
	Write32(p, LapTime); //LapTime
	Write8(p, RemoveOwnProp.size()); //RemoveOwnPropNum
	for (size_t i = 0; i < RemoveOwnProp.size(); i++) {
		Write32(p, RemoveOwnProp[i]); //RemoveOwnPropIdx[]
	}
	
	Write32(p, SrcUin); //SrcUin
	Write16(p, 0); //EffectResult

	len = p - buf;
	SendToClient(Client, 518, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyPropPoint(ClientNode* Client,UINT Uin, UINT PropID)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Uin); //Uin
	Write32(p, PropID); //PropID
	Write8(p, 0); //PropType
	Write32(p, Uin); //DstUin
	Write32(p, 5); //PropPointInc
	Write32(p, 0); //PropPointDec
	Write32(p, 99); //PropPointTotal
	Write8(p, 0); //AttackNum
	Write8(p, 0); //AttackTeammateNum
	Write8(p, 0); //ProtectNum
	Write8(p, 0); //ProtectTeammatetNum
	Write8(p, 0); //ComboFlag

	len = p - buf;
	SendToClient(Client, 1502, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestChangeAimState(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT DstUin = Read32(p);
	bool AimState = Read8(p);
	bool DstType = Read8(p);
	ClientNode* TargetClient = GetClient(DstUin);
	NotifyChangeAimState(TargetClient,Uin,DstUin, AimState,DstType);
}

void NotifyChangeAimState(ClientNode* Client,UINT Uin, UINT DstUin, UINT AimState, UINT DstType)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	WriteUn(p, Uin); //Uin
	WriteUn(p, DstUin); //DstUin
	Write8(p, AimState); //AimState
	Write8(p, DstType); //DstType

	len = p - buf;
	SendToClient(Client, 519, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestGetBattleModeCarSkillInfoList(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	int CarID = Read32(p);
	UINT Time = Read32(p);
	ResponseGetBattleModeCarSkillInfoList(Client);
}

void ResponseGetBattleModeCarSkillInfoList(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, Client->Uin); //Uin
	Write32(p, Client->KartID); //CarID
	{ //BattleModeSkillInfoList
		BYTE* pBattleModeSkillInfoList = p;
		Write16(pBattleModeSkillInfoList, 0); //len

		Write8(pBattleModeSkillInfoList, 1); //SkillNum
		for (size_t i = 0; i < 1; i++)
		{ //SkillInfo
			BYTE* pSkillInfo = pBattleModeSkillInfoList;
			Write16(pSkillInfo, 0); //len

			Write16(pSkillInfo, 1); //SkillId
			Write8(pSkillInfo, 1); //SkillLevel

			len = pSkillInfo - pBattleModeSkillInfoList;
			Set16(pBattleModeSkillInfoList, (WORD)len);
			pBattleModeSkillInfoList += len;
		}

		len = pBattleModeSkillInfoList - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 10929, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetSpecialBroadcasts(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	bool Type = Read8(p);
	ResponseGetSpecialBroadcasts(Client, Type);
}

void ResponseGetSpecialBroadcasts(ClientNode* Client, bool Type)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write8(p, Type); //Type
	Write8(p, 1); //BroadcastNum
	for (size_t i = 0; i < 1; i++)
	{ //Broadcast
		BYTE* pBroadcast = p;
		Write16(pBroadcast, 0); //len

		Write8(pBroadcast, 0); //BCFlag
		Write16(pBroadcast, 0); //ConTime
		Write16(pBroadcast, 0); //ContentLen
		Write32(pBroadcast, 0); //BroadcastType
		Write8(pBroadcast, 0); //DisplayFlag
		Write32(pBroadcast, 0); //ID1
		Write32(pBroadcast, 0); //ID2
		Write16(pBroadcast, 0); //AttachLen

		len = pBroadcast - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 362, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifySecondPassInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write8(p, Client->SecPwdStats); //Status
	Write8(p, Client->Verify); //Verify
	Write32(p, Client->FrozenLeftSeconds); //FrozenLeftSeconds
	Write32(p, Client->FrozenSeconds); //FrozenSeconds

	len = p - buf;
	SendToClient(Client, 21015, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestOpSecondPass(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	char Op = Read8(p);//0设置二级密码 4验证二级密码 3 关闭二级密码 6 修改密码 2 忘记密码

	char* CurPass = ReadString(p, MaxSecPwd);
	char* NewPass = ReadString(p, MaxSecPwd);
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	if (Op==0) {
		ResponseOpSecondPass(Client, Op, 1);


		sql = "UPDATE BaseInfo SET SecPwd=?,SecPwdStatus=1 WHERE Uin=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_text(stmt, 1, NewPass, strlen(NewPass), SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 2, Client->Uin);
			result = sqlite3_step(stmt);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	if (Op == 2) {
		ResponseStartGame(Client, "忘记二级密码联系管理员");
	}
	else if (Op == 3) {
		if (strcmp(CurPass, Client->SecPwd) == 0) {
			Client->SecPwdStats = 0;
			ResponseOpSecondPass(Client, Op, 1);
			NotifySecondPassInfo(Client);
			sql = "UPDATE BaseInfo SET SecPwdStatus=0,Verify=0 WHERE Uin=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);
				result = sqlite3_step(stmt);
			}
			else
			{
				Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			
		}
		else {
			ResponseStartGame(Client, "二级密码错误");
		}
	}
	else if (Op == 4) {
		if (strcmp(CurPass, Client->SecPwd) == 0) {
			Client->Verify = 1;
			ResponseOpSecondPass(Client, Op, 1);
			NotifySecondPassInfo(Client);
			sql = "UPDATE BaseInfo SET Verify=1 WHERE Uin=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);
				result = sqlite3_step(stmt);
			}
			else
			{
				Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		else {
			ResponseStartGame(Client, "二级密码错误");
		}
	}
	
}

void ResponseOpSecondPass(ClientNode* Client, char Op,UINT Status)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write8(p, Op); //Op
	Write8(p, Status); //Status
	Write16(p, 0); //ResultID

	len = p - buf;
	SendToClient(Client, 21017, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyClientXCarEvent(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write32(p, (UINT)time(nullptr)); //Time
	Write32(p, Client->Uin); //TriggerUin
	Write32(p, 10020); //KartID
	Write32(p, 10000); //EventFlag
	Write32(p, 10000); //EventDuration
	Write8(p, 1); //EnhancedPhysInfoCount
	for (size_t i = 0; i < 1; i++)
	{ //EnhancedPhysInfo
		BYTE* pEnhancedPhysInfo = p;
		Write16(pEnhancedPhysInfo, 0); //len

		Write32(pEnhancedPhysInfo, 99999); //PID
		Write32(pEnhancedPhysInfo, 100); //AffectID
		Write32(pEnhancedPhysInfo, 60000); //Param
		Write32(pEnhancedPhysInfo, 66000); //LifeTime

		len = pEnhancedPhysInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 5100, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
