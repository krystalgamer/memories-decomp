#ifndef MEMORIES_DECOMP_FILE_CD_TRANSFER_H
#define MEMORIES_DECOMP_FILE_CD_TRANSFER_H

#include "../types.h"

/* The DS command-completion callbacks the loader hands to DsCommand and
   DsPacket, each named for the command it completes. Each one is registered
   with that command and is called back with the drive's event code:
     DslDiskError  the command needs retrying - bump D_8009B130 and reissue
                   the identical command, which is why each handler names
                   itself;
     DslComplete   the command completed - do that command's own work and
                   clear the in-flight bit 0x400 of D_8009B0F4.
   Every other event is ignored.

   The first three serve the ordinary data read. The File_Xa* four serve the
   sector-range path, which pauses, sets the subheader filter, reads with
   DslModeRT|DslModeSF|DslModeAP - real-time XA-ADPCM - and then polls the
   head position until the end sector.

   They are declared here rather than at the one call site because that site
   spelled all seven `void (void)`, which is the wrong arity for all of
   them. */
void File_ReadNCB(u8 event);
void File_SeekLCB(u8 event);
void File_PauseCB(u8 event);

/* Takes the event as a word and masks it, rather than as u8 like its
   siblings. */
void File_XaPauseCB(s32 event);

void File_XaSetfilterCB(u8 event);
void File_XaReadSCB(u8 event);

/* The DslGetlocL callback, and the only one with a second parameter: on
   completion the drive hands back the position, which this converts and
   stores. It has no retry arm, and it clears bit 0x800 rather than 0x400. */
void File_XaGetlocLCB(u8 event, s32 position);

#endif
