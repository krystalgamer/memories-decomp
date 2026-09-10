/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 3 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_output_state.c.
 */
#include "../types.h"
#include "../psyq/libspu.h"

#include "../game/sound.h"
#include "../game/sound_output_state.h"

#include "../game/sound_buffer_init.h"

s32 func_80045208(u16 arg0, s32 unused)
{
    SDValue *a = g_SDValue;
    u16 code = arg0;
    register u8 **table asm("$2");
    s32 kind;
    SDCommand req;

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
                    req.command = 0x24;
                    req.field_0002 = code;
                    req.field_0004 = first;
                    req.field_000C = (s32)second;
                    req.field_0008 = kind;
                    SD_EnqueueCommand(&req);
                    func_80044DA0();
                }
                g_SDValue->flags_0040 = (g_SDValue->flags_0040 | 1) & 0xFFFB;
                return 1;
            }
        }
    }
    return 0;
}

