#include "Database.h"
#include "Item.h"
#include <stdio.h>
#include <time.h>
#include "sqlite/sqlite3.h"
#pragma comment(lib, "sqlite3.lib")
sqlite3 *UserDB;
sqlite3 *PlayerDB;
extern BYTE shumiaoyunshu[100];
extern BYTE chewangkaoyan[100];
extern BYTE baofengtiaozhan[2048];
extern BYTE taskaward[128];
extern BYTE StartTimerChallenge[1024];
extern BYTE StartTimerChallenge2[1024];
extern BYTE BorderOver[187];
extern BYTE RankedOver[402];
extern BYTE BorderOnePlayerInfo[1305];
extern BYTE RankedMainBoard[7024];
extern BYTE jump24238[91];
extern BYTE jump24255[91];
void InitTaskInfo()
{
	//模拟封包 对照数据结构
	//暴风车队NPC可用自动跑老地图104-138
	memcpy(baofengtiaozhan, "\x00\x00\xea\x60\x00\x00\x00\x49\x00\x00\x00\x79\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x9b\x28\x9c\x8c\x6b\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x01\x6e\x00\x00\x00\x01\xcc\xd8\xc0\xef\xcb\xb9\xcc\xb9\x00\x00\x00\x00\x00\x00\x00\x00\x00\xb1\xa9\xb7\xe7\xcc\xec\xca\xb9\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x46\x00\x00\x27\x26\x00\x00\x2c\x77\x00\x00\x28\x3f\x00\x00\x2c\x79\x00\x00\x2c\x75\x00\x00\x2c\x78\x00\x00\x2c\x76\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2c\x7a\x00\x00\x00\x00\x00\x00\x2c\x70\x00\x00\x27\x2d\x00\x00\x27\x75\x00\x00\x27\x73\x00\x00\x28\x08\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x27\x10\x00\x22\x00\x00\x00\x41\x00\x00\x00\x0f\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x22\x00\x00\x00\x37\x00\x00\x00\x0a\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x01\x86\x9f\x00\x00\x00\xc8\x00\x22\x00\x00\x00\x3c\x00\x00\x00\x14\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\xc8\x00\x00\x00\x00\x00\x22\x00\x00\x00\x46\x00\x00\x00\x14\x00\x00\x00\x0a\x00\x00\x00\x05\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\x00\xff\xff\xff\x9c\x00\x22\x00\x00\x00\x50\x00\x00\x00\x19\x00\x00\x00\x14\x00\x00\x00\x0a\x00\x00\x00\x05\x00\x00\x00\x01\xff\xff\xff\x9c\xff\xfe\x79\x61\x04\x00\x12\x00\x00\x00\x08\x00\x00\x00\x00\x00\x0f\x42\x36\x00\x00\x27\x0f\x00\x12\x00\x00\x00\x0a\x00\x00\x1f\x40\x00\x00\x03\xe7\x00\x00\x00\x14\x00\x12\x00\x00\x00\x01\x00\x00\x13\x88\x00\x00\x00\x14\xff\xff\xff\xec\x00\x12\x00\x00\x00\x02\x00\x00\x13\x88\xff\xff\xff\xec\xff\xff\xff\x9c\x00\x00\x00\x00\x01\x6e\x00\x00\x00\x02\xbf\xad\xc0\xf2\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xb1\xa9\xb7\xe7\xcc\xec\xca\xb9\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x46\x00\x00\x27\x22\x00\x00\x2c\x07\x00\x00\x28\x42\x00\x00\x2c\x09\x00\x00\x2c\x05\x00\x00\x2c\x08\x00\x00\x2c\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2c\x0a\x00\x00\x00\x00\x00\x00\x2c\x6b\x00\x00\x28\x7c\x00\x00\x27\x75\x00\x00\x27\x73\x00\x00\x2b\xed\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x27\x10\x00\x22\x00\x00\x00\x1e\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x22\x00\x00\x00\x28\x00\x00\x00\x0a\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x01\x86\x9f\x00\x00\x00\xc8\x00\x22\x00\x00\x00\x37\x00\x00\x00\x14\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\xc8\x00\x00\x00\x00\x00\x22\x00\x00\x00\x3c\x00\x00\x00\x14\x00\x00\x00\x0a\x00\x00\x00\x05\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\x00\xff\xff\xff\x9c\x00\x22\x00\x00\x00\x50\x00\x00\x00\x19\x00\x00\x00\x14\x00\x00\x00\x0a\x00\x00\x00\x05\x00\x00\x00\x01\xff\xff\xff\x9c\xff\xfe\x79\x61\x04\x00\x12\x00\x00\x00\x08\x00\x00\x1f\x40\x00\x00\x00\xc8\x00\x00\x00\x01\x00\x12\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x12\x00\x00\x00\x01\x00\x00\x03\xe8\x00\x00\x00\x00\xff\xff\xfe\x0c\x00\x12\x00\x00\x00\x02\x00\x00\x00\x00\xff\xff\xfc\x19\xff\xff\xd8\xf1\x00\x00\x00\x00\x01\x6e\x00\x00\x00\x03\xbc\xd3\xce\xc4\xcb\xb9\xc0\xfb\x00\x00\x00\x00\x00\x00\x00\x00\x00\xb1\xa9\xb7\xe7\xcc\xec\xca\xb9\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x46\x00\x00\x27\x26\x00\x00\x2c\x0d\x00\x00\x27\x8c\x00\x00\x2c\x0f\x00\x00\x2c\x0b\x00\x00\x2c\x0e\x00\x00\x2c\x0c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2c\x10\x00\x00\x00\x00\x00\x00\x2b\x8b\x00\x00\x28\x7e\x00\x00\x27\x75\x00\x00\x27\x73\x00\x00\x28\x03\x00\x00\x29\x89\x00\x00\x00\x05\x00\x00\x27\x10\x00\x22\x00\x00\x00\x28\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x22\x00\x00\x00\x19\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x01\x86\x9f\x00\x00\x01\x2c\x00\x22\x00\x00\x00\x28\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x01\x2c\x00\x00\x00\x00\x00\x22\x00\x00\x00\x50\x00\x00\x00\x0a\x00\x00\x00\x0a\x00\x00\x00\x02\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\x00\xff\xff\xff\x38\x00\x22\x00\x00\x00\x50\x00\x00\x00\x14\x00\x00\x00\x0a\x00\x00\x00\x05\x00\x00\x00\x05\x00\x00\x00\x01\xff\xff\xff\x38\xff\xfe\x79\x61\x04\x00\x12\x00\x00\x00\x08\x00\x00\x1f\x40\x00\x00\x03\x20\x00\x00\x00\x32\x00\x12\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x00\x00\x12\x00\x00\x00\x01\x00\x00\x13\x88\x00\x00\x00\xc8\xff\xff\xff\x38\x00\x12\x00\x00\x00\x02\x00\x00\x00\x00\xff\xff\xff\xce\xff\xfe\x79\x61\x00\x00\x00\x00\x01\x6e\x00\x00\x00\x04\xd2\xc1\xc1\xab\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xb1\xa9\xb7\xe7\xcc\xec\xca\xb9\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x46\x00\x00\x27\x22\x00\x00\x2c\x13\x00\x00\x27\x77\x00\x00\x27\x36\x00\x00\x2c\x11\x00\x00\x2c\x14\x00\x00\x2c\x12\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2c\x15\x00\x00\x00\x00\x00\x00\x2b\xe6\x00\x00\x27\x2d\x00\x00\x27\x75\x00\x00\x27\x73\x00\x00\x29\xb8\x00\x00\x28\xbc\x00\x00\x00\x05\x00\x00\x27\x10\x00\x22\x00\x00\x00\x28\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x22\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x01\x86\x9f\x00\x00\x00\xc8\x00\x22\x00\x00\x00\x0f\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\xc8\x00\x00\x00\x00\x00\x22\x00\x00\x00\x19\x00\x00\x00\x0a\x00\x00\x00\x0a\x00\x00\x00\x02\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\x00\xff\xff\xff\x38\x00\x22\x00\x00\x00\x32\x00\x00\x00\x14\x00\x00\x00\x0a\x00\x00\x00\x03\x00\x00\x00\x05\x00\x00\x00\x01\xff\xff\xff\x38\xff\xfe\x79\x61\x04\x00\x12\x00\x00\x00\x08\x00\x00\x00\x00\x00\x0f\x42\x36\x00\x00\x27\x0f\x00\x12\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x27\x0f\x00\x00\x03\xe7\x00\x12\x00\x00\x00\x01\x00\x00\x0f\xa0\x00\x00\x00\x64\xff\xff\xfe\x0c\x00\x12\x00\x00\x00\x02\x00\x00\x00\x00\xff\xff\xfc\x19\xff\xfe\x79\x61\x00\x00\x00\x00\x01\x6e\x00\x00\x00\x05\xb6\xf7\xd7\xf4\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xb1\xa9\xb7\xe7\xcc\xec\xca\xb9\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x46\x00\x00\x27\x26\x00\x00\x2c\x9d\x00\x00\x28\x53\x00\x00\x00\x00\x00\x00\x2c\x9b\x00\x00\x2c\x9e\x00\x00\x2c\x9c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2c\x9f\x00\x00\x00\x00\x00\x00\x2b\xe5\x00\x00\x27\x2d\x00\x00\x27\x75\x00\x00\x27\x73\x00\x00\x28\x75\x00\x00\x27\x97\x00\x00\x00\x0a\x00\x00\x27\x10\x00\x22\x00\x00\x00\x28\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x22\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x01\x86\x9f\x00\x00\x00\x32\x00\x22\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\x32\x00\x00\x00\x00\x00\x22\x00\x00\x00\x0c\x00\x00\x00\x02\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\x00\xff\xff\xff\x38\x00\x22\x00\x00\x00\x12\x00\x00\x00\x05\x00\x00\x00\x0a\x00\x00\x00\x05\x00\x00\x00\x05\x00\x00\x00\x01\xff\xff\xff\x38\xff\xfe\x79\x61\x04\x00\x12\x00\x00\x00\x08\x00\x00\x00\x00\x00\x0f\x42\x36\x00\x00\x27\x0f\x00\x12\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x27\x0f\x00\x00\x03\xe7\x00\x12\x00\x00\x00\x01\x00\x00\x03\xe8\x00\x00\x00\x64\xff\xff\xff\x9c\x00\x12\x00\x00\x00\x02\x00\x00\x01\xf4\xff\xff\xff\xb0\xff\xff\xd8\xf1\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 1928);
	memcpy(shumiaoyunshu, "\x00\x00\xea\x60\x00\x00\x00\x6e\x00\x00\x00\xc7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\xdf\x20\xd2\xed\xf9\x00\x02\x00\x02\xbf\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 98);
	//memcpy(chewangkaoyan, "\x00\x00\xea\x60\x00\x00\x00\x48\x00\x00\x00\x71\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x76\x7a\x7b\x38\xf9\x00\x03\x00\x00\xd6\xd8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x07\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",98);
	memcpy(taskaward, "\x00\x00\x27\x11\x00\x13\x00\x00\x00\x46\x01\x00\x00\x00\x00\x00\x00\x00\x0d\x00\x00\x00\x00\x00\x00\x73\x33\x00\x5b\x00\x00\x01\x90\x00\x00\xcd\x69\x00\x00\x00\x00\x00\x0F\xFF\xFF\xFF\x00\x00\x64\x00\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03\xfc\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 127);
	memcpy(StartTimerChallenge, "\x00\x00\x1f\x40\x00\x00\x01\x58\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06\xe0\x68\x45\xe0\x16\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x27\x11\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\xff\xff\xff\xff\x00\x01\x00\x0b\x00\x00\x27\x11\x00\x00\x68\x2e\x00\x00\x00\x00\x00\x32\x00\x00\x00\x01\x64\x69\xa9\xa7\x04\x00\x89\x03\x25\x01\x01\x64\x69\xa9\xa7\x22\x22\x5a\xbe\x01\x01\x91\x8f\x12\xa1\x16\x72\x36\xcb\xae\x15\x41\x6b\x67\x98\x20\x42\xe4\x91\xe9\x21\xe8\xe8\x62\x58\x85\x00\x00\x00\x00\x04\xff\xff\x00\x00\x00\xc8\x00\x00\x00\xc8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7c\xbf\xf1\x01\x00\x1d\x00\x00\x27\x11\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x05\xbe\xaa\xaa\xe3\xcb\x3a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0a\x00\x0b\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03\x00\x37\x00\x00\x02\x00\x19\x00\x00\x05\x00\x00\x02\x58\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\x0b\xb8\x00\x19\x00\x00\x05\x00\x00\x01\x2c\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00\x03\xe8\x00\x00\x03\xe8\x00\x3b\x00\x01\x02\x00\x19\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x32\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\x00\x00\x00\x1d\x00\x01\x06\x00\x00\x00\x00\x00\x00\x9c\x40\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\x27\x10\x00\x00\x00\x00\x00\x0a\x00\x00\x01\x00\x05\x00\x02\x00\x00\x00\x00\x80\x00\x00\x00\x00\x00\x00", 589);

	memcpy(jump24238, "\x00\x00\x27\x11\x00\x00\x00\x03\x00\x00\x00\x31\x00\x00\x00\x03\x00\x00\x27\x10\x00\x00\x79\x00\x00\x00\x00\x0a\x00\x00\x00\x0a\x00\x00\x00\x01\x00\x31\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x10\x02\x4a\x00\x00\x00\x01\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x61\x6c", 91);

	memcpy(StartTimerChallenge2, "\x00\x00\x1f\x40\x00\x00\x01\xaf\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x39\x52\xf3\x7d\xac\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x27\x11\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\xa1\x08\x00\x01\x00\x0b\x00\x00\x27\x11\x00\x00\x68\x2e\x00\x00\x00\x01\x0a\x32\x00\x00\x00\x00\x64\x72\x23\xfd\x65\x02\x30\x02\x2e\x01\x01\x64\x72\x23\xfd\x12\x8c\x17\x3e\x01\x01\x70\x4e\x24\xae\x45\xd2\xb2\xfa\x36\x7b\x4b\xe7\xe1\x03\x1e\xa8\x8e\xa1\xa6\x39\xfe\x08\xb3\x92\xe3\x00\x00\x00\x00\x04\xff\xff\x00\x00\x00\xc8\x00\x00\x00\xc8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x14\xaf\x5d\x01\x00\x35\x00\x00\x27\x11\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x01\x03\x00\x08\x00\xcc\x00\xcc\x00\x01\x00\x08\x00\xcf\x00\xcf\x00\x01\x00\x08\x00\xd2\x00\xd2\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x0a\xaa\x1d\xb2\x76\x6c\x86\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2e\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\xcb\x00\x20\x00\x00\x00\x01\x00\x16\x00\x00\x27\x11\x00\x00\x00\x03\x00\x00\x00\x0d\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x03\x00\x00\x27\x11\x00\x01\x00\x00\x00\x01\x03\x00\x08\x00\xcc\x00\x00\x00\x00\x00\x08\x00\xcf\x00\x00\x00\x00\x00\x08\x00\xd2\x00\x00\x00\x00\x00\x02\x00\x84\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\xc1\xd2\xd1\xe6\xb7\xc9\xd0\xd0\x00\x42\xc1\xf7\xbb\xf0\xc1\xfa\xbe\xa7\xb8\xb3\xd3\xe8\xc8\xfc\xb3\xb5\xb7\xc9\xd0\xd0\xb5\xc4\xc4\xdc\xc1\xa6\xa3\xac\xb3\xd6\xd0\xf8\xca\xb1\xbc\xe4\x35\xc3\xeb\xa3\xac\xc0\xe4\xc8\xb4\xca\xb1\xbc\xe4\x31\x35\xc3\xeb\xa3\xac\xca\xb9\xd3\xc3\xb4\xce\xca\xfd\x31\xb4\xce\x01\x00\x00\x00\x00\x00\x00\x00\x09\x00\x00\x00\x00\x00\x00\x00\x00\x13\x88\x00\x00\x00\x01\x63\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03\x00\x37\x00\x00\x02\x00\x19\x00\x00\x05\x00\x00\x02\x58\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\x0b\xb8\x00\x19\x00\x00\x05\x00\x00\x01\x2c\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00\x03\xe8\x00\x00\x03\xe8\x00\x3b\x00\x01\x02\x00\x19\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x32\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\x00\x00\x00\x1d\x00\x01\x06\x00\x00\x00\x00\x00\x00\x9c\x40\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\x27\x10\x00\x00\x00\x00\x00\x0a\x00\x00\x01\x00\x05\x00\x02\x00\x00\x00\x07\xe0\x00\x00\x00\x00\x00\x00", 816);
	//4级烈焰飞行 
	//技能时长00 00 1b 58 或者00 00 13 88 后面是冷却00 00 27 10 技能次数03

	memcpy(jump24255, "\x00\x00\x00\x03\x00\x00\x00\x77\x00\xFF\xFF\xFF\x00\x01\x0f\xe3\x00\x01\x0f\xe3\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x61\x6c", 91);


	memcpy(BorderOver, "\x00\xb1\x00\x00\x27\x11\x00\x02\xa2\x93\x00\x00\x00\x01\x00\x00\x00\x54\x00\x03\x02\x64\x00\x00\x01\xf4\x00\x00\x26\x56\x00\x00\x00\x64\x00\x00\x3d\x36\x00\x00\x00\x64\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x06\x00\x00\x00\x0f\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00\x00\x0f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x30\x17\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x01\x01\x0f\xa0\x00\x00\x06\x00\x00\x01\x01\x00\x00\x00\x00\x00\x00\x00\x42\x68\x00\x04\x00\x00\x00\x00", 187);
	memcpy(RankedOver, "\x01\x01\x79\x00\x00\x27\x11\x00\x01\xac\x54\x00\x00\x00\x00\x00\x00\x01\x90\x00\x01\x01\x70\x00\x00\x00\x9c\x00\x03\x14\x17\x02\x00\x00\x00\x05\x00\x00\x00\x07\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x90\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x26\x00\x00\x0b\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\xbb\x00\x16\x00\x00\x00\x16\x00\x00\x00\x04\x00\x00\x00\x13\x00\x00\x00\x28\x00\x00\x00\xa9\x00\x16\x00\x00\x00\x16\x00\x00\x00\x04\x00\x00\x00\x1d\x00\x00\x00\x28\x00\x00\x00\xb3\x00\x24\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x68\x00\x00\x00\x04\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x0a\x00\x00\x00\x17\x00\x00\x00\x04\x00\x00\x00\x06\x00\x00\x00\x26\x00\x00\x01\x2c\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x30\x17\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x01\x01\x00\x00\x00\x06\x00\x00\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x59\xd8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 402);
	memcpy(BorderOnePlayerInfo, "\x03\x25\x42\x59\xcb\x12\x44\xd8\x02\x0e\x00\x00\x00\x00\x00\x01\x23\xb2\xe2\xca\xd4\xd4\xb1\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x41\x02\x00\x00\x00\x04\xff\xff\xff\xff\x00\x0c\x1e\x1b\x04\x38\x06\x98\x00\x04\x78\xdf\x00\x01\xeb\xe4\x00\x00\xb3\xdc\x00\x13\xcc\xb9\x09\xf7\x37\x36\x09\xf7\x37\x36\x00\x00\x5b\xf4\x00\x00\x00\xf0\x00\x00\x00\x00\x0f\x74\x6a\xf9\x00\x01\x53\x18\x01\xcc\x80\xb1\x00\x25\x3d\xa6\x00\x00\x04\x1a\x00\x22\x4f\xc8\x00\x00\x05\xb4\x00\x09\x44\xcd\x00\x00\xa0\x17\x4b\x3e\x94\x22\xb5\xad\xcd\xcb\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x36\x1c\x00\x00\x03\x40\x00\x3d\x01\x00\x00\x08\x7f\x00\x05\x09\x6b\x00\x99\x02\x66\x04\x31\x0f\x51\xdd\x85\x03\x00\x06\xba\x33\x00\x00\x00\x00\x00\x00\x01\x29\xb1\x00\x0d\x7e\x05\x00\x00\x00\x02\x08\x00\x00\x03\x16\x00\x00\x01\x40\x00\x00\x00\x00\x00\x20\x22\x2d\x00\x00\x05\x7e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x60\x64\x9c\x7f\x00\x00\x07\x60\x00\x00\x00\x00\x00\x00\x00\x00\x00\x70\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x69\x00\x2e\xd3\xc5\xc3\xc0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x09\x00\x14\x8b\xd9\x00\x00\x62\xc8\x00\x00\x00\x00\x00\x03\x96\x59\xc3\xf7\xc3\xc4\x00\x00\x00\x00\x00\x00\x01\x00\x19\x00\x01\xb0\x76\x00\x00\x00\x01\x00\x00\x10\xe0\x01\x64\x71\xbe\x17\x40\x00\x00\x00\x00\x00\x00\x01\x00\x57\x42\x59\xcb\x12\x00\x01\xed\xdd\xb0\xac\xc0\xf6\xc9\xaf\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x1e\x04\x00\x03\x00\x0a\x00\x07\x01\x01\x00\x00\x00\x46\x00\x0a\x00\x01\x02\x01\x00\x00\x00\xc8\x00\x0a\x00\x02\x03\x01\x00\x00\x02\xee\x00\x00\x00\x05\x03\x00\x07\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x01\x07\x00\x00\x0f\x00\x34\xf7\x23\x07\x00\x00\xd2\x52\x00\x00\x00\xb4\x00\x00\x07\x01\x00\x00\x9a\x42\x00\x00\x00\x01\x01\x00\x0f\x03\x00\x00\x03\xfd\x00\x00\x0a\x0f\x00\x00\x00\x07\x01\x00\x1b\x00\x01\xbd\x86\xd3\xc5\xd1\xc5\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x06\x02\x00\x01\x00\x08\x01\x00\x00\x91\x6e\x0a\x00\x00\x00\x12\x00\x00\x00\x04\x00\x00\x00\x32\x00\x00\x00\x81\x00\x00\x00\x00\x00\x00\x35\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0c\x01\x00\x00\x7b\xc4\x00\x00\x00\xb4\x0a\x00\x00\x00\x12\x00\x00\x00\x3b\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x64\x00\x00\x00\x00\x00\x0b\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x35\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x16\x00\x00\x00\x1a\x00\x00\x00\x01\x00\x00\x00\x96\x00\x01\x86\x9f\x00\x00\x05\x69\x00\x00\x00\x00\x00\x00\x01\x00\x07\x01\x00\x04\x06\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 805);

}
void InitDB()
{
	sqlite3_config(SQLITE_CONFIG_SERIALIZED);
	sqlite3_initialize();

	// 打开数据库
	sqlite3_open_v2("./db/User.db", &UserDB, SQLITE_OPEN_READWRITE, NULL);
	sqlite3_open_v2("./db/Player.db", &PlayerDB, SQLITE_OPEN_READWRITE, NULL);
}

void CloseDB()
{
	const char *sql = NULL;
	int result;

	sql = "VACUUM";
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);
	sqlite3_close(PlayerDB);

	sql = "VACUUM";
	result = sqlite3_exec(UserDB, sql, NULL, NULL, NULL);
	sqlite3_close(UserDB);

	sqlite3_shutdown();
}
void UserDB_ConnID(DWORD Uin, DWORD ConnID)
{//Lupin
	
	char Query[MAX_PATH] = "";

	//IsLogin - Account Check
	sprintf_s(Query, sizeof(Query), "UPDATE User SET ConnID=%d WHERE Uin=%d;", ConnID, Uin - 10000);
	sqlite3_exec(UserDB, Query, NULL, NULL, NULL);

}
void PlayDB_ClearDress(UINT Uin, UINT SuitIdx,UINT GenderType)
{//Lupin
	
	char Query[MAX_PATH] = "";

	//IsLogin - Account Check
	sprintf_s(Query, sizeof(Query), "DELETE FROM DressInfo WHERE Uin=%d AND SuitIdx=%d AND GenderType =%d;", Uin, SuitIdx, GenderType);
	sqlite3_exec(PlayerDB, Query, NULL, NULL, NULL);
}
void PlayDB_SaveDress(UINT Uin, UINT SuitIdx, UINT ItemID, UINT GenderType)
{//Lupin
	
	char Query[MAX_PATH] = "";

	//IsLogin - Account Check
	sprintf_s(Query, sizeof(Query), "INSERT INTO DressInfo(Uin,SuitIdx,ItemID,GenderType) VALUES(%d, %d, %d, %d);", Uin, SuitIdx, ItemID, GenderType);
	sqlite3_exec(PlayerDB, Query, NULL, NULL, NULL);
}
void PlayDB_UpdateItemStatus(UINT Uin,UINT ItemID, UINT Status)
{//Lupin
	
	char Query[MAX_PATH] = "";

	//IsLogin - Account Check
	sprintf_s(Query, sizeof(Query), "UPDATE ITEM SET STATUS =%d WHERE ItemID = %d AND Uin=%d", Status, ItemID, Uin);
	sqlite3_exec(PlayerDB, Query, NULL, NULL, NULL);
}

BOOL UserDB_Register(const char *Username, const char *Password)
{

	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;

	sql = "INSERT INTO User (Name, Password) VALUES(?, ?)";
	result = sqlite3_prepare_v2(UserDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_text(stmt, 1, Username, strlen(Username), SQLITE_TRANSIENT);
		sqlite3_bind_blob(stmt, 2, Password, strlen(Password), SQLITE_TRANSIENT);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	return result == SQLITE_DONE;
}

DWORD UserDB_GetUin(const char *Username)
{
	
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	int uin = 0;

	sql = "SELECT Uin, Password  FROM User  WHERE Name=?";
	result = sqlite3_prepare_v2(UserDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_text(stmt, 1, Username, strlen(Username), SQLITE_TRANSIENT);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			uin = sqlite3_column_int(stmt, 0);
			uin += 10000; // 必须大于10000,否则会出问题
		}
		else
		{
			// 未注册
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (uin > 10000) {
		
		sql = "SELECT Uin,hasLogin  FROM User  WHERE Uin = ? AND hasLogin=1";
		result = sqlite3_prepare_v2(UserDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, uin - 10000);
			uin = -1;
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				if (sqlite3_column_int(stmt, 1)==1) {
					uin = sqlite3_column_int(stmt, 0);
					uin += 10000; // 必须大于10000,否则会出问题
				}
				
			}
			else
			{
				// 未注册
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	else {
		uin = -1;
	}
	return uin;
}

BOOL UserDB_CmpPassword(DWORD Uin, const char *Password)
{
	
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	BOOL ret = FALSE;

	sql = "SELECT Password  FROM User  WHERE Uin=?;";
	result = sqlite3_prepare_v2(UserDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin - 10000);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			char *pwd = (char *)sqlite3_column_blob(stmt, 0);
			if (strcmp(pwd, Password) == 0)
			{
				ret = TRUE;
			}
		}
		else
		{
			// 未注册
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	return ret;
}

ItemInfo PlayerDB_AddItem(DWORD Uin, DWORD ItemID, DWORD ItemNum, int AvailPeriod, bool Status)
{
	Z_INFO("物品ID:%d,\n", ItemID);
	ItemInfo Item = {};
	bool exists = false;
	bool SetMysql = getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1;
	if (SetMysql)
	{
		
	}
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	if (!exists&& !SetMysql) {
		Item.ItemID = ItemID;
		sql = "SELECT ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute FROM Item WHERE Uin=? AND ItemID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, ItemID);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				Item.ItemNum = sqlite3_column_int(stmt, 0);
				Item.AvailPeriod = sqlite3_column_int(stmt, 1);
				Item.Status = sqlite3_column_int(stmt, 2);
				Item.ObtainTime = sqlite3_column_int(stmt, 3);
				Item.OtherAttribute = sqlite3_column_int(stmt, 4);
				Item.ItemType = 0;
				exists = true;
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	
	if (Item.AvailPeriod >= 1 && Item.AvailPeriod != -1)
	{ // 检查 AvailItem 是否 >= 1 且条件不是 -1
		int newAvailPeriod = AvailPeriod;
		if (AvailPeriod == -1)
		{
			newAvailPeriod = 0;
		}
		else if (Item.AvailPeriod > 1)
		{//减少数据库查询
			newAvailPeriod = Item.AvailPeriod;
		}
		if (SetMysql) {
			
		}
		else {
			sql = "UPDATE Item SET AvailPeriod=? WHERE Uin=? AND ItemID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, newAvailPeriod);
				sqlite3_bind_int(stmt, 2, Uin);
				sqlite3_bind_int(stmt, 3, ItemID);
				result = sqlite3_step(stmt);
			}
		}
	}
	if (exists)
	{
		if (Item.AvailPeriod == 0)
		{
			Item.ItemNum += ItemNum;
			if (SetMysql) {
				
			}
			else {
				// 增加数量
				sql = "UPDATE Item SET ItemNum=ItemNum+? WHERE Uin=? AND ItemID=?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, ItemNum);
					sqlite3_bind_int(stmt, 2, Uin);
					sqlite3_bind_int(stmt, 3, ItemID);
					sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
			
			
		}
		else if (Item.AvailPeriod > 0)
		{
			Item.AvailPeriod += AvailPeriod;
			if (SetMysql) {
				
			}
			else {
				// 增加有效期
				sql = "UPDATE Item SET AvailPeriod=AvailPeriod+? WHERE Uin=? AND ItemID=?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, AvailPeriod);
					sqlite3_bind_int(stmt, 2, Uin);
					sqlite3_bind_int(stmt, 3, ItemID);
					sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
			
			
		}
		else
		{
			// ResponseStartGame(GetClient(Uin),"重复获得物品");
			// 重复获取相同永久物品 暂不处理
		}
	}
	else
	{
		Item.ItemNum = ItemNum;
		Item.AvailPeriod = AvailPeriod;
		Item.Status = Status;
		Item.ObtainTime = (int)time(nullptr);
		Item.OtherAttribute = 0;
		Item.ItemType = 0;

		if (SetMysql) {
			
		}
		else {
			sql = "INSERT INTO Item (Uin,ItemID,ItemNum,AvailPeriod,Status) VALUES (?,?,?,?,?);";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Uin);
				sqlite3_bind_int(stmt, 2, ItemID);
				sqlite3_bind_int(stmt, 3, ItemNum);
				sqlite3_bind_int(stmt, 4, AvailPeriod);
				sqlite3_bind_int(stmt, 5, Status);
				result = sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		

		if (GetItemType(ItemID) == EAIT_CAR)
		{
			int ID = 0;
			if (SetMysql) {
				
			}
			else {
				sql = "INSERT INTO KartStoneGroove (Uin,KartID) VALUES (?,?);";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Uin);
					sqlite3_bind_int(stmt, 2, ItemID);
					result = sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;

				// 宝石信息
				
				sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Uin);
					sqlite3_bind_int(stmt, 2, ItemID);
					result = sqlite3_step(stmt);
					if (result == SQLITE_ROW)
					{
						ID = sqlite3_column_int(stmt, 0);
					}
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
			{//宝石默认信息
				loadKartStone(ItemID, ID, GetClient(Uin));
			}
			if (SetMysql) {
				
			}
			else {
				// 改装信息
				sql = "INSERT INTO KartRefit(Uin,KartID) VALUES (?,?);";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Uin);
					sqlite3_bind_int(stmt, 2, ItemID);
					result = sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
			
		}
		else if (GetItemType(ItemID) == EAIT_PET)
		{ // 宠物信息
			if (SetMysql) {
				
			}
			else {
				sql = "INSERT INTO Pet(Uin,PetId,Name,Experience,PL) VALUES (?,?,?,?,?);";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Uin);
					sqlite3_bind_int(stmt, 2, ItemID);
					sqlite3_bind_text(stmt, 3, "神宠", strlen("神宠"), SQLITE_TRANSIENT);
					sqlite3_bind_int(stmt, 4, 238966);
					sqlite3_bind_int(stmt, 5, 1);
					result = sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
			
		}
	}
	return Item;
}
ItemInfo PlayerDB_AddItem2(DWORD Uin, DWORD ItemID, DWORD ItemNum, int AvailPeriod, bool Status)
{
	if (GetItemType(ItemID) == EAIT_PROP)
	{
		ItemNum = 999;
		AvailPeriod = 0;
	}
	else if(GetItemType(ItemID) == EAIT_PET || GetItemType(ItemID) == EAIT_CAR|| GetItemType(ItemID) == EAIT_SKIN|| GetItemType(ItemID) == EAIT_CLOTHES)
	{
		ItemNum = 1;
		AvailPeriod = -1;
	}else
	{
		ItemNum = 1;
		AvailPeriod = 0;
		
	}
	Z_INFO("物品ID:%d,\n", ItemID);
	ItemInfo Item = {};
	bool exists = false;
	bool SetMysql = getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1;
	if (SetMysql)
	{
		
	}
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	if (!exists && !SetMysql) {
		Item.ItemID = ItemID;
		sql = "SELECT ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute FROM Item WHERE Uin=? AND ItemID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, ItemID);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				Item.ItemNum = sqlite3_column_int(stmt, 0);
				Item.AvailPeriod = sqlite3_column_int(stmt, 1);
				Item.Status = sqlite3_column_int(stmt, 2);
				Item.ObtainTime = sqlite3_column_int(stmt, 3);
				Item.OtherAttribute = sqlite3_column_int(stmt, 4);
				Item.ItemType = 0;
				exists = true;
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}

	if (Item.AvailPeriod >= 1 && Item.AvailPeriod != -1)
	{ // 检查 AvailItem 是否 >= 1 且条件不是 -1
		int newAvailPeriod = AvailPeriod;
		if (AvailPeriod == -1)
		{
			newAvailPeriod = 0;
		}
		else if (Item.AvailPeriod > 1)
		{//减少数据库查询
			newAvailPeriod = Item.AvailPeriod;
		}
		if (SetMysql) {
			
		}
		else {
			sql = "UPDATE Item SET AvailPeriod=? WHERE Uin=? AND ItemID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, newAvailPeriod);
				sqlite3_bind_int(stmt, 2, Uin);
				sqlite3_bind_int(stmt, 3, ItemID);
				result = sqlite3_step(stmt);
			}
		}
	}
	if (exists)
	{
		if (Item.AvailPeriod == 0)
		{
			Item.ItemNum += ItemNum;
			if (SetMysql) {
				
			}
			else {
				// 增加数量
				sql = "UPDATE Item SET ItemNum=ItemNum+? WHERE Uin=? AND ItemID=?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, ItemNum);
					sqlite3_bind_int(stmt, 2, Uin);
					sqlite3_bind_int(stmt, 3, ItemID);
					sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}


		}
		else if (Item.AvailPeriod > 0)
		{
			Item.AvailPeriod += AvailPeriod;
			if (SetMysql) {
				
			}
			else {
				// 增加有效期
				sql = "UPDATE Item SET AvailPeriod=AvailPeriod+? WHERE Uin=? AND ItemID=?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, AvailPeriod);
					sqlite3_bind_int(stmt, 2, Uin);
					sqlite3_bind_int(stmt, 3, ItemID);
					sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}


		}
		else
		{
			// ResponseStartGame(GetClient(Uin),"重复获得物品");
			// 重复获取相同永久物品 暂不处理
		}
	}
	else
	{
		Item.ItemNum = ItemNum;
		Item.AvailPeriod = AvailPeriod;
		Item.Status = Status;
		Item.ObtainTime = (int)time(nullptr);
		Item.OtherAttribute = 0;
		Item.ItemType = 0;

		if (SetMysql) {
			
		}
		else {
			sql = "INSERT INTO Item (Uin,ItemID,ItemNum,AvailPeriod,Status) VALUES (?,?,?,?,?);";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Uin);
				sqlite3_bind_int(stmt, 2, ItemID);
				sqlite3_bind_int(stmt, 3, ItemNum);
				sqlite3_bind_int(stmt, 4, AvailPeriod);
				sqlite3_bind_int(stmt, 5, Status);
				result = sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}


		if (GetItemType(ItemID) == EAIT_CAR)
		{
			int ID = 0;
			if (SetMysql) {
				
			}
			else {
				sql = "INSERT INTO KartStoneGroove (Uin,KartID) VALUES (?,?);";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Uin);
					sqlite3_bind_int(stmt, 2, ItemID);
					result = sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;

				// 宝石信息

				sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Uin);
					sqlite3_bind_int(stmt, 2, ItemID);
					result = sqlite3_step(stmt);
					if (result == SQLITE_ROW)
					{
						ID = sqlite3_column_int(stmt, 0);
					}
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
			{//宝石默认信息
				loadKartStone(ItemID, ID, GetClient(Uin));
			}
			if (SetMysql) {
				
			}
			else {
				// 改装信息
				sql = "INSERT INTO KartRefit(Uin,KartID) VALUES (?,?);";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Uin);
					sqlite3_bind_int(stmt, 2, ItemID);
					result = sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}

		}
		else if (GetItemType(ItemID) == EAIT_PET)
		{ // 宠物信息
			if (SetMysql) {
				
			}
			else {
				sql = "INSERT INTO Pet(Uin,PetId,Name,Experience,PL) VALUES (?,?,?,?,?);";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Uin);
					sqlite3_bind_int(stmt, 2, ItemID);
					sqlite3_bind_text(stmt, 3, "神宠", strlen("神宠"), SQLITE_TRANSIENT);
					sqlite3_bind_int(stmt, 4, 238966);
					sqlite3_bind_int(stmt, 5, 1);
					result = sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
			
		}
	}
	return Item;
}

DWORD PlayerDB_GetMapRecord(DWORD Uin, int MapID)
{
	
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;

	int ret = -1;
	sql = "SELECT Record  FROM MapRecord  WHERE Uin=? AND MapID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, MapID);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			ret = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	return ret;
}

bool PlayerDB_SetMapRecord(DWORD Uin, int MapID, DWORD Record)
{
	
	bool NewRecord = true;
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;

	sql = "SELECT Record  FROM MapRecord  WHERE Uin=? AND MapID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, MapID);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			if ((DWORD)sqlite3_column_int(stmt, 0) < Record)
			{
				NewRecord = false;
			}
			else
			{
				sqlite3_finalize(stmt);
				stmt = NULL;
				sql = "UPDATE MapRecord SET Record=?  WHERE Uin=? AND MapID=?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Record);
					sqlite3_bind_int(stmt, 2, Uin);
					sqlite3_bind_int(stmt, 3, MapID);
					result = sqlite3_step(stmt);
				}
			}
		}
		else
		{
			sqlite3_finalize(stmt);
			stmt = NULL;
			sql = "INSERT INTO MapRecord (Uin, MapID, Record) VALUES(?, ?, ?)";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Uin);
				sqlite3_bind_int(stmt, 2, MapID);
				sqlite3_bind_int(stmt, 3, Record);
				result = sqlite3_step(stmt);
			}
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	return NewRecord;
}

void PlayerDB_GetCharacterInfo(ClientNode *Client)
{
	sqlite3_stmt *stmt = NULL;

	if (sqlite3_prepare_v2(PlayerDB, "SELECT NickName,Gender,Country,License,Experience,SuperMoney,Money,WinNum,SecondNum,ThirdNum,TotalNum,CurHonor,TotalHonor,TodayHonor,RelaxTime,MonthDurationBefor,MonthDurationCur,Charm,DurationGame,DanceExp,Coupons,Admiration,LuckMoney,TeamWorkExp,AchievePoint,RegTime,Signature,AppellationType,NobleID FROM BaseInfo WHERE Uin = ?; ", -1, &stmt, NULL) == SQLITE_OK)
	{

		sqlite3_bind_int(stmt, 1, Client->Uin);

		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			char szCharName[MaxNickName] = "";
			memcpy(szCharName, (char *)sqlite3_column_text(stmt, 0), MaxNickName);
			strcpy_s(Client->NickName, szCharName);

			Client->Gender = sqlite3_column_int(stmt, 1);
			Client->Country = sqlite3_column_int(stmt, 2);
			Client->License = sqlite3_column_int(stmt, 3);
			Client->Experience = sqlite3_column_int(stmt, 4);
			Client->SuperMoney = sqlite3_column_int(stmt, 5);
			Client->Money = sqlite3_column_int(stmt, 6);
			Client->WinNum = sqlite3_column_int(stmt, 7);
			Client->SecondNum = sqlite3_column_int(stmt, 8);
			Client->ThirdNum = sqlite3_column_int(stmt, 9);
			Client->TotalNum = sqlite3_column_int(stmt, 10);
			Client->CurHonor = sqlite3_column_int(stmt, 11);
			Client->TotalHonor = sqlite3_column_int(stmt, 12);
			Client->TodayHonor = sqlite3_column_int(stmt, 13);
			Client->RelaxTime = sqlite3_column_int(stmt, 14);
			Client->MonthDurationBefor = sqlite3_column_int(stmt, 15);
			Client->MonthDurationCur = sqlite3_column_int(stmt, 16);
			Client->Charm = sqlite3_column_int(stmt, 17);
			Client->DurationGame = sqlite3_column_int(stmt, 18);
			Client->DanceExp = sqlite3_column_int(stmt, 19);
			Client->Coupons = sqlite3_column_int(stmt, 20);
			Client->Admiration = sqlite3_column_int(stmt, 21);
			Client->LuckMoney = sqlite3_column_int(stmt, 22);
			Client->TeamWorkExp = sqlite3_column_int(stmt, 23);
			Client->AchievePoint = sqlite3_column_int(stmt, 24);
			Client->RegTime = sqlite3_column_int(stmt, 25);

			char Signature[MaxSignature] = "";
			memcpy(Signature, (char *)sqlite3_column_text(stmt, 26), MaxSignature);
			strcpy_s(Client->Signature, Signature);
			Client->AppellationType = sqlite3_column_int(stmt, 27);
			Client->NobleID = sqlite3_column_int(stmt, 28);
		}
	}
}

