#ifndef MEMORIES_DECOMP_MODEL_PACKET_HANDLERS_H
#define MEMORIES_DECOMP_MODEL_PACKET_HANDLERS_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"

/* One keyframe of a translation-and-rotation track: a position, then three
 * angles in MODEL_ANGLE_FULL_TURN units. */
typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 rx;
    s16 ry;
    s16 rz;
} ModelKeyframe;

/* The words an animation driver finds right after the header_size words of
 * its header: the sequence being played, the keyframe it is leaving, the
 * keyframe it is heading for, and an optional record the interpolated values
 * are copied back to. */
typedef struct {
    GsSEQ *seq;
    ModelKeyframe *source;
    ModelKeyframe *target;
    ModelKeyframe *out;
} ModelAnimParams;

/* Interpolates one animation track; func_8005C768 returns it for type
 * 0x03002019. It is an HMD animation driver, so it takes the argument block
 * libhmd hands every GsU_03xxxxxx driver. */
int func_8005CEF0(GsARGUNIT_ANIM *sp);

/* Installs the packet handler for one model's animation stream.
 *
 * func_8005C768 maps a packet's type word to the routine that consumes it,
 * returning it as a plain address because the slot it is written into is
 * untyped. It answers with the library's own GsU_00000000 for anything it
 * does not recognise, so callers get a valid handler rather than a null. */
void func_8005C6A0(s32 *object, u8 *entry);
void *func_8005C768(u32 value);

#endif
