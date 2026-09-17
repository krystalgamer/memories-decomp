#include "../types.h"
#include "display_object.h"
#include "display_object_core.h"
#include "text_box_lifecycle.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "duel_effect.h"
#include "func_800610E0.h"

void func_800610E0(int mode){DuelEffectChannel*parent;DisplayObject*obj;D_8009B34E=mode;parent=TextBox_Create(3,10,0x30,0x78,0x120,0x10);parent->field_59=0x12;func_80039A14(parent);obj=DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(),2);DisplayObject_ConfigureSpriteAtPosition(obj,parent->field_3C+mode*160,parent->field_40,0,4,0xE,0xC,0x208);obj->flags|=DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET|DISPLAY_OBJECT_FLAG_SCREEN_SPACE;DisplayObject_SelectOrderingTable1(obj);DisplayObject_SetDepthOffset(obj,0x11);parent->field_2C=obj;}
