#include "../types.h"
#include "duel_card.h"

extern u8 D_800EAE88[];

extern s32 Duel_CollectFieldCardsByType(u8 **out, s32 arg1, s32 arg2);
extern s32 Duel_CheckEquip(s32 card_a, s32 card_b);
extern s32 func_80026C0C(s32 arg0);
extern s32 Duel_CollectFieldCardsBelowType(u8 **out, s32 arg1, s32 arg2);

s32 func_80026DC8(void) {
    u8 *a[DUEL_FIELD_ROW_SIZE + 1];
    u8 *b[DUEL_FIELD_ROW_SIZE + 1];
    u8 **q;
    u8 **r;
    u8 *e;
    u8 *f;
    s32 n;
    s32 u;
    s32 w;

    if (Duel_CollectFieldCardsByType(b, 0, 0x17) != 0) {
        Duel_CollectFieldCardsByType(a, DUEL_FIELD_ROW_SIZE, -1);
        e = b[0];
        if (e != (u8 *)0) {
            r = b;
            do {
                f = a[0];
                if (f != (u8 *)0) {
                    q = a;
                    do {
                        if (Duel_CheckEquip(*(s16 *)(e + 0xC),
                                            *(s16 *)(f + 0xC)) != 0) {
                            D_800EAE88[0] =
                                *(s8 *)(e + 0x18) % DUEL_FIELD_ROW_SIZE + 0xB;
                            D_800EAE88[1] = 0;
                            D_800EAE88[6] =
                                *(s8 *)(f + 0x18) % DUEL_FIELD_ROW_SIZE + 1;
                            D_800EAE88[7] = 0;
                            D_800EAE88[8] = 0;
                            return 0;
                        }
                        q++;
                        f = *q;
                    } while (f != (u8 *)0);
                }
                r++;
                e = *r;
            } while (e != (u8 *)0);
        }

        n = func_80026C0C(DUEL_FIELD_ROW_SIZE);
        if (n < 0) {
            return 1;
        }
        Duel_CollectFieldCardsBelowType(a, 0, 0x14);
        e = b[0];
        if (e != (u8 *)0) {
            r = b;
            do {
                f = a[0];
                if (f != (u8 *)0) {
                    q = a;
                    do {
                        if (Duel_CheckEquip(*(s16 *)(e + 0xC),
                                            *(s16 *)(f + 0xC)) != 0) {
                            u = *(s8 *)(f + 0x18);
                            D_800EAE88[0] =
                                u % DUEL_FIELD_ROW_SIZE + 0xB;
                            w = *(s8 *)(e + 0x18);
                            D_800EAE88[2] = 0;
                            D_800EAE88[7] = 0;
                            D_800EAE88[8] = 0;
                            D_800EAE88[6] =
                                n % DUEL_FIELD_ROW_SIZE + 1;
                            D_800EAE88[1] =
                                w % DUEL_FIELD_ROW_SIZE + 0xB;
                            return 0;
                        }
                        q++;
                        f = *q;
                    } while (f != (u8 *)0);
                }
                r++;
                e = *r;
            } while (e != (u8 *)0);
        }
    }
    return 1;
}
