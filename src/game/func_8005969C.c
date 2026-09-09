#include "../types.h"
#include "model.h"
#include "model_slot_state_updates.h"

void func_8005969C(int index,int type)
{
    ModelSlot *e=&D_800F2C40[index]; if((unsigned)(type-4)>=29)type=8; e->field_E0D=type; if(e->field_E16==0x3e)func_80059700(index,1);
}
