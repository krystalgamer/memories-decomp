#ifndef MEMORIES_DECOMP_FILE_TRANSFER_BUFFER_H
#define MEMORIES_DECOMP_FILE_TRANSFER_BUFFER_H

#include "../types.h"
#include "file_constants.h"
#include "mem_card.h"

/* A card's 2D art record is the largest load known to land here: seven
 * sectors read by CD DMA (notes/research/Unchiga_Symbols/findings.md F125). */
#define FILE_TRANSFER_BUFFER_SECTORS 7

/* 0x801DC000, bound as gLibrary_aCardArtRecord in each region's symbols.txt.
 * File_SetPositionTable hands its address to File_InitTransferState
 * (src/game/file_setup.c), which stores it into gFile_TransferBufferBase
 * (File_InitTransferState, file_transfer_runtime.c); the loader then fills it
 * a sector at a time. The two memory-card dialogs also reach it, always by
 * address: MemCardDialog_UpdateSave and MemCardDialog_UpdateTradeSave
 * (mem_card.c) pass it to MemCardReadFile as the destination of a 0x480-byte
 * read of a saved state block and compare the result as a SaveDataState.
 * The contents are phase dependent; gFile_abPackageStagingBuffer
 * (file_package_staging.h) is named 0x1000 bytes higher and
 * gText_GlyphUploadScratch 0x2000 lower. */
typedef union {
    u8 bytes[FILE_TRANSFER_BUFFER_SECTORS * FILE_SECTOR_SIZE];
    /* LIBMCRD's MemCardReadFile destination type. */
    unsigned long words[
        FILE_TRANSFER_BUFFER_SECTORS * FILE_SECTOR_SIZE / sizeof(unsigned long)
    ];
    SaveDataState save_state;
} FileTransferBuffer;

extern FileTransferBuffer gLibrary_aCardArtRecord;

#endif
