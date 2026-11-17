#ifndef MEMORIES_DECOMP_MODEL_RECORD_TABLES_H
#define MEMORIES_DECOMP_MODEL_RECORD_TABLES_H

#include "../types.h"
#include "../ygo_types.h"

/* The display-object stream handler table, the 0xB2-byte model record
   table and the corner table, declared here so the source that defines
   them and the three that read them cannot drift apart. */
struct DisplayObject;
extern s32 (*gDisplayObjectStream_apfnCommandHandlers[])(struct DisplayObject *, const u8 *);
extern u8 gModel_abBackgroundRecords[];

/* gModel_aShadowFanSteps: eight x/z direction pairs in Q12, one per 45
   degrees, which are the rim points of Model_DrawSlot's ground shadow fan. */
typedef struct {
    s16 x;
    s16 z;
} ModelShadowFanStep;

extern ModelShadowFanStep gModel_aShadowFanSteps[];

typedef char ModelShadowFanStep_size_must_be_4[
    sizeof(ModelShadowFanStep) == 4 ? 1 : -1
];

#endif