void PlayerDB_UpdateCharacterInfo(ClientNode *Client)
{
	
	sqlite3_stmt *stmt = NULL;

	if (sqlite3_prepare_v2(PlayerDB, "UPDATE BaseInfo SET NickName=?,Gender=?,Country=?,License=?,Experience=?,SuperMoney=?,Money=?,WinNum=?,SecondNum=?,ThirdNum=?,TotalNum=?,CurHonor=?,TotalHonor=?,TodayHonor=?,RelaxTime=?,MonthDurationBefor=?,MonthDurationCur=?,Charm=?,DurationGame=?,DanceExp=?,Coupons=?,Admiration=?,LuckMoney=?,TeamWorkExp=?,AchievePoint=?,RegTime=?,Signature=? WHERE Uin = ?; ", -1, &stmt, NULL) == SQLITE_OK)
	{

		sqlite3_bind_text(stmt, 1, Client->NickName, strnlen_s(Client->NickName, MaxNickName), SQLITE_TRANSIENT);
		sqlite3_bind_int(stmt, 2, Client->Gender);
		sqlite3_bind_int(stmt, 3, Client->Country);
		sqlite3_bind_int(stmt, 4, Client->License);
		sqlite3_bind_int(stmt, 5, Client->Experience);
		sqlite3_bind_int(stmt, 6, Client->SuperMoney);
		sqlite3_bind_int(stmt, 7, Client->Money);
		sqlite3_bind_int(stmt, 8, Client->WinNum);
		sqlite3_bind_int(stmt, 9, Client->SecondNum);
		sqlite3_bind_int(stmt, 10, Client->ThirdNum);
		sqlite3_bind_int(stmt, 11, Client->TotalNum);
		sqlite3_bind_int(stmt, 12, Client->CurHonor);
		sqlite3_bind_int(stmt, 13, Client->TodayHonor);
		sqlite3_bind_int(stmt, 14, Client->TotalHonor);
		sqlite3_bind_int(stmt, 15, Client->RelaxTime);
		sqlite3_bind_int(stmt, 16, Client->MonthDurationBefor);
		sqlite3_bind_int(stmt, 17, Client->MonthDurationCur);
		sqlite3_bind_int(stmt, 18, Client->Charm);
		sqlite3_bind_int(stmt, 19, Client->DurationGame);
		sqlite3_bind_int(stmt, 20, Client->DanceExp);
		sqlite3_bind_int(stmt, 21, Client->Coupons);
		sqlite3_bind_int(stmt, 22, Client->Admiration);
		sqlite3_bind_int(stmt, 23, Client->LuckMoney);
		sqlite3_bind_int(stmt, 24, Client->TeamWorkExp);
		sqlite3_bind_int(stmt, 25, Client->AchievePoint);
		sqlite3_bind_int(stmt, 26, Client->RegTime);
		sqlite3_bind_text(stmt, 27, Client->Signature, strnlen_s(Client->Signature, MaxSignature), SQLITE_TRANSIENT);
		sqlite3_bind_int(stmt, 28, Client->Uin);

		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}
	else
	{
#ifdef DEBUG
		Z_ERROR("更新角色信息错误\n");
#endif // DEBUG
	}
}

