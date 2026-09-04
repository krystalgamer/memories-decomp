#include "../types.h"

extern int Campaign_TestStoryFlag(int);extern void func_8003BF00(void);extern void File_RequestAsyncTransfer(int,int,int,int,void*,int,int);extern void func_800137E4(void);
void func_8003C0C0(void){int x=0;if(Campaign_TestStoryFlag(0x47))x=0x9E;File_RequestAsyncTransfer(0,0,x+0x1FD9,0x9E,func_8003BF00,0,0);func_800137E4();}
