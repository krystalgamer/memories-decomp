#ifndef MEMORIES_DECOMP_CREDITS_SECRET_NUMBERS_H
#define MEMORIES_DECOMP_CREDITS_SECRET_NUMBERS_H

#include "../ygo_types.h"

#define CREDITS_SECRET_NUMBER_COUNT 5

/* The five eight-digit secret numbers the credits display, each stored as a
   high/low four-digit pair. Main_RunCredits picks one by the save's game id
   modulo five. src/game/credits_secret_numbers.c owns them at 0x80090B50. */
extern u16 gCredits_awSecretNumbers[CREDITS_SECRET_NUMBER_COUNT][2];

#endif
