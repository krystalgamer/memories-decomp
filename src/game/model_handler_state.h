#ifndef MEMORIES_DECOMP_MODEL_HANDLER_STATE_H
#define MEMORIES_DECOMP_MODEL_HANDLER_STATE_H

#include "../types.h"

/* Model-handler state and diagnostics at 0x8009AFE4-0x8009B057.
 *
 * func_800540B4 updates the five leading state fields. The following word
 * pairs and mutable words are consumed by the neighbouring unmatched model
 * handlers, and the fixed-size strings are their diagnostic labels and
 * formatting fragments. Address-based names remain because the exact field
 * meanings are not yet established. */
extern u8 D_8009AFE4;
extern u8 D_8009AFE5;
extern u16 D_8009AFE6;
extern u8 D_8009AFE8;
extern u8 D_8009AFE9;
extern u32 D_8009AFEC[2];
/* The control dispatcher needs split absolute addresses for these calls.
 * Its unknown-bound view changes no storage extent. */
#ifdef MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY
extern char D_8009AFF4[];
#else
extern char D_8009AFF4[8];
#endif
/* The unknown-bound view selects the intro controller's split absolute
 * addressing; the definition remains the same two .sdata words. */
#ifdef MODEL_HANDLER_OFFSET_ABSOLUTE
extern u32 D_8009AFFC[];
#else
extern u32 D_8009AFFC[2];
#endif
extern u32 D_8009B004;
extern u32 D_8009B008;
extern char D_8009B00C[8];
extern char D_8009B014[8];
extern char D_8009B01C[8];
extern char D_8009B024[8];
extern char D_8009B02C[4];
#ifdef MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY
extern char D_8009B030[];
extern char D_8009B038[];
extern char D_8009B040[];
extern char D_8009B048[];
extern char D_8009B050[];
#else
extern char D_8009B030[8];
extern char D_8009B038[8];
extern char D_8009B040[8];
extern char D_8009B048[8];
extern char D_8009B050[8];
#endif

extern char D_80011574[];

#endif
