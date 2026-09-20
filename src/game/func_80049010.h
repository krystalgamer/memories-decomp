#ifndef MEMORIES_DECOMP_FUNC_80049010_H
#define MEMORIES_DECOMP_FUNC_80049010_H

/* Retires the driver's two pending requests: releases the held voice at
   field_157E and stops the sequence at field_157A, then clears the pending
   pair at field_1586/field_1588. */
void SD_ResetMusicState(void);

#endif
