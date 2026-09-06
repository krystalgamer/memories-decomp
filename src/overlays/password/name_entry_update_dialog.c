#include "../../types.h"

typedef struct {
    u8 pad0[8];
    u16 f8;
    u8 pad10[38];
    s16 f48;
    s16 f50;
    u8 pad52[44];
    s16 f96;
    u8 pad98[10];
    u8 f108;
} Caret;

typedef struct {
    u8 pad0[44];
    Caret *f44;
    s32 f48;
    u16 f52;
    u16 f54;
    u8 pad56[33];
    u8 f89;
} Box;

extern u16 D_8016D4D2;
extern u8 D_8016D400;
extern s8 D_8009B34D;
extern u16 D_8009B398;
extern u8 D_801B125A[];
extern u8 D_800EB0F8[];
extern Box D_800EB1C0;
extern u8 *D_8016D418;
extern u8 D_8016D41C;

extern void func_8003B6AC(s32, s32);
extern Box *func_80035BE4(s32, s32, s32, s32, s32, s32);
extern void func_80039A14(Box *);
extern s32 func_800374F4(Box *);
extern void func_80039794(void);
extern void *func_8004002C(void);
extern Caret *func_800400AC(void *, s32);
extern void func_800404CC(Caret *, s32, s32, s32, s32, s32, s32, s32);
extern void func_80042918(Caret *);
extern void func_800428EC(Caret *, s32);
extern void func_80043178(Caret *);
extern void func_80043230(Caret *, s32, s32, s32);
extern void func_80035B7C(Box *);
extern void func_80039934(Box *, s32, s32);
extern void SD_SEPlayFull(s32);
extern void func_8003FF34(void);
extern void func_80015B00(void);
extern Caret *func_80042B40(s32);
extern void func_8004036C(Caret *);
extern void func_8003BC40(u8 *, u8 *, s32);
extern void func_80039A60(u8 *);
extern s32 NameEntry_AdjustLength(s32, s32);
extern void func_8016913C(void);

void NameEntry_UpdateDialog(void)
{
    Box *box;
    Caret *caret;
    u8 flags;
    u8 fb;
    u8 fc;
    u8 fd;
    u8 fe;
    u8 ff;
    u8 fg;
    s32 id;
    s16 pos;
    u8 *p;
    u8 *next;
    u8 *panel;
    s32 c;
    s32 term;

    if (D_8016D4D2 != 0) {
        flags = D_8016D400;
        if ((flags & 4) == 0) {
            D_8016D400 = flags | 4;
            func_8003B6AC(2, 2);
            box = func_80035BE4(2, D_8016D4D2 & 0xFFF, 16, 248,
                                 288, 48);
            box->f89 = 20;
            id = D_8016D4D2;
            if ((id & 0x8000) == 0) {
                if ((id & 0x4000) == 0) {
                    func_80039A14(box);
                    D_8009B34D = 0;
                }
                box->f48 = func_800374F4(box);
            } else {
                box->f52 |= 8;
                do {
                    func_80039794();
                } while (box->f48 == 0);
            }
            caret = func_800400AC(func_8004002C(), 2);
            func_800404CC(caret, 16, 248, 0, 0, 0, 23, 257);
            func_80042918(caret);
            func_800428EC(caret, 19);
            caret->f8 |= 8;
            box->f44 = caret;
            func_80043178(caret);
            caret->f96 = -1024;
            D_8016D400 |= 2;
            return;
        }
        box = &D_800EB1C0;
        caret = box->f44;
        if ((flags & 2) != 0) {
            pos = caret->f96;
            if (pos >= 0) {
                caret->f96 = pos - 85;
                func_80043230(caret, 16, 248, (s16)(pos - 85));
                if (caret->f96 < 0) {
                    caret->f48 = 16;
                    fb = D_8016D400;
                    caret->f50 = 248;
                    D_8016D400 = fb & 0xF9;
                    func_80035B7C(box);
                    D_8016D4D2 = 0;
                    return;
                }
            } else {
                caret->f96 = pos + 85;
                func_80043230(caret, 16, 176, (s16)(pos + 85));
                if (caret->f96 >= 0) {
                    caret->f48 = 16;
                    fc = D_8016D400;
                    caret->f50 = 176;
                    D_8016D400 = fc & 0xFD;
                }
            }
            func_80039934(box, caret->f48, caret->f50);
            return;
        }
        if ((D_8016D4D2 & 0x8000) == 0) {
            if ((D_8009B398 & 0xE0) == 0) {
                SD_SEPlayFull(11);
                return;
            }
        } else {
            func_80039794();
            if ((*(u32 *)&box->f52 & 0x2008) != 0x2000) {
                return;
            }
        }
        if (D_8009B34D != 0) {
            fd = D_8016D400;
            D_8016D400 = fd & 0xDF;
        }
        if ((D_8016D400 & 0x20) != 0) {
            D_8016D4D2 = 0;
        }
        func_80043178(caret);
        fe = D_8016D400;
        caret->f96 = 1024;
        D_8016D400 = fe | 2;
        return;
    }
    flags = D_8016D400;
    if ((flags & 0x20) != 0) {
        SD_SEPlayFull(45);
        func_8003FF34();
        func_80015B00();
        D_8016D400 = D_8016D400 | 0x10;
        return;
    }
    if ((flags & 0x80) != 0) {
        caret = func_80042B40(6);
        if (caret == 0) {
            return;
        }
        if ((caret->f108 & 0x40) == 0) {
            return;
        }
        func_8004036C(caret);
        ff = D_8016D400;
        D_8016D400 = ff & 0x7F;
        func_8003BC40(D_801B125A, D_8016D418, 6);
        func_80035BE4(3, 254, 112, 204, 96, 16);
        panel = D_800EB0F8;
        panel[390] = 16;
        panel[391] = 16;
        func_80039A60(panel + 300);
        NameEntry_AdjustLength(1, 6);
        return;
    }
    if (func_80042B40(1) != 0) {
        return;
    }
    if (func_80042B40(2) != 0) {
        return;
    }
    if ((D_8016D400 & 0x40) == 0) {
        func_8016913C();
        return;
    }
    term = 0xFF;
    p = D_801B125A;
    c = *p;
    next = 0;
    goto ztest;
scan:
    if (*p >= 0xF0) {
        p++;
    }
    next = p + 1;
    p = next;
    goto load;
zloop:
    p++;
load:
    c = *p;
ztest:
    if (c == 0) {
        goto zloop;
    }
    if (c != 0xFF) {
        goto scan;
    }
    fg = D_8016D400;
    D_8016D400 = fg & 0xBF;
    if (next != 0) {
        *next = term;
        D_8016D4D2 = 0x80F5;
        D_8016D41C = 0;
        D_8016D400 |= 0x20;
        SD_SEPlayFull(48);
        return;
    }
    SD_SEPlayFull(9);
}
