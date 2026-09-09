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

/* The digit cursor's record, as the three sources that touch it describe it
 * between them. Previously each kept its own view: digit_cursor.c called it
 * PasswordNode and named the flags, position and kind; the shop updater
 * called it Cursor and named the timer and update flags; the shop
 * initializer only assigned the pointer and spelled it u8 *. The three never disagreed about a byte --
 * they named different ones -- so this is their union rather than a new
 * claim, and the padding keeps every named field at the offset its own
 * source already used. */
typedef struct {
    u8 pad00[0x8];
    u16 flags;        /* 0x08 */
    u8 pad0A[0x26];
    s16 x;            /* 0x30 */
    s16 y;            /* 0x32 */
    u8 pad34[0x2C];
    s16 timer;        /* 0x60 */
    u8 pad62[0x7];
    u8 kind;          /* 0x69 */
    u8 pad6A[0x2];
    u8 updateFlags;   /* 0x6C */
} PasswordCursorView;

extern PasswordCursorView *gPassword_pDigitCursorWidget;

extern PasswordCardPreviewView *D_8016D4D8;

/* Shop/password-entry state. All five sources that use these already include
 * this header, so the local copies they carried existed only because the
 * declarations were missing here.
 *
 *   gPassword_abDigits     The eight entered digits.
 *   gPassword_nDigitIndex  Which of them the cursor is on.
 *   D_8016D424             The starchip count the shop screen displays.
 *
 * gPassword_pDigitCursorWidget IS here, above, as PasswordCursorView *.
 * This paragraph used to say it was not, and to give three conflicting
 * spellings as the reason; the declaration was added later and the
 * paragraph was left behind, so the header asserted the opposite of what it
 * did. Two of those three spellings are gone: shop.c, which absorbed the
 * former shop_setup.c and shop_update.c, is one unit and uses the shared
 * view. digit_cursor.c is the last source with a local struct of its own. */
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
