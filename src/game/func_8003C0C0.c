#include "../types.h"
#include "campaign_flags.h"
#include "file_transfer.h"

extern void func_8003BF00(void);extern void func_800137E4(void);
void func_8003C0C0(void){int x=0;if(Campaign_TestStoryFlag(CAMPAIGN_FLAG_TOURNAMENT_COMPLETE))x=0x9E;File_RequestAsyncTransfer(0,0,x+0x1FD9,0x9E,func_8003BF00,0,0);func_800137E4();}
