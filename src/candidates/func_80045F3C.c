/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 3 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_runtime.c.
 */
#define FUNC_80049F50_RETURNS_S16
#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "../types.h"
#include "../game/func_80044DC0.h"
#include "../psyq/libspu.h"
#include "../game/sound.h"
#include "../game/sound_voice_constants.h"
#include "../unmatched.h"

void SD_UpdateRuntime(void)
{
    register SDValue *p asm("$3");
    u8 *e;
    s32 mask;
    s32 i;
    s32 off;
    register SDValue *q asm("$4");
    register SDValue *r asm("$2");

    SpuGetAllKeysStatus((char *)g_SDValue->field_15D8);
    p = g_SDValue;
    mask = (p->field_15EC == 3) << SD_VOICE_SLOT_KEY_SHIFT;
    if (p->field_15ED == 3) {
        mask |= SD_VOICE_SLOT_MASK_BASE << 1;
    }
    if (p->field_15EE == 3) {
        mask |= SD_VOICE_SLOT_MASK_BASE << 2;
    }
    if (p->field_15EF == 3) {
        mask |= SD_VOICE_SLOT_MASK_BASE << 3;
    }
    if (mask != 0) {
        SpuSetKey(0, mask);
    }

    p = g_SDValue;
    if ((p->flags_0040 & 0x80) != 0 &&
        p->field_157E != -1 &&
        func_80049F50() != 1) {
        func_80049C40(g_SDValue->field_157E);
        p = g_SDValue;
        p->field_1588 = -0x80;
        p->field_158A = 0;
        p->flags_0040 = p->flags_0040 & 0xFF7F;
    }

    SD_UpdateFades();
    p = g_SDValue;
    if (p->field_007C != 0) {
        func_80045514();
        return;
    }
    if (p->command_count == 0) {
        return;
    }
    e = (u8 *)&p->commands.c[0];
    /* Four separate case bodies, not two shared ones: adjacent case values
       that share a body are merged into a case range, and the range tests GCC
       then emits are not the target's equality dispatch. Cross-jumping merges
       the duplicated bodies again, so nothing is spent on them. */
    switch (p->commands.c[0].command) {
    case 0x44:
        if (p->field_1588 == 0) {
            p->field_1588 = *(u16 *)(e + 8);
            p->field_1584 = e[2];
        }
        break;
    case 0x45:
        if (p->field_1588 == 0) {
            p->field_1588 = *(u16 *)(e + 8);
            p->field_1584 = e[2];
        }
        break;
    case 0x29:
        if (p->field_0512 == 0) {
            p->field_0512 = *(u16 *)(e + 8);
            p->field_0049 = e[2];
        }
        break;
    case 0x2A:
        if (p->field_0512 == 0) {
            p->field_0512 = *(u16 *)(e + 8);
            p->field_0049 = e[2];
        }
        break;
    }

    p = g_SDValue;
    p->field_007C = e[0];
    g_SDValue->field_007D = 0;
    p = g_SDValue;
    p->field_004E = *(u16 *)(e + 2);
    p->field_0050 = *(u32 *)(e + 4);
    p->field_0054 = *(u32 *)(e + 8);
    p->field_0058 = *(u32 *)(e + 0xC);
    p->field_007E = e[1];
    /* Read the global twice rather than copying r into q. CSE turns the
       second read into the target's `move $a0,$v0`, and unlike an explicit
       copy it leaves the eight stores addressed off the first register. */
    r = g_SDValue;
    *(SDCommandTail *)&r->field_005C[0] = *(SDCommandTail *)(e + 0x10);
    q = g_SDValue;
    q->field_15F4 = 8;
    q->command_count = q->command_count - 1;
    func_80045514();

    p = g_SDValue;
    e += 0x30;
    i = 0;
    if (p->command_count <= 0) {
        return;
    }
    off = 0;
    do {
        /* Retail adds the record base to the running byte offset in that
           order, so these keep the byte form; SD_COMMAND_QUEUE_BYTE_OFFSET is
           asserted against SDValue.commands in sound.h. */
        *(u8 *)((u8 *)p + off + SD_COMMAND_QUEUE_BYTE_OFFSET) = e[0];
        *(SDCommand *)((u8 *)g_SDValue + off + SD_COMMAND_QUEUE_BYTE_OFFSET) =
            *(SDCommand *)e;
        off += 0x30;
        p = g_SDValue;
        i += 1;
        e += 0x30;
    } while (i < p->command_count);
}
