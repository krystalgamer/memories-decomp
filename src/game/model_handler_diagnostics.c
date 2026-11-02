#include "../types.h"
#include "model_handler_state.h"

ModelDebugDisplayState gModelDebug_Display = {0};
char gModelDebug_szNormalColor[8] = "~cff0";
char gModelDebug_szSelectedColor[8] = "~cf00";
char gModelDebug_szDigitFormat[8] = "%s%d";
char gModelDebug_szVersusSeparator[8] = " VS ";
char gModelDebug_szLineBreak[4] = "\n";
char gModel_szBackTrace[8] = "BACK\n";
char gModel_szStoneTrace[8] = "STONE\n";
char gModel_szHitTrace[8] = "HIT\n";
char gModel_szOuchTrace[8] = "OUCH\n";
char gModel_szTermTrace[8] = "TERM\n";
