#ifndef YUGIOH_GAME_FUNC_8005EBF4_H
#define YUGIOH_GAME_FUNC_8005EBF4_H

#include "../types.h"

/* One entry of the keyframe ring at D_800F5788. The record is treated as an
   opaque 0x28-byte blob here: the evaluator only ever indexes into it by
   channel through `(u8 *)key + k * 8`, so no field layout is asserted. */
typedef struct {
    u8 bytes[0x28];
} Key;

/* Timing/state tail used by func_8005E808; Key stays opaque to the evaluator. */
#ifdef MODEL_KEYFRAME_TIMING_VIEW
typedef struct {
    u8 pad_00[0x20];
    s16 field_20;
    u16 field_22;
    s16 field_24;
    u8 field_26;
    u8 pad_27;
} ModelKeyframeTimingView;

typedef char ModelKeyframeTimingView_size_must_be_0x28[
    sizeof(ModelKeyframeTimingView) == 0x28 ? 1 : -1
];
#endif

/* The ring itself, ten records, declared here because the evaluator and
 * dispatcher share it and this is where Key lives. Two facts bound it at
 * ten: the next named object, D_800F5918 (c_symbols.ld:396, model.h:541-542),
 * starts at +0x190 = 10 * 0x28, and func_8005F91C writes a record only while
 * `D_8009B078 < 0xA` (model_effect_state.c; D_8009B078 is a u8,
 * model_transfer_flags.h:47). That unit stores through a u8 pointer: two
 * 8-byte ModelEffectEndpoint copies at +0 and +8, halfwords at +6, +0xE,
 * +0x20, +0x22 and +0x24, a byte at +0x26 (:51-66), and stores the ring's
 * address into D_8009B074. func_8005EBF4 forms Key pointers from the ring and
 * derives each channel pointer as `(u8 *)key + k * 8`.
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

/* Computes the duration/bound at +0x22 from the signed control at +0x20
 * and the key's two endpoint channels. Preserves the caller's byte view. */
void func_8005E808(u8 *key);

/* Seeds the pose the evaluator above starts from. Declared here because the
   two live in one translation unit. */
void func_8005F070(s32 enabled);

#endif
