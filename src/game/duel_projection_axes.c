#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "gpu_packets.h"

/* The two axis sweeps of the globe wireframe, called from func_80029934 with
 * everything living in PSX scratchpad: `packet` is the LINE_G3 at 0x1F800000
 * whose three packed screen-coordinate words RotAverage3 writes, `points` is
 * the run of SVECTOR at 0x1F800038 that it projects, and `control` is the
 * RotAverage3 flag pair at 0x1F800060. func_80029684 sweeps the x components
 * of the three points and func_800297DC the z components, which is the only
 * difference between them; both halve the span and reproject when
 * RotAverage3 reports the triangle clipped.
 *
 * The offsets these used to be spelled with -- dst[0]/dst[4]/dst[8] on an
 * s16 pointer and (long *)(arg0 + 8) style casts -- are the members of those
 * two SDK types, and func_80029934's candidate note in functions.csv already
 * identified both records.
 */

void func_80029684(
    LINE_G3 *packet,
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
        (long *)&packet->x0,
        (long *)&packet->x1,
        (long *)&packet->x2,
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
            (long *)&packet->x0,
            (long *)&packet->x1,
            (long *)&packet->x2,
            &control[0],
            &control[1]
        );
    }
    func_8005B260((u32 *)packet, ot, 1, 1);
}

void func_800297DC(
    LINE_G3 *packet,
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
        (long *)&packet->x0,
        (long *)&packet->x1,
        (long *)&packet->x2,
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
            (long *)&packet->x0,
            (long *)&packet->x1,
            (long *)&packet->x2,
            &control[0],
            &control[1]
        );
    }
    func_8005B260((u32 *)packet, ot, 1, 1);
}
