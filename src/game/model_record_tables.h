#ifndef MEMORIES_DECOMP_MODEL_RECORD_TABLES_H
#define MEMORIES_DECOMP_MODEL_RECORD_TABLES_H

#include "../types.h"
#include "../ygo_types.h"

/* The display-object stream handler table, the 0xB2-byte model record
   table and the corner table, declared here so the source that defines
   them and the three that read them cannot drift apart. */
extern s32 (*D_80090FEC[])(DisplayObjectStreamState *, const u8 *);
extern u8 D_80091008[];
extern u16 D_800914E8[];

#endif
