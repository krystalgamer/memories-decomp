#include "../types.h"
#include "display_object_config.h"
#include "display_object_api.h"
#include "card_constants.h"
#include "duel_card_display_state.h"
#include "duel_hand.h"
#include "sound.h"
#include "display_object_helpers.h"
#include "func_8001B7AC.h"

void func_8001B7AC(DuelHandStackState *arg)
{
 register DuelHandStackState*object=arg;register DuelHandSlot*slot=&D_800EA030[object->slot_index];register DuelCardDisplayObject*child;
 ((DuelCardDisplayObject*)slot->object)->out_y-=4;child=func_800400AC(func_8004002C(),1);func_80040510((DisplayObjectConfigView*)child,((DuelCardDisplayObject*)slot->object)->out_x,((DuelCardDisplayObject*)slot->object)->out_y,0x10,0x10,object->count<<4,0xB8,0xB,0x250,0xFC);
 func_80042918((DisplayObject *)child);func_800428EC(child,(s8)(slot->object[0x16]+1));slot->child=(u8*)child;object->count++;slot->active_09=object->count;SD_SEPlayFull(0x2F);
}
