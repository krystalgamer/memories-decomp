#include "../types.h"
#include "duel_monster_removal_rules.h"

/* Initialized data at 0x80090A4C. Each pair stores the card ID minus 600 and
   the monster-type selector; Crush Card instead stores its 1500-ATK threshold
   in CARD_STAT_SCALE units. */
u8 gDuel_abMonsterRemovalRules[] = {
    653 - DUEL_MONSTER_REMOVAL_CARD_ID_BASE, 3,   /* Warrior Elimination */
    656 - DUEL_MONSTER_REMOVAL_CARD_ID_BASE, 2,   /* Eternal Rest */
    660 - DUEL_MONSTER_REMOVAL_CARD_ID_BASE, 14,  /* Stain Storm */
    661 - DUEL_MONSTER_REMOVAL_CARD_ID_BASE, 150, /* Crush Card */
    662 - DUEL_MONSTER_REMOVAL_CARD_ID_BASE, 9,   /* Eradicating Aerosol */
    663 - DUEL_MONSTER_REMOVAL_CARD_ID_BASE, 18,  /* Breath of Light */
    664 - DUEL_MONSTER_REMOVAL_CARD_ID_BASE, 12,  /* Eternal Draught */
    0, 0, /* Retail alignment before the following effect-handler table. */
};
