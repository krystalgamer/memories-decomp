#ifndef MEMORIES_DECOMP_CAMPAIGN_FLAGS_H
#define MEMORIES_DECOMP_CAMPAIGN_FLAGS_H

#include "../types.h"

#define CAMPAIGN_FLAG_ID_MASK 0x7FF
#define CAMPAIGN_FLAG_BANK_OFFSET 0x618
#define CAMPAIGN_FLAG_BYTE_SHIFT 3
#define CAMPAIGN_FLAG_BIT_INDEX_MASK 7
#define CAMPAIGN_FLAG_BYTE_HIGH_BIT 0x80
#define CAMPAIGN_FLAG_DUELIST_DEFEATED_BASE 0x1F
#define CAMPAIGN_FLAG_TOURNAMENT_COMPLETE 0x47
#define CAMPAIGN_FLAG_LIBRARY_CARD_BASE 0x120
#define CAMPAIGN_FLAG_PASSWORD_USED_BASE 0x400

/* Tests require a clear bit; updates clear it when this modifier is set. */
#define CAMPAIGN_FLAG_CLEAR_MODIFIER 0x8000
#define CAMPAIGN_FLAG_COMMAND_WRITE 0x4000
#define CAMPAIGN_FLAG_COMMAND_WORD_MASK 0xFFFF
#define CAMPAIGN_FLAG_COMMAND_PAYLOAD_MASK \
    (CAMPAIGN_FLAG_COMMAND_WORD_MASK ^ CAMPAIGN_FLAG_COMMAND_WRITE)

/* A normal query returns the bit mask, not a normalized Boolean; with
   CAMPAIGN_FLAG_CLEAR_MODIFIER set it returns 1 when the bit is clear. */
s32 Campaign_TestStoryFlag(s32 flag);

/* Shared packed flag-update interface; the established public name is retained. */
void Library_UpdateCardUsedFlag(s32 flag);

#endif
