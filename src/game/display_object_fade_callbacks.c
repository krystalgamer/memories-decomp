#define D_8009B0D8_IS_VOLATILE
#include "../types.h"
#include "duel_effect.h"
#include "graphics_frame.h"
#include "display_object_fade.h"
#include "func_80039AD4.h"
#include "display_object_fade_callbacks.h"

void func_80039AFC(DuelEffectChannel *record)
{
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        record->field_15 = 2;
        *(s32 *)&record->field_04 = 0;
        record->field_14 = 32;
    }
    record->field_14 = record->field_14 - D_8009B0D8 * 2;
    if ((record->field_13 & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE) == 0) {
        record->field_04 = record->field_04 + D_8009B0D8 * 16;
        if ((s8)record->field_04 < 0) {
            record->field_04 = 128;
            record->field_14 = 16;
            record->field_13 |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
        record->field_06 = record->field_04;
    } else {
        record->field_05 = record->field_05 + D_8009B0D8 * 16;
        if ((s8)record->field_05 < 0) {
            record->field_14 = 0;
            record->field_15 = 0;
            record->field_13 = 0;
        }
        record->field_07 = record->field_05;
    }
}

void func_80039BE0(DuelEffectChannel *p)
{
    s32 v;

    if (!DisplayObjectFade_MarkInitialized(p)) {
        p->field_15 = 2;
        *(u32 *)&p->field_04 = 0x80808080;
        p->field_14 = 0;
    }
    if (!(p->field_13 & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        v = p->field_04 - (D_8009B0D8 << 4);
        if (v <= 0) {
            p->field_13 |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
            v = 0;
        }
        p->field_04 = v;
        p->field_05 = v;
    } else {
        v = p->field_06 - (D_8009B0D8 << 4);
        if (v <= 0) {
            func_80039AD4(p);
            v = 0;
        }
        p->field_06 = v;
        p->field_07 = v;
    }
}

void func_80039C94(DuelEffectChannel *arg0) {
    if (DisplayObjectFade_MarkInitialized(arg0) == 0) {
        s32 a;
        s32 b;

        arg0->field_15 = 1;
        a = arg0->field_0C;
        b = arg0->field_0E;
        arg0->field_08 = 0;
        arg0->field_09 = 0;
        arg0->field_0A = 0;
        arg0->field_04 = ((s16)a >> 4) + ((s16)b >> 3) + 1;
    }

    if (!(arg0->field_13 & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        s32 v = arg0->field_04 - 1;

        arg0->field_04 = v;

        if ((u8)v == 0) {
            arg0->field_13 |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
    } else {
        s32 v = arg0->field_08 + 4;

        arg0->field_0A = v;
        arg0->field_09 = v;
        arg0->field_08 = v;

        if (v >= 0x40) {
            func_80039AD4(arg0);
        }
    }
}
