#include "../types.h"

#include "sound.h"

struct Request {
    u8 tag;
    u8 pad01;
    s16 f02;
    s32 f04;
    s32 f08;
    s32 f0C;
    u8 pad10[0x30 - 0x10];
};

extern void func_800464F0(void);
extern void func_80044DA0(void);
extern void func_80045BE8(struct Request *);

s32 func_80045208(u16 arg0, s32 unused)
{
    SDValue *a = g_SDValue;
    u16 code = arg0;
    register u8 **table asm("$2");
    s32 kind;
    struct Request req;

    if (a->flags_004A & 0x80) {
        if ((a->flags_004A & 0x40) || code <= 0x9FFF) {
            if (arg0 & 0x8000) {
                *(s16 *)((u8 *)a + 0x534) = arg0;
                switch (arg0 & 0xF000) {
                case 0x8000:
                    code = arg0 + 0x8000;
                    table = *(u8 ***)((u8 *)a + 0x51C);
                    kind = 0x50;
                    break;
                case 0x9000:
                    code = arg0 + 0x7000;
                    table = *(u8 ***)((u8 *)a + 0x518);
                    kind = 0x60;
                    break;
                default:
                    code = code + 0x6000;
                    kind = 0x70;
                    table = *(u8 ***)((u8 *)g_SDValue + 0x520);
                    break;
                }
                {
                    register s32 first asm("$16");
                    register u8 *const second asm("$17") = (u8 *)table + 8;

                    first = table ? (s32)*table : (s32)*table;

                    func_800464F0();
                    req.tag = 0x24;
                    req.f02 = code;
                    req.f04 = first;
                    req.f0C = (s32)second;
                    req.f08 = kind;
                    func_80045BE8(&req);
                    func_80044DA0();
                }
                g_SDValue->flags_0040 = (g_SDValue->flags_0040 | 1) & 0xFFFB;
                return 1;
            }
        }
    }
    return 0;
}
