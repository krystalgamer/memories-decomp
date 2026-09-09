#ifndef MEMORIES_DECOMP_FILE_TRANSFER_DESCRIPTOR_VIEW_H
#define MEMORIES_DECOMP_FILE_TRANSFER_DESCRIPTOR_VIEW_H

#include "../types.h"

/* The word-granularity view of a file transfer descriptor, used by the two
 * mode-configuring transfer callbacks, func_8002BD0C and func_800577B0.
 *
 * This is the same 0x48-byte record as FileTransferDescriptor in ygo_types.h,
 * and the evidence is not circumstantial:
 *
 *   - both functions are handed to File_RequestAsyncTransfer as the
 *     FileTransferCallback, so the pointer they receive is the descriptor;
 *   - the size is 0x48, which is FILE_TRANSFER_DESCRIPTOR_WORD_COUNT words;
 *   - x, y, w and h at 0x00..0x07 agree with the descriptor by name and type;
 *   - field30's {lo, hi} pair lands exactly on counter (0x30) and field_32
 *     (0x32), and field46 on done (0x46).
 *
 * It is nonetheless a separate description on purpose, for the same reason
 * display_object_config.h is separate from display_object.h. Both functions
 * write 0x30 as one word, which the descriptor spells as two u16 halves.
 * Reaching it as `*(s32 *)&...` instead of a union member is semantically
 * identical but not equivalent to the compiler: it reorders six instructions
 * in func_8002BD0C, because GCC schedules a union member store differently
 * from a store through a cast. Verified by changing only that one access in
 * the original file, with the struct untouched, and reproducing the mismatch.
 *
 * So the canonical descriptor owns the half-width fields and this view records
 * the word shape these two callbacks work in. Neither replaces the other, and
 * merging them costs the byte-exact build until the descriptor grows a union
 * at 0x30 that its own 31 existing readers can still reach.
 */
typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    s32 field08;
    s32 field0C;
    u8 pad10[0xC];
    s32 field1C;
    u8 pad20[0x10];
    union {
        struct {
            s16 lo;
            s16 hi;
        } h;
        s32 w;
    } field30;
    u8 pad34[0x12];
    u8 field46;
} FileTransferDescriptorView;

typedef char FileTransferDescriptorView_size_must_be_0x48[
    sizeof(FileTransferDescriptorView) == 0x48 ? 1 : -1
];

#endif
