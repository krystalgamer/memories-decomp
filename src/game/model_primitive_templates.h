#ifndef MEMORIES_DECOMP_MODEL_PRIMITIVE_TEMPLATES_H
#define MEMORIES_DECOMP_MODEL_PRIMITIVE_TEMPLATES_H

#include "../types.h"

/* Prototype GPU primitive words the model handlers stamp copies of.
 *
 * Each word carries a GPU command byte in its top position over a colour in
 * the low three: 0x24, 0x2C, 0x34 and 0x3C are the textured polygon opcodes,
 * 0x4A and 0x4E the line opcodes, and the run closes with an 0xE1 draw-mode
 * word and three zero words. Two colour families appear, 0x808080 and
 * 0x404040, which is a full-brightness template and a half-brightness one.
 *
 * These are declared as separate u32 objects rather than one array because
 * that is how the image names them: splat emits a distinct label every four
 * bytes, and the sixty-seven functions that reach into this run are model
 * handlers that address the words individually. Nothing observed groups them,
 * so no array shape is claimed here.
 *
 * The names come from the words Model_DrawSlot's MODEL_STAMP_TEMPLATES
 * writes each draw: the tint colour over POLY_FT3/FT4/GT3/GT4 (0x24, 0x2C,
 * 0x34, 0x3C, gaining the 0x02 semi-transparency bit when the fade mode is
 * set), the same four with the semi-transparency bit always set (0x26, 0x2E,
 * 0x36, 0x3E), and LINE_F3/LINE_F4 with it set (0x4A, 0x4E).
 *
 * What this does NOT claim: which handler uses which word. The mapping from
 * template to caller is not established, and none of the readers is matched C
 * yet. */
extern u32 gModel_dwPolyFt3Template;
extern u32 gModel_dwPolyFt4Template;
extern u32 gModel_dwPolyGt3Template;
extern u32 gModel_dwPolyGt4Template;
extern u32 gModel_dwPolyFt3SemiTemplate;
extern u32 gModel_dwPolyFt4SemiTemplate;
extern u32 gModel_dwPolyGt3SemiTemplate;
extern u32 gModel_dwPolyGt4SemiTemplate;
extern u32 gModel_dwLineF3Template;
extern u32 gModel_dwLineF4Template;
extern u32 D_8009AFD4;
extern u32 D_8009AFD8;
extern u32 D_8009AFDC;
extern u32 D_8009AFE0;

#endif
