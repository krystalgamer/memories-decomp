#include "../types.h"
#include "sound.h"

extern SDValue *g_SDValue __attribute__((section(".data")));

struct Request {
    u8 tag;
    u8 pad01[3];
    s32 f04;
    s32 f08;
    s32 f0C;
    u8 pad10[0x30 - 0x10];
};

extern void func_800471D0(s32, s32, s32, s32, s32, s32);
extern s32 func_80045BE8(struct Request *);

void func_80047788(s32 arg0)
{
    struct Request req;
    SDValue *a;
    SDValue *b;
    SDValue *c;
    s32 step;
    s32 off;
    SDValueLink *entry;

    a = g_SDValue;
    step = (s32)(*(u16 *)((u8 *)a + 2) << 3);
    step = (step + 0x7FF) / 0x800;
    step = step + 1;
    off = (arg0 & 0xFFFF) * 8;
    entry = (SDValueLink *)(off + (s32)a->field_0448);
    func_800471D0(a->field_0438, 0x801E6800,
                  step + *(u16 *)entry, entry->field_0004, 0x800, 0x10);
    b = g_SDValue;
    req.tag = 0x51;
    req.f04 = b->field_0438;
    req.f0C = 0x801E6800;
    func_80045BE8(&req);
    c = g_SDValue;
    off += (s32)c->field_0448;
    entry = (SDValueLink *)off;
    c->field_0438 = c->field_0438 + entry->field_0004;
}
