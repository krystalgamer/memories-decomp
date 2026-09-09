#include "../types.h"
#include "card_constants.h"
#include "duel_monster_removal_rules.h"

/* Initialized data at 0x80090A4C. Each pair stores the card ID minus 600 and
   the monster-type selector; Crush Card instead stores its 1500-ATK threshold
   in CARD_STAT_SCALE units. */
u8 gDuel_abMonsterRemovalRules[] = {
    DUEL_WARRIOR_ELIMINATION_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE, 3,
    DUEL_ETERNAL_REST_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE, 2,
    DUEL_STAIN_STORM_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE, 14,
    DUEL_CRUSH_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE, 150,
    DUEL_ERADICATING_AEROSOL_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE, 9,
    DUEL_BREATH_OF_LIGHT_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE, 18,
    DUEL_ETERNAL_DRAUGHT_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE, 12,
    0, 0, /* Retail alignment before the following effect-handler table. */
};
