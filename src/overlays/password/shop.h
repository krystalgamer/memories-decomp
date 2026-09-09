#ifndef MEMORIES_DECOMP_PASSWORD_SHOP_H
#define MEMORIES_DECOMP_PASSWORD_SHOP_H

#include "../../types.h"
#include "../../game/duel_effect.h"

/* Known prefix of the preview's control object, not its full allocation. */
typedef struct {
    u8 pad0[0x8];
    u16 flags;
    u8 pad0A[0x17];
    u8 phase;
    u8 pad22[0x10];
    u16 y;
} PasswordCardPreviewView;

typedef void (*PasswordCursorUpdate)(u8 *object);

extern PasswordCardPreviewView *D_8016D4D8;

/* Shop/password-entry state. All five sources that use these already include
 * this header, so the local copies they carried existed only because the
 * declarations were missing here.
 *
 *   gPassword_abDigits     The eight entered digits.
 *   gPassword_nDigitIndex  Which of them the cursor is on.
 *   D_8016D424             The starchip count the shop screen displays.
 *
 * gPassword_pDigitCursorWidget is NOT here. Its three declarers spell it
 * three different ways -- u8 * in shop_setup.c, Cursor * in shop_update.c and
 * PasswordNode * in digit_cursor.c -- and the latter two are structs defined
 * locally in those files rather than shared anywhere. Picking one spelling
 * here would spread whichever is wrong, and reconciling the two local structs
 * is #2501 work that wants its own change. */
extern u8 gPassword_abDigits[];
extern s32 gPassword_nDigitIndex;
extern u16 D_8016D424;

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
