#include "../types.h"

/* The main-menu trade confirmation widgets: the ready indicator and the
 * decision menu. Each builds a blocking text box and hangs a cursor sprite
 * off it as the box's aux object. */
#include "text_box_blocking_build.h"
#include "trade_confirmation_widgets.h"
#include "display_object.h"
#include "display_object_system.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "duel_effect_states.h"

void MainMenu_CreateTradeReadyIndicator(s32 mode){DuelEffectChannel*parent;DisplayObject*obj;gText_BranchSelectorA=mode;parent=TextBox_Create(3,10,0x30,0x78,0x120,0x10);parent->depth_offset=0x12;TextBox_BuildBlocking(parent);obj=DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(),2);DisplayObject_ConfigureSpriteAtPosition(obj,parent->rect_x+mode*160,parent->rect_y,0,4,0xE,0xC,0x208);obj->flags|=DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET|DISPLAY_OBJECT_FLAG_SCREEN_SPACE;DisplayObject_SelectOrderingTable1(obj);DisplayObject_SetDepthOffset(obj,0x11);parent->aux_object=obj;}

#include "text_box_blocking_build.h"
#include "trade_confirmation_widgets.h"
#include "display_object.h"
#include "duel_effect_states.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "text_constants.h"

void MainMenu_CreateTradeDecisionMenu(s32 offset){u8*base=gText_abColorSlots;DuelEffectChannel*parent;DisplayObject*obj;base[0]=base[1]=base[2]=4;base[offset]=0;parent=TextBox_Create(3,0x20,0x58,0x40,0xA0,0x80);parent->depth_offset=0x15;TextBox_BuildBlocking(parent);obj=DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(),2);DisplayObject_ConfigureSpriteAtPosition(obj,parent->rect_x+8,parent->rect_y,0,4,0xD,0xC,0x208);obj->flags|=DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET|DISPLAY_OBJECT_FLAG_SCREEN_SPACE;DisplayObject_SelectOrderingTable1(obj);DisplayObject_SetDepthOffset(obj,0x14);parent->aux_object=obj;}
