#include "../types.h"
#include "text_box_blocking_build.h"
#include "trade_confirmation_widgets.h"
#include "display_object.h"
#include "display_object_core.h"
#include "text_box_lifecycle.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "duel_effect.h"
#include "func_800610E0.h"

void MainMenu_CreateTradeReadyIndicator(s32 mode){DuelEffectChannel*parent;DisplayObject*obj;gText_BranchSelectorA=mode;parent=TextBox_Create(3,10,0x30,0x78,0x120,0x10);parent->field_59=0x12;TextBox_BuildBlocking(parent);obj=DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(),2);DisplayObject_ConfigureSpriteAtPosition(obj,parent->rect_x+mode*160,parent->rect_y,0,4,0xE,0xC,0x208);obj->flags|=DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET|DISPLAY_OBJECT_FLAG_SCREEN_SPACE;DisplayObject_SelectOrderingTable1(obj);DisplayObject_SetDepthOffset(obj,0x11);parent->field_2C=obj;}
