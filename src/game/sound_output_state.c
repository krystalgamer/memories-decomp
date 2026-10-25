/* The sound driver's CD-audio control, SD_InitBufferState (0x80044D48)
   through SD_ClearBusyFlag: the CD mix state first -- work-buffer pointer
   setup (which first centres the pan and applies CD volume 255), the pan
   reset that sets both channel volumes to SD_CHANNEL_VOLUME_MAX, the three CD
   volume/pan setters and the CD volume reader -- then the CD command queuers,
   CD-level measurement and busy-state completion. */

#include "../types.h"
#include "sd_set_cd_volume.h"
#include "sound.h"
#include "../psyq/libcd.h"
#include "sound_mix.h"

void SD_InitBufferState(void)
{
    SDValue *base;

    SD_ResetCdPan();
    SD_SetCdVolume(255);
    base = g_SDValue;
    base->buffer_ptrs_153C[0] = base->buffer_053C[0];
    base->buffer_ptrs_153C[1] = base->buffer_053C[1];
    base->buffer_ptrs_153C[2] = base->buffer_053C[2];
    base->field_0512 = 0;
    base->buffer_ptrs_153C[3] = base->buffer_053C[3];
}

void SD_ResetCdPan(void)
{
    g_SDValue->channel_volume[0] = SD_CHANNEL_VOLUME_MAX;
    g_SDValue->channel_volume[1] = SD_CHANNEL_VOLUME_MAX;
}

/* Sibling of SD_SetCdVolumeImmediate: builds the same 4-byte CdMix volume
   packet, but here val0/val1-or-val2 are scaled DSP-style from a0 instead
   of copied verbatim. g_SDValue's fields 0x514/0x515 are
   per-channel volume scalars; f42 and f533 combine with (a0+1) to derive a
   shared 0-8191 "shift" factor, which each channel's scalar is then
   multiplied against and rescaled (>>7) into its packet byte. f48 picks
   which of val1/val2 carries the scaled value (the other gets 0), matching
   SD_SetCdVolumeImmediate's f48-based slot selection. Reloads
   g_SDValue through a fresh alias for the trailing f510 write, same
   pattern as project_d8009b45c_reload_pointer_trick. */
void SD_SetCdVolume(s16 a0) {
    SDValue *p = g_SDValue;
    s32 prod1 = (a0 + 1) * p->mix_scale;
    s32 prod2 = p->mix_multiplier * (prod1 >> 8);
    s32 shift = ((u32)(u16)prod2) >> 3;
    CdlATV local;
    s16 s0 = a0;

    local.val0 = (shift * p->channel_volume[0]) >> 7;
    local.val3 = 0;

    if (p->output_type == 0) {
        local.val1 = 0;
        local.val2 = (shift * p->channel_volume[1]) >> 7;
    } else {
        local.val1 = (shift * p->channel_volume[1]) >> 7;
        local.val2 = 0;
    }

    CdMix(&local);

    g_SDValue->cd_volume = s0;
}

void SD_SetCdPan(s32 pan)
{
    s16 saved = pan;

    if ((u16)((u32)pan - 1) < SD_CHANNEL_VOLUME_MAX) {
        g_SDValue->channel_volume[0] = -SD_CHANNEL_VOLUME_MAX - saved;
        g_SDValue->channel_volume[1] = SD_CHANNEL_VOLUME_MAX;
    } else if ((u16)((u32)pan + SD_CHANNEL_VOLUME_MAX) <
        SD_CHANNEL_VOLUME_MAX) {
        g_SDValue->channel_volume[0] = SD_CHANNEL_VOLUME_MAX;
        g_SDValue->channel_volume[1] = -SD_CHANNEL_VOLUME_MAX - saved;
    } else {
        g_SDValue->channel_volume[0] = SD_CHANNEL_VOLUME_MAX;
        g_SDValue->channel_volume[1] = SD_CHANNEL_VOLUME_MAX;
    }
    SD_SetCdVolume(g_SDValue->cd_volume);
}

