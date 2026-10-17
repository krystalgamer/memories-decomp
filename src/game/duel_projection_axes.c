#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "gpu_packets.h"
#include "duel_projection_axes.h"

#define LINE_G3_COORD_WORD(packet, member) ((PSXLONG *)&(packet)->member)

/* The two axis sweeps of the globe wireframe, called from Library_DrawWireframeGlobe with
 * everything living in PSX scratchpad: `packet` is the LINE_G3 at 0x1F800000
 * whose three packed screen-coordinate words RotAverage3 writes, `points` is
 * the run of SVECTOR at 0x1F800038 that it projects, and `control` is the
 * RotAverage3 flag pair at 0x1F800060. The helpers vary only x or z.
 * A negative projection-flags word causes one half-span retry, not a loop
 * or a successful-clipping guarantee. Both paths submit the packet.
 *
 * The offsets these used to be spelled with -- dst[0]/dst[4]/dst[8] on an
 * s16 pointer and (long *)(arg0 + 8) style casts -- are the members of those
 * two SDK types. Library_DrawWireframeGlobe uses the same scratchpad records.
 */

void Library_DrawGlobeXSpan(
    LINE_G3 *packet,
    GsOT *ot,
    SVECTOR *points,
    PSXLONG *control,
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
        LINE_G3_COORD_WORD(packet, x0),
        LINE_G3_COORD_WORD(packet, x1),
        LINE_G3_COORD_WORD(packet, x2),
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
            LINE_G3_COORD_WORD(packet, x0),
            LINE_G3_COORD_WORD(packet, x1),
            LINE_G3_COORD_WORD(packet, x2),
            &control[0],
            &control[1]
        );
    }
    func_8005B260((u32 *)packet, ot, 1, 1);
}

void Library_DrawGlobeZSpan(
    LINE_G3 *packet,
    GsOT *ot,
    SVECTOR *points,
    PSXLONG *control,
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
        LINE_G3_COORD_WORD(packet, x0),
        LINE_G3_COORD_WORD(packet, x1),
        LINE_G3_COORD_WORD(packet, x2),
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
            LINE_G3_COORD_WORD(packet, x0),
            LINE_G3_COORD_WORD(packet, x1),
            LINE_G3_COORD_WORD(packet, x2),
            &control[0],
            &control[1]
        );
    }
    func_8005B260((u32 *)packet, ot, 1, 1);
}
