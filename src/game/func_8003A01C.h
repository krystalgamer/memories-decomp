#ifndef MEMORIES_DECOMP_FUNC_8003A01C_H
#define MEMORIES_DECOMP_FUNC_8003A01C_H

#include "../types.h"
#include "../ygo_types.h"

/* The file-transfer phase callback func_8003A560 installs for a duel effect's
 * VRAM upload. `mode` selects the phase; the descriptor it is handed is the
 * one File_TryRequestAsyncTransfer returned, and every offset the function
 * touches is a named FileTransferDescriptor member.
 *
 * Phase 2 programs the descriptor's leading RECT with the clut rectangle --
 * x 512, y position * 2 + 240, w 256, h 2 -- and hands it straight to
 * LoadImage2. func_8003A560 writes exactly those four values into its own
 * slot->clut_rect when it does the upload inline instead. Phase 0 likewise
 * puts 832 - position * 192 in the 0x30 halfword pair, which is the
 * slot->image_rect.x that function computes, and matches what
 * FileTransferDescriptor's own comment calls the VRAM position pair the
 * phase-2 paths write there.
 *
 * The two `*(s32 *)&p->phase_size = FILE_SECTOR_SIZE;` stores in phases 1 and
 * 2 are spelled that way deliberately. Written as the plain assignment,
 * GCC cross-jumps the two phases' tails into one shared
 * block and the executable comes out eight bytes short; retail keeps them
 * duplicated. Only that one statement does it -- `done`, `value_08` and
 * `value_0C` all convert plainly in the same tails, and phase 0's
 * `p->phase_size = 48 * FILE_SECTOR_SIZE;` is fine because the larger
 * constant needs its own
 * lui/ori either way. */
void func_8003A01C(FileTransferDescriptor *descriptor, s32 mode);

#endif
