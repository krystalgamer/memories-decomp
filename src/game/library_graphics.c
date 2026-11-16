#include "../types.h"

/* Library screen graphics, one translation unit, US [0x80029590, 0x80029934):
 * the owner of gLibrary_State, Library_CreateGridSectionSprites and the
 * globe's two axis-span helpers (#39). Library_DrawWireframeGlobe's distinct
 * -fno-cse-skip-blocks profile bounds it above and card_preview.c below. */
#include "display_object.h"
#include "display_object_system.h"
#include "display_object_layout.h"
#include "library.h"

/* Zero-initialized storage this unit owns at 0x800EA1E8-0x800EAD88, defined in
 * address order ahead of the headers that declare it (#2602). */
LibraryState gLibrary_State;

#include "library_graphics.h"

void Library_CreateGridSectionSprites(void)
{
    s32 i = 0;
    LibraryMotionState *state = &gLibrary_State.motion;
    for (; i < 8; i++) {
        DisplayObject *object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition(object, 8, (i >> 1) * 178 + 8,
                      0, 3, i, 25, 262);
        if (i & 1)
            object->xy.h.x = 168;
        object->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
        object->flags &= ~DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SelectOrderingTable3(object);
        DisplayObject_SetDepthOffset(object, 0);
        state->display_objects.by_role.slots[i] = object;
    }
}

#include "graphics_packet_submission.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "gpu_packets.h"

/* The two axis sweeps of the globe wireframe, called from Library_DrawWireframeGlobe with
 * everything living in PSX scratchpad: `packet` is the GlobeLinePacket (a LINE_G3) at 0x1F800000
 * whose three packed screen-coordinate words RotAverage3 writes, `points` is
 * the run of SVECTOR at 0x1F800038 that it projects, and `control` is the
 * RotAverage3 flag pair at 0x1F800060. The helpers vary only x or z.
 * A negative projection-flags word causes one half-span retry, not a loop
 * or a successful-clipping guarantee. Both paths submit the packet.
 *
 * The offsets these used to be spelled with -- dst[0]/dst[4]/dst[8] on an
 * s16 pointer and (long *)(arg0 + 8) style casts -- are the members of those
 * records. Library_DrawWireframeGlobe uses the same scratchpad records.
 */

void Library_DrawGlobeXSpan(
    GlobeLinePacket *packet,
    GsOT *ot,
    SVECTOR *points,
    long *control,
    int origin,
    int span
)
{
    int reduced;

    points[0].vx = origin + span;
    points[1].vx = origin + (span * 3) / 4;
    points[2].vx = origin;
    RotAverage3(
        &points[0],
        &points[1],
        &points[2],
        &packet->xy0.sxy,
        &packet->xy1.sxy,
        &packet->xy2.sxy,
        &control[0],
        &control[1]
    );
    if (control[1] < 0) {
        reduced = span / 2;
        points[0].vx = origin + reduced;
        points[1].vx = origin + (reduced * 3) / 4;
        RotAverage3(
            &points[0],
            &points[1],
            &points[2],
            &packet->xy0.sxy,
            &packet->xy1.sxy,
            &packet->xy2.sxy,
            &control[0],
            &control[1]
        );
    }
    Graphics_SubmitDrawModePacket((u32 *)packet, ot, 1, 1);
}

void Library_DrawGlobeZSpan(
    GlobeLinePacket *packet,
    GsOT *ot,
    SVECTOR *points,
    long *control,
    int origin,
    int span
)
{
    int reduced;

    points[0].vz = origin + span;
    points[1].vz = origin + (span * 3) / 4;
    points[2].vz = origin;
    RotAverage3(
        &points[0],
        &points[1],
        &points[2],
        &packet->xy0.sxy,
        &packet->xy1.sxy,
        &packet->xy2.sxy,
        &control[0],
        &control[1]
    );
    if (control[1] < 0) {
        reduced = span / 2;
        points[0].vz = origin + reduced;
        points[1].vz = origin + (reduced * 3) / 4;
        RotAverage3(
            &points[0],
            &points[1],
            &points[2],
            &packet->xy0.sxy,
            &packet->xy1.sxy,
            &packet->xy2.sxy,
            &control[0],
            &control[1]
        );
    }
    Graphics_SubmitDrawModePacket((u32 *)packet, ot, 1, 1);
}
