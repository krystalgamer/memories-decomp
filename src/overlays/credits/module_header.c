#include "../../types.h"
#include "module_header.h"

/* The credits package word: PAL builds it into the model_intro images. */
#ifdef VERSION_EUROPE
u32 D_80180000 = 0x34;
#else
u32 D_80180000 = 0x10;
#endif
