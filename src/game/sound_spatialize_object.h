#ifndef MEMORIES_DECOMP_SOUND_SPATIALIZE_OBJECT_H
#define MEMORIES_DECOMP_SOUND_SPATIALIZE_OBJECT_H

#include "sound.h"

/* Computes one secondary sound object's pan position and stereo level pair.
 *
 * `object` is an SDSecondaryObject out of SDSecondaryState::objects; `channel`
 * is the SDSecondaryRecord out of SDSecondaryState::channels that the object's
 * channel_index selects. The two resident callers and the retained note-start
 * candidate select entries in these distinct arrays from the same state base.
 *
 * The pan sum combines the channel's pan (controller 0x0A) with the object's
 * own two pan bytes; the level folds in the channel's volume (controller 7)
 * and expression (controller 0x0B). Results land in the object's `pan`,
 * `level_left` and `level_right`.
 *
 * The function returns nothing; the two declarations that gave it an int
 * return were guesses that never had a value to use. */
void SD_SpatializeSecondaryObject(SDSecondaryObject *object,
                                 SDSecondaryRecord *channel);

#endif
