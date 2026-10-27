#ifndef MEMORIES_DECOMP_SOUND_STATE_CONTROL_H
#define MEMORIES_DECOMP_SOUND_STATE_CONTROL_H

#include "../types.h"

/* Stores `vsync_mode` in the secondary sound state's byte 0x509 and mirrors
 * whether it is non-zero into timer_update_inhibit. */
void SD_SetSequenceVSyncMode(s32 vsync_mode);

/* Takes SE, BGM and XA LBAs from Sound_InitFrontend. Zero arguments clear
 * data_source_flags bits 0x01, 0x02 and 0x40; nonzero arguments do not set them.
 * Resets bank_load_phase and raises 0x0A in status_flags. The XA-zero clear
 * is 0x40, not the loader/CD-wrapper gate at 0x80. */
void SD_InitDataSourceFlags(s32 se_lba, s32 bgm_lba, s32 master_xa_lba);

#endif
