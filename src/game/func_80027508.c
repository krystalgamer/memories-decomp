#include "../types.h"
#include "card_constants.h"
#include "duel_card.h"
#include "duel_grid.h"
#include "func_80026C0C.h"
#include "../psyq/rand.h"

extern s32 func_80026DC8(void);
extern s32 func_80027060(void);
extern s32 func_8002712C(void);
extern s32 func_80027228(void);
extern s32 Rand_GetInterval(s32);

extern u8 D_8009B1D5;
extern u8 D_800EAE88[];
extern s32 gDuel_adwCardStats[];

/* AI turn action pick: tries the equip, trap and spell searches at random
   odds, then picks a random card from the side's back-row records and fills
   the D_800EAE88 selection with a field slot for it: a monster goes to the
   front row with a random flag, anything else to the back row, forcing the
   flag on for an equip. Always returns 0. */
s32 func_80027508(void) {
    DuelCardRecord *card;
    s32 slot;

    if ((rand() & 1) == 0 && func_80026DC8() == 0) {
        return 0;
    }
    if ((rand() & 1) == 0 && func_80027060() == 0) {
        return 0;
    }
    if ((rand() & 3) == 0 && func_8002712C() == 0) {
        return 0;
    }
    if (func_80027228() == 0) {
        return 0;
    }
    card = &D_801A7AD8[
        D_8009B1D5 * DUEL_CARD_SIDE_RECORD_COUNT +
        Rand_GetInterval(DUEL_FIELD_ROW_SIZE)
    ];
    if (((gDuel_adwCardStats[*(s16 *)&card->card_id - 1] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) < CARD_TYPE_MAGIC) {
        s8 v;

        slot = func_80026C0C(DUEL_FIELD_ROW_SIZE);
        if (slot < 0) {
            slot = Rand_GetInterval(DUEL_FIELD_ROW_SIZE);
        }
        v = card->table_index;
        D_800EAE88[1] = 0;
        D_800EAE88[6] = slot % DUEL_FIELD_ROW_SIZE + 1;
        D_800EAE88[0] = v % DUEL_FIELD_ROW_SIZE + 0xB;
        D_800EAE88[7] = rand() & 1;
        D_800EAE88[8] = 1;
    } else {
        s8 v;

        slot = func_80026C0C(DUEL_FIELD_SIDE_ZONE_COUNT);
        if (slot < 0) {
            slot = Rand_GetInterval(DUEL_FIELD_ROW_SIZE);
        }
        v = card->table_index;
        D_800EAE88[1] = 0;
        D_800EAE88[7] = 0;
        D_800EAE88[6] = slot % DUEL_FIELD_ROW_SIZE + 6;
        D_800EAE88[0] = v % DUEL_FIELD_ROW_SIZE + 0xB;
        D_800EAE88[8] = rand() & 1;
        if (((gDuel_adwCardStats[*(s16 *)&card->card_id - 1] >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK) == CARD_TYPE_EQUIP) {
            D_800EAE88[8] = 1;
        }
    }
    return 0;
}
