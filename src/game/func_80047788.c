#include "../types.h"
#include "sound.h"

extern SDValue *g_SDValue __attribute__((section(".data")));

extern void func_800471D0(s32, s32, s32, s32, s32, s32);

void func_80047788(s32 arg0)
{
    SDCommand req;
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
    off = (arg0 & SD_VALUE_LINK_INDEX_MASK) * SD_VALUE_LINK_RECORD_SIZE;
    entry = (SDValueLink *)(off + (s32)a->field_0448);
    func_800471D0(a->field_0438, 0x801E6800,
                  step + *(u16 *)entry, entry->field_0004, 0x800, 0x10);
    b = g_SDValue;
    req.command = 0x51;
    req.field_0004 = b->field_0438;
    req.field_000C = 0x801E6800;
    SD_EnqueueCommand(&req);
    c = g_SDValue;
    off += (s32)c->field_0448;
    entry = (SDValueLink *)off;
    c->field_0438 = c->field_0438 + entry->field_0004;
}
