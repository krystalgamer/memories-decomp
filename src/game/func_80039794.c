#include "../types.h"
#include "func_8004036C.h"

typedef struct {
    u8 pad_00[0x64];
} Slot;

typedef struct {
    void *obj;
    u16 flags;
} Ent;

typedef struct {
    u16 lo;
    u16 hi;
} TblEnt;

extern Slot D_800EB0F8[];
extern TblEnt D_801D9000[];
extern s16 D_8009B35A;
extern u8 D_8009B356;
extern u8 D_8009B0C1 __attribute__((section(".data")));
extern u16 gInput_wPad1Pressed __attribute__((section(".data")));

extern void func_800393B0(void *);
extern void *Dialog_OpenChoice(void *);
extern void SD_SEPlayFull(s32);
extern void func_8003B50C(s32);
extern void DuelEffect_ProcessEntries(void *);

void func_80039794(void)
{
    Slot *p;
    Ent *q;
    s32 reset_value;
    TblEnt *table;
    s32 n;
    s32 cnt;
    s32 idx;
    s32 kind;
    s32 arg;
    u16 f;

    p = D_800EB0F8;
    n = 4;
    reset_value = -1;
    table = D_801D9000;
    q = (Ent *)((u8 *)p + 0x30);
    do {
        if (q->flags & 0x8000) {
            D_8009B35A = 0;
            if ((q->flags & 0x2000) == 0) {
                D_8009B35A = reset_value;
                cnt = -1;
                for (;;) {
                    func_800393B0(p);
                    cnt++;
                    f = q->flags;
                    if (f & 0x2000) {
                        if (f & 8) {
                            q->obj = Dialog_OpenChoice(p);
                        }
                        break;
                    }
                    if (f & 0x1C00) {
                        goto reset;
                    }
                    if (cnt >= D_8009B0C1) {
                        break;
                    }
                    continue;
reset:
                    D_8009B35A = reset_value;
                }
            } else {
                if (q->flags & 8) {
                    if (gInput_wPad1Pressed & 0xC0) {
                        q->flags &= 0xFFF7;
                        func_8004036C(q->obj);
                        q->obj = 0;
                        SD_SEPlayFull(0xB);
                    }
                }
            }
            arg = -1;
            idx = D_8009B35A;
            if (idx >= 0) {
                kind = table[idx].hi & 7;
                if (kind == 4) {
                    arg = D_8009B356;
                } else {
                    D_8009B356 = kind;
                    arg = kind;
                }
            }
            func_8003B50C(arg);
            DuelEffect_ProcessEntries(p);
        }
        q = (Ent *)((u8 *)q + 0x64);
        n--;
        p++;
    } while (n != 0);
}