void SD_SetCdVolumeImmediate(s32 value)
{
    CdlATV data;
    SDValue *first;
    SDValue *second;

    g_SDValue->mix_multiplier = 7;
    data.val0 = value;
    data.val3 = 0;
    if (g_SDValue->output_type == 0) {
        data.val1 = 0;
        data.val2 = value;
    } else {
        data.val1 = value;
        data.val2 = 0;
    }
    CdMix(&data);
    first = g_SDValue;
    first->field_0049 = value;
    second = g_SDValue;
    first->cd_volume = value;
    second->field_0512 = 0;
}

s32 SD_GetCdVolume(void)
{
    return g_SDValue->cd_volume;
}

#include "../psyq/libspu.h"

#include "sound.h"
#include "sound_output_state.h"

/* Queues CD fade command 41: field_0008 is the signed per-update step and
   field_0002 the target volume, loaded into field_0512/field_0049 when
   SD_UpdateRuntime dequeues it. field_0001 becomes field_007E; while it is
   nonzero, SD_StepActiveCommand keeps the command active until the fade
   ends. No caller is present in the retail executable or its overlays. */
s32 SD_EnqueueCdFade(s32 a0, s32 a1, s32 a2) {
    SDCommand s;
    s.command = 41;
    s.field_0008 = (s16) a0;
    s.field_0002 = (s16) (u8) a1;
    s.field_0001 = (u8) a2;
    return SD_EnqueueCommand(&s);
}

void SD_StartCdFade(s16 value, u8 flag, s32 unused)
{
    g_SDValue->field_0512 = value;
    g_SDValue->field_0049 = flag;
}

#ifndef VERSION_JAPAN
#include "sound_mix.h"

/*
 * SD_MeasureCdAudioLevel: decoded output measurement without register pins
 *
 * The 192-byte routine matches all 48 instructions under the unit's
 * gcc_2_8_1_g0 profile. The same-symbol alias below retains its measured
 * volatile pointer view while the surrounding functions keep sound.h's
 * ordinary declaration.
 *
 * Three source properties replace the historical eight register bindings and
 * compiler barrier. The volatile pointer view retains the snapshot-to-state
 * handoff; removing it loses one instruction. Returning the level directly on
 * the unmuted path gives the required return-register allocation. Finally, the
 * single-iteration level-read scope keeps the signed high-halfword read before
 * the flag read; flattening it exchanges the two words at +0x90 and +0x94.
 *
 * The routine preserves the selected CD half, all 256 signed sample squares,
 * the unsigned eight-bit shift of each square, both accumulator
 * initializations and final publications, and the low-two-bit output gate.
 */
extern SDValue *G32 volatile g_SDValue_output_level asm("g_SDValue");

s32 SD_MeasureCdAudioLevel(void)
{
    s32 select = SpuReadDecodedData(
        (SpuDecodedData *)g_SDValue_output_level->buffer_053C, SPU_CDONLY
    );
    SDValue *choice_state = g_SDValue_output_level;
    s16 *values;
    s32 i;
    SDValue *loaded;
    SDValue *state;

    choice_state->decoded_half = select;
    if (select == SPU_DECODED_FIRSTHALF) {
        values = (s16 *)choice_state->buffer_ptrs_153C[0];
    } else {
        values = (s16 *)choice_state->buffer_ptrs_153C[1];
    }
    loaded = g_SDValue_output_level;
    i = 0;
    state = loaded;
    state->output_level.sum = 0;
    state->field_1550.sum = 0;
    do {
        s32 value = *values;
        u32 square = value * value;
        state->output_level.sum += square >> 8;
        i++;
        values++;
    } while (i < SD_MIX_SAMPLE_COUNT);
    {
        s32 result;
        s32 flags;
        s32 other;
        state = g_SDValue_output_level;
        do {
            result = state->output_level.halves[1];
        } while (0);
        flags = state->flags_0040;
        other = state->field_1550.halves[1];
        flags &= 3;
        state->output_level.sum = result;
        state->field_1550.sum = other;
        if (!flags) {
            return result;
        }
        return 0;
    }
}

