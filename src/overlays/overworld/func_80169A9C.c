#include "../../types.h"

typedef struct { u8 pad0[8]; u16 unk8; u8 pad10[38]; u16 unk30; u16 unk32; } Obj;
typedef struct { u8 pad0[12]; u16 a; u16 b; u8 pad16[50]; } Slot;

extern u16 D_800C4E68;
extern u8 D_800C4DC9;
extern u8 D_800C4DCC;
extern u8 D_800C4D82;
extern u8 D_8011464E;
extern u8 D_8011464F;
extern u16 D_8011C410;
extern Slot D_80169E54[];
extern Obj *D_8016A278;
extern Obj *D_8016A288;
extern u8 D_8016A29C;
extern u8 D_8016A2BC;
extern u8 D_8016A2BD;
extern u8 D_8016A2C8;
extern u8 D_8016A2C9;

extern s32 func_80169230(void);
extern void func_801680E4(s32);
extern void func_801682D0(s32);
extern Obj *func_80168A48(s32);
extern void func_80066574(Obj *);
extern void func_80168624(void);
extern void func_80021EA4(void);
extern void func_80168040(void);
extern void func_80065BFC(s32);
extern void func_800158C8(void);
extern s32 func_801697D0(void);

void func_80169A9C(void)
{
    s32 r;

    if (D_800C4E68 & 4) {
        D_8016A288->unk8 ^= 0x40;
    }
    if (D_8016A29C != 0) {
        if (func_80169230()) {
            return;
        }
        D_8016A29C = 0;
        func_801680E4(D_8016A2BC);
        func_801682D0(D_8016A2BC);
        if (D_8016A2BC >= 10) {
            if (D_8016A278 == 0) {
                D_8016A278 = func_80168A48(D_8016A2BC);
            }
            D_8016A278->unk30 = D_80169E54[D_8016A2BC].a;
            D_8016A278->unk32 = D_80169E54[D_8016A2BC].b;
        } else {
            func_80066574(D_8016A278);
            D_8016A278 = 0;
        }
    }
    if (D_800C4E68 & 0x100) {
        D_8016A2C9 ^= 1;
    }
    if (D_8016A2C9 != 0) {
        func_80168624();
        return;
    }
    if (D_8016A2BD != 0) {
        if ((D_8016A2BD & 0x80) == 0) {
            D_8016A2BD |= 0x80;
            func_80021EA4();
            D_8011464F = 2;
            func_80168040();
            func_80066574(D_8016A278);
            func_80065BFC(4);
        }
        D_8011C410 -= 2;
        func_800158C8();
        if ((D_8011464E & 0x80) == 0) {
            D_800C4D82 = D_8016A2BC + 32;
            D_800C4DCC = 2;
        }
        return;
    }
    if (D_800C4E68 & 0x800) {
        D_800C4DCC = D_800C4DC9;
    }
    D_8016A2C8 = D_8016A2BC;
    r = func_801697D0();
    if (r >= 0) {
        if (r & 0x8000) {
            D_8016A2BD = 1;
        } else {
            if ((D_8016A2BC = r) < 10) {
                func_80066574(D_8016A278);
                D_8016A278 = 0;
            }
            func_80168040();
            D_8016A29C = 1;
            return;
        }
    }
}
