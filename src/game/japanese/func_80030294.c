#include "../../types.h"

/* SLPM-86398 build of src/game/func_80030294.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * as is. */
#define D_8009AF4C gJapanese_D_8009AF4C
#define D_8009AF54 gJapanese_D_8009AF54
#define D_8009B2B4 gJapanese_DebugInterfaceFieldB4
#define D_8009B2B8 gJapanese_DebugInterfaceFieldB8
#define D_8009B2C0 gJapanese_DebugInterfaceFieldC0
#define D_8009B2E0 gJapanese_DebugInterfaceFieldE0
#define D_8009B2EA gJapanese_DebugInterfaceFlags
#define D_8009B2EC gJapanese_DebugInterfaceData

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DEBUG_SCREEN_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define DEBUG_SCREEN_CANCEL_BUTTON PAD_BUTTON_CROSS

#include "../func_80030294.c"
