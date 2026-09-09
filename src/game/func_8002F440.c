#include "../types.h"
#include "script_command_busy.h"
#include "fade.h"
#include "script_state.h"
#include "func_8002F440.h"

extern unsigned char D_800E9ECE[],D_800E9ECF[];
void func_8002F440(void){unsigned char*base=D_800E9ECE;int command;if(func_8002E3B4()==0){command=*D_8009B290++;Fade_InitOut();if(command&0x3F)D_800E9ECF[0]=command&0x3F;if((command&0x80)==0)goto clear;goto end;}if(base[0]&0x80)goto end;clear:D_8009B27C=0;end:;}
