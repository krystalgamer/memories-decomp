#ifndef MEMORIES_DECOMP_CAMERA_VIEW_H
#define MEMORIES_DECOMP_CAMERA_VIEW_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* The reference view the game keeps for GsSetRefView2, and the copy it
   stashes one record later.

   This is a Psy-Q type, not a game type: func_800530C4 fills the eight longs
   and then calls `GsSetRefView2((GsRVIEW2 *)&D_800F56F0)`, which is the SDK
   telling us the layout. Ten files used to declare it six different ways --
   `Block { int v[8]; }`, `LocalVectorSet { s32 x0, pad4, x8, xC, pad10,
   x14; }`, `Vec32 { s32 x, y, z; }`, `Mtx32 { s32 w[8]; }`, `u8 []`,
   `u16 []` and a bare `unsigned char` -- and every one of them is a partial
   view of GsRVIEW2's vpx/vpy/vpz, vrx/vry/vrz, rz and super.

   The header exists rather than an extern per file because GsRVIEW2 needs
   three Psy-Q headers behind it, and two of the ten users otherwise pull all
   three in just to name an opaque base pointer. */
/* The pose snapshot func_8005F070 refreshes each frame: element 0 is the
   viewpoint, element 1 the reference point, both taken as the low halfword
   of the corresponding GsRVIEW2 long, and element 2 is the distance, yaw
   and pitch triple at D_8009B478. The three-element shape is confirmed by
   func_8005EBF4, which indexes it with an eight-byte stride, and by the
   gaps the writer leaves at +6, +14 and +22, which are exactly SVECTOR's
   pad member. */
extern SVECTOR D_800F5768[3];

extern GsRVIEW2 D_800F56F0;
extern GsRVIEW2 D_800F5710;

#endif
