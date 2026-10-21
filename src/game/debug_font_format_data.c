#include "../types.h"
#include "debug_font_format_data.h"

char gDebugMenu_szLineBreak[4] __attribute__((section(".sdata"))) = "\n";
char gDebugMenu_szStringLineFormat[4] __attribute__((section(".sdata"))) = "%s\n";
