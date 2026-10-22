#include "../../types.h"

#define VERSION_JAPAN
#define gDuel_aCardRecords gJapanese_DuelCardRecords
#define DUEL_INIT_TERRAIN_SECTOR(value) ((((value) * 15) * 16 - (value)) + 0x16B5)
#define DUEL_INIT_TERRAIN_SECTOR_COUNT 0xEF
#define DUEL_INIT_DUELIST_DATA_FIRST_SECTOR 0x1D3E
#define D_8009B369 gJapanese_DuelFlag
#define gDuel_abFieldGridCardRecordIndices gJapanese_DuelFieldGridCardRecordIndices
/* 0x110 below the US address, like its neighbour gDuel_pSide0SaveData; splat labels it
   D_8009B0CC in the Japanese image. */
#define gDuel_pSide1SaveData D_8009B0CC
#include "../duel_init_scene.c"
