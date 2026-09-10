/*
 * Stages the five-card Exodia presentation, sparkle phases, centre burst, and
 * result handoff. Current best under gcc_2_8_1_g8_split: 280/280
 * instructions, opcode multiset distance 0, and 114 differing positions.
 *
 * The callback address remains pinned to preserve retail's
 * materialise-then-move sequence. Address hoists, index-first arithmetic, and
 * locals spanning intervening stores reproduce the rest of the allocation.
 *
 * Residual: two opposite delay-slot choices. This build fills the branch slot
 * before the D_8009B260 test where retail leaves a nop, while retail fills the
 * later jump slot with the effect-record store. The multiset and instruction
 * count are identical; the difference is placement, not missing operations.
 */
#include "../types.h"
#include "../game/duel_card_pick_cursor.h"
#include "../game/duel_card_layout.h"
#define MAIN_MODE_STATE_NEXT_IN_DATA
#define MAIN_MODE_STATE_ACTIVE_IN_DATA
#include "../game/main_mode_state.h"

extern u16 D_8009B23A;
extern u16 D_8009B162;
extern u16 D_8009B1D0;
extern u8 D_8009B1B9;
extern u8 D_8009B1D5;
extern u8 gDuel_bWinnerSide;
extern u8 *D_8009B214;
extern u8 *D_8009B21C;
extern DuelCardPickCursor *D_8009B1B4;
extern u8 *D_8009B17C;

extern u8 D_8015C424[];
extern u8 D_80090918[];
extern u8 *D_800E9EF0[];
extern u8 D_800E9F10[];
extern s32 D_800E9F04[];
extern u8 D_800E9FF0[];
extern u8 D_800EA030[];
extern u8 D_800E9ECF[];
extern u8 D_8009B260[];
extern u16 D_800EF658[];
extern u8 D_8009B369 __attribute__((section(".data")));

extern void func_8001EC70(void);
extern void func_8004036C(u8 *);
extern void func_80015C84(void);
extern void SD_BGMFadeOutWithStep(s32);
extern void SD_SEPlayFull(s32);
extern s32 func_80042B40(s32);
extern u8 *func_8002C604(s32);
extern s32 rand(void);
extern s32 Rand_GetInterval(s32);
extern void func_8002C68C(s32);
extern void func_8003FF88(s32);
extern void func_800156DC(void);
extern void func_800472A8(s32);
extern void func_80059C18(s32);

