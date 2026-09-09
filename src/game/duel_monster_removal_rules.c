#include "../types.h"
#include "card_constants.h"
#include "duel_monster_removal_rules.h"

/* Initialized data at 0x80090A4C. Each pair stores the card ID minus 600 and
   the monster-type selector; Crush Card instead stores its 1500-ATK threshold
   in CARD_STAT_SCALE units. */
u8 gDuel_abMonsterRemovalRules[] = {
    DUEL_WARRIOR_ELIMINATION_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE,
    CARD_TYPE_WARRIOR,
    DUEL_ETERNAL_REST_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE,
    CARD_TYPE_ZOMBIE,
    DUEL_STAIN_STORM_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE,
    CARD_TYPE_MACHINE,
    DUEL_CRUSH_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE,
    DUEL_CRUSH_CARD_ATTACK_THRESHOLD / CARD_STAT_SCALE,
    DUEL_ERADICATING_AEROSOL_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE,
    CARD_TYPE_INSECT,
    DUEL_BREATH_OF_LIGHT_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE,
    CARD_TYPE_ROCK,
    DUEL_ETERNAL_DRAUGHT_CARD_ID - DUEL_MONSTER_REMOVAL_CARD_ID_BASE,
    CARD_TYPE_FISH,
    0, 0, /* Retail alignment before the following effect-handler table. */
};
