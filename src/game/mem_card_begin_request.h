#ifndef MEMORIES_DECOMP_MEM_CARD_BEGIN_REQUEST_H
#define MEMORIES_DECOMP_MEM_CARD_BEGIN_REQUEST_H

#include "../types.h"

/* Claims the single request slot the low-level card driver runs.
 *
 * Returns 0 while an earlier request is still pending (gMemCard_bRequest is
 * only negative when idle); otherwise records the channel and request code
 * in gMemCard_bChannel and gMemCard_bRequest, resets gMemCard_bRetries,
 * gMemCard_bLoadStep and gMemCard_bRequestStep, and marks gMemCard_nIOResult
 * pending. The unmatched poll at 0x80044838 dispatches on the code and puts
 * the slot back to -1 once it hands the caller a result:
 *
 *    1  MemCard_ReqCardInfo       _card_info only; reports the card's state
 *    2  MemCard_ReqLoadDirectory  info, clear and load; leaves the files listed
 *    3  MemCard_ReqReadFile       open O_RDONLY|O_NOWAIT, lseek, read
 *    4  MemCard_ReqWriteFile      open O_WRONLY|O_NOWAIT, lseek, write
 *    8  MemCard_ReqCreateFile     open O_CREAT with the block count
 *   11  MemCard_ReqReadSector     _card_read of one 128-byte sector
 *   12  MemCard_ReqWriteSector    _card_write of one 128-byte sector */
s32 MemCard_BeginRequest(s32 chan, s32 request);

/* The driver retains word-valued filename/buffer addresses. File offsets and
 * lengths are bytes; raw requests take a sector number and create takes blocks. */
s32 MemCard_ReqLoadDirectory(s32 channel);
s32 MemCard_ReqReadFile(s32 channel, s32 name, s32 buffer, s32 offset, s32 size);
s32 MemCard_ReqReadSector(s32 channel, s32 buffer, s32 sector);
s32 MemCard_ReqWriteFile(s32 channel, s32 name, s32 buffer, s32 offset, s32 size);
s32 MemCard_ReqWriteSector(s32 channel, s32 buffer, s32 sector);
s32 MemCard_ReqCreateFile(s32 channel, s32 name, s32 blocks);

#endif
