#include "../types.h"
#include "duel_grid.h"

typedef struct {
    s8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u8 unkC;
    u8 unkD;
    s16 unkE;
    s16 unk10;
    u8 unk12[2];
    s16 unk14;
    u8 unk16[2];
    s8 unk18;
    u8 unk19[7];
} RankEntry;

extern u8 *D_8009B1E8;
extern u8 gDuel_bWinnerSide;
extern RankEntry D_800E9FF0[DUEL_SIDE_COUNT];
extern s32 D_801D5608[16][DUEL_SIDE_COUNT];

s32 Duel_CalcRankScoreChange(s32, s32);

/* Fills the duel result display: rank letters at +0x34 ('D', '@', 'E',
   with the middle one 'B' or 'A' when the winner's first byte is +/-40),
   then both side scores at +0x2C start at 50 and are adjusted through
   Duel_CalcRankScoreChange for each stat, and the raw stats into
   D_801D5608[stat][side]. */
void Duel_CalcRankScore(void) {
    u8 *p;
    RankEntry *e;
    s32 i;
    s32 v;
    s32 *q;

    p = D_8009B1E8;
    e = D_800E9FF0;
    q = &D_801D5608[0][0];
    p[0x34] = 0x44;
    p[0x35] = 0x40;
    p[0x36] = 0x45;
    if (D_800E9FF0[gDuel_bWinnerSide].unk0 == 0x28) {
        p[0x35] = 0x42;
    }
    if (D_800E9FF0[gDuel_bWinnerSide].unk0 == -0x28) {
        p[0x35] = 0x41;
    }

    *(s32 *)(p + 0x30) = 50;
    *(s32 *)(p + 0x2C) = 50;
    for (i = 0; i < DUEL_SIDE_COUNT; i++, e++, q++) {
        *(s32 *)(p + 0x2C + i * 4) += e->unk0;
        v = e->unk18; q[0 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) += Duel_CalcRankScoreChange(6, v);
        v = e->unk14; q[1 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) += Duel_CalcRankScoreChange(7, v);
        q[2 * DUEL_SIDE_COUNT] = e->unkE;
        q[3 * DUEL_SIDE_COUNT] = e->unkB;
        v = e->unk2; q[4 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) += Duel_CalcRankScoreChange(1, v);
        q[5 * DUEL_SIDE_COUNT] = e->unk10;
        q[6 * DUEL_SIDE_COUNT] = e->unkC;
        v = e->unk3; q[7 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) += Duel_CalcRankScoreChange(2, v);
        q[8 * DUEL_SIDE_COUNT] = e->unk7;
        v = e->unk4; q[9 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) += Duel_CalcRankScoreChange(3, v);
        v = e->unk8; q[10 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) += Duel_CalcRankScoreChange(8, v);
        v = e->unk9; q[11 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) += Duel_CalcRankScoreChange(9, v);
        q[12 * DUEL_SIDE_COUNT] = e->unkA;
        v = e->unk5; q[13 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) += Duel_CalcRankScoreChange(4, v);
        v = e->unk6; q[14 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) += Duel_CalcRankScoreChange(5, v);
        v = e->unk1; q[15 * DUEL_SIDE_COUNT] = v;
        *(s32 *)(p + 0x2C + i * 4) += Duel_CalcRankScoreChange(0, v);
    }
}
