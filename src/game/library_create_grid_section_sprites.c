#include "../types.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "display_object_helpers.h"
#include "library_card_grid.h"

/* Zero-initialized storage this unit owns at 0x800EA1E8-0x800EAD88, defined in
 * address order ahead of the headers that declare it (#2602). */
LibraryState gLibrary_State;

#include "library_create_grid_section_sprites.h"

void Library_CreateGridSectionSprites(void)
{
    s32 i = 0;
    LibraryMotionState *state = &gLibrary_State.motion;
    for (; i < 8; i++) {
        DisplayObject *object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(object, 8, (i >> 1) * 178 + 8,
                      0, 3, i, 25, 262);
        if (i & 1)
            object->field_30.h.field_30 = 168;
        object->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
        object->flags &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SelectOrderingTable3(object);
        DisplayObject_SetDepthOffset(object, 0);
        state->display_objects.by_role.slots[i] = object;
    }
}
