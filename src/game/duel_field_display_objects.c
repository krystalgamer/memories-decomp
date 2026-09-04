#include "../types.h"

typedef struct DisplayObjectSlot {
    u8 pad_00[8];
    u16 flags;
    u8 pad_0A[0x1A];
    void (*callback)(void *self);
    u16 x;
    u16 y;
    u8 pad_2C[0x3E];
    u8 state;
    u8 pad_6B[5];
} DisplayObjectSlot;

typedef struct {
    u8 pad_00[4];
    DisplayObjectSlot *object;
    u8 pad_08[7];
    s8 x;
    s8 y;
    u8 pad_11[6];
    u8 table_index;
} DuelFieldDisplaySource;

typedef struct {
    u16 x;
    u16 y;
} DuelFieldPosition;

extern u8 D_800907D8[];
extern DuelFieldPosition D_80090800[];
extern u8 D_8009B1D5;

extern void func_80023144(DuelFieldDisplaySource *, u8);
extern s32 func_8004002C(void);
extern DisplayObjectSlot *func_800400AC(s32, s32);
extern void *func_80040468(DisplayObjectSlot *, s32, s32, s32, s32, s32);
extern void func_80015D18(void *);

void func_8002348C(DuelFieldDisplaySource *source)
{
    u8 *table = D_800907D8;
    s32 index = source->y * 5 + source->x + D_8009B1D5 * 20;

    func_80023144(source, table[index]);
}

void func_800234E4(DuelFieldDisplaySource *source)
{
    s32 index;
    DisplayObjectSlot *object;
    DuelFieldPosition *position;

    index = source->y * 5 + source->x;
    object = func_800400AC(func_8004002C(), 2);
    func_80040468(
        object,
        4,
        3,
        source->table_index + D_8009B1D5 * 4,
        0x1F,
        0x100
    );

    {
        u8 *base = (u8 *)D_80090800;
        s32 offset = index * 4 + D_8009B1D5 * 0x50;

        position = (DuelFieldPosition *)(base + offset);
    }
    object->x = position->x;
    object->y = position->y;
    object->flags = object->flags | 0x28;
    object->callback = func_80015D18;
    source->object = object;
}
