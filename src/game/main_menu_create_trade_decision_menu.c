#include "../types.h"
#include "text_box_blocking_build.h"
#include "trade_confirmation_widgets.h"
#include "display_object.h"
#include "duel_effect.h"
#include "display_object_core.h"
#include "text_box_lifecycle.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "text_constants.h"

void MainMenu_CreateTradeDecisionMenu(s32 offset){u8*base=gText_abColorSlots;DuelEffectChannel*parent;DisplayObject*obj;base[0]=base[1]=base[2]=4;base[offset]=0;parent=TextBox_Create(3,0x20,0x58,0x40,0xA0,0x80);parent->depth_offset=0x15;TextBox_BuildBlocking(parent);obj=DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(),2);DisplayObject_ConfigureSpriteAtPosition(obj,parent->rect_x+8,parent->rect_y,0,4,0xD,0xC,0x208);obj->flags|=DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET|DISPLAY_OBJECT_FLAG_SCREEN_SPACE;DisplayObject_SelectOrderingTable1(obj);DisplayObject_SetDepthOffset(obj,0x14);parent->aux_object=obj;}
