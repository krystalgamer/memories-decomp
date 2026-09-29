/* CANDIDATE SUPPORT, NOT BUILT: the call-site prototypes the parked trap,
 * shower and vortex candidates in this directory were measured with. The
 * matched units use dispatch.h, color_helpers.h and the other shared headers
 * instead; this copy keeps the candidates reproducible without adding
 * duplicate declarations under src/. */
#ifndef MEMORIES_DECOMP_DUEL_EFFECT_ROUTINES_H
#define MEMORIES_DECOMP_DUEL_EFFECT_ROUTINES_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/duel_effect_request.h"
#include "utility_helpers.h"
#include "packet_helpers.h"
#include "textured_quads.h"
#include "layered_drawing.h"
#include "drawing_tail.h"

/* Texture words the dispatcher packs once per bank load from the 21 image
 * descriptors, and the mode word of each. D_8015B748 itself is declared by
 * textured_quads.h. */
extern GsIMAGE D_8015A1E4[21];
extern u16 D_8015A430[21];

void func_80146258(s32 id, s32 phase, void *buffer, DuelEffectRequest *request);

/* The effect routines the dispatcher selects, in id order. The prototypes
 * of the ones without a matched body are the dispatcher's call sites. */
void func_80154688(void *buffer, s32 phase);
void func_80157794(void *buffer, s32 phase);
void func_80153200(void *buffer, s32 phase, s16 value);
void func_80149F90(void *buffer, s32 phase);
void func_80159AAC(void *buffer, s32 phase);
void func_80157E10(void *buffer, s32 phase);
void func_80154B30(void *buffer, s32 phase);
void func_801587D8(void *buffer, s32 phase);
void func_80147B18(void *buffer, s32 phase);
void func_801593A8(void *buffer, s32 phase);
void func_8014C8FC(void *buffer, s32 phase);
void func_80146760(void *buffer, s32 phase);
void func_80150E00(void *buffer, s32 phase);
void func_801503F8(void *buffer, s32 phase, s16 value);
void func_801481A8(void *buffer, s32 phase);
void func_8014FF40(void *buffer, s32 phase);
void func_80151558(void *buffer, s32 phase);
void func_80148BA4(void *buffer, s32 phase);
void func_80154084(void *buffer, s32 phase);
void func_80153ADC(void *buffer, s32 phase);
void func_8014E3EC(void *buffer, s32 phase);
void func_8014A8E4(void *buffer, s32 phase);
void func_80152048(void *buffer, s32 phase);
void func_8014D3E8(void *buffer, s32 phase);

/* The colour transitions of color_transition.c, declared as the effect
 * routines call them. func_8015405C is declared with u8 parameters where
 * it is defined; the vortex effect passes it a u16 field three times with
 * no masking, so this header carries the wider view and is not included
 * together with color_helpers.h. */
void func_80153F28(u8 *color, u16 step);
s32 func_80153F98(u8 *color, u8 red, u8 green, u8 blue, u16 step);
void func_8015405C(u16 high, u16 middle, u16 low);

/* Bank helpers without a matched body in this tree, declared from their
 * call sites in the effect routines. */
void func_8014EA7C(u16 radius, SVECTOR *vectors);
void func_8014EB1C(u16 x, u16 y, SVECTOR *vectors, u16 count);
void func_8014EE0C(u16 x_radius, u16 y_radius, s16 angle, SVECTOR *vectors, u16 count);
void func_8014EC8C(u16 width, u16 depth, u16 height, u16 angle, SVECTOR *vertices, u16 count);
void func_8014EF2C(u16 count, SVECTOR *vectors);
void func_8014FABC(u16 count, u16 step, u16 radius, u16 height, SVECTOR *vertices);

#endif
