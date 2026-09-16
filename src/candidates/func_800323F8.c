/*
 * Sets up the Build Deck screen: initialises the two 0x6344-byte deck
 * records (recent card drops, the 40-card deck list with its stats, and the
 * 722-card trunk list with its counts), sorts both lists, then creates the
 * screen's eleven display objects. Current best under
 * gcc_2_8_1_g8_split_no_strength_reduce: 464 instructions against 464 with
 * an empty opcode census, with no hard register assignments and no inline
 * assembly.
 *
 * Levers measured on this body:
 * - -fno-strength-reduce: the target's loops keep explicit counters and
 *   unbiased cursors;
 * - explicit cursors: the recent-drop table walked backwards, the per-card
 *   byte cursor, the deck-list record cursor and the u16 deck-word cursor;
 * - the D_80090DD8 entry address summed as an integer in a local and read
 *   as z[1], so the index is not folded into the hoisted %lo;
 * - the card-stat address built as v--, v *= 4, v += tb after the store of
 *   v, which keeps the store first and the branch slot a nop;
 * - in the tail, +0x30 is written before +0x4C.
 *
 * Residual: register assignment only. Retail hoists D_80090DD8 and
 * D_801D4244 into $fp and $s7 in the other order, and the two list loops
 * rotate their counters and cursors among $t0-$t2 and $a2/$a3.
 */
#define GRAPHICS_VIEWPORT_IN_DATA
#include "../types.h"
#include "../game/graphics_frame.h"
#include "../game/duel_reward_setup.h"
#include "../game/duel_card.h"
#include "../game/build_deck_transition_state.h"
#include "../game/build_deck_card_counts.h"
#include "../game/card_list_sort.h"
#include "../game/func_80031874.h"
#include "../game/display_object_core.h"
#include "../game/display_object_helpers.h"
#include "../game/sound.h"
#include "../game/card_type_icon_table.h"