void PlayerDB_UpdateMoneyExp(ClientNode *Client)
{
	
	sqlite3_stmt *stmt = NULL;

	if (sqlite3_prepare_v2(PlayerDB, "UPDATE BaseInfo SET Experience=?,SuperMoney=?,Money=?,Charm=?,DanceExp=?,Coupons=?,Admiration=?,LuckMoney=?,TeamWorkExp=?,AchievePoint=? WHERE Uin = ?; ", -1, &stmt, NULL) == SQLITE_OK)
	{

		sqlite3_bind_int(stmt, 1, Client->Experience);
		sqlite3_bind_int(stmt, 2, Client->SuperMoney);
		sqlite3_bind_int(stmt, 3, Client->Money);
		sqlite3_bind_int(stmt, 4, Client->Charm);
		sqlite3_bind_int(stmt, 5, Client->DanceExp);
		sqlite3_bind_int(stmt, 6, Client->Coupons);
		sqlite3_bind_int(stmt, 7, Client->Admiration);
		sqlite3_bind_int(stmt, 8, Client->LuckMoney);
		sqlite3_bind_int(stmt, 9, Client->TeamWorkExp);
		sqlite3_bind_int(stmt, 10, Client->AchievePoint);
		sqlite3_bind_int(stmt, 11, Client->Uin);

		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}
	else
	{
#ifdef DEBUG
		Z_ERROR("更新金币经验错误\n");
#endif
	}
}
void PlayerDB_AddStoneGroove(ClientNode *Client, int KartID, int StoneGrooveType)
{
	int ID = -1;
	int StoneGrooveID = -1;
	bool SetMysql = false;
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
		
	}
	else {
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;


		sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, KartID);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				ID = sqlite3_column_int(stmt, 0);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		if (ID == -1)
		{
			return;
		}
		else
		{
			sql = "SELECT StoneGrooveID FROM KartStone WHERE ID = ? ORDER BY StoneGrooveID ASC;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, ID);
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{ // StoneGrooveInfo
					StoneGrooveID = sqlite3_column_int(stmt, 0);
				}
			}
			sqlite3_finalize(stmt);
			stmt = NULL;

			if (StoneGrooveID > -1)
			{
				StoneGrooveID++;
				sql = "INSERT INTO KartStone (ID,StoneGrooveID,StoneUseOccaType) VALUES (?,?,?);";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, ID);
					sqlite3_bind_int(stmt, 2, StoneGrooveID);
					sqlite3_bind_int(stmt, 3, StoneGrooveType);
					result = sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
		}
	}
	
}
int PlayerDB_RemoveSkillStone(ClientNode *Client, int KartID, int StoneGrooveID)
{
	bool SetMysql = false;
	int ID = -1;
	int SkillStoneID = -1;
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
		
	}
	else {
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;


		sql = "SELECT ID FROM KartStoneGroove WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, KartID);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				ID = sqlite3_column_int(stmt, 0);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		if (ID == -1)
		{
			return -1;
		}
		else
		{
			sql = "SELECT SkillStoneID FROM KartStone WHERE ID=? AND StoneGrooveID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, ID);
				sqlite3_bind_int(stmt, 2, StoneGrooveID);
				result = sqlite3_step(stmt);
				if (result == SQLITE_ROW)
				{
					SkillStoneID = sqlite3_column_int(stmt, 0);
				}
			}
			if (SkillStoneID > -1)
			{
				sqlite3_finalize(stmt);
				stmt = NULL;
				sql = "UPDATE KartStone SET SkillStoneID=0 WHERE ID=? AND StoneGrooveID=?";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, ID);
					sqlite3_bind_int(stmt, 2, StoneGrooveID);
					sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
		}
	}
	
	return SkillStoneID;
}
BOOL PlayerDB_DeleteItem(DWORD Uin, int ItemID)
{
	if (ItemID == 10020)
	{
		return FALSE;
	}
	
	sqlite3_stmt *stmt = NULL;
	BOOL result = FALSE;

	if (sqlite3_prepare_v2(PlayerDB, "DELETE FROM Item  WHERE  Uin=? AND ItemID = ?;", -1, &stmt, NULL) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, ItemID);

		if (sqlite3_step(stmt) == SQLITE_DONE)
		{
			result = TRUE;
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (result == TRUE&&GetItemType(ItemID)==EAIT_CAR) {
		//删除改装信息
		if (sqlite3_prepare_v2(PlayerDB, "DELETE FROM KartRefit  WHERE  Uin=? AND KartID = ?;", -1, &stmt, NULL) == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, ItemID);

			if (sqlite3_step(stmt) == SQLITE_DONE)
			{
				result = TRUE;
			}
		}
		//删除宝石信息
		if (sqlite3_prepare_v2(PlayerDB, "DELETE FROM KartStoneGroove  WHERE  Uin=? AND KartID = ?;", -1, &stmt, NULL) == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, ItemID);

			if (sqlite3_step(stmt) == SQLITE_DONE)
			{
				result = TRUE;
			}
		}
		if (sqlite3_prepare_v2(PlayerDB, "DELETE FROM KartStone  WHERE ID in(select id from  KartStoneGroove where uin = ? and kartid =?);", -1, &stmt, NULL) == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, ItemID);

			if (sqlite3_step(stmt) == SQLITE_DONE)
			{
				result = TRUE;
			}
		}
	}
	return result;
}
int PlayerDB_GetItemNum(DWORD Uin, DWORD ItemID)
{
	
	sqlite3_stmt *stmt = NULL;
	int ItemNum = 0;

	if (sqlite3_prepare_v2(PlayerDB, "SELECT ItemNum FROM Item WHERE Uin=? AND ItemID=? And AvailPeriod=0 ;", -1, &stmt, NULL) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, ItemID);

		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			ItemNum = sqlite3_column_int(stmt, 0);
			// Row && ItemNum = 0
			if (ItemNum == 0)
				PlayerDB_DeleteItem(Uin, ItemID);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	return ItemNum;
}
int PlayerDB_GetAvailPeriod(DWORD Uin, DWORD ItemID)
{
	
	sqlite3_stmt *stmt = NULL;
	int AvailPeriod = 0;

	if (sqlite3_prepare_v2(PlayerDB, "SELECT AvailPeriod FROM Item WHERE Uin=? AND ItemID=? And ItemNum=1 ;", -1, &stmt, NULL) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, ItemID);

		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			AvailPeriod = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	return AvailPeriod;
}
int PlayerDB_UseItem(DWORD Uin, DWORD ItemID, int UseCount)
{
	int ItemNum = PlayerDB_GetItemNum(Uin, ItemID);

	ItemNum -= UseCount;
	
	sqlite3_stmt *stmt = NULL;
	if (ItemNum > 0)
	{ // Update Item
		if (sqlite3_prepare_v2(PlayerDB, "UPDATE Item SET ItemNum=? WHERE Uin = ? AND ItemID = ?;", -1, &stmt, NULL) == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, ItemNum);
			sqlite3_bind_int(stmt, 2, Uin);
			sqlite3_bind_int(stmt, 3, ItemID);
			sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	else
	{
		PlayerDB_DeleteItem(Uin, ItemID);
		ItemNum = 0;
	}

	return ItemNum;
}
ItemInfo PlayerDB_GetItemInfo(DWORD Uin, UINT ItemID)
{
	
	
	sqlite3_stmt *stmt = NULL;

	ItemInfo Item{};

	if (sqlite3_prepare_v2(PlayerDB, "SELECT ItemID,ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute  FROM Item WHERE Uin = ? And ItemID = ? And IsWareHouse = 0;", -1, &stmt, NULL) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, ItemID);

		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			Item.ItemID = sqlite3_column_int(stmt, 0);
			Item.ItemNum = sqlite3_column_int(stmt, 1);
			Item.AvailPeriod = sqlite3_column_int(stmt, 2);
			Item.Status = sqlite3_column_int(stmt, 3);
			Item.ObtainTime = sqlite3_column_int(stmt, 4);
			Item.OtherAttribute = sqlite3_column_int(stmt, 5);
			Item.ItemType = 0;
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	return Item;
}
void PlayerDB_DoWareHouseOperation(UINT Uin, DWORD ItemID, int OpType)
{
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
		if (OpType == 0)
		{
		}
		else
		{
		}
		
	}
	else {
		const char* sql;

		if (OpType == 0)
		{
			sql = "UPDATE Item SET IsWareHouse=1 WHERE Uin=? AND ItemID=?;";
		}
		else
		{
			sql = "UPDATE Item SET IsWareHouse=0 WHERE Uin=? AND ItemID=?;";
		}

		sqlite3_stmt* stmt = NULL;
		if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL) == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, ItemID);
			sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	
}
BOOL PlayerDB_CheckNickNameUse(DWORD Uin, const char *Nickname)
{
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
	}
	sqlite3_stmt *stmt = NULL;
	BOOL ret = FALSE;

	if (sqlite3_prepare_v2(PlayerDB, "Select * From BaseInfo Where NickName = ?", -1, &stmt, NULL) == SQLITE_OK)
	{
		sqlite3_bind_text(stmt, 1, Nickname, strlen(Nickname), SQLITE_TRANSIENT);

		if (sqlite3_step(stmt) != SQLITE_ROW)
		{
			ret = TRUE;
		}
	}

	sqlite3_finalize(stmt);
	stmt = NULL;

	return ret;
}
void loadNotKartStone(UINT ItemID, UINT ID) {
	
	for (int StoneGrooveID = 1; StoneGrooveID <= 5; StoneGrooveID++)
	{
		if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
		{
			if (StoneGrooveID > 3) {
			}
			else {
			}
			
		}
		else {
			const char* sql = NULL;
			sqlite3_stmt* stmt = NULL;
			int result;
			sql = "INSERT INTO KartStone (ID,StoneGrooveID,StoneUseOccaType) VALUES (?,?,?);";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, ID);
				sqlite3_bind_int(stmt, 2, StoneGrooveID);
				if (StoneGrooveID > 3) {
					sqlite3_bind_int(stmt, 3, 1); // 三角
				}
				else {
					sqlite3_bind_int(stmt, 3, 0); // 圆形
				}

				result = sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		
	}
}
void loadPrevKartStone(UINT ItemID, UINT ID) {
	
	KartItem* KartItem = GetKartItem(ItemID);
	if (!KartItem)
	{
		loadDefaultKartStone(ItemID, ID);
		return;
	}
	else if (KartItem->GrooveInfoCount==0) {
		loadDefaultKartStone(ItemID, ID);
		return;
	}
	for (int StoneGrooveID = 0; StoneGrooveID < KartItem->GrooveInfoCount; StoneGrooveID++)
	{
		if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
		{
		}
		else {
			const char* sql = NULL;
			sqlite3_stmt* stmt = NULL;
			int result;
			sql = "INSERT INTO KartStone (ID,StoneGrooveID,StoneUseOccaType) VALUES (?,?,?);";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, ID);
				sqlite3_bind_int(stmt, 2, KartItem->GrooveInfo[StoneGrooveID].ID);
				sqlite3_bind_int(stmt, 3, KartItem->GrooveInfo[StoneGrooveID].Type); // 圆形
				result = sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		
	}
}
void loadDefaultKartStone(UINT ItemID, UINT ID) {
	
	char FilePath[MAX_PATH];
	sprintf_s(FilePath, ".\\Kart\\%d.yml", ItemID);
	bool isFileExists;
	struct stat buffer;
	isFileExists = (stat(FilePath, &buffer) == 0);
	if (!isFileExists)
	{
		loadNotKartStone(ItemID, ID);
		return;
	}
	else {
		YAML::Node Itemconfig = YAML::LoadFile(FilePath);
		const YAML::Node GrooveInfo = Itemconfig["GrooveInfo"];
		if (GrooveInfo)
		{
			UINT GrooveInfoCount = (BYTE)GrooveInfo.size();
			for (size_t i = 0; i < GrooveInfoCount; i++)
			{
				if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
				{
				}
				else {
					const char* sql = NULL;
					sqlite3_stmt* stmt = NULL;
					int result;
					sql = "INSERT INTO KartStone (ID,StoneGrooveID,StoneUseOccaType) VALUES (?,?,?);";
					result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
					if (result == SQLITE_OK)
					{
						sqlite3_bind_int(stmt, 1, ID);
						sqlite3_bind_int(stmt, 2, GrooveInfo[i]["ID"].as<UINT>());
						sqlite3_bind_int(stmt, 3, GrooveInfo[i]["Type"].as<UINT>()); // 圆形
						result = sqlite3_step(stmt);
					}
					sqlite3_finalize(stmt);
					stmt = NULL;
				}
				
			}
		}
		else
		{
			loadNotKartStone(ItemID, ID);
		}
	}
}
void loadKartStone(UINT ItemID,UINT ID,ClientNode* Client) {

	if (getGameConfig()["PreWriteKartPhysParam"] && getGameConfig()["PreWriteKartPhysParam"].as<UINT>() == 1)
	{//提前加载模式
		loadPrevKartStone(ItemID,ID);
	}
	else {//默认加载模式
		loadDefaultKartStone(ItemID, ID);
	}
	if (Client) {
		if (getGameConfig()["Global"]["SkillStoneUin"])
		{
			INT Uin = getGameConfig()["Global"]["SkillStoneUin"].as<INT>();
			if (Uin == -1)
			{
				NotifyUpdateKartSkillStoneInfo3(Client, ItemID, ItemID);
			}
			else if (Client->Uin == Uin)
			{
				NotifyUpdateKartSkillStoneInfo3(Client, ItemID, ItemID);
			}
			else {
				NotifyUpdateKartSkillStoneInfo2(Client, ItemID, ItemID);
			}
		}
		else {
			NotifyUpdateKartSkillStoneInfo2(Client, ItemID, ItemID);
		}
	}

}

