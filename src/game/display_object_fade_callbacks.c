#define D_8009B0D8_IS_VOLATILE
#include "../types.h"
#include "../psyq/rand.h"
#include "duel_effect.h"
#include "graphics_frame.h"
#include "display_object_fade.h"
#include "func_80039B3C.h"
#include "func_80039AD4.h"
#include "display_object_fade_callbacks.h"
#include "duel_effect_entry_occupancy.h"

#define DUEL_EFFECT_FIELD_04_WORD(type, object) \
    (*(type *)&(object)->field_04)

/* The European build's entries are 0x16 bytes (EuropeanDuelEffectEntry):
   its callbacks reach their fields through this view. */
#define EU_ENTRY(record) ((EuropeanDuelEffectEntry *)(record))
#define EU_ENTRY_FADE_VALUE(record) (((u8 *)(record))[0x15])

#ifdef VERSION_EUROPE_FUNC_80039D84
void func_80039D84(DuelEffectChannel *record, void *context)
{
    EU_ENTRY(record)->field_14 = ((rand() & 0x1F) + 8) | -0x80;
    EU_ENTRY(record)->field_14 = ((rand() & 0x1F) + 32) | -0x80;
    EU_ENTRY(record)->field_13 = 4;
    func_80039B3C(record, context);
}
#endif

#ifdef VERSION_EUROPE_FUNC_80039DF8
void func_80039DF8(DuelEffectChannel *record)
{
    s32 value;

    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        EU_ENTRY(record)->field_15 = 3;
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
            EU_ENTRY(record)->field_15 = 0;
            EU_ENTRY(record)->field_13 = 0;
            return;
        }
    }
    EU_ENTRY_FADE_VALUE(record) = value;
}
#endif

#ifdef VERSION_EUROPE_FUNC_80039D24
void func_80039D24(DuelEffectChannel *record, void *context)
{
    EU_ENTRY(record)->field_14 = (rand() & 0x1F) + 8;
    EU_ENTRY(record)->field_14 = (rand() & 0x1F) + 32;
    EU_ENTRY(record)->field_13 = 4;
    func_80039B3C(record, context);
}
#endif

#if (!defined(VERSION_JAPAN) || defined(VERSION_JAPAN_FUNC_80039AFC)) && \
    (!defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_80039AFC))
#ifdef VERSION_EUROPE
void func_80039AFC(DuelEffectChannel *record)
{
    /* The European entry (0x16 bytes, two-byte aligned) clears field_04 as
       two halfwords, and the fade runs at half the US steps. */
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        EU_ENTRY(record)->field_15 = 2;
        *(u16 *)&EU_ENTRY(record)->field_04 = 0;
        *(u16 *)&EU_ENTRY(record)->field_06 = 0;
        EU_ENTRY(record)->field_14 = 16;
    }
    EU_ENTRY(record)->field_14 =
        EU_ENTRY(record)->field_14 - *(u8 *)&D_8009B0D8;
    if ((EU_ENTRY(record)->field_13 &
         DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE) == 0) {
        EU_ENTRY(record)->field_04 =
            EU_ENTRY(record)->field_04 + D_8009B0D8 * 16;
        if ((s8)EU_ENTRY(record)->field_04 < 0) {
            EU_ENTRY(record)->field_04 = 128;
            EU_ENTRY(record)->field_14 = 8;
            EU_ENTRY(record)->field_13 |=
                DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
        }
        EU_ENTRY(record)->field_06 = EU_ENTRY(record)->field_04;
    } else {
        EU_ENTRY(record)->field_05 =
            EU_ENTRY(record)->field_05 + D_8009B0D8 * 16;
        if ((s8)EU_ENTRY(record)->field_05 < 0) {
            EU_ENTRY(record)->field_14 = 0;
            EU_ENTRY(record)->field_15 = 0;
            EU_ENTRY(record)->field_13 = 0;
        }
        EU_ENTRY(record)->field_07 = EU_ENTRY(record)->field_05;
    }
}
#else
void func_80039AFC(DuelEffectChannel *record)
{
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        record->field_15 = 2;
        DUEL_EFFECT_FIELD_04_WORD(s32, record) = 0;
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
#endif

#endif

#if (!defined(VERSION_JAPAN) || defined(VERSION_JAPAN_FUNC_80039BE0)) && \
    (!defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_80039BE0))
#ifdef VERSION_EUROPE
void func_80039BE0(DuelEffectChannel *p)
{
    s32 v;

    /* The European entry sets field_04 as two halfwords, and the fade
       steps by D_8009B0D8 * 24 where the US build steps by * 16. */
    if (!DisplayObjectFade_MarkInitialized(p)) {
        EU_ENTRY(p)->field_15 = 2;
        *(u16 *)&EU_ENTRY(p)->field_04 = 0x8080;
        *(u16 *)&EU_ENTRY(p)->field_06 = 0x8080;
        EU_ENTRY(p)->field_14 = 0;
    }
    if (!(EU_ENTRY(p)->field_13 & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        v = EU_ENTRY(p)->field_04;
        v -= D_8009B0D8 * 24;
        if (v <= 0) {
            EU_ENTRY(p)->field_13 |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
            v = 0;
        }
        EU_ENTRY(p)->field_04 = v;
        EU_ENTRY(p)->field_05 = v;
    } else {
        v = EU_ENTRY(p)->field_06;
        v -= D_8009B0D8 * 24;
        if (v <= 0) {
            DisplayObjectFade_ReleaseChannel(p);
            v = 0;
        }
        EU_ENTRY(p)->field_06 = v;
        EU_ENTRY(p)->field_07 = v;
    }
}
#else
void func_80039BE0(DuelEffectChannel *p)
{
    s32 v;

    if (!DisplayObjectFade_MarkInitialized(p)) {
        p->field_15 = 2;
        DUEL_EFFECT_FIELD_04_WORD(u32, p) = 0x80808080;
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
            DisplayObjectFade_ReleaseChannel(p);
            v = 0;
        }
        p->field_06 = v;
        p->field_07 = v;
    }
}
#endif
#endif

#if (!defined(VERSION_JAPAN) || defined(VERSION_JAPAN_FUNC_80039C94)) && \
    (!defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_80039C94))
#ifdef VERSION_EUROPE
void func_80039C94(DuelEffectChannel *record) {
    if (DisplayObjectFade_MarkInitialized(record) == 0) {
        s32 a;
        s32 b;

        EU_ENTRY(record)->field_15 = 1;
        a = EU_ENTRY(record)->field_0C;
        b = EU_ENTRY(record)->field_0E;
        EU_ENTRY(record)->field_08 = 0;
        EU_ENTRY(record)->field_09 = 0;
        EU_ENTRY(record)->field_0A = 0;
        EU_ENTRY(record)->field_04 = ((s16)a >> 4) + ((s16)b >> 3) + 1;
    }

    if (!(EU_ENTRY(record)->field_13 & DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE)) {
        s32 v = EU_ENTRY(record)->field_04 - 1;

        EU_ENTRY(record)->field_04 = v;

        if ((u8)v == 0) {
            EU_ENTRY(record)->field_13 |= DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE;
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
            DisplayObjectFade_ReleaseChannel(arg0);
        }
    }
}
#endif
#endif
