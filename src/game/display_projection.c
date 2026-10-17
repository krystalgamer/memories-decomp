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

void DisplayObject_ProjectLinkedGroundPosition(DisplayObject *object)
{
    DisplayObject *entry;

    entry = gDisplayObject_aPool + object->field_6A;
    object->position.word = entry->position.word;
    DisplayObject_ProjectGroundPosition(object);
}
