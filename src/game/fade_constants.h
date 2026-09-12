#ifndef MEMORIES_DECOMP_FADE_CONSTANTS_H
#define MEMORIES_DECOMP_FADE_CONSTANTS_H

/* Draw thirty independently stepped bands instead of only the tail box. */
#define FADE_FLAG_BANDED 0x01
/* Retain the overlay latch at level zero and use its configured OT depth. */
#define FADE_FLAG_KEEP_OVERLAY 0x02
/* With KEEP_OVERLAY, suppress the two secondary ordering-table submissions. */
#define FADE_FLAG_HIDE_SECONDARY_ORDERING_TABLE 0x04
/* Shade each channel from the stored tint instead of using a grey level. */
#define FADE_FLAG_TINTED 0x10
/* At black, restore the stored tint and re-arm the transition once. */
#define FADE_FLAG_RESTORE_TINT_AFTER_BLACK 0x20
/* Fade_Update advances the current level while this bit is set. */
#define FADE_FLAG_ACTIVE 0x80

/* D_8009B141 values consumed by Graphics_BeginFrame. */
#define FADE_ORDERING_TABLE_ACTIVE 0x01
#define FADE_ORDERING_TABLE_HIDE_SECONDARY 0x80

#endif
