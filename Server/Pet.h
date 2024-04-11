#pragma once

#include "yaml-cpp/yaml.h"
void LoadPetLevelConfig();
void GetPetInfo(ClientNode *Client);
UINT GetPetLevel(UINT Exp);
UINT GetPetStatus(UINT Level, UINT PetID);
UINT GetPetMaxExp(UINT Level);
YAML::Node GetPetConfig(UINT PetID);
void RequestGetPetInfo(ClientNode* Client, BYTE* Body, size_t BodyLen); //24528
void ResponseGetPetInfo(ClientNode* Client); //24528
void RequestGetPetShowInfo(ClientNode* Client, BYTE* Body, size_t BodyLen); //24426
void ResponseGetPetShowInfo(ClientNode* Client, UINT PETID); //24426
void ResponseGetPetShowMoreInfo(ClientNode* Client, UINT PETID); //24527
void RequestUpdatePetName(ClientNode *Client, BYTE *Body, size_t BodyLen); // 178
void ResponseUpdatePetName(ClientNode *Client);                            // 178
void NotifyBroadcastPetEvolution(ClientNode* Client, UINT PetID, UINT Status);
PetInfo* GetPetItem(UINT PetID,UINT Uin);