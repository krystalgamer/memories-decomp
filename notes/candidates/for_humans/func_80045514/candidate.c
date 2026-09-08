#include "../../../../src/types.h"

typedef struct {
    u8 pad_00[0x40];
    u16 flags;
    u8 pad_42[7];
    u8 b49;
    u8 pad_4A[4];
    u16 h4E;
    s32 w50;
    u16 h54;
    u8 pad_56[2];
    u8 *p58;
    s32 w5C;
    s32 w60;
    u8 pad_64[4];
    s32 w68;
    s32 w6C;
    s32 w70;
    s32 w74;
    u8 pad_78[4];
    u8 b7C;
    u8 b7D;
    u8 b7E;
    u8 pad_7F[0x4CC - 0x7F];
    s32 w4CC;
    u8 pad_4D0[0x510 - 0x4D0];
    s16 h510;
    s16 h512;
    u8 pad_514[0x528 - 0x514];
    u32 w528;
    u32 w52C;
    u8 b530;
    u8 b531;
    u8 b532;
    u8 b533;
    u16 h534;
    u8 pad_536[0x1564 - 0x536];
    u8 *p1564;
    u8 pad_1568[0x157A - 0x1568];
    s16 h157A;
    u16 h157C;
    s16 h157E;
    u8 pad_1580[0x1586 - 0x1580];
    s16 h1586;
    u16 h1588;
    u8 b158A;
    u8 pad_158B[0x1618 - 0x158B];
    u8 b1618;
    u8 b1619[0x10];
    u8 b1629[0x10];
    u8 b1639[0x10];
} SD;

typedef struct {
    s32 cmd;
    s32 arg;
    u8 pad_08[4];
    s32 wC;
    s32 w10;
    s32 w14;
    s32 w18;
    s16 h1C;
    u8 b1E;
    u8 b1F;
} Cmd;

typedef struct {
    u32 a;
    u32 b;
} Pair;

typedef struct {
    u16 h0;
    u16 count;
    u8 pad_04[0xC];
    u16 idx[1];
} List;

extern SD *g_SDValue __attribute__((section(".data")));
extern Cmd *D_8009B460 __attribute__((section(".data")));

extern s32 func_80014C40(void *, void *);
extern void func_800476B4(u8 *, s32);
extern u8 func_80045484(void);
extern void SD_ArmBusyCallback(void);
extern s16 func_800496C4(u8 *, s32, s32);
extern void func_80049C40(s32);
extern void func_80049F10(s32, s32);
extern void func_80049CB0(s16);
extern s16 func_80049A64(u8 *, s16);
extern void func_80049AF4(s32, s32);

