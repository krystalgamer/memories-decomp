#include "../types.h"
#include "../psyq/rand.h"

typedef struct {
    u8 unk0[0x28];
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s16 unk34;
    s16 unk36;
    s16 unk38;
    s16 unk3A;
    u8 unk3C[0x17];
    u8 unk53;
    u8 unk54;
    u8 unk55[2];
    u8 unk57;
    u8 unk58;
    u8 unk59;
    u8 unk5A;
    u8 unk5B;
    s16 unk5C;
    s16 unk5E;
    u8 unk60;
    u8 unk61;
    u8 unk62[2];
} Rec64;

extern u8 D_8009B142 __attribute__((section(".data")));
extern u8 D_8009B143 __attribute__((section(".data")));
extern u8 D_8009B144 __attribute__((section(".data")));
extern u8 D_8009B318 __attribute__((section(".data")));
extern volatile u16 gInput_wPad1Pressed __attribute__((section(".data")));
extern u8 D_8009B428;
extern Rec64 D_800EB0F8[];

void TextBox_Destroy(u8 *arg0);

s32 func_80043BCC(void) {
    s32 r;
    u32 f;
    s32 g;

    func_800159D8();

    for (;;) {
        D_8009B428 = 0;
        for (;;) {
            rand();
            func_80012D4C();
            f = D_8009B428;
            if ((f & 1) == 0) {
                if ((f & 0x80) == 0) {
                    D_8009B428 = f | 0x80;
                    func_8002CD8C();
                    func_8003594C(0);
                    D_8009B142 = 0xFF;
                    D_8009B143 = 0xFF;
                    D_8009B144 = 0xFF;
                    continue;
                }
                if (f & 0x40) {
                    if (gInput_wPad1Pressed & 0x8C0) {
                        TextBox_Destroy((u8 *)D_800EB0F8);
                        D_8009B428 = 1;
                    }
                    continue;
                }
                g = D_8009B318;
                if (g & 0x80) {
                    continue;
                }
                D_8009B428 = 1;
                if ((g & 0x40) == 0) {
                    func_800156DC();
                }
                func_80015AD8();
                continue;
            }
            if ((f & 0x80) == 0) {
                D_8009B428 = f | 0x80;
                func_8002CD8C();
                func_8018001C(0, 0);
                func_800157DC();
            }
            r = func_80180390();
            if (r != -1) {
                break;
            }
        }
        func_8003FF34();
        Fade_WaitOut();
        func_80180DD0();
        func_8002CD8C();
        if (r != -2) {
            return r;
        }
    }
}
