#include "../../types.h"
#include "../../ygo_types.h"
#include "module_state.h"

/* Zero-initialized screen state at 0x8016D400-0x8016D590, defined in address
 * order. Japan and Europe place the same names at other addresses and in
 * another order, so these are separate variables, not fields of one record.
 * The three PADDING_VARIABLE arrays cover bytes no consumer has been seen
 * to name. */
u8 gNameEntry_bStateFlags = 0;
s8 gNameEntry_bKeyboardColumn = 0;
u8 gNameEntry_bKeyboardRow = 0;
u8 gNameEntry_bGlyphVramSlot = 0;
struct DisplayObject *gNameEntry_pSelectionFrame = 0;
u8 gNameEntry_bGlyphSequence = 0;
static PADDING_VARIABLE(4) = {0};
u8 gPassword_abDigits[8] = {0};
u8 *gNameEntry_pNameBuffer = 0;
u8 D_8016D41C = 0;
struct DisplayObject *gPassword_pDigitCursorWidget = 0;
u16 gPassword_wShopState = 0;
u8 gNameEntry_bSavedRow = 0;
s32 gPassword_nDigitIndex = 0;
s8 gNameEntry_bCaretPos = 0;
struct DuelEffectResourceRecord *gPassword_pCardCache = 0;
s16 gNameEntry_wCursorTargetX = 0;
s16 gNameEntry_wCursorTargetY = 0;
u32 gPassword_dwPriceRemaining = 0;
struct DisplayObject *gNameEntry_pCaret = 0;
struct DisplayObject *gPassword_apDigitCursorDecorations[4] = {0};
static PADDING_VARIABLE(0x80) = {0};
u8 gNameEntry_bCharacterSet = 0;
u16 gNameEntry_wPendingDialogId = 0;
u16 gNameEntry_wSelectionFrameFlags = 0;
struct DisplayObject *gPassword_pCardPreview = 0;
u16 gPassword_wCardId = 0;
static PADDING_VARIABLE(0xB0) = {0};