void SD_EnqueueCdStop(void)
{
    SDValue *state = g_SDValue;
    s32 count;

    if ((state->flags_004A & 0x80) == 0)
        return;
    count = state->command_count;
    if (state->commands.c[count].command == 0x11)
        return;
    if (count > 0) {
        if (state->commands.c[count - 1].command == 0x11)
            return;
        if (count >= 2) {
            if (state->commands.c[count - 2].command == 0x11)
                return;
        }
    }
    SD_StartCdFade(-0x20, 0, 1);
    {
        SDCommand local;
        local.command = 0x11;
        SD_EnqueueCommand(&local);
    }
}

s16 SD_EnqueueCdSePlay(u16 value, s32 unused)
{
    return SD_EnqueueCdPlay(value, 0x80);
}

s32 SD_EnqueueCdPlay(u16 arg0, s32 unused)
{
    SDValue *a = g_SDValue;
    u16 code = arg0;
    u8 **table;
    s32 kind;
    u8 *second;
    SDCommand req;

    if (a->flags_004A & 0x80) {
        if ((a->flags_004A & 0x40) || code <= 0x9FFF) {
            if (arg0 & 0x8000) {
                *(s16 *)&a->field_0534 = arg0;
                switch (arg0 & 0xF000) {
                case 0x8000:
                    code = arg0 + 0x8000;
                    table = (u8 **)a->bank_0518[1];
                    second = (u8 *)&table[2];
                    kind = 0x50;
                    break;
                case 0x9000:
                    code = arg0 + 0x7000;
                    table = (u8 **)a->bank_0518[0];
                    second = (u8 *)&table[2];
                    kind = 0x60;
                    break;
                default:
                    code = code + 0x6000;
                    kind = 0x70;
                    table = (u8 **)g_SDValue->bank_0518[2];
                    second = (u8 *)&table[2];
                    break;
                }
                {
                    s32 first;

                    first = table ? (second ? (s32)*table : (s32)*table) : (s32)*table;

                    SD_DropQueuedCdPlayCommands();
                    req.command = 0x24;
                    req.field_0002 = code;
                    req.field_0004 = first;
                    req.field_000C = (s32)second;
                    req.field_0008 = kind;
                    SD_EnqueueCommand(&req);
                    SD_ResetCdPan();
                }
                g_SDValue->flags_0040 = (g_SDValue->flags_0040 | 1) & 0xFFFB;
                return 1;
            }
        }
    }
    return 0;
}

void SD_EnqueueCdSeek(s32 arg0)
{
    SDCommand req;
    SDValue *a;
    SDValue *b;
    SDValue *c;
    s32 value;
    u16 code;
    u8 **table;
    s32 kind;
    u8 *second;

    a = g_SDValue;
    code = arg0;
    if ((a->flags_004A & 0x80) == 0) {
        return;
    }
    if ((a->flags_004A & 0x40) == 0) {
        if ((u32)(code & 0xFFFF) > 0x9FFF) {
            return;
        }
    }
    if ((arg0 & 0x8000) == 0) {
        return;
    }
    value = arg0 & 0xF000;
    *(s16 *)&a->field_0534 = arg0;
    switch (value) {
    case 0x8000:
        value = arg0 + value;
        code = value;
        table = (u8 **)a->bank_0518[1];
        second = (u8 *)&table[2];
        kind = 0x50;
        break;
    case 0x9000:
        code = arg0 + 0x7000;
        table = (u8 **)a->bank_0518[0];
        second = (u8 *)&table[2];
        kind = 0x60;
        break;
    default:
        code += 0x6000;
        kind = 0x70;
        b = g_SDValue;
        table = (u8 **)b->bank_0518[2];
        second = (u8 *)&table[2];
        break;
    }
    {
        s32 first;

        /* The equivalent paths preserve retail's request-store order. */
        first = table ? (second ? (s32)*table : (s32)*table) : (s32)*table;

        SD_DropQueuedCdPlayCommands();
        req.command = 0x21;
        req.field_0002 = code;
        req.field_0004 = first;
        req.field_000C = (s32)second;
        req.field_0008 = kind;
        SD_EnqueueCommand(&req);
    }
    c = g_SDValue;
    c->flags_0040 = (c->flags_0040 | 1) & 0xFFFB;
}

void SD_ClearBusyFlag(void)
{
    g_SDValue->busy = 0;
}
#endif
