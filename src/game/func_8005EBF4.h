#ifndef YUGIOH_GAME_FUNC_8005EBF4_H
#define YUGIOH_GAME_FUNC_8005EBF4_H

#include "../types.h"
#include "model_effect_key.h"

/* One entry of the keyframe ring at D_800F5788. func_8005F91C installs two
   requested endpoints, func_8005FB30 resolves them into the following pair,
   and func_8005E808 maintains the trailing radius state. */

/* The ring itself, ten records, declared here because the evaluator and
 * dispatcher share it and this is where Key lives. Two facts bound it at
 * ten: the next named object, D_800F5918 (c_symbols.ld:396, model.h:541-542),
 * starts at +0x190 = 10 * 0x28, and func_8005F91C writes a record only while
 * `D_8009B078 < 0xA` (model_effect_state.c; D_8009B078 is a u8,
 * model_transfer_flags.h:47). The same unit stores the ring's address into
 * D_8009B074.
 * func_8005DBA4, still assembly, forms the address at func_8005DBA4.s:34,
 * :38-40 and :807-809. The evaluator used to declare it `Key []` and
 * func_8005F91C used a local `u8 []` declaration. */
extern Key D_800F5788[10];

/* One 8-byte channel record. The evaluator reads the first three halfwords as
   the interpolated components; `w` covers the halfword at +6, which the
   keyframe records use as a channel kind and which the current pose leaves
   untouched. */
typedef struct {
    s16 x;
    s16 y;
    s16 z;
    u16 w;
} Coeff;

void func_8005EBF4(Key *cur, s32 k, s32 scale, s32 den, s16 *out);

/* Seeds the pose the evaluator above starts from. Declared here because the
   two live in one translation unit. */
void func_8005F070(s32 enabled);

#endif
