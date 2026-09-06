#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

extern void func_80044DC0(s32);
extern s32 func_80049F50(void);
extern void func_80049C40(s32);
extern void func_80049CB0(s32);
extern void func_800498F8(s32);

/* Each block re-reads g_SDValue rather than caching it once: the driver block
   is reachable through the global, so every store through one pointer forces
   the next read. */
void func_80046DE8(void)
{
    SDValue *p;
    SDValue *q;
    SDValue *r;
    SDValue *s;
    SDValue *t;
    SDValue *u;
    SDValue *w;
    s32 i;
    s32 value;

    p = g_SDValue;
    p->mix_scale = ((u8 *)p)[0x1649];
    *(s16 *)((u8 *)p + 0x44) = ((u8 *)p)[0x164A];
    func_80044DC0(0);
    q = g_SDValue;
    q->field_0512 = 0;
    if (q->field_157E != -1) {
        if ((s16)func_80049F50() != 1) {
            func_80049C40(g_SDValue->field_157E);
            g_SDValue->flags_0040 &= 0xFF7F;
        }
        func_80049CB0(g_SDValue->field_157E);
    }
    r = g_SDValue;
    value = r->field_157A;
    *(s16 *)((u8 *)r + 0x157C) = -1;
    r->field_157E = -1;
    if (value != -1) {
        func_800498F8(value);
    }
    s = g_SDValue;
    s->field_1578 = -1;
    s->field_157A = -1;
    s->field_1588 = 0;
    SpuSetKey(0, 0xFFFFFF);
    g_SDValue->voice_active_mask = 0;
    g_SDValue->field_0435 = 0;
    for (i = 0; i < 4; i++) {
        *((u8 *)g_SDValue + i + 0x40C) = 0;
        g_SDValue->voice_ids[i] = 0;
    }
    t = g_SDValue;
    ((u8 *)t)[0x7C] = 0;
    t->command_count = 0;
    ((u8 *)g_SDValue)[0x7D] = 0;
    u = g_SDValue;
    ((u8 *)u)[0x7E] = 0;
    w = g_SDValue;
    *(s16 *)((u8 *)u + 0x4E) = 0;
    *(s32 *)((u8 *)u + 0x50) = 0;
    *(s32 *)((u8 *)u + 0x54) = 0;
    *(s32 *)((u8 *)u + 0x58) = 0;
    w->flags_0040 = 0;
}