UINT PlayerDB_GetItemNum1(UINT Uin, UINT ItemID)
{//查询道具
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	UINT num = 0;

	sql = "SELECT ItemNum FROM Item WHERE Uin=? and ItemID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, ItemID);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			num = sqlite3_column_int(stmt, 0);
		}
		else
		{
			//没有这个道具
			num = 0;
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	return num;
}
void PlayerDB_DelItem(UINT Uin, UINT ItemID)
{
	sqlite3_stmt* stmt = NULL;
	if (sqlite3_prepare_v2(PlayerDB, "DELETE FROM Item  WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, ItemID);
		sqlite3_step(stmt);
	}

	sqlite3_finalize(stmt);
	stmt = NULL;
}

void PlayerDB_Reduce(UINT Uin, UINT ItemID, int Num)
{
	UINT ItemNum = PlayerDB_GetItemNum(Uin, ItemID);
	if (ItemNum == Num) {
		PlayerDB_DelItem(Uin, ItemID);
		return;
	}
	sqlite3_stmt* stmt = NULL;
	if (sqlite3_prepare_v2(PlayerDB, "UPDATE Item SET ItemNum=ItemNum-? WHERE Uin = ? AND ItemID = ?;", -1, &stmt, NULL) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Num);
		sqlite3_bind_int(stmt, 2, Uin);
		sqlite3_bind_int(stmt, 3, ItemID);
		sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
}