#define D_8009B0D8_IS_VOLATILE
#include "../types.h"
#include "../psyq/rand.h"
#include "../psyq/libgte.h"
#include "duel_effect.h"
#include "graphics_frame.h"
#include "display_object_fade.h"
#include "func_80039B3C.h"
#include "display_object_fade_callbacks.h"
#include "duel_effect_entry_occupancy.h"
#include "graphics_frame_controls.h"
#ifdef VERSION_EUROPE
#include "european_glyph_fade_callbacks.h"
#endif

#define DUEL_EFFECT_FIELD_04_WORD(type, object) \
    (*(type *)&(object)->field_04)

/* The European build's entries are 0x16 bytes (EuropeanDuelEffectEntry):
   its callbacks reach their fields through this view. */
#define EU_ENTRY(record) ((EuropeanDuelEffectEntry *)(record))
#define EU_ENTRY_FADE_VALUE(record) (((u8 *)(record))[0x15])

#ifdef VERSION_EUROPE_FUNC_80039B3C
void DuelEffect_UpdateGlyphSpiralFade(DuelEffectChannel *record, void *context)
{
    s32 radius;
    s32 value;
    s32 next_angle;
    u32 flags;
    u16 *origin_x;
    u16 *origin_y;

    origin_x = (u16 *)&EU_ENTRY(record)->field_04;
    origin_y = (u16 *)&EU_ENTRY(record)->field_06;

    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        EU_ENTRY_FADE_VALUE(record) = 0;
        EU_ENTRY(record)->render_mode = 3;
        if ((rand() & 0xFF) >= 0x80) {
            EU_ENTRY(record)->field_14 |= 0x40;
        }
        *origin_x = EU_ENTRY(record)->local_x;
        *origin_y = EU_ENTRY(record)->local_y;
        *(u16 *)&EU_ENTRY(record)->field_08 = rand() & 0xFFF;
        *(u16 *)&EU_ENTRY(record)->field_0A = 0x100;
        if (EU_ENTRY(record)->field_14 & 0x80) {
            EU_ENTRY_FADE_VALUE(record) = 0x80;
            *(u16 *)&EU_ENTRY(record)->field_0A = 0;
        }
    }

    flags = EU_ENTRY(record)->field_14;
    if (flags & 0x80) {
        radius = *(u16 *)&EU_ENTRY(record)->field_0A + 10;
        if (radius < 0x140) {
            flags &= 0x7F;
            goto assign_value;
        }
        DisplayObjectFade_ReleaseChannel(record);
        return;
    }

    radius = *(u16 *)&EU_ENTRY(record)->field_0A - 10;
    value = radius;
    if (radius > 0) {
        goto clamp_value;
    }
    EU_ENTRY(record)->local_x = *origin_x;
    EU_ENTRY(record)->local_y = *origin_y;
    EU_ENTRY(record)->render_mode = 0;
    EU_ENTRY(record)->handler_state = 0;
    return;

assign_value:
    value = radius;

clamp_value:
    *(u16 *)&EU_ENTRY(record)->field_0A = radius;
    if (radius >= 0xA1) {
        value = 0xA0;
    }
    EU_ENTRY_FADE_VALUE(record) = (0xA0 - value) * 0x80 / 0xA0;

    if (flags & 0x40) {
        flags &= 0x3F;
        next_angle = *(u16 *)&EU_ENTRY(record)->field_08 - flags;
    } else {
        next_angle = *(u16 *)&EU_ENTRY(record)->field_08 + flags;
    }
    value = next_angle & 0xFFF;
    *(u16 *)&EU_ENTRY(record)->field_08 = value;
    EU_ENTRY(record)->local_x =
        *origin_x + rcos(value) * radius / 0x1000;
    EU_ENTRY(record)->local_y =
        *origin_y + rsin(value) * radius / 0x1000;
}
#endif

#ifdef VERSION_EUROPE_FUNC_80039D84
void DuelEffect_StartGlyphSpiralConceal(DuelEffectChannel *record, void *context)
{
    EU_ENTRY(record)->field_14 = ((rand() & 0x1F) + 8) | -0x80;
    EU_ENTRY(record)->field_14 = ((rand() & 0x1F) + 32) | -0x80;
    EU_ENTRY(record)->handler_state = 4;
    DuelEffect_UpdateGlyphSpiralFade(record, context);
}
#endif

