#ifndef MEMORIES_DECOMP_PASSWORD_SHOP_H
#define MEMORIES_DECOMP_PASSWORD_SHOP_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "../../game/duel_effect_states.h"
#include "../../game/mem_card.h"
#include "module_state.h"
#include "../../game/file_staging_buffer.h"

/* Shop/password-entry state. Every password-shop function lives in shop.c
 * and uses these shared declarations, Password_UpdateShopScreen (0x8016A37C)
 * included: it is matched there in C, and its former candidate is retired.
 *
 *   gPassword_abDigits     The eight entered digits.
 *   gPassword_nDigitIndex  Which of them the cursor is on.
 *   gPassword_wShopState   The shop step (state & 0x1F, steps 0-4) with 0x8000/0x4000
 *                          per-step progress flags; the starchip deduction
 *                          counts down in gPassword_dwPriceRemaining.
 *
 * gPassword_pDigitCursorWidget is declared in module_state.h as a
 * DisplayObject *. `shop.c` owns the cursor helpers, the initializer and the
 * updater, and reaches the cursor through the record's own members: its
 * spin in field_20.b.rotation_z, its slide target in field_18.target, the
 * timer in field_60 and the slide flag in update_state. */

/* The shop's price table is the shop_cards view of gFile_StagingBuffer
 * (file_staging_buffer.h). */

/* gPassword_szDigits is the eight-digit string Password_RefreshDigitDisplay rebuilds. */
extern u8 gPassword_szDigits[];

/* Builds text-box record 0 and returns it; both call sites ignore the
   record. */
DuelEffectChannel *Password_CreateMessageBox(s32 messageId, s32 flags);
void Password_InitShopScreen(void);
void Password_UpdateShopScreen(void);
void Password_UpdateDigitCursor(struct DisplayObject *object);
void Password_UpdateDigitCursorDecoration(struct DisplayObject *object);
/* Retain caller setup; the selected card comes from cached slot 0. */
void Password_RecreateCardPreview(s32 ignored);

/* Rebuilds the eight-digit entry box and the starchip counter from the
   current values. Both are called on entry and again after every change. */
void Password_RefreshDigitDisplay(void);
void Password_RefreshStarchipDisplay(void);

/* Points the digit cursor at `object` and starts its slide towards it. */
void Password_SetDigitCursorTarget(struct DisplayObject *object);

/* Resolves the eight entered digits to a card id, or 0 when they do not
   match any password. */
s32 Password_LookupCardID(void);

#endif
