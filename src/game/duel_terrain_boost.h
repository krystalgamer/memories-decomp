#ifndef MEMORIES_DECOMP_DUEL_TERRAIN_BOOST_H
#define MEMORIES_DECOMP_DUEL_TERRAIN_BOOST_H

#include "../types.h"
#include "card_constants.h"

/* Six terrains, matching the range Duel_GetTerrainBoost indexes with
   gDuel_bTerrain[0] - 1. */
#define DUEL_TERRAIN_COUNT 6
#define DUEL_TERRAIN_STAT_ADJUSTMENT 500
#define DUEL_TERRAIN_BOOST_UNIT \
    (DUEL_TERRAIN_STAT_ADJUSTMENT / CARD_STAT_SCALE)
#define DUEL_TERRAIN_PENALTY_UNIT (-DUEL_TERRAIN_BOOST_UNIT)

/* gDuel_bTerrain is a SINGLE BYTE at 0x8009B364. symbols.txt puts
   gFreeDuel_bReturnFlags at 0x8009B365, so there is no room for a second
   element, and every reader in the tree uses index 0 or reads it as a scalar.

   It is nevertheless declared several incompatible ways on purpose. The
   declaration is the lever that picks an addressing form, and which lever a TU
   needs follows from the -G setting of its compiler profile, so the spelling is
   a property of the profile rather than a matter of taste:

     -G0 profiles (gcc_2_8_1_g0) put nothing in small data, so a plain scalar
     already gets lui %hi / %lo and needs no help:
         func_80024DC8.c, src/candidates/func_80071460.c,
         src/candidates/func_80038530.c
             extern u8 gDuel_bTerrain;            (also spelled unsigned char)

     -G8 profiles would make a one-byte object small data and address it
     %gp_rel, so a TU that needs the absolute form must push the symbol out of
     small data. An array of unknown or large size does that:
         func_8001798C.c        (gcc_2_8_1_g8)         extern u8 gDuel_bTerrain[];
         duel_card_record_lifecycle.c (gcc_2_8_1_g8_split) extern u8 gDuel_bTerrain[];

     ...or section(".data") does it while keeping the scalar, which is what
     buys the assembler macro form those two functions need:
         src/candidates/func_800179F4.c (gcc_2_8_1_g8_split),
         main_run_animated_battle.c
             extern u8 gDuel_bTerrain __attribute__((section(".data")));

     func_80024E58 was the one that needed a NUMBER. Its profile compiled at
     -G8 but assembled at -G4 (gcc_2_8_1_cc_g8_as_g4_split), so the array had
     to have a size the assembler could see to be above 4:
             extern u8 gDuel_bTerrain[8];
     #3859 moved it to src/candidates/func_80024E58.c for that profile.

   That last one is why the forms are not interchangeable. Measured: relaxing
   func_80024E58's [8] to an incomplete [] cost 4 bytes of text, while the
   same relaxation in func_8001798C.c is exact. The 8 is a threshold, not a
   length; no spelling here claims the object has more than one byte.

   c_symbols.ld also defines gDuel_bTerrainCodegenAlias at the same 0x8009B364
   so Duel_GetTerrainBoost can materialize the one byte's address twice in
   one function, which retail does and a single name cannot reproduce. */
#ifdef DUEL_TERRAIN_SCALAR_IN_DATA
extern u8 gDuel_bTerrain __attribute__((section(".data")));
#endif

/* Attack modifier in CARD_STAT_SCALE units, one row per monster card type and
   one column per terrain. Duel_GetTerrainBoost rejects cardType >=
   CARD_TYPE_MAGIC before indexing, so only the monster types have rows.
   src/game/duel_terrain_boost.c owns the table at 0x800909D4. */
extern s8 gDuel_aTerrainBoost[CARD_TYPE_MAGIC][DUEL_TERRAIN_COUNT];

/* Returns the attack/defence adjustment the active terrain gives a card of
   `cardType`, already multiplied by CARD_STAT_SCALE, or 0 when there is no
   terrain or the card is not a monster. */
s32 Duel_GetTerrainBoost(s32 cardType);

#endif
