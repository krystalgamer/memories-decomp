#include "../types.h"
#include "build_deck_card_counts.h"
#include "card_constants.h"
#include "card_list_sort.h"
#include "display_object_layout.h"
#include "duel_card.h"
#include "text_box_lifecycle.h"
#include "text_box_runtime.h"
#include "text_staging.h"

/* The Build Deck screen's count box refresh: it copies the two totals at
   +0x5A9C and +0x5AA0 of the screen record into D_801D5608 and opens the
   count text box. The screen's other card-count helpers follow it in the
   image; func_80031F7C is in func_80031F7C.c and the rest are candidates
   under src/candidates/.

   Compiles to an identical object at gcc_2_8_1_g0_split. Bounded below by
   the card-list text boxes, which need gcc_2_8_1_g0, and above by
   func_80031EE4, now a candidate in src/candidates/func_80031EE4.c. */

void func_80031E5C(u8 *arg0) {
    u8 *p;
    D_801D5608[0].build_deck.chest = *(u32 *)(arg0 + 0x5A9C);
    D_801D5608[0].build_deck.deck = *(u32 *)(arg0 + 0x5AA0);
    p = TextBox_CreateFlagged(3, 0xE, 0x16, 0x17, 0x280, 0x10, 0x100);
    func_80039A14((struct DuelEffectChannel *)p);
    *(u16 *)(*(u8 **)(p + 0x28) + 8) &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
}