#ifdef VERSION_EUROPE_FUNC_80039DF8
void DuelEffect_UpdateGlyphLinearFade(DuelEffectChannel *record)
{
    s32 value;

    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        EU_ENTRY(record)->render_mode = 3;
        value = EU_ENTRY(record)->field_14;
        EU_ENTRY_FADE_VALUE(record) = 0;
        if (value != 0) {
            EU_ENTRY_FADE_VALUE(record) = 0x80;
        }
    }

    value = EU_ENTRY_FADE_VALUE(record);
    if (EU_ENTRY(record)->field_14 != 0) {
        value -= 5;
        if (value <= 0) {
            DisplayObjectFade_ReleaseChannel(record);
            return;
        }
    } else {
        value += 5;
        if (value >= 0x80) {
            EU_ENTRY(record)->render_mode = 0;
            EU_ENTRY(record)->handler_state = 0;
            return;
        }
    }
    EU_ENTRY_FADE_VALUE(record) = value;
}
#endif

#ifdef VERSION_EUROPE_FUNC_80039E88
void DuelEffect_UpdateGlyphFadeIn(DuelEffectChannel *record)
{
    EU_ENTRY(record)->field_14 = 0;
    DuelEffect_UpdateGlyphLinearFade(record);
}
#endif

#ifdef VERSION_EUROPE_FUNC_80039EA8
void DuelEffect_UpdateGlyphFadeOut(DuelEffectChannel *record)
{
    EU_ENTRY(record)->field_14 = 0x80;
    DuelEffect_UpdateGlyphLinearFade(record);
}
#endif

#ifdef VERSION_EUROPE_FUNC_80039D24
void DuelEffect_StartGlyphSpiralReveal(DuelEffectChannel *record, void *context)
{
    EU_ENTRY(record)->field_14 = (rand() & 0x1F) + 8;
    EU_ENTRY(record)->field_14 = (rand() & 0x1F) + 32;
    EU_ENTRY(record)->handler_state = 4;
    DuelEffect_UpdateGlyphSpiralFade(record, context);
}
#endif

#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_80039AFC)
#ifdef VERSION_EUROPE
void DuelEffect_UpdateGlyphReveal(DuelEffectChannel *record)
{
    /* The European entry (0x16 bytes, two-byte aligned) clears field_04 as
       two halfwords, and the fade runs at half the US steps. */
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        EU_ENTRY(record)->render_mode = 2;
        *(u16 *)&EU_ENTRY(record)->field_04 = 0;
        *(u16 *)&EU_ENTRY(record)->field_06 = 0;
        EU_ENTRY(record)->field_14 = 16;
    }
    EU_ENTRY(record)->field_14 =
        EU_ENTRY(record)->field_14 - *(u8 *)&gGraphics_nBaseFrameStep;
    if ((EU_ENTRY(record)->handler_state &
         DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE) == 0) {
        EU_ENTRY(record)->field_04 =
            EU_ENTRY(record)->field_04 + gGraphics_nBaseFrameStep * 16;
        if ((s8)EU_ENTRY(record)->field_04 < 0) {
            EU_ENTRY(record)->field_04 = 128;
            EU_ENTRY(record)->field_14 = 8;
            EU_ENTRY(record)->handler_state |=
                DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
        EU_ENTRY(record)->field_06 = EU_ENTRY(record)->field_04;
    } else {
        EU_ENTRY(record)->field_05 =
            EU_ENTRY(record)->field_05 + gGraphics_nBaseFrameStep * 16;
        if ((s8)EU_ENTRY(record)->field_05 < 0) {
            EU_ENTRY(record)->field_14 = 0;
            EU_ENTRY(record)->render_mode = 0;
            EU_ENTRY(record)->handler_state = 0;
        }
        EU_ENTRY(record)->field_07 = EU_ENTRY(record)->field_05;
    }
}
#else
void DuelEffect_UpdateGlyphReveal(DuelEffectChannel *record)
{
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        record->render_mode = 2;
        DUEL_EFFECT_FIELD_04_WORD(s32, record) = 0;
        record->field_14 = 32;
    }
    record->field_14 = record->field_14 - gGraphics_nBaseFrameStep * 2;
    if ((record->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE) == 0) {
        record->field_04 = record->field_04 + gGraphics_nBaseFrameStep * 16;
        if ((s8)record->field_04 < 0) {
            record->field_04 = 128;
            record->field_14 = 16;
            record->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
        record->field_06 = record->field_04;
    } else {
        record->field_05 = record->field_05 + gGraphics_nBaseFrameStep * 16;
        if ((s8)record->field_05 < 0) {
            record->field_14 = 0;
            record->render_mode = 0;
            record->handler_state = 0;
        }
        record->field_07 = record->field_05;
    }
}
#endif

