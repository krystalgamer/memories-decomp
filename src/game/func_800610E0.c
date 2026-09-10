#include "../types.h"
#include "display_object.h"
#include "display_object_api.h"
#include "text_box_lifecycle.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "duel_effect.h"

void func_800610E0(int mode){DuelEffectChannel*parent;DisplayObject*obj;D_8009B34E=mode;parent=TextBox_Create(3,10,0x30,0x78,0x120,0x10);parent->field_59=0x12;func_80039A14((u8*)parent);obj=func_800400AC(func_8004002C(),2);func_800404CC(obj,parent->field_3C+mode*160,parent->field_40,0,4,0xE,0xC,0x208);obj->flags|=0x28;func_80042918(obj);func_800428EC((u8*)obj,0x11);parent->field_2C=obj;}
