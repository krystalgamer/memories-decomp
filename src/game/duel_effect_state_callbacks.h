#ifndef MEMORIES_DECOMP_DUEL_EFFECT_STATE_CALLBACKS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_STATE_CALLBACKS_H

#include "../types.h"
#include "duel_effect.h"

/* Fifteen entries of the D_80090E64 text-box state table, plus the choice
 * object constructor used by the first interaction state. Each callback takes
 * the state record, tests or sets its 0x51 byte, and advances from there.
 *
 * They are reached only through the table, which indexes on the low five bits
 * of that same 0x51 byte -- so the byte is both the dispatch key and the state
 * each of these updates. */
void Text_BeginPageClear(DuelEffectChannel *object);
void Text_WaitForPageAdvance(DuelEffectChannel *object);
/* Shared signed-byte Square-hold countdown; not per-channel storage. */
extern s8 gText_PageAdvanceHoldCountdown;
/* State 5 stages command 5; optional text operands are Y8 then X16. */
void Text_StageScriptImage(DuelEffectChannel *object);
/* State 11 stages command 7, with the original signed Y-threshold wait. */
void Text_StartViewportTween(DuelEffectChannel *object);
#ifdef VERSION_EUROPE
extern void TextBox_WaitForScriptCompletion(DuelEffectChannel *object)
    asm("func_80037544");
#else
void TextBox_WaitForScriptCompletion(DuelEffectChannel *object);
#endif
/* State 6: wait for setup, then finish or run the ordinary continuation once. */
void Text_ContinueDisplayEffect(DuelEffectChannel *object);
/* State 7 releases on step zero; state 8 only observes completion. */
void Text_WaitForDisplayEffectAndRelease(DuelEffectChannel *object);
void Text_WaitForDisplayEffect(DuelEffectChannel *object);
/* States 14/9 gate on the existing low activity bits, then hand off to 8. */
void Text_RequestDisplayEffectFadeOut(DuelEffectChannel *object);
void Text_RequestDisplayEffectCrossfade(DuelEffectChannel *object);
void DuelEffect_WaitForFileTransfer(DuelEffectChannel *object);
void Text_WaitForBgmFade(DuelEffectChannel *object);
/* State 15: initialization consumes a word and decrements in the same call. */
void Text_WaitForDelay(DuelEffectChannel *object);
/* State 16: shares that counter; optional jitter precedes completion restore. */
void Text_WaitWithViewportShake(DuelEffectChannel *object);
/* Shared delay counter and shake origin, not per-channel state. */
extern s16 gText_WaitCounter;
extern u16 gText_ShakeViewportOrigin[2];
void Text_WaitForCdSoundTransfer(DuelEffectChannel *object);

/* Shared CD-sound wait phase, not the independent script-dispatch gate. */
extern u8 gText_CdSoundWaitPhase;
/* Shared gate stores 5/7, but dispatch indexes the current command word,
 * not this byte. Redispatch clears it only for whole-word completion;
 * new-box initialization also clears it. */
extern u8 gText_ScriptDispatchGate;

#endif
