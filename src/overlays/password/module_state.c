#include "../../types.h"
#include "../../ygo_types.h"
#include "module_state.h"

/* Zero-initialized screen state at 0x8016D400-0x8016D590, defined in address
 * order. Japan and Europe place the same names at other addresses and in
 * another order, so these are separate variables, not fields of one record.
 * The three D_ arrays cover bytes no consumer has been seen to name. */
#define PASSWORD_STATE __attribute__((section(".data")))

u8 gNameEntry_bStateFlags PASSWORD_STATE = 0;
s8 gNameEntry_bKeyboardColumn PASSWORD_STATE = 0;
u8 gNameEntry_bKeyboardRow PASSWORD_STATE = 0;
u8 gNameEntry_bGlyphVramSlot PASSWORD_STATE = 0;
SelectionFrame *gNameEntry_pSelectionFrame PASSWORD_STATE = 0;
u8 gNameEntry_bGlyphSequence PASSWORD_STATE = 0;
u8 D_8016D40C[4] PASSWORD_STATE = {0};
u8 gPassword_abDigits[8] PASSWORD_STATE = {0};
u8 *gNameEntry_pNameBuffer PASSWORD_STATE = 0;
u8 D_8016D41C PASSWORD_STATE = 0;
PasswordCursorView *gPassword_pDigitCursorWidget PASSWORD_STATE = 0;
u16 gPassword_wShopState PASSWORD_STATE = 0;
u8 gNameEntry_bSavedRow PASSWORD_STATE = 0;
s32 gPassword_nDigitIndex PASSWORD_STATE = 0;
s8 gNameEntry_bCaretPos PASSWORD_STATE = 0;
struct DuelEffectResourceRecord *gPassword_pCardCache PASSWORD_STATE = 0;
s16 gNameEntry_wCursorTargetX PASSWORD_STATE = 0;
s16 gNameEntry_wCursorTargetY PASSWORD_STATE = 0;
u32 gPassword_dwPriceRemaining PASSWORD_STATE = 0;
u8 *gNameEntry_pCaret PASSWORD_STATE = 0;
struct DisplayObject *gPassword_apDigitCursorDecorations[4] PASSWORD_STATE = {0};
u8 D_8016D450[0x80] PASSWORD_STATE = {0};
u8 gNameEntry_bCharacterSet PASSWORD_STATE = 0;
u16 gNameEntry_wPendingDialogId PASSWORD_STATE = 0;
u16 gNameEntry_wSelectionFrameFlags PASSWORD_STATE = 0;
PasswordCardPreviewView *gPassword_pCardPreview PASSWORD_STATE = 0;
u16 gPassword_wCardId PASSWORD_STATE = 0;
u8 D_8016D4E0[0xB0] PASSWORD_STATE = {0};
