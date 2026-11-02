#ifndef GAME_GRAPHICS_ORDERING_TABLE_VIEWS_H
#define GAME_GRAPHICS_ORDERING_TABLE_VIEWS_H

#include "ordering_tables.h"

#ifdef ORDERING_TABLE_SLOT1_ARRAY
extern GsOT *gGraphics_pOrderingTable1[4];
#else
extern GsOT *gGraphics_pOrderingTable1;
#endif

#ifdef ORDERING_TABLE_SLOT2_ARRAY
extern GsOT *gGraphics_pOrderingTable2[];
#else
extern GsOT *gGraphics_pOrderingTable2;
#endif

extern GsOT *gGraphics_pOrderingTable3;

#endif
