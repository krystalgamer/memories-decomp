#ifndef MEMORIES_DECOMP_SOUND_DRIVER_H
#define MEMORIES_DECOMP_SOUND_DRIVER_H
#include "../types.h"

/* The sound output unit's exported entry points.
 *
 * SD_SetOutputType is deliberately absent: sound.h already declares it, and
 * one function belongs in one header.
 *
 * SD_GetStatusFlags returns the driver's flag word sign-extended from a
 * halfword, so it is s32 rather than the u32 its callers assumed. Every
 * observed use masks the result, so the two spellings agree in practice --
 * but only one of them matches the definition. */
s32 SD_GetStatusFlags(void);
void SD_Shutdown(void);
void SD_VSync(void);
/* Both fade the CD out and queue file-transfer command 32 with a sector
   offset and a source selector (0x10, 0x20 or 0x40). SD_StepActiveCommand
   copies the tail into the FileRequestSlot that SD_ConfigureTransferPhase
   consumes: an SPU phase (destination, encoded size) written through SpuWrite
   one staged sector at a time, then a RAM phase read directly to its
   destination. SD_EnqueueRamTransfer(ram, offset, ram_size, source) queues
   only the RAM phase; SD_EnqueueSpuRamTransfer(spu, ram, offset, spu_size,
   ram_size, source) queues both. Size parameters remain positive byte
   counts or negated sector counts; zero skips that phase. */
void SD_EnqueueRamTransfer(
    s32 ram_destination, s32 sector_offset, s32 ram_size, s32 source_selector
);
void SD_EnqueueSpuRamTransfer(
    s32 spu_destination, s32 ram_destination, s32 sector_offset,
    s32 spu_size, s32 ram_size, s32 source_selector
);
/* Prepares a BGM command without starting it: bit-15 ids queue a CD seek,
   other ids queue their music package load. */
void SD_BGMPreload(s32 command_id);
void SD_BGMPlayDefaultStep(u32 command_id);
void SD_BGMPlayWithStep(s32 command_id, s32 step);
void SD_BGMStop(u32 command_id);
void SD_BGMStopWithStep(u16 id, s32 step);
void SD_BGMFade(s32 step, s32 target);

/* Installed by SD_InitState in gFile_RangeReadCallback and run by File_StepActiveTransfer
 * when a sector-range CdlReadS stream starts (state 5): starts the CD input
 * fade up to 255 by 64 per update, clears s_wSD_STATUS bits 0x1/0x2 and sets
 * 0x4. When s_wSD_STATUS 0x80 is set and its level step is not
 * negative, it also stores step -16 in s_wSD_SEQ_FADE_STEP and 220 in s_bSD_BGM_FADE_TARGET.
 * SD_UpdateFades clamps the s_wSD_SEQ_FADE_LEVEL level that s_wSD_SEQ_FADE_STEP steps against
 * s_bSD_SEQ_FADE_TARGET (0xFF once a sequence starts), so that dip is undone in the same
 * update. s_bSD_BGM_FADE_TARGET is instead the target of the separate s_wSD_BGM_FADE_LEVEL ramp
 * driven by s_wSD_BGM_FADE_STEP (set by SD_StartBgmFade), so 220 retargets that ramp
 * if it is active. */
void SD_FadeInStreamAudio(void);
/* Installed in gFile_RangePauseCallback and run on the stream's pause (state 1) and end
 * (state 6): starts the CD input fade down to 0 by 64 per update and clears
 * s_wSD_STATUS bit 0x4. When s_wSD_STATUS 0x80 is set, it also stores step 8 in
 * s_wSD_SEQ_FADE_STEP and 255 in s_bSD_BGM_FADE_TARGET. */
void SD_FadeOutStreamAudio(void);
/* SD_InitState's SPU common setup: the mask selects master volume 0x3FFF,
 * CD volume 0x7FFF and CD mix on. The packet's CD reverb field is also
 * written (off) but SPU_COMMON_CDREV is not in the mask, so it is not
 * applied. */
void SD_InitSpuCommonVolume(void);

/* Stores `vsync_mode` in the secondary sound state's byte 0x509 and mirrors
 * whether it is non-zero into timer_update_inhibit. */
void SD_SetSequenceVSyncMode(s32 vsync_mode);

/* Takes SE, BGM and XA LBAs from Sound_InitFrontend. Zero arguments clear
 * s_bSD_DATA_SOURCE bits 0x01, 0x02 and 0x40; nonzero arguments do not set them.
 * Resets s_dwSD_BANK_LOAD_PHASE and raises 0x0A in s_wSD_STATUS. The XA-zero clear
 * is 0x40, not the loader/CD-wrapper gate at 0x80. */
void SD_InitDataSourceFlags(s32 se_lba, s32 bgm_lba, s32 master_xa_lba);

#endif
