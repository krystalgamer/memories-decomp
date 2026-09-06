#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

typedef struct {
    u8 type;
    u8 pad01;
    s16 value;
    s32 pad04;
    s32 data;
    u8 pad0C[0x24];
} SDSequenceCommand;

extern void func_80045BE8(SDSequenceCommand *);
extern void func_80046294(void);
extern void func_800471D0(s32, s32, s32, s32, s32, s32);
extern void func_80049010(void);
extern void func_80049138(s16, s32);
extern s32 func_800496C4(void *, s32, s32);
extern void func_8004A6D8(void);
extern void func_8004B854(void);
extern void func_8004B9E0(void);
extern void func_80074E60(void);

void func_80049230(s32 value, s32 data);

void func_80049200(s32 value)
{
    func_80049230((s16)value, g_SDValue->field_164B);
}

void func_80049230(s32 value, s32 data)
{
    SDSequenceCommand command;
    s16 small = value;

    if (small < 0) {
        func_80049010();
        return;
    }
    if (*g_SDValue->music_track != (small >> 4))
        func_80049138(small, 1);
    command.type = 0x48;
    command.value = value;
    command.data = (s16)data;
    func_80045BE8(&command);
    g_SDValue->field_1582 = data;
    g_SDValue->field_1584 = 255;
}

void SD_Init(void)
{
    func_80046294();
    func_80049010();
    SpuSetReverbModeType(0);
}

void func_80049308(void)
{
    SDValue *state = g_SDValue;

    if (state->field_157A != 0) {
        u8 *entry = (u8 *)state->music_track;
        s32 result = func_800496C4(entry + 0x50, 0, *(s32 *)(entry + 0x0C));

        g_SDValue->field_157A = result;
        if ((result << 16) != 0) {
            g_SDValue->field_1578 = -1;
            g_SDValue->field_157A = -1;
            return;
        }
    }
    {
        register SDValue *final = g_SDValue;
        register u16 *entry = final->music_track;
        final->field_1578 = *entry;
    }
}

void func_80049394(void *entry)
{
    func_80049010();
    if (entry != 0)
        g_SDValue->music_track = entry;
    else
        g_SDValue->music_track = (u16 *)0x801EA800;
    *g_SDValue->music_track = 0xFFFF;
}

void func_800493F8(void)
{
    u16 *entry;

    func_80049010();
    entry = (u16 *)0x801EA800;
    *(u16 **)((u8 *)g_SDValue + 0x1564) = entry;
    entry[0] = 0xFFFF;
}

void func_80049434(void)
{
    SDSecondaryState *state;
    SDSecondaryState *other;
    SDSecondaryState *third;
    SDSecondaryState *fourth;
    SDSecondaryState *fifth;
    SDSecondaryState *final;

    D_8009B458->flag_0500 = 0;
    D_8009B458->flag_0501 = 0;
    D_8009B458->flag_0502 = 0;
    state = D_8009B458;
    state->object_count = SD_SECONDARY_OBJECT_COUNT;
    state->field_0814 = 0;
    other = D_8009B458;
    state->field_0512 = 127;
    state->field_0514 = 127;
    state->field_0516 = 127;
    other->field_0815 = 0;
    third = D_8009B458;
    third->transfer.field_0019 = 127;
    fourth = D_8009B458;
    third->transfer.field_0000 = -1;
    fourth->transfer.field_001A = 127;
    fifth = D_8009B458;
    fifth->transfer.field_001B = 64;
    final = D_8009B458;
    final->field_07E0 = -1;
    final->field_07E2 = 0;
    final->field_07E6 = 127;
    final->field_07E4 = 127;
    final->field_080C = 0;
    final->field_0810 = 0;
    final->field_081C = 0x1000;
}

void func_800494F4(s32 *data)
{
    u32 i = 0;

    D_8009B458 = (SDSecondaryState *)data;
    do {
        *data = 0;
        i++;
        data++;
    } while (i < 530);
    func_80074E60();
    func_8004A6D8();
    func_80049434();
}

void func_80049544(void)
{
    s32 value = D_8009B458->field_081C;

    if (value > 0) {
        if (value < 4)
            func_8004B854();
    }
    D_8009B458->field_0814 = 1;
}

void func_80049594(s32 value)
{
    D_8009B458->field_081C = value;
}

void func_800495A4(void)
{
    if (D_8009B458->field_0814)
        func_8004B9E0();
}

void func_800495DC(void)
{
    D_8009B458->field_0815 = 0;
}

void func_800495EC(void)
{
    D_8009B458->field_0815 = 1;
}
