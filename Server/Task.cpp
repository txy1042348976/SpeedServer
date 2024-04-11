#include "Message.h"
#include "GameServer.h"
#include <mutex>
#include <stdio.h>
#include "z_debug.h"

std::map<UINT, TaskInfo *> TaskList;
std::mutex taskMutex;
TaskInfo *GetTaskInfo(UINT TaskID)
{
	TaskInfo *pTaskInfo = TaskList[TaskID];
	return pTaskInfo;
}
void LoadTaskConfig()//任务列表
{
	if (TaskList.size()>0){
		Z_ERROR("TaskConfig inited before!\n");
		exit(0);
	}
	try
	{
		YAML::Node Config = YAML::LoadFile(".\\Config\\TaskList.yml");
		for (size_t i = 0; i < Config.size(); i++)
		{
			YAML::Node Task = Config[i];
			TaskInfo *TaskIn = new TaskInfo;
			TaskList[Task["TaskID"].as<UINT>()] = TaskIn;
			TaskIn->TaskID = Task["TaskID"].as<UINT>();
			TaskIn->MapID = Task["MapID"].as<UINT>();
			if (Task["MapID"].as<std::string>() == "true")
			{
				TaskIn->DBType = true;
			}
			if (Task["EHexToByte"])
			{
				TaskIn->EHexToByte = Task["EHexToByte"].as<std::string>();
			}
			if (Task["EHexToByteLen"])
			{
				TaskIn->EHexToByteLen = Task["EHexToByteLen"].as<INT>();
			}
			TaskIn->PrevStatus = Task["PrevStatus"].as<UINT>();
			TaskIn->Status = Task["Status"].as<UINT>();
		}
	}
	catch (const std::exception &)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		Z_INFO("任务列表加载文件出错\n");
		exit(0);
	}
	Z_INFO("任务列表加载完成[%d]!\n", TaskList.size());
}

void RequestGetLicenseInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseGetLicenseInfo(Client);
}

void ResponseGetLicenseInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin

	Write16(p, 0); // LicenseNum

	Write8(p, 0); // ReasonLen

	len = p - buf;
	SendToClient(Client, 145, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetSystemTaskList(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	UINT TaskVersion = Read32(Body);

	ResponseGetSystemTaskList(Client, TaskVersion);
}

void ResponseGetSystemTaskList(ClientNode *Client, UINT TaskVersion)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	Write32(p, TaskVersion); // LatestTaskVersion

	Write32(p, 0); // TaskNum
	/*
m_astSystemTaskInfo[].m_iTaskID
m_astSystemTaskInfo[].m_iPackID
m_astSystemTaskInfo[].m_szTaskName[17]
m_astSystemTaskInfo[].m_shTaskIntroLen
m_astSystemTaskInfo[].m_bSuccDialogLen
m_astSystemTaskInfo[].m_bFailDialogLen
m_astSystemTaskInfo[].m_bAwardInfoLen
m_astSystemTaskInfo[].m_bAwardOtherInfoLen
m_astSystemTaskInfo[].m_bProgressLen
m_astSystemTaskInfo[].m_shExecuteNum
m_astSystemTaskInfo[].m_bPrevTaskNum
m_astSystemTaskInfo[].m_aiPrevTaskID[]
m_astSystemTaskInfo[].m_shLogicType
m_astSystemTaskInfo[].m_shTaskType
m_astSystemTaskInfo[].m_bOperatorType
m_astSystemTaskInfo[].m_iOperatorParam
m_astSystemTaskInfo[].m_iOption
m_astSystemTaskInfo[].m_uiIdentity
m_astSystemTaskInfo[].m_iNeedNum
m_astSystemTaskInfo[].m_shShowType
m_astSystemTaskInfo[].m_iTotalTime
m_astSystemTaskInfo[].m_bDiffInfoLen
m_astSystemTaskInfo[].m_shTaskShortIntroLen
m_astSystemTaskInfo[].m_shJumpFuncNameLen
	*/

	Write32(p, 0); // TaskPackNum
	/*
m_astSystemTaskPackInfo[].m_iPackID
m_astSystemTaskPackInfo[].m_szPackName[17]
m_astSystemTaskPackInfo[].m_shPackType
m_astSystemTaskPackInfo[].m_shTaskNum
m_astSystemTaskPackInfo[].m_aiTaskID[]
m_astSystemTaskPackInfo[].m_ushPriority
m_astSystemTaskPackInfo[].m_shPackMode
	*/

	len = p - buf;
	SendToClient(Client, 152, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestSkipFreshTask(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseSkipFreshTask(Client);
}

void ResponseSkipFreshTask(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin

	len = p - buf;
	SendToClient(Client, 183, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetTaskList(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	// WORD len;

	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseGetTaskList(Client);
}

#include <vector>

void ResponseGetTaskList(ClientNode *Client)
{

	// int n = 100;   //n好像不能太大
	// std::vector<int>TaskIn(n);

	// for (size_t i = 0; i < n; i++)
	//{
	//	TaskIn[i] = 1 + i;   //1000可以变， 比如1100、1200、   或者2000、3000、
	// }

	std::vector<uint8_t> buf(8192 * 4);
	// BYTE buf[8192];
	uint8_t *p = buf.data();
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	// int TaskIn[] = { 73,110,1701,1404,1405,1406,1407,1408,1409,1410,1411 };
	/*TaskIn[97] = 1656;
	TaskIn[98] = 1657;
	TaskIn[99] = 1658;*/
	Write16(p, (WORD)TaskList.size()); // TaskNum
	taskMutex.lock();
	std::map<UINT, TaskInfo *>::iterator iter;
	for (iter = TaskList.begin(); iter != TaskList.end(); iter++)
	{ // PlayerTaskInfo
		BYTE *pPlayerTaskInfo = p;
		Write16(pPlayerTaskInfo, 0); // len
		TaskInfo *TaskInfo = iter->second;
		{ // PlayerDBTaskInfo
			BYTE *pPlayerDBTaskInfo = pPlayerTaskInfo;
			Write16(pPlayerDBTaskInfo, 0); // len

			Write32(pPlayerDBTaskInfo, TaskInfo->TaskID);	 // TaskID
			Write16(pPlayerDBTaskInfo, 0);					 // OptionID
			Write8(pPlayerDBTaskInfo, TaskInfo->Status);	 // Status
			Write8(pPlayerDBTaskInfo, TaskInfo->PrevStatus); // PrevStatus
			Write32(pPlayerDBTaskInfo, 0);					 // ProgressRate
			Write32(pPlayerDBTaskInfo, 0);					 // Other
			Write32(pPlayerDBTaskInfo, 0);					 // TakeTime
			Write32(pPlayerDBTaskInfo, 0);					 // LastUpdateTime

			len = pPlayerDBTaskInfo - pPlayerTaskInfo;
			Set16(pPlayerTaskInfo, (WORD)len);
			pPlayerTaskInfo += len;
		}
		Write16(pPlayerTaskInfo, 0); // MapID
		Write8(pPlayerTaskInfo, 0);	 // IsRecommandTask

		len = pPlayerTaskInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	taskMutex.unlock();
	Write8(p, 0);  // ReasonLen
	Write16(p, 0); // RecommendLen
	Write32(p, 0); // SpecialTaskID
	Write8(p, 0);  // RecomTaskPackNum
	for (size_t i = 0; i < 0; i++)
	{ // RecomTaskPack
		BYTE *pRecomTaskPack = p;
		Write16(pRecomTaskPack, 0); // len

		Write32(pRecomTaskPack, 0); // TaskPackID
		Write32(pRecomTaskPack, 0); // SpecialFlag

		len = pRecomTaskPack - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // BlackListNum
	for (size_t i = 0; i < 0; i++)
	{ // TaskBlackList
		BYTE *pTaskBlackList = p;
		Write16(pTaskBlackList, 0); // len

		Write8(pTaskBlackList, 0);	// EntityType
		Write32(pTaskBlackList, 0); // EntityID

		len = pTaskBlackList - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf.data();
	SendToClient(Client, 135, buf.data(), len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetRandomTask(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UCHAR Flag = Read8(Body);

	ResponseGetRandomTask(Client);
}

void ResponseGetRandomTask(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	Write32(p, 0);			 // TaskPackNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //SystemTaskPackInfo
		BYTE* pSystemTaskPackInfo = p;
		Write16(pSystemTaskPackInfo, 0); //len

		Write32(pSystemTaskPackInfo, 0); //PackID
		WriteString(pSystemTaskPackInfo, 0); //PackName[]
		Write16(pSystemTaskPackInfo, 0); //PackType
		Write16(pSystemTaskPackInfo, 0); //TaskNum
		Write32(pSystemTaskPackInfo, 0); //TaskID[]
		Write16(pSystemTaskPackInfo, 0); //Priority
		Write16(pSystemTaskPackInfo, 0); //PackMode

		len = pSystemTaskPackInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, 0); // TaskNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //SystemTaskInfo
		BYTE* pSystemTaskInfo = p;
		Write16(pSystemTaskInfo, 0); //len

		Write32(pSystemTaskInfo, 0); //TaskID
		Write32(pSystemTaskInfo, 0); //PackID
		WriteString(pSystemTaskInfo, 0); //TaskName[]
		Write16(pSystemTaskInfo, 0); //TaskIntroLen
		Write8(pSystemTaskInfo, 0); //SuccDialogLen
		Write8(pSystemTaskInfo, 0); //FailDialogLen
		Write8(pSystemTaskInfo, 0); //AwardInfoLen
		Write8(pSystemTaskInfo, 0); //AwardOtherInfoLen
		Write8(pSystemTaskInfo, 0); //ProgressLen
		Write16(pSystemTaskInfo, 0); //ExecuteNum
		Write8(pSystemTaskInfo, 0); //PrevTaskNum
		Write32(pSystemTaskInfo, 0); //PrevTaskID[]
		Write16(pSystemTaskInfo, 0); //LogicType
		Write16(pSystemTaskInfo, 0); //TaskType
		Write8(pSystemTaskInfo, 0); //OperatorType
		Write32(pSystemTaskInfo, 0); //OperatorParam
		Write32(pSystemTaskInfo, 0); //Option
		Write32(pSystemTaskInfo, 0); //Identity
		Write32(pSystemTaskInfo, 0); //NeedNum
		Write16(pSystemTaskInfo, 0); //ShowType
		Write32(pSystemTaskInfo, 0); //TotalTime
		Write8(pSystemTaskInfo, 0); //DiffInfoLen
		Write16(pSystemTaskInfo, 0); //TaskShortIntroLen
		Write16(pSystemTaskInfo, 0); //JumpFuncNameLen

		len = pSystemTaskInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write16(p, 0); // TaskNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //TaskInfo
		BYTE* pTaskInfo = p;
		Write16(pTaskInfo, 0); //len

		{ //PlayerDBTaskInfo
			BYTE* pPlayerDBTaskInfo = pTaskInfo;
			Write16(pPlayerDBTaskInfo, 0); //len

			Write32(pPlayerDBTaskInfo, 0); //TaskID
			Write16(pPlayerDBTaskInfo, 0); //OptionID
			Write8(pPlayerDBTaskInfo, 0); //Status
			Write8(pPlayerDBTaskInfo, 0); //PrevStatus
			Write32(pPlayerDBTaskInfo, 0); //ProgressRate
			Write32(pPlayerDBTaskInfo, 0); //Other
			Write32(pPlayerDBTaskInfo, 0); //TakeTime
			Write32(pPlayerDBTaskInfo, 0); //LastUpdateTime

			len = pPlayerDBTaskInfo - pTaskInfo;
			Set16(pTaskInfo, (WORD)len);
			pTaskInfo += len;
		}
		Write16(pTaskInfo, 0); //MapID
		Write8(pTaskInfo, 0); //IsRecommandTask

		len = pTaskInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write16(p, 0); // TaskDescNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //TaskDescInfo
		BYTE* pTaskDescInfo = p;
		Write16(pTaskDescInfo, 0); //len

		Write32(pTaskDescInfo, 0); //TaskID
		Write8(pTaskDescInfo, 0); //RandTaskType
		Write32(pTaskDescInfo, 0); //GameMode
		Write32(pTaskDescInfo, 0); //GameSubMode
		Write32(pTaskDescInfo, 0); //ID
		Write16(pTaskDescInfo, 0); //PlayerNum
		Write16(pTaskDescInfo, 0); //Score
		Write16(pTaskDescInfo, 0); //NeedNum

		len = pTaskDescInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	len = p - buf;
	SendToClient(Client, 187, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}