#include "../types.h"
#include "card_constants.h"
#include "duel_card_checks.h"

#define FUSION_TABLE_BYTES(table) ((u8 *)(table))
#define FUSION_TABLE_OFFSETS(table) ((u16 *)(table))

s32 Duel_CheckEquip(s32 key_card_id, s32 candidate_card_id)
{
    u16 *table_cursor = gDuel_awEquipTable;

    while (1) {
        s32 entry_card_id = table_cursor[0];
        s32 remaining_partners;

        if (entry_card_id == 0) {
            return 0;
        }
        remaining_partners = table_cursor[1];
        table_cursor += 2;
        if (entry_card_id == key_card_id) {
            do {
                if (candidate_card_id == *table_cursor) {
                    return candidate_card_id;
                }
                remaining_partners--;
                table_cursor++;
            } while (remaining_partners != 0);
            return 0;
        }
        table_cursor += remaining_partners;
    }
}

s32 Duel_CheckFusion(s32 card_a, s32 card_b)
{
    u8 *fusion_bytes = FUSION_TABLE_BYTES(gDuel_aFusionTable);
    u8 *pair_cursor;
    s32 table_offset;
    s32 remaining_pairs;
    s32 packed_high_bits;

    if (card_b < card_a) {
        s32 swap_card_id = card_b;
        card_b = card_a;
        card_a = swap_card_id;
    }
    table_offset = FUSION_TABLE_OFFSETS(fusion_bytes)[card_a];
    if (table_offset == 0) {
        return 0;
    }
    pair_cursor = fusion_bytes + table_offset;
    remaining_pairs = pair_cursor[0];
    if (remaining_pairs == 0) {
        remaining_pairs = FUSION_TABLE_EXTENDED_COUNT_BASE - pair_cursor[1];
        pair_cursor++;
    }
    pair_cursor++;
    do {
        packed_high_bits = pair_cursor[0];
        if ((((packed_high_bits << FUSION_TABLE_FIRST_PARTNER_SHIFT) &
              FUSION_TABLE_CARD_ID_HIGH_MASK) | pair_cursor[1]) == card_b) {
            return ((packed_high_bits << FUSION_TABLE_FIRST_RESULT_SHIFT) &
                    FUSION_TABLE_CARD_ID_HIGH_MASK) | pair_cursor[2];
        }
        if ((((packed_high_bits << FUSION_TABLE_SECOND_PARTNER_SHIFT) &
              FUSION_TABLE_CARD_ID_HIGH_MASK) | pair_cursor[3]) == card_b) {
            return ((packed_high_bits << FUSION_TABLE_SECOND_RESULT_SHIFT) &
                    FUSION_TABLE_CARD_ID_HIGH_MASK) | pair_cursor[4];
        }
        pair_cursor += FUSION_TABLE_ENTRY_SIZE;
        remaining_pairs -= FUSION_TABLE_PAIRS_PER_ENTRY;
    } while (remaining_pairs > 0);
    return 0;
}
