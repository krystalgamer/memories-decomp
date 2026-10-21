#include "../types.h"
#include "model_handler_state.h"

ModelDebugDisplayState gModelDebug_Display __attribute__((section(".sdata"))) = {0};
char gModelDebug_szNormalColor[8] __attribute__((section(".sdata"))) = "~cff0";
char gModelDebug_szSelectedColor[8] __attribute__((section(".sdata"))) = "~cf00";
char gModelDebug_szDigitFormat[8] __attribute__((section(".sdata"))) = "%s%d";
char gModelDebug_szVersusSeparator[8] __attribute__((section(".sdata"))) = " VS ";
char gModelDebug_szLineBreak[4] __attribute__((section(".sdata"))) = "\n";
char gModel_szBackTrace[8] __attribute__((section(".sdata"))) = "BACK\n";
char gModel_szStoneTrace[8] __attribute__((section(".sdata"))) = "STONE\n";
char gModel_szHitTrace[8] __attribute__((section(".sdata"))) = "HIT\n";
char gModel_szOuchTrace[8] __attribute__((section(".sdata"))) = "OUCH\n";
char gModel_szTermTrace[8] __attribute__((section(".sdata"))) = "TERM\n";
