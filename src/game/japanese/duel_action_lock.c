#include "../../types.h"
#define VERSION_JAPAN

/* SLPM-86398 build of src/game/duel_action_lock.c. The Japanese WA.MRG gives each
 * terrain package 0xEF sectors (see duel_load_terrain_package.c), and a
 * terrain's effect data starts at 0x1784 + 0xEF * terrain. */
#define DUEL_TERRAIN_EFFECT_SECTOR(terrain) \
    ((((terrain) * 15) * 16 - (terrain)) + 0x1784)
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#include "../duel_action_lock.c"
