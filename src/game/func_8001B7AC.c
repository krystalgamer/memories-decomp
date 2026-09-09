#include "../types.h"
#include "display_object_config.h"
#include "display_object_api.h"
#include "card_constants.h"
#include "duel_card_display_state.h"
#include "duel_hand.h"
#include "sound.h"
#include "display_object_helpers.h"

typedef struct Object{char p0[4];struct Object*child;char p8[6];s8 index;char pF[6];u8 count;}Object;
void func_8001B7AC(Object*arg)
{
 register Object*object=arg;register DuelHandSlot*slot=&D_800EA030[object->index];register DuelCardDisplayObject*child;
 ((DuelCardDisplayObject*)slot->object)->out_y-=4;child=func_800400AC(func_8004002C(),1);func_80040510((DisplayObjectConfigView*)child,((DuelCardDisplayObject*)slot->object)->out_x,((DuelCardDisplayObject*)slot->object)->out_y,0x10,0x10,object->count<<4,0xB8,0xB,0x250,0xFC);
 func_80042918(child);func_800428EC(child,(s8)(slot->object[0x16]+1));slot->child=(u8*)child;object->count++;slot->active_09=object->count;SD_SEPlayFull(0x2F);
}
