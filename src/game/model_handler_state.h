#ifndef MEMORIES_DECOMP_MODEL_HANDLER_STATE_H
#define MEMORIES_DECOMP_MODEL_HANDLER_STATE_H

#include "../types.h"

/* Model-handler state and diagnostics at 0x8009AFE4-0x8009B057.
 *
 * func_800540B4 updates the five leading state fields. The following word
 * pairs and mutable words are consumed by the neighbouring unmatched model
 * handlers, and the fixed-size strings are their diagnostic labels and
 * formatting fragments. Address-based names remain because the exact field
 * meanings are not yet established. D_8009B004 is a four-byte debug-state
 * record: two independent state bytes and a height halfword that is updated
 * unsigned but consumed signed. It is defined with
 * ModelDebug_UpdateController so MASPSX can see its small-data extent; the
 * surrounding prefix and diagnostic suffix retain their original separate
 * storage. */
extern u8 D_8009AFE4;
extern u8 D_8009AFE5;
extern u16 D_8009AFE6;
extern u8 D_8009AFE8;
extern u8 D_8009AFE9;
/* The mode 15 controller copies this view offset through a split absolute
 * address; its unknown-bound view changes no storage extent. */
#ifdef MODEL_HANDLER_VIEW_OFFSET_ABSOLUTE
extern u32 D_8009AFEC[];
#else
extern u32 D_8009AFEC[2];
#endif
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
typedef union {
    u32 word;
    u8 bytes[4];
    u16 halfwords[2];
    struct {
        u8 field_00;
        u8 field_01;
        s16 height;
    } fields;
} ModelDebugState;

typedef char ModelDebugState_size_must_be_4[
    sizeof(ModelDebugState) == 4 ? 1 : -1
];
typedef char ModelDebugState_height_offset_must_be_2[
    (u32)&((ModelDebugState *)0)->fields.height == 2 ? 1 : -1
];

extern ModelDebugState D_8009B004;
typedef union {
    u32 word;
    u8 display_enabled;
} ModelDebugDisplayState;

typedef char ModelDebugDisplayState_size_must_be_4[
    sizeof(ModelDebugDisplayState) == 4 ? 1 : -1
];

extern ModelDebugDisplayState D_8009B008;
#ifdef MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY
/* The debug controller uses absolute string addresses. Their definitions
 * retain eight bytes each (four for B02C); no larger backing is implied. */
extern char D_8009B00C[];
extern char D_8009B014[];
extern char D_8009B01C[];
extern char D_8009B024[];
extern char D_8009B02C[];
#else
extern char D_8009B00C[8];
extern char D_8009B014[8];
extern char D_8009B01C[8];
extern char D_8009B024[8];
extern char D_8009B02C[4];
#endif
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

/* Original sequence-handler assertion format and source-file label. */
extern char D_800117EC[];
extern char D_80011814[];

#endif
