#ifndef MEMORIES_DECOMP_MODEL_SEQUENCE_DESCRIPTOR_H
#define MEMORIES_DECOMP_MODEL_SEQUENCE_DESCRIPTOR_H

#include "../types.h"

/* One 32-bit entry of an HMD animation sequence (the GsARGUNIT_ANIM ctop
 * array that GsSEQ ti/ci/ii index). Bits 0-15 are the keyframe parameter
 * index, bits 16-23 the entry length in sixteenths of a frame step (tframe)
 * and bits 24-30 the interpolation type that selects the update handler.
 * Bit 31 marks a control entry rather than a keyframe; walkers test it on
 * the whole word, so the word arm stays the accessor for that test. */
typedef union {
    u32 word;
    struct {
        u16 param_index;
        u8 tframe;
        u8 type;
    } f;
} ModelSeqDescriptor;

#define MODEL_SEQ_DESCRIPTOR_CONTROL 0x80000000
#define MODEL_SEQ_DESCRIPTOR_TYPE_MASK 0x7F

typedef char ModelSeqDescriptor_size_must_be_4[
    sizeof(ModelSeqDescriptor) == 4 ? 1 : -1];
typedef char ModelSeqDescriptor_tframe_offset_must_be_2[
    (u32)&((ModelSeqDescriptor *)0)->f.tframe == 2 ? 1 : -1];
typedef char ModelSeqDescriptor_type_offset_must_be_3[
    (u32)&((ModelSeqDescriptor *)0)->f.type == 3 ? 1 : -1];

#endif
