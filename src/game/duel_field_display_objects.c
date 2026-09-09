#include "../types.h"
#include "duel_side_state.h"
#include "duel_grid.h"
#include "display_object_api.h"
#include "display_projection.h"

#include "duel_card.h"
#include "display_object.h"
#include "display_object_config.h"

typedef struct {
    u8 pad_00[4];
    DisplayObject *object;
    u8 pad_08[7];
    s8 x;
    s8 y;
    u8 pad_11[6];
    u8 table_index;
} DuelFieldDisplaySource;
extern void func_80023144(DuelFieldDisplaySource *, u8);
extern void func_80023144(DuelFieldDisplaySource *, u8);

void func_8002348C(DuelFieldDisplaySource *source)
{
    u8 *table = D_800907D8;
    s32 index =
        source->y * DUEL_FIELD_ROW_SIZE + source->x +
        D_8009B1D5 * DUEL_FIELD_SIDE_GRID_SLOT_COUNT;

    func_80023144(source, table[index]);
}

void func_800234E4(DuelFieldDisplaySource *source)
{
    s32 index;
    DisplayObject *object;
    DuelFieldPosition *position;

    index = source->y * DUEL_FIELD_ROW_SIZE + source->x;
    object = func_800400AC(func_8004002C(), 2);
    func_80040468(
        (u8 *)object,
        4,
        3,
        source->table_index + D_8009B1D5 * 4,
        0x1F,
        0x100
    );

    {
        u8 *base = (u8 *)D_80090800;
        s32 offset = index * sizeof(DuelFieldPosition) +
            D_8009B1D5 * DUEL_FIELD_SIDE_POSITION_BYTES;

        position = (DuelFieldPosition *)(base + offset);
    }
    object->position.h.field_28 = position->x;
    object->position.h.field_2A = position->y;
    object->flags = object->flags | 0x28;
    object->update = (DisplayObjectCallback)func_80015D18;
    source->object = object;
}
