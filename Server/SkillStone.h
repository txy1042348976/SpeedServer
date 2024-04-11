#pragma once
#include "Message.h"

void RequestInlaySkillStone(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseInlaySkillStone(ClientNode *Client, int KartID, int StoneGrooveID, int SkillStoneID); // 901

void RequestAddStoneGroove(ClientNode *Client, BYTE *Body, size_t BodyLen);                         // 903
void ResponseAddStoneGroove(ClientNode *Client, int KartID, int CarveKnifeID, int StoneGrooveType); // 903

void RequestRemoveSkillStone(ClientNode *Client, BYTE *Body, size_t BodyLen);                                          // 912
void ResponseRemoveSkillStone(ClientNode *Client, int KartID, int StoneGrooveID, int RemovalToolID, int SkillStoneID); // 912