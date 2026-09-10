#ifndef MEMORIES_DECOMP_SD_READ_SEQUENCE_HEADER_H
#define MEMORIES_DECOMP_SD_READ_SEQUENCE_HEADER_H

#include "../types.h"

/* Track and tempo stream setup on D_8009B458. It resets the record's stream
 * fields, reads the timebase and the raw tempo out of the sequence, derives
 * the tempo step from them, scales it by the timebase mode, and stores it
 * before handing the track to SD_ReadSequenceByte.
 *
 * The return type is s32 and the function returns 1 unconditionally. That is
 * the definition's own spelling, and it is what this header carries. The
 * local declaration it replaces said `void`, which nothing could catch: the
 * sole caller, SD_StartSequenceTracks, discards the result, so no call site
 * ever compared the two. Declaring it here, in a header the defining unit
 * includes, makes the compiler check it on every build.
 *
 * What this does NOT claim: that the returned 1 means anything. No caller
 * reads it, so the value is unexamined rather than understood. */
s32 SD_ReadSequenceHeader(void);

#endif