#endif

#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_80039BE0)
#ifdef VERSION_EUROPE
void DuelEffect_UpdateGlyphConceal(DuelEffectChannel *p)
{
    s32 v;

    /* The European entry sets field_04 as two halfwords, and the fade
       steps by gGraphics_nBaseFrameStep * 24 where the US build steps by * 16. */
    if (!DisplayObjectFade_MarkInitialized(p)) {
        EU_ENTRY(p)->render_mode = 2;
        *(u16 *)&EU_ENTRY(p)->field_04 = 0x8080;
        *(u16 *)&EU_ENTRY(p)->field_06 = 0x8080;
        EU_ENTRY(p)->field_14 = 0;
    }
    if (!(EU_ENTRY(p)->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        v = EU_ENTRY(p)->field_04;
        v -= gGraphics_nBaseFrameStep * 24;
        if (v <= 0) {
            EU_ENTRY(p)->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
            v = 0;
        }
        EU_ENTRY(p)->field_04 = v;
        EU_ENTRY(p)->field_05 = v;
    } else {
        v = EU_ENTRY(p)->field_06;
        v -= gGraphics_nBaseFrameStep * 24;
        if (v <= 0) {
            DisplayObjectFade_ReleaseChannel(p);
            v = 0;
        }
        EU_ENTRY(p)->field_06 = v;
        EU_ENTRY(p)->field_07 = v;
    }
}
#else
void DuelEffect_UpdateGlyphConceal(DuelEffectChannel *p)
{
    s32 v;

    if (!DisplayObjectFade_MarkInitialized(p)) {
        p->render_mode = 2;
        DUEL_EFFECT_FIELD_04_WORD(u32, p) = 0x80808080;
        p->field_14 = 0;
    }
    if (!(p->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        v = p->field_04 - (gGraphics_nBaseFrameStep << 4);
        if (v <= 0) {
            p->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
            v = 0;
        }
        p->field_04 = v;
        p->field_05 = v;
    } else {
        v = p->field_06 - (gGraphics_nBaseFrameStep << 4);
        if (v <= 0) {
            DisplayObjectFade_ReleaseChannel(p);
            v = 0;
        }
        p->field_06 = v;
        p->field_07 = v;
    }
}
#endif
#endif

#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_80039C94)
#ifdef VERSION_EUROPE
void DuelEffect_UpdateDelayedGlyphRotation(DuelEffectChannel *record) {
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        s32 a;
        s32 b;

        EU_ENTRY(record)->render_mode = 1;
        a = EU_ENTRY(record)->local_x;
        b = EU_ENTRY(record)->local_y;
        EU_ENTRY(record)->field_08 = 0;
        EU_ENTRY(record)->field_09 = 0;
        EU_ENTRY(record)->field_0A = 0;
        EU_ENTRY(record)->field_04 = ((s16)a >> 4) + ((s16)b >> 3) + 1;
    }

    if (!(EU_ENTRY(record)->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        s32 v = EU_ENTRY(record)->field_04 - 1;

        EU_ENTRY(record)->field_04 = v;

        if ((u8)v == 0) {
            EU_ENTRY(record)->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
    } else {
        s32 v = EU_ENTRY(record)->field_08 + 4;

        EU_ENTRY(record)->field_0A = v;
        EU_ENTRY(record)->field_09 = v;
        EU_ENTRY(record)->field_08 = v;

        if (v >= 0x40) {
            DisplayObjectFade_ReleaseChannel(record);
        }
    }
}
#else
void DuelEffect_UpdateDelayedGlyphRotation(DuelEffectChannel *arg0) {
    if (DisplayObjectFade_MarkInitialized(arg0) == 0) {
        s32 a;
        s32 b;

        arg0->render_mode = 1;
        a = arg0->local_x;
        b = arg0->local_y;
        arg0->field_08 = 0;
        arg0->field_09 = 0;
        arg0->field_0A = 0;
        arg0->field_04 = ((s16)a >> 4) + ((s16)b >> 3) + 1;
    }

    if (!(arg0->handler_state & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        s32 v = arg0->field_04 - 1;

        arg0->field_04 = v;

        if ((u8)v == 0) {
            arg0->handler_state |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
    } else {
        s32 v = arg0->field_08 + 4;

        arg0->field_0A = v;
        arg0->field_09 = v;
        arg0->field_08 = v;

        if (v >= 0x40) {
            DisplayObjectFade_ReleaseChannel(arg0);
        }
    }
}
#endif
#endif
