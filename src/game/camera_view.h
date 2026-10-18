#ifndef MEMORIES_DECOMP_CAMERA_VIEW_H
#define MEMORIES_DECOMP_CAMERA_VIEW_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"

#define CAMERA_POSE_VECTOR_COUNT 3

/* The reference view the game keeps for GsSetRefView2, and the copy it
   stashes one record later.

   This is a Psy-Q type, not a game type: Model_ResetSceneView fills the eight
   longs and then calls `GsSetRefView2((GsRVIEW2 *)&gModel_RView)`, which is the
   SDK telling us the layout. Ten files used to declare it six different ways --
   `Block { int v[8]; }`, `LocalVectorSet { s32 x0, pad4, x8, xC, pad10,
   x14; }`, `Vec32 { s32 x, y, z; }`, `Mtx32 { s32 w[8]; }`, `u8 []`,
   `u16 []` and a bare `unsigned char` -- and every one of them is a partial
   view of GsRVIEW2's vpx/vpy/vpz, vrx/vry/vrz, rz and super.

   The header exists rather than an extern per file because GsRVIEW2 needs
   three Psy-Q headers behind it, and two of the ten users otherwise pull all
   three in just to name an opaque base pointer. */
/* The pose snapshot Model_CaptureKeyframePose refreshes each frame: element 0 is the
   viewpoint, element 1 the reference point, both taken as the low halfword
   of the corresponding GsRVIEW2 long, and element 2 is the distance, yaw
   and pitch triple at gModel_wViewDistance. The three-element shape is confirmed by
   Model_EvaluateKeyframeSpline, which indexes it with an eight-byte stride, and by the
   gaps the writer leaves at +6, +14 and +22, which are exactly SVECTOR's
   pad member. */
extern SVECTOR D_800F5768[CAMERA_POSE_VECTOR_COUNT];

#ifdef D_800F56F0_AS_HALFWORDS
extern u16 gModel_RView[];
#else
extern GsRVIEW2 gModel_RView;
#endif

/* Advance the camera move or follow/orbit the selected model slots. */
void func_80051A48(void);

/* Returns the shared reference view. Partial word/byte consumers keep an
   explicit view at their boundary; the underlying object is GsRVIEW2. */
GsRVIEW2 *Model_GetCameraViewBuffer(void);
extern GsRVIEW2 D_800F5710;

/* 0x800F56A0, the GsCOORDUNIT Model_ResetSceneView rebuilds in the block after
 * its GsSetRefView2 call (func_80052D2C.c:201): it zeroes rot (:204-206),
 * matrix.t (:207-209) and super (:210), passes &o->rot and &o->matrix to
 * RotMatrix_gte (:211) and clears flg (:212). Model_GetCameraCoordinateUnit returns its
 * address as GsCOORDUNIT *; nothing in src or the listings
 * calls it. gModel_RView above starts at +0x50, the size of GsCOORDUNIT. The
 * two units used to declare it GsCOORDUNIT and u8. */
extern GsCOORDUNIT D_800F56A0;
GsCOORDUNIT *Model_GetCameraCoordinateUnit(void);

#endif
