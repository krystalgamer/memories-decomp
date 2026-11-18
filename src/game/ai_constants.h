#ifndef YUGIOH_GAME_AI_CONSTANTS_H
#define YUGIOH_GAME_AI_CONSTANTS_H

#define AI_ACTIVE_CARD_RECORD_SIZE 0x0C
#define AI_ACTIVE_CARD_SIDE_BYTE_STRIDE 0x294

/* Slot map of gDuel_aActiveCards, taken from Ai_GetCardRange, which is the
   game's own authoritative table of the six ranges. Slot 0 is the zero entry
   the field searches start from, so an index of 0 doubles as "nothing found";
   the player's slots run from 1 and the opponent's sit one side stride
   further on. Each field row is AI_ACTIVE_CARD_ROW_SLOT_COUNT slots wide and
   the ranges are inclusive, so the last slot of a row is
   AI_SLOT_ROW_LAST(first). AI_ACTIVE_CARD_ROW_SLOT_COUNT is DUEL_FIELD_ROW_SIZE;
   ai.h asserts that, this header stays free of type includes. */
#define AI_ACTIVE_CARD_ROW_SLOT_COUNT 5
#define AI_ACTIVE_CARD_SIDE_SLOT_STRIDE \
    (AI_ACTIVE_CARD_SIDE_BYTE_STRIDE / AI_ACTIVE_CARD_RECORD_SIZE)
#define AI_SLOT_NONE 0
#define AI_SLOT_OWN_MONSTER_FIRST 1
#define AI_SLOT_OWN_SPELL_FIRST \
    (AI_SLOT_OWN_MONSTER_FIRST + AI_ACTIVE_CARD_ROW_SLOT_COUNT)
#define AI_SLOT_OWN_HAND_FIRST \
    (AI_SLOT_OWN_SPELL_FIRST + AI_ACTIVE_CARD_ROW_SLOT_COUNT)
#define AI_SLOT_OPPONENT_MONSTER_FIRST \
    (AI_SLOT_OWN_MONSTER_FIRST + AI_ACTIVE_CARD_SIDE_SLOT_STRIDE)
#define AI_SLOT_OPPONENT_SPELL_FIRST \
    (AI_SLOT_OWN_SPELL_FIRST + AI_ACTIVE_CARD_SIDE_SLOT_STRIDE)
#define AI_SLOT_OPPONENT_HAND_FIRST \
    (AI_SLOT_OWN_HAND_FIRST + AI_ACTIVE_CARD_SIDE_SLOT_STRIDE)
#define AI_SLOT_ROW_LAST(first) ((first) + AI_ACTIVE_CARD_ROW_SLOT_COUNT - 1)

/* The zone operand of the CountCards, FindFirstCard, FindCard and MatchType
   instructions, decoded by Ai_GetCardRange (notes/ai-scripts.md). "Own" is
   the AI's side and "opponent" the player's; the _UNUSED zones skip cards
   flagged DUEL_CARD_FLAG_USED_THIS_TURN. */
enum {
    AI_ZONE_OWN_MONSTERS = 0,
    AI_ZONE_OWN_MONSTERS_UNUSED = 1,
    AI_ZONE_OWN_SPELLS = 2,
    AI_ZONE_OWN_SPELLS_UNUSED = 3,
    AI_ZONE_OWN_HAND = 4,
    AI_ZONE_OPPONENT_MONSTERS = 5,
    AI_ZONE_OPPONENT_MONSTERS_UNUSED = 6,
    AI_ZONE_OPPONENT_SPELLS = 7,
    AI_ZONE_OPPONENT_SPELLS_UNUSED = 8,
    AI_ZONE_OPPONENT_HAND = 9
};

/* The zone operand of FindStrongest and FindWeakest, decoded by
   Ai_GetWinningCardRange: a different encoding from AI_ZONE_*. The two
   opponent-monster filters also skip defence- or attack-position cards. */
enum {
    AI_STAT_ZONE_OWN_MONSTERS = 0,
    AI_STAT_ZONE_OWN_MONSTERS_UNUSED = 1,
    AI_STAT_ZONE_OWN_HAND = 2,
    AI_STAT_ZONE_OPPONENT_MONSTERS = 3,
    AI_STAT_ZONE_OPPONENT_ATTACKERS_UNUSED = 4,
    AI_STAT_ZONE_OPPONENT_DEFENDERS_UNUSED = 5,
    AI_STAT_ZONE_OPPONENT_HAND = 6
};

/* Card-state query results and the matching FindFirstCard operand domain. */
#define AI_CARD_STATE_EMPTY 0
#define AI_CARD_STATE_FACE_UP 1
#define AI_CARD_STATE_FACE_DOWN 2

#define AI_SCRIPT_CARD_SET_COUNT 32
#define AI_SCRIPT_COMBO_CARD_COUNT 6
#define AI_SCRIPT_PERCENT_SCALE 100
#define AI_SCRIPT_MEMORY_COUNT 20
#define AI_SCRIPT_RETURN_STACK_COUNT 8
#define AI_SCRIPT_TYPE_SET_COUNT 25
#define AI_SCRIPT_CARD_SET_EMPTY 0
#define AI_SCRIPT_TYPE_SET_EMPTY 0
#define AI_SCRIPT_TYPE_SET_ENCODING_BIAS 1
#define AI_SCRIPT_CARD_SET_ENTRY_SIZE 2

#endif
