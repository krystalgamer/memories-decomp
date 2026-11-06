#define FADE_BAND_COUNT 32
#define FADE_TRANSITION_STATE_SIZE 0x2A

#include "../../types.h"
#define FADE_RUNTIME_REGIONAL_STORAGE
/* This build's own zero-initialized storage at 0x800EAE28-0x800EAE58 (#2602); its
 * layout differs from the North American block the shared source defines. */
struct FadeTransitionState gFade_State;
#define FADE_RUNTIME_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C098-0x8009C0AC (#2602); its
 * layout differs from the North American block the shared source defines. */
u8 gFade_bOrderingTableMode;
u8 gGraphics_bClearBlue;
u8 gGraphics_bClearGreen;
u8 gGraphics_bClearRed;
u8 gFade_bTargetBlue;
u8 gFade_bTargetGreen;
u8 gFade_bTargetRed;
static PADDING_VARIABLE(1);
s16 gGraphics_sViewportX;
s16 gGraphics_sViewportY;
u8 gFade_bKeepOverlayDepth;
u8 gFade_bWhiteLatch;
static PADDING_VARIABLE(2);
u16 gDuel_wAiSelectionStepDelay;
s16 gDuel_wViewTweenDistanceTarget;

/* Europe draws 32 bands over 256 lines. */
#include "../fade_runtime.c"