void func_800323F8(u8 *arg0, u8 *arg1, s32 arg2, s32 arg3)
{
    u8 *p;
    u8 *q;
    u8 *o;
    u8 *o2;
    u8 *r;
    u8 *c;
    u8 *b;
    u8 *s;
    u8 *z;
    u8 *dd;
    u16 *t7;
    u16 *w;
    u16 *pt;
    u8 *pj;
    u8 *pk;
    s32 *tb;
    s32 *e;
    s32 i;
    s32 j;
    s32 k;
    s32 x;
    s32 one;
    s32 f80;
    s32 cnt;
    s32 sum;
    s32 v;

    func_80032328();
    SD_BGMPlay(0x70E0);
    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
    p = arg0;
    func_80032370();
    i = 0;
    one = 1;
    tb = gDuel_adwCardStats;
    q = p + 4;
    gBuildDeck_pState = (BuildDeckTransitionState *)p;
    p[0x6343] = arg3;
    *(u8 **)p = arg1;
    *(s32 *)(p + 0x6344) = arg2;
    p[0x6342] = 0;
    p[0xC686] = 0;
    do {
        if (*(s32 *)p != 0) {
            t7 = (u16 *)gDuel_awRecentCardDrops;
            q[0x6066] = 0;
            j = 1;
            pj = p + j;
            do {
                pj[0x606A] = 0;
                k = 15;
                pk = pj;
                pt = t7 + 15;
                do {
                    if ((s16)*pt == j) {
                        pk[0x606A] = k + 1;
                    }
                    k--;
                    pt--;
                } while (k >= 0);
                j++;
                pj++;
            } while (j < 0x2D3);
            r = p + 0x2D50;
            cnt = 0;
            q[0x5A93] = one;
            k = cnt;
            *(s16 *)(q + 0x5A8A) = 0;
            *(s16 *)(q + 0x5A88) = 0;
            q[0x5A94] = 0;
            q[0x5A92] = 0;
            z = (u8 *)(q[0x5A93] * 16 + (s32)D_80090DD8);
            q[0x5A91] = z[1] & 0xF;
            c = p + 0x2D58;
            w = *(u16 **)p;
            for (; k < 0x28; k++, c += 0x10, w++) {
                c[5] = 0;
                *(s16 *)(c - 4) = 0;
                v = *w;
                if (v != 0) {
                    *(s16 *)(c - 4) = v;
                    v--;
                    v *= 4;
                    v += (s32)tb;
                    c[5] = one;
                    c[2] = (*(s32 *)v >> 26) & 0x1F;
                    *(s16 *)(c - 2) = (*(s32 *)v & 0x1FF) * 10;
                    cnt++;
                    *(s16 *)c = ((*(s32 *)v >> 9) & 0x1FF) * 10;
                }
            }
            *(s16 *)(r + 0x284) = -1;
            *(s32 *)(q + 0x5A9C) = cnt;
            *(s16 *)(q + 0x5A8C) = 0x28;
            *(s16 *)(q + 0x5A8E) = 0x28;
            func_80032C48((CardList *)(p + 0x2D50));
            func_8003201C(p);
            r = q;
            sum = 0;
            j = sum;
            f80 = 0x80;
            q[0x2D47] = 0;
            *(s16 *)(q + 0x2D3E) = 0;
            *(s16 *)(q + 0x2D3C) = 0;
            *(s16 *)(q + 0x2D40) = 0x2D2;
            q[0x2D48] = 0;
            q[0x2D46] = 0;
            v = q[0x2D47] * 16;
            v += (s32)D_80090DD8;
            q[0x2D45] = ((u8 *)v)[1] & 0xF;
            c = q + 0xD;
            e = tb;
            b = *(u8 **)p + 0x50;
            for (; j < 0x2D2; e++, j++, c += 0x10, b++) {
                x = j + 1;
                c[0] = 0;
                *(s16 *)(c - 9) = x;
                c[-3] = (*e >> 26) & 0x1F;
                *(s16 *)(c - 7) = (*e & 0x1FF) * 10;
                *(s16 *)(c - 5) = ((*e >> 9) & 0x1FF) * 10;
                x += (s32)p;
                s = (u8 *)x;
                s[0x5D97] = *b;
                if (*b != 0) {
                    c[0] = one;
                    sum += *b;
                } else if (s[0x5AC4] != 0) {
                    c[0] = f80;
                }
            }
            *(s16 *)(r + 0x2D24) = 0;
            *(s32 *)(q + 0x5A98) = sum;
            *(s16 *)(q + 0x2D42) = 0x2D2;
            *(s16 *)(q + 0x2D40) = 0x2D2;
            func_80032C48((CardList *)q);
        }
        i++;
        q += 0x6344;
        p += 0x6344;
    } while (i < 2);

    p = (u8 *)gBuildDeck_pState;
    o = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
    func_800404CC(o, 0, 0, 0, 4, 0, 0xC, 0x208);
    o2 = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 6);
    o2[0x67] = 0;
    *(s32 *)(o2 + 0x30) = *(s32 *)(o + 0x30);
    *(s32 *)(o2 + 0x4C) = (s32)func_80031874;
    o = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
    func_800404CC(o, 0x140, 0, 0, 4, 1, 0xC, 0x208);
    o2 = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 6);
    o2[0x67] = 1;
    *(s32 *)(o2 + 0x30) = *(s32 *)(o + 0x30);
    *(s32 *)(o2 + 0x4C) = (s32)func_80031874;
    *(s16 *)(p + 0x633E) = 2;
    o = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
    func_800404CC(o, 0x136, 0x29, 0, 4, 0xC, 0xC, 0x208);
    *(u16 *)(o + 8) |= 0x20;
    func_800428EC(o, 8);
    *(u8 **)(p + 0x2D3C) = o;
    o = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
    func_800404CC(o, 0x26A, 0x29, 0, 4, 0xC, 0xC, 0x208);
    *(u16 *)(o + 8) |= 0x20;
    func_800428EC(o, 8);
    *(u8 **)(p + 0x5A88) = o;
    o = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
    func_800404CC(o, 0, 0x2A, 0, 4, 2, 0xC, 0x208);
    *(u16 *)(o + 8) |= 0x20;
    func_800428EC(o, 0xA);
    *(u8 **)(p + 0x2D38) = o;
    o = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
    func_800404CC(o, 0x148, 0x2A, 0, 4, 3, 0xC, 0x218);
    *(u16 *)(o + 8) |= 0x20;
    func_800428EC(o, 0xA);
    *(u8 **)(p + 0x5A84) = o;
    o = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
    func_800404CC(o, 0, 0, 0, 4, 9, 0xC, 0x208);
    *(u16 *)(o + 8) |= 0x20;
    func_800428EC(o, 0xA);
    o = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
    func_800404CC(o, 0x140, 0, 0, 4, 0xA, 0xC, 0x208);
    *(u16 *)(o + 8) |= 0x20;
    func_800428EC(o, 0xA);
    o = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
    func_800404CC(o, 0, 0, 0, 4, 0xB, 0xC, 0x208);
    *(u16 *)(o + 8) |= 0x20;
    func_800428EC(o, -4);
    o = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
    func_800404CC(o, 0x140, 0, 0, 4, 0xB, 0xC, 0x208);
    *(u16 *)(o + 8) |= 0x20;
    func_800428EC(o, -4);
    o = func_800400AC(DisplayObject_FindFreeGeneralSlot(), 2);
    func_800404CC(o, 0x140, 0, 3, 0, 3, 0xB, 0x2F8);
    func_800428EC(o, -4);
    func_80031E5C(p);
}
