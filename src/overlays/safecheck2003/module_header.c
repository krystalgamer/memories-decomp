#include "../../types.h"
#include "module_header.h"

/* The game's WA_MRG package word ahead of Sony's safechk object. */
#ifdef VERSION_EUROPE
u32 D_80168000 = 0x09;
#else
u32 D_80168000 = 0x16;
#endif
