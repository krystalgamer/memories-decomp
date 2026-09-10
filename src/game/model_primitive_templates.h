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
 * What this does NOT claim: which handler uses which word. The opcode and
 * colour fields are legible from the values themselves, but the mapping from
 * template to caller is not established, and none of the readers is matched C
 * yet. */
extern u32 D_8009AFAC;
extern u32 D_8009AFB0;
extern u32 D_8009AFB4;
extern u32 D_8009AFB8;
extern u32 D_8009AFBC;
extern u32 D_8009AFC0;
extern u32 D_8009AFC4;
extern u32 D_8009AFC8;
extern u32 D_8009AFCC;
extern u32 D_8009AFD0;
extern u32 D_8009AFD4;
extern u32 D_8009AFD8;
extern u32 D_8009AFDC;
extern u32 D_8009AFE0;

#endif
