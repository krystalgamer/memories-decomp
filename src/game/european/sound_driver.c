#include "../../types.h"
#define SOUND_OUTPUT_TRANSITION_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C3B8-0x8009C3C8 (#2602); its
 * layout differs from the North American block the shared source defines. */
struct SDSecondaryState *gSD_pSecondaryState;
struct FileRequestSlot *gSD_pFileRequest;
struct SDValue *g_SDValue;
static PADDING_VARIABLE(4);

/* The European builds compile the shared unit through this wrapper so that
 * they can give it their own zero-initialized storage (#2602). */
#define VERSION_EUROPE
#include "../sound_driver.c"
