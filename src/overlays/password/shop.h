#ifndef MEMORIES_DECOMP_PASSWORD_SHOP_H
#define MEMORIES_DECOMP_PASSWORD_SHOP_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "../../game/duel_effect.h"
#include "../../game/save_data.h"
#include "module_state.h"

/* The digit cursor's record, as the cursor helpers, shop updater and
 * initializer describe it between them. Their former private views never
 * disagreed about a byte -- they named different ones -- so this is their
 * union rather than a new claim, and the padding keeps every named field at
 * the offset its own function already used. */
/* Shop/password-entry state. Every password-shop function lives in shop.c
 * and uses these shared declarations, Password_UpdateShopScreen (0x8016A37C)
 * included: it is matched there in C, and its former candidate is retired.
 *
 *   gPassword_abDigits     The eight entered digits.
 *   gPassword_nDigitIndex  Which of them the cursor is on.
 *   D_8016D424             The starchip count the shop screen displays.
 *
 * gPassword_pDigitCursorWidget IS here, above, as PasswordCursorView *.
 * This paragraph used to say it was not, and to give three conflicting
 * spellings as the reason; the declaration was added later and the
 * paragraph was left behind, so the header asserted the opposite of what it
 * did. `shop.c` owns the cursor helpers, the initializer and the updater,
 * so every password-shop user consumes the shared view. */

/* The password shop's view of the 0x801A8000 staging buffer: while this
 * screen runs it holds two words per card, the first being the card's price
 * in starchips. card_list_rows.h explains why each subsystem keeps its own
 * declaration of this address. */
extern u32 D_801A8000[];

/* D_801A8008 is the password table Password_LookupCardID searches, and
 * D_801B1245 is the eight-digit string Password_RefreshDigitDisplay rebuilds. */
extern s32 D_801A8008[];
extern u8 D_801B1245[];

/* Builds text-box record 0 and returns it; both call sites ignore the
   record. */
DuelEffectChannel *Password_CreateMessageBox(s32 messageId, s32 flags);
void Password_InitShopScreen(void);
void Password_UpdateShopScreen(void);
void Password_UpdateDigitCursor(u8 *object);
void Password_UpdateDigitCursorDecoration(u8 *object);
/* Retain caller setup; the selected card comes from cached slot 0. */
void Password_RecreateCardPreview(s32 ignored);

/* Rebuilds the eight-digit entry box and the starchip counter from the
   current values. Both are called on entry and again after every change. */
void Password_RefreshDigitDisplay(void);
void Password_RefreshStarchipDisplay(void);

/* Points the digit cursor at `object` and starts its slide towards it. */
void Password_SetDigitCursorTarget(u8 *object);

/* Resolves the eight entered digits to a card id, or 0 when they do not
   match any password. */
s32 Password_LookupCardID(void);

#endif
