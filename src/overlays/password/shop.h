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

/* The digit cursor's record, as the cursor helpers, shop updater and
 * initializer describe it between them. Their former private views never
 * disagreed about a byte -- they named different ones -- so this is their
 * union rather than a new claim, and the padding keeps every named field at
 * the offset its own function already used. */
typedef struct {
    u8 pad00[0x8];
    u16 flags;        /* 0x08 */
    u8 pad0A[0x0E];
    /* Where x/y are heading. Password_SetDigitCursorTarget writes this pair
       and nothing else, Password_UpdateDigitCursor derives its velocity from
       target minus live, and on the last tick copies the pair straight into
       x/y as one word. */
    s16 target_x;     /* 0x18 */
    s16 target_y;     /* 0x1A */
    u8 pad1C[0x14];
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

/* Shop/password-entry state. Every password-shop function now lives in
 * shop.c and uses these shared declarations.
 *
 *   gPassword_abDigits     The eight entered digits.
 *   gPassword_nDigitIndex  Which of them the cursor is on.
 *   D_8016D424             The starchip count the shop screen displays.
 *
 * gPassword_pDigitCursorWidget IS here, above, as PasswordCursorView *.
 * This paragraph used to say it was not, and to give three conflicting
 * spellings as the reason; the declaration was added later and the
 * paragraph was left behind, so the header asserted the opposite of what it
 * did. `shop.c` now owns the cursor helpers, initializer and updater together,
 * so every password-shop user consumes the shared view. */
extern u8 gPassword_abDigits[];
extern s32 gPassword_nDigitIndex;
extern u16 D_8016D424;

/* 0x801D07E0, named gLibrary_dwStarchips in config/slus_01411/symbols.txt:62.
 * Password_RefreshStarchipDisplay copies it into the scalar D_801D5608 alias.
 * Password_UpdateShopScreen compares it with the selected card price and
 * reaches it again as `pool[504]` after `pool = D_801D0000`, a load and store
 * at +2016 that this declaration leaves alone. Nothing pins the sign: the
 * copy and subtraction are the same either way, and the compare is unsigned
 * because its other operand is u32. u32 here follows the other `dw` names in
 * the tree's headers. */
extern u32 gLibrary_dwStarchips;

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
