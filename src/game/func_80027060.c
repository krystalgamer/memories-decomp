#include "../types.h"
#include "card_constants.h"
#include "duel_card.h"

extern s32 Duel_CollectFieldCardsByType(u8 **, s32, s32);
extern s32 func_80026C0C(s32);
extern u8 D_800EAE88[];

s32 func_80027060(void) {
    u8 *sp10[6];
    s32 slot;
    s8 v;

    if (Duel_CollectFieldCardsByType(sp10, 0, CARD_TYPE_TRAP) == 0) {
        return 1;
    }
    slot = func_80026C0C(0xA);
    if (slot < 0) {
        return 1;
    }
    v = sp10[0][0x18];
    D_800EAE88[8] = 1;
    D_800EAE88[1] = 0;
    D_800EAE88[7] = 0;
    D_800EAE88[6] = slot % DUEL_FIELD_ROW_SIZE + 6;
    D_800EAE88[0] = v % DUEL_FIELD_ROW_SIZE + 0xB;
    return 0;
}