void func_80045514(void)
{
    switch (g_SDValue->b7C) {
    case 81:
        func_800476B4(g_SDValue->p58, g_SDValue->w50);
        goto clear_7d_7c;

    case 32:
        switch (g_SDValue->b7D) {
        case 0:
            if (func_80014C40(0, 0) != 0) {
                return;
            }
            if (g_SDValue->w4CC != 0) {
                return;
            }
            {
                SD *sd = g_SDValue;
                Cmd *b = D_8009B460;

                b->w10 = sd->w6C;
                b->wC = sd->w68;
                b->arg = sd->w60;
                b->w18 = sd->w74;
                b->b1F = 0;
                b->h1C = 0;
                b->w14 = sd->w70;
                D_8009B460->b1E = 0;
            }
            D_8009B460->cmd = g_SDValue->w5C;
            if (D_8009B460->w18 == 0 && D_8009B460->w14 == 0) {
                g_SDValue->b7D = 0;
                goto clear_7c;
            }
            switch (g_SDValue->w5C & 0xF0) {
            case 0x10:
                D_8009B460->cmd = 4;
                func_80014C40(D_8009B460, g_SDValue->b1629);
                break;
            case 0x20:
                D_8009B460->cmd = 5;
                func_80014C40(D_8009B460, g_SDValue->b1619);
                break;
            case 0x40:
                D_8009B460->cmd = 6;
                func_80014C40(D_8009B460, g_SDValue->b1639);
                break;
            }
            SD_ArmBusyCallback();
            g_SDValue->b7D = g_SDValue->b7D + 1;
            return;
        case 1:
            break;
        default:
            return;
        }
        if (g_SDValue->b1618 != 0) {
            return;
        }
        g_SDValue->b7D = 0;
        g_SDValue->b7C = 0;
        if ((func_80045484() & 0xFF) != 0) {
            return;
        }
        g_SDValue->flags = g_SDValue->flags & 0xFFFC;
        return;

    case 17:
        switch (g_SDValue->b7D) {
        case 0:
            if (g_SDValue->h510 == 0) {
                g_SDValue->b7D = 1;
                return;
            }
            if (g_SDValue->h512 < 0) {
                return;
            }
            g_SDValue->h512 = -0x20;
            g_SDValue->b49 = 0;
            return;
        case 1:
            break;
        default:
            return;
        }
        g_SDValue->h534 = 0xFFFF;
        if ((func_80045484() & 0xFF) != 0) {
            goto clear_7d_7c;
        }
        g_SDValue->b7D = 0;
        g_SDValue->flags = g_SDValue->flags & 0xFFF8;
        g_SDValue->b7C = 0;
        return;

    case 33:
        if (func_80014C40(0, 0) != 0) {
            goto clear_7d_7c;
        }
        {
            Pair *e = (Pair *)(g_SDValue->p58 + g_SDValue->h4E * 8);

            D_8009B460->w10 = 0;
            D_8009B460->w18 = 0;
            D_8009B460->wC = 0;
            D_8009B460->w14 = 0;
            D_8009B460->h1C = 0;
            D_8009B460->b1F = g_SDValue->b530;
            D_8009B460->b1E = g_SDValue->b531;
            D_8009B460->cmd = 6;
            D_8009B460->arg = (e->a & 0xFFFFFF) + g_SDValue->w50;
            func_80014C40(D_8009B460, 0);
        }
        goto clear_7d_7c;

    case 36:
        if (func_80014C40(0, 0) != 0) {
            goto clear_7d_7c;
        }
        {
            Pair *e = (Pair *)(g_SDValue->p58 + g_SDValue->h4E * 8);
            u32 a = e->a;
            u32 b = e->b;

            g_SDValue->w528 = a & 0xFFFFFF;
            g_SDValue->b531 = (a & 0x1F000000) >> 24;
            g_SDValue->w52C = b & 0xFFFFFF;
            g_SDValue->b530 = (b & 0x1F000000) >> 24;
            g_SDValue->b532 = b >> 31;
            g_SDValue->b533 = a >> 29;
            g_SDValue->w528 = g_SDValue->w528 + g_SDValue->w50;
            g_SDValue->w52C = g_SDValue->w52C + g_SDValue->w50;
            D_8009B460->w10 = 0;
            D_8009B460->w18 = 0;
            D_8009B460->wC = 0;
            D_8009B460->w14 = 0;
            D_8009B460->h1C = (u16)g_SDValue->w52C - (u16)g_SDValue->w528 + 0x10;
            D_8009B460->b1F = g_SDValue->b530;
            D_8009B460->b1E = g_SDValue->b531;
            D_8009B460->cmd = 6;
            D_8009B460->arg = g_SDValue->w528;
            func_80014C40(D_8009B460, 0);
        }
        goto clear_7d_7c;

    case 41:
    case 42:
        if (g_SDValue->b7E != 0 && g_SDValue->h512 != 0) {
            return;
        }
        g_SDValue->b7C = 0;
        goto clear_7d;

    case 72:
        if (g_SDValue->h157A != 0) {
            u8 *list = g_SDValue->p1564;

            g_SDValue->h157A = func_800496C4(list + 0x50, 0,
                                             *(s32 *)(list + 0xC));
            if ((s16)g_SDValue->h157A != 0) {
                g_SDValue->b7C = 0;
                return;
            }
        }
        if (g_SDValue->h157E == 0) {
            if ((g_SDValue->flags & 0x80) != 0) {
                func_80049C40(0);
                func_80049F10(0, 0);
                g_SDValue->h1586 = 0;
                g_SDValue->h1588 = 0;
                g_SDValue->flags = g_SDValue->flags & 0xFF7F;
            }
            func_80049CB0(g_SDValue->h157E);
            g_SDValue->h157E = -1;
        }
        {
            List *l = (List *)g_SDValue->p1564;
            u32 i = g_SDValue->h4E & 0xF;

            if (i >= l->count) {
                g_SDValue->b7C = 0;
                return;
            }
            g_SDValue->h157E = func_80049A64((u8 *)l + l->idx[i] * 16,
                                             g_SDValue->h157A);
            if ((s16)g_SDValue->h157E == 0) {
                g_SDValue->h157C = g_SDValue->h4E;
                func_80049AF4(1, 1);
                g_SDValue->b158A = 0xFF;
                g_SDValue->flags = g_SDValue->flags | 0x80;
                g_SDValue->h1588 = g_SDValue->h54;
            }
        }
        g_SDValue->b7C = 0;
        goto clear_7d;

    case 0:
        break;
    }

clear_7d_7c:
    g_SDValue->b7D = 0;
clear_7c:
    g_SDValue->b7C = 0;
    return;

clear_7d:
    g_SDValue->b7D = 0;
}
