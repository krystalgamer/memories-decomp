#include "../types.h"
#include "duel_side_state.h"
#include "display_object.h"
#include "display_projection.h"
#include "screen_projection.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "func_80016784.h"
#include "ordering_tables.h"

void func_80015DB8(DisplayObject *object)
{
    DisplayObject *entry;

    entry = D_800EFE48 + object->field_6A;
    object->position.word = entry->position.word;
    func_80015D18(object);
}
