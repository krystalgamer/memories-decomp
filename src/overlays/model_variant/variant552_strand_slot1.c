#include "../../types.h"
#define MODEL_VARIANT458_STRAND_POINTS 0x480
#define MODEL_VARIANT458_STRAND_LINE 0x890
#define MODEL_VARIANT458_STRAND_STATE 0x95C
#define MODEL_VARIANT458_STRAND_ORIGIN(work, i) MODEL_VARIANT_WORD(work, 0x8F0 + (i) * 4)
#define MODEL_VARIANT458_STRAND_SPAN 0x94E
#define MODEL_VARIANT458_STRAND_STEP(work) 1
#define MODEL_VARIANT458_STRAND_DEPTH(otz) ((otz) >> 2)
/* This image-tail copy was linked against an earlier resident layout, so its
 * library and model calls land on that layout's addresses. */
#define func_80058F10 func_800258E4
#define rsin func_800474D8
#define rcos func_800475A8
#define ScaleMatrix func_80048308
#define RotTransPers4 func_80048A98
#define RotMatrix func_80048CF8
#define GsSortLine func_8004AF68
#define GsSetLsMatrix func_8004C3D4
#define GsGetLs func_8004D5E8
#include "variant458_strand.c"