void func_80018FEC(void)
{
    u16 flags;
    u8 *obj;
    u8 *pose;
    u8 *rec;
    u8 *fx;
    s32 i;
    s32 anim;
    s32 n;
    s32 r;
    s32 t14;
    u8 *cards;
    u8 *poses;
    u8 **objs;
    register void (*fn)(void) __asm__("$2");
    s32 fnv;
    u8 *g;
    s32 py;
    s32 t;
    s32 k;
    u8 **slot;
    s8 side;
    u8 *other;

    flags = D_8009B23A;
    if ((flags & 0x8000) == 0) {
        i = 0;
        cards = D_8015C424;
        poses = D_80090918;
        fn = func_8001EC70;
        fnv = (s32)fn;
        objs = D_800E9EF0;
        D_8009B23A = flags | 0x8000;
        obj = D_8009B214;
        D_8009B1B4 = (DuelCardPickCursor *)&D_800E9F10[D_8009B1D5 * 0x70];
        *(s16 *)(obj + 0x28) = -0x40;
        *(u16 *)(obj + 0x2C) = 0x10;
        obj[0x6C] = 1;
        *(s32 *)(obj + 0x24) = fnv;
        *(u16 *)(obj + 0x2A) = *(u16 *)(obj + 0x32);
        obj = D_8009B21C;
        *(s16 *)(obj + 0x28) = 0x180;
        *(u16 *)(obj + 0x2C) = 0x10;
        obj[0x6C] = 1;
        *(s32 *)(obj + 0x24) = fnv;
        *(u16 *)(obj + 0x2A) = *(u16 *)(obj + 0x32);
        rec = D_800EA030;
next_obj:
        obj = *(u8 **)rec;
        g = (u8 *)(obj[0x6A] * DUEL_CARD_RECORD_SIZE + (u32)cards + 0x48000);
        anim = *(s16 *)(g + 0x36C0) - 0x11;
        pose = (u8 *)(anim * 3 + (u32)poses);
        *(s16 *)(obj + 0x28) = pose[1] - 0x1A;
        py = pose[2];
        *(u16 *)(obj + 0x2C) = 0xB4;
        obj[0x6C] = 1;
        *(s32 *)(obj + 0x24) = fnv;
        *(s16 *)(obj + 0x2A) = py - 0x1E;
        objs[pose[0]] = obj;
        *(u8 **)rec = 0;
        i++;
        rec += 0xC;
        if (i < 5) {
            goto next_obj;
        }
        D_800E9F04[0] = 0;
        func_8004036C(*(u8 **)((u8 *)D_8009B1B4 + 4));
        D_8009B162 = 8;
        D_8009B1D0 = 0;
        D_8009B1B9 = 0;
        D_8009B17C = 0;
        D_8009B23A = D_8009B23A | 0x4000;
        func_80015C84();
        D_800E9ECF[0] = 2;
        SD_BGMFadeOutWithStep(2);
        return;
    }
    if (flags & 0x4000) {
        if (flags & 0x2000) {
            if ((D_8009B260[0] & 1) != 0) {
                return;
            }
            D_8009B23A = flags & 0xBFFF;
            D_8009B1B9 = 0;
            D_8009B1D0 = 0;
            return;
        }
        if (D_8009B17C != 0) {
            if (D_8009B17C[0x1D] == 0) {
                return;
            }
            SD_SEPlayFull(0x1D);
            if (func_80042B40(1) == 0) {
                D_8009B23A = D_8009B23A | 0x2000;
                return;
            }
        }
        fx = func_8002C604(0x13);
        D_8009B17C = fx;
        *(u16 *)(fx + 0) = (rand() & 0xFF) + 0x20;
        r = Rand_GetInterval(0xB0);
        n = D_8009B1B9;
        t14 = *(s32 *)(fx + 0x14);
        *(u16 *)(fx + 2) = r + 0x20;
        *(s32 *)(fx + 0x14) = t14 + ((n & 3) << 13);
        D_8009B1B9 = n + 1;
        return;
    }
    if (flags & 0x2000) {
        t = D_8009B1D0 - 1;
        D_8009B1D0 = t;
        if ((s16)t > 0) {
            return;
        }
        if ((s8)D_8009B1B9 >= 5) {
            D_8009B23A = (flags & 0xDFFF) | 0x1000;
            fx = func_8002C604(0x13);
            *(u16 *)(fx + 0) = 0xA0;
            *(u16 *)(fx + 2) = 0x78;
            *(s32 *)(fx + 0x14) = *(s32 *)(fx + 0x14) + 0x8000;
            D_8009B17C = fx;
            return;
        }
        D_8009B1D0 = 4;
        fx = func_8002C604(0);
        k = (s8)D_8009B1B9;
        slot = &D_800E9EF0[k];
        *(u16 *)(fx + 0) = *(u16 *)(*slot + 0x30) + 0x1A;
        *(u16 *)(fx + 2) = *(u16 *)(*slot + 0x32) + 0x1E;
        *(s32 *)(fx + 0x14) = *(s32 *)(fx + 0x14) + ((k << 12) + 0xA000);
        *(u16 *)(fx + 0x1A) = 9;
        SD_SEPlayFull(0x17);
        D_8009B1B9 = D_8009B1B9 + 1;
        return;
    }
    if (flags & 0x1000) {
        if (D_8009B17C[0x1D] == 0) {
            return;
        }
        D_8009B23A = flags & 0xEFFF;
        SD_SEPlayFull(0x1D);
        func_8002C68C(0x18);
        func_8003FF88(0x8021);
        return;
    }
    func_800156DC();
    D_800EF658[0] = 0x309;
    side = D_8009B1D5;
    gDuel_bWinnerSide = side;
    D_800E9FF0[(u8)side * 0x20] = 0x28;
    other = &D_800E9FF0[(D_8009B1D5 ^ 1) * 0x20];
    *(u16 *)(other + 0x14) = 0;
    *(u16 *)(other + 0x12) = 0;
    func_800472A8(0x7310);
    func_80059C18(0x7310);
    D_8009B369 = 1;
    D_8009B269 = 3;
    D_8009B26C = 1;
}
