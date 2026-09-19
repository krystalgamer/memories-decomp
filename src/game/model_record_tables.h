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

/* How func_800540B4 reads D_800914E8: eight x/z direction pairs in Q12, one
   per 45 degrees, which are the rim points of the ground shadow fan. The
   definition stays sixteen halfwords; the reader indexes it through this
   view so loop.c turns the index into retail's walking pointer. */
typedef struct {
    s16 x;
    s16 z;
} ModelShadowFanStep;

typedef char ModelShadowFanStep_size_must_be_4[
    sizeof(ModelShadowFanStep) == 4 ? 1 : -1
];

#endif
