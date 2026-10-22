#ifndef MEMORIES_DECOMP_DUEL_RESULT_LOCALIZED_SPRITES_H
#define MEMORIES_DECOMP_DUEL_RESULT_LOCALIZED_SPRITES_H

#include "../types.h"
#include "duel_result_outro.h"

/* Localized duel-result outro sprite tables, one opponent/no-opponent pair
 * per language. DuelScene_UpdateResultOutro selects the pair by the language
 * index D_8009C02B, whose boot/reset default each single-language build sets
 * through BUILD_LANGUAGE_INDEX (1 French, 2 German, 3 Italian, 4 Spanish);
 * the debug language editor can store any value 1-4 in any of these builds,
 * so a table is chosen by the selector value rather than by the build. The
 * table within the pair is chosen by gDuel_bOpponentID >= 0, as the North
 * American path
 * chooses tent_DuelResultSpriteSpecsOpponent/NoOpponent. Each is indexed by
 * the winning side, then by up to ten specs. All four localized images bind
 * all eight tables at the same addresses. */
extern DuelResultSpriteSpec gDuelResult_aFrenchSpriteSpecsOpponent[2][10];
extern DuelResultSpriteSpec gDuelResult_aFrenchSpriteSpecsNoOpponent[2][10];
extern DuelResultSpriteSpec gDuelResult_aGermanSpriteSpecsOpponent[2][10];
extern DuelResultSpriteSpec gDuelResult_aGermanSpriteSpecsNoOpponent[2][10];
extern DuelResultSpriteSpec gDuelResult_aItalianSpriteSpecsOpponent[2][10];
extern DuelResultSpriteSpec gDuelResult_aItalianSpriteSpecsNoOpponent[2][10];
extern DuelResultSpriteSpec gDuelResult_aSpanishSpriteSpecsOpponent[2][10];
extern DuelResultSpriteSpec gDuelResult_aSpanishSpriteSpecsNoOpponent[2][10];
extern s32 gDuelResult_nOutroSpriteCount;

#endif
