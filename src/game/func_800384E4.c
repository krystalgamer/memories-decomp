#include "../types.h"
#include "duel_effect.h"
#include "duel_effect_command.h"

void func_800384E4(DuelEffectChannel*object){register DuelEffectChannel*obj;register u8**stream;register u8*current;register unsigned int value;obj=object;obj->flags_34&=0xEFFF;stream=&((u8**)obj)[obj->stream_58];current=*stream;value=*current;current++;*stream=current;if(value)obj->flags_34|=0x1000;}
