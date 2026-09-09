#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#include "../types.h"
#include "func_80044DC0.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_output_state.h"
#include "sound_transfer_lifecycle.h"
#include "sound_voice_selection.h"

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
    SpuSetKey(SPU_OFF, 0xFFFFFF);
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

#include "sound_init.h"
#include "sound_pending_entries.h"
#include "sound_output.h"
extern void func_800490F0(s16, u8);
extern void func_80049108(s16, u8);
/* Not a duplicate of the sound_init.h declaration: a second addressing view
   of the same symbol. The one call below narrows its second argument to s16,
   and the canonical (s32, s32) prototype would widen it back. */
extern void func_80049230_s16(s32, s16) asm("func_80049230");
extern SDValue * volatile D_8009B45C_volatile asm("g_SDValue");
void func_80046F58(void)
{
    SD_KeyOffVoiceSlots();
    func_8004763C();
    SD_Init();
    func_80049640();
    SpuSetIRQ(SPU_OFF);
    SpuQuit();
}

void SD_SetOutputType(s16 value)
{
    SDValue *state = g_SDValue;

    if (state->output_type != value) {
        state->output_type = value;
        if (value == 0)
            func_800495DC();
        else
            func_800495EC();
        func_80044DC0(g_SDValue->cd_volume);
    }
}

s32 func_80047008(void)
{
    return g_SDValue->output_type;
}

void func_8004701C(s32 value)
{
    g_SDValue->flags_004A =
        (g_SDValue->flags_004A & 0xF0) | value;
}

s32 func_8004703C(void)
{
    return (s16)g_SDValue->flags_0040;
}

void func_80047050(void)
{
    if (D_8009B458->field_0509)
        func_800495A4();
    SD_UpdateRuntime();
    if (g_SDValue->flags_0040 & 8)
        func_80046A08();
}

void func_800470B0(s32 first, s32 second, s32 third, s32 fourth)
{
    SDCommand command;

    func_8004503C(-32, 0, 1);
    g_SDValue->flags_0040 |= 2;
    command.command = 32;
    command.field_0010 = fourth;
    command.field_0014 = second;
    command.field_0020 = first;
    command.field_0028 = third;
    command.field_001C = 0;
    command.field_0024 = 0;
    SD_EnqueueCommand(&command);
}

void func_80047140(s32 first, s32 second, s32 third, s32 fourth)
{
    SDCommand command;

    func_8004503C(-32, 0, 1);
    g_SDValue->flags_0040 |= 2;
    command.command = 32;
    command.field_0010 = fourth;
    command.field_0014 = second;
    command.field_0020 = 0;
    command.field_0028 = 0;
    command.field_001C = first;
    command.field_0024 = third;
    SD_EnqueueCommand(&command);
}

void func_800471D0(
    s32 a0,
    s32 a1,
    s32 a2,
    s32 a3,
    s32 a4,
    s32 a5
)
{
    SDCommand command;

    func_8004503C(-32, 0, 1);
    g_SDValue->flags_0040 |= 2;
    command.command = 32;
    command.field_0010 = a5;
    command.field_0014 = a2;
    command.field_0020 = a1;
    command.field_0028 = a4;
    command.field_001C = a0;
    command.field_0024 = a3;
    SD_EnqueueCommand(&command);
}

void func_80047278(u32 value)
{
    func_800472A8(value >> 16);
    func_80047AD0(value & SD_COMMAND_VALUE_MASK);
}

void func_800472A8(s32 arg0)
{
    register s32 v asm("v1") = arg0;

    if ((g_SDValue->flags_004A & 2) == 0) {
        return;
    }
    if (arg0 & 0x8000) {
        func_80045334(v & SD_COMMAND_VALUE_MASK);
    } else {
        register u32 masked asm("v0") =
            (u32)(v & SD_COMMAND_VALUE_MASK);

        if (masked >= SD_BGM_COMMAND_BASE) {
            arg0 -= SD_BGM_COMMAND_BASE;
        }
        func_80049138((s16)arg0, 1);
    }
}

void func_80047314(u32 value)
{
    func_8004733C(value & SD_COMMAND_VALUE_MASK, D_8009B45C_volatile->field_164B);
}

void func_8004733C(s32 arg0, s32 arg1)
{
    register s32 v asm("s1") = arg0;

    if ((D_8009B45C_volatile->flags_004A & 2) == 0) {
        return;
    }
    if (arg0 & 0x8000) {
        func_800473CC(SD_BGM_COMMAND_BASE);
        func_80045208(v & SD_COMMAND_VALUE_MASK, (s16)arg1);
    } else {
        register u32 masked asm("v0") =
            (u32)(v & SD_COMMAND_VALUE_MASK);

        if (masked >= SD_BGM_COMMAND_BASE) {
            arg0 -= SD_BGM_COMMAND_BASE;
        }
        func_80049230((s16)arg0, (s16)arg1);
    }
}

void func_800473CC(u32 value)
{
    func_800473F0(value & SD_COMMAND_VALUE_MASK, -32);
}

void func_800473F0(u16 flags, s32 value)
{
    if ((flags & 0x8000) != 0)
        func_80045114();
    else
        func_80049230_s16(-1, value);
}

void func_80047430(s32 value, s32 flag)
{
    func_80049108(value, flag);
}

void func_80047458(s32 value, s32 flag)
{
    func_800490F0(value, flag);
}
