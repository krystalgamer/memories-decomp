#include "../../types.h"

#define VERSION_JAPAN
#define gDuel_aCardRecords gJapanese_DuelCardRecords
#define DUEL_INIT_TERRAIN_SECTOR(value) ((((value) * 15) * 16 - (value)) + 0x16B5)
#define DUEL_INIT_TERRAIN_SECTOR_COUNT 0xEF
#define DUEL_INIT_DUELIST_DATA_FIRST_SECTOR 0x1D3E
#define D_8009B369 gJapanese_DuelFlag
#define D_8009B21C gJapanese_D_8009B21C
#define gDuel_abFieldGridCardRecordIndices gJapanese_DuelFieldGridCardRecordIndices
/* 0x110 below the US address, like its neighbour D_8009B1D8; splat labels it
   D_8009B0CC in the Japanese image. */
#define D_8009B1DC D_8009B0CC
#include "../duel_init_scene.c"
