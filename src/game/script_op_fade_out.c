#include "../types.h"
#include "script_command_busy.h"
#include "fade.h"
#include "script_state.h"
#include "script_op_fade_out.h"
#include "../unmatched.h"
#include "fade_state_bytes.h"

void Script_OpFadeOut(void){unsigned char*base=gFade_bFlags;int command;if(ScriptCommand_MarkStarted()==0){command=*gScript_StreamCursor++;Fade_InitOut();if(command&0x3F)gFade_bStep[0]=command&0x3F;if((command&0x80)==0)goto clear;goto end;}if(base[0]&0x80)goto end;clear:gScript_wCommandWord=0;end:;}
