#include "../types.h"
#include "display_object.h"
#include "duel_effect.h"
#include "display_object_api.h"
#include "text_box_lifecycle.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "text_constants.h"
#include "func_800611D0.h"

void func_800611D0(int offset){unsigned char*base=gText_abColorSlots;DuelEffectChannel*parent;DisplayObject*obj;base[0]=base[1]=base[2]=4;base[offset]=0;parent=TextBox_Create(3,0x20,0x58,0x40,0xA0,0x80);parent->field_59=0x15;func_80039A14(parent);obj=func_800400AC(func_8004002C(),2);func_800404CC(obj,parent->field_3C+8,parent->field_40,0,4,0xD,0xC,0x208);obj->flags|=0x28;func_80042918(obj);func_800428EC((u8*)obj,0x14);parent->field_2C=obj;}
