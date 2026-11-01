#ifndef MEMORIES_DECOMP_MODEL_DISTANCE_QUERIES_DATA_H
#define MEMORIES_DECOMP_MODEL_DISTANCE_QUERIES_DATA_H

#include "../types.h"

/* Zero-initialized data that model_distance_queries.c defines (#2602), for the units that
 * reach it without the unit's other declarations. */

/* An interior label, not separate storage: gModel_aSlots[2].model_id, the
 * stage slot's model id (0x800F2C40 + 2 * 0xE20 + 0xDF8). Model_SetSlotProperties
 * reads it through this label to pick gModel_pBackgroundRecord. */
extern u16 gModel_awStageSlotModelId[];

#endif
