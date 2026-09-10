#ifndef MEMORIES_DECOMP_FILE_CD_TRANSFER_H
#define MEMORIES_DECOMP_FILE_CD_TRANSFER_H

#include "../types.h"

/* The DS command-completion callbacks the loader hands to DsCommand and
   DsPacket. Each one is registered with the command it belongs to and is
   called back with the drive's event code:
     5  the command needs retrying - bump D_8009B130 and reissue the identical
        command, which is why each handler names itself;
     2  the command completed - do that command's own work and clear the
        in-flight bit 0x400 of D_8009B0F4.
   Every other event is ignored.

   They are declared here rather than at the one call site because that site
   spelled all seven `void (void)`, which is the wrong arity for all of
   them. */
void func_800140A0(u8 event);
void func_80014134(u8 event);
void func_800141A8(u8 event);

/* Takes the event as a word and masks it, rather than as u8 like its
   siblings. */
void func_80014220(s32 event);

void func_80014294(u8 event);
void func_80014308(u8 event);

/* The seek callback, and the only one with a second parameter: on completion
   the drive hands back the position, which this converts and stores. It has
   no retry arm, and it clears bit 0x800 rather than 0x400. */
void func_80014390(u8 event, s32 position);

#endif
