#include "../types.h"
#include "save_data.h"

extern unsigned char D_801D3200[],gDuel_awPlayerDeck[],D_80010384[];extern void Util_CopyWords(void*,void*,int);extern void func_8003D03C(void*);extern void func_8003F758(void*,int,void*,int);
void SaveData_RequestWrite(void){Util_CopyWords(D_801D3200,gDuel_awPlayerDeck,SAVE_DATA_STATE_SIZE);func_8003D03C(D_801D3200-0x200);func_8003F758(D_801D3200,0xD00,D_80010384,2);}
