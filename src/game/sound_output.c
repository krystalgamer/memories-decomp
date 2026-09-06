#include "../types.h"
#include "sound.h"

extern void func_80044DC0(s32);
extern void func_8004503C(s32, s32, s32);
extern void func_80045334(s32);
extern void func_80045BE8(SDCommand *);
extern void func_80045F3C(void);
extern void func_80046A08(void);
extern void func_800472A8(s32);
extern void func_8004763C(void);
extern void func_80047AD0(u32);
extern void func_80047EC4(void);
extern void func_80049138(s32, s32);
extern void func_800495A4(void);
extern void func_800495DC(void);
extern void func_800495EC(void);
extern void func_80049640(void);
extern void func_80075B60(void);
extern void func_80076D90(s32);
extern void SD_Init(void);

void func_80046F58(void)
{
    func_80047EC4();
    func_8004763C();
    SD_Init();
    func_80049640();
    func_80076D90(0);
    func_80075B60();
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
    func_80045F3C();
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
    func_80045BE8(&command);
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
    func_80045BE8(&command);
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
    func_80045BE8(&command);
}

void func_80047278(u32 value)
{
    func_800472A8(value >> 16);
    func_80047AD0(value & 0xFFFF);
}

void func_800472A8(s32 arg0)
{
    register s32 v asm("v1") = arg0;

    if ((g_SDValue->flags_004A & 2) == 0) {
        return;
    }
    if (arg0 & 0x8000) {
        func_80045334(v & 0xFFFF);
    } else {
        register u32 masked asm("v0") = (u32)(v & 0xFFFF);

        if (masked >= 0x7000) {
            arg0 -= 0x7000;
        }
        func_80049138((s16)arg0, 1);
    }
}
