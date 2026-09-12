#include "../types.h"
#include "../game/duel_card.h"
#include "../game/duel_card_checks.h"
#include "../game/duel_side_state.h"
#include "../game/duel_grid.h"
#include "../game/duel_check_quit_input.h"
#include "../game/duel_field_equip_search.h"
#include "../game/rand_get_interval.h"
#include "../game/duel_card_selection.h"
#include "../psyq/rand.h"

extern u8 D_800EAE88[];

/* Current best under gcc_2_8_1_g8_split: 269/271 instructions, opcode
 * distance 8, and 176 differing positions.
 *
 * The AI's per-turn play decision. Writes the chosen action into the three
 * bytes at D_800EAE88 + 9: the action code at +9, its argument at +0xA, and a
 * flag at +0xB. It does nothing while bit 0x1000 of D_8009B16C is set.
 *
 * One turn in four it first looks for an equip: it collects the field's
 * monsters (type 0x17) and the hand's magic cards, and takes the first pair
 * Duel_CheckEquip accepts. Failing that it collects the type-0x14 monsters and
 * picks one at random. Otherwise, and when neither search finds anything, it
 * walks the five hand slots from 0xA to 0xE for the first card whose top two
 * flag bits read 10, and decides what to do with it through func_800278A0 and
 * func_8002778C, falling back to scanning the opponent's five monster zones
 * for one with 0x90000000 set. */
s32 func_800279BC(void)
{
    DuelCardRecord *pool[6];
    DuelCardRecord *listb[6];
    DuelCardRecord *lista[6];
    DuelCardRecord **pa;
    DuelCardRecord **pb;
    DuelCardRecord *ea;
    DuelCardRecord *eb;
    DuelCardRecord *rec;
    DuelCardRecord *scan;
    s32 slot;
    s32 n;
    s32 i;
    s32 count;
    s32 v;
    u8 *grid;
    u8 *out;
    DuelCardRecord *recs;
    DuelCardRecord *scanbase;

    D_800EAE88[9] = 0;
    if (D_8009B16C & 0x1000) {
        return 0;
    }
    if ((rand() & 3) != 0) {
        slot = 0xA;
        goto walk;
    }
    {
        Duel_CollectFieldCardsByType((DuelCardRecord **)lista, 0xA, 0x17);
        Duel_CollectFieldCardsByType((DuelCardRecord **)listb, 5, -1);
        ea = lista[0];
        if (ea != 0) {
            pa = lista;
            do {
                eb = listb[0];
                if (eb != 0) {
                    pb = listb;
                    do {
                        if (Duel_CheckEquip((s16)ea->card_id,
                                            (s16)eb->card_id)) {
                            goto found;
                        }
                        pb++;
                        eb = *pb;
                    } while (eb != 0);
                }
                pa++;
                ea = *pa;
            } while (ea != 0);
        }
        n = Duel_CollectFieldCardsByType((DuelCardRecord **)lista, 0xA, 0x14);
        if (n == 0) {
            goto no_equip;
        }
        n--;
        if (n != 0) {
            n = Rand_GetInterval(n + 1);
        }
        rec = lista[n];
        D_800EAE88[0xA] = 0;
        D_800EAE88[0xB] = 0;
        D_800EAE88[9] = (s8)rec->table_index % 5 + 6;
        return 0;
found:
        D_800EAE88[9] = (s8)ea->table_index % 5 + 6;
        D_800EAE88[0xB] = 0;
        D_800EAE88[0xA] = (s8)eb->table_index % 5 + 1;
        return 0;
no_equip:
        slot = 0xA;
    }
walk:
    grid = D_800907D8;
    out = D_800EAE88;
    recs = D_801A7AD8;
    scanbase = &D_801A7AD8[5];
loop:
    {
        rec = &recs[grid[slot + D_8009B1D5 * 20]];
        if ((*(u32 *)((u8 *)rec + 0x14) & 0xC0000000) == 0x80000000) {
            v = func_800278A0(rec);
            if (v < 0) {
                if ((rand() & 3) != 0) {
                    if ((rand() & 1) != 0) {
                        count = 0;
                        i = 0;
                        scan = &scanbase[(D_8009B1D5 ^ 1) * 15];
                        pa = pool;
                        do {
                            if ((*(u32 *)((u8 *)scan + 0x14) & 0x90000000)
                                == 0x90000000) {
                                *pa = scan;
                                pa++;
                                count++;
                            }
                            i++;
                            scan++;
                        } while (i < 5);
                        if (count != 0) {
                            n = count - 1;
                            if (n != 0) {
                                n = Rand_GetInterval(count);
                            }
                            v = (s8)pool[n]->table_index;
                            goto have;
                        }
                    }
                    goto none;
                }
                v = func_8002778C((DuelSelectionSource *)rec);
                if (v < 0) {
                    goto none;
                }
            }
have:
            if (D_8009B1C8->field_19 == 0) {
                out[0xB] = 0;
                out[0xA] = v % 5 + 0x38;
                out[9] = (s8)rec->table_index % 5 + 1;
                return 0;
            }
none:
            out[0xA] = 0;
            out[0xB] = 1;
            out[9] = (s8)((s8)rec->table_index % 15) - 4;
            return 0;
        }
        slot++;
    }
    if (slot < 0xF) {
        goto loop;
    }
    return 0;
}
