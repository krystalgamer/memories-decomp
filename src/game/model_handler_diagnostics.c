#include "../types.h"
#include "model_handler_state.h"

ModelDebugDisplayState gModelDebug_Display __attribute__((section(".sdata"))) = {0};
char gModelDebug_szNormalColor[8] __attribute__((section(".sdata"))) = "~cff0";
char gModelDebug_szSelectedColor[8] __attribute__((section(".sdata"))) = "~cf00";
char gModelDebug_szDigitFormat[8] __attribute__((section(".sdata"))) = "%s%d";
char gModelDebug_szVersusSeparator[8] __attribute__((section(".sdata"))) = " VS ";
char gModelDebug_szLineBreak[4] __attribute__((section(".sdata"))) = "\n";
char D_8009B030[8] __attribute__((section(".sdata"))) = "BACK\n";
char D_8009B038[8] __attribute__((section(".sdata"))) = "STONE\n";
char D_8009B040[8] __attribute__((section(".sdata"))) = "HIT\n";
char D_8009B048[8] __attribute__((section(".sdata"))) = "OUCH\n";
char D_8009B050[8] __attribute__((section(".sdata"))) = "TERM\n";
