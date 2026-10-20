#include "../../types.h"

/* SLES-03947 build of src/game/duel_effect_state_callbacks.c. */
#define VERSION_EUROPE
#define func_800379C4 func_8003775C
#define func_8003787C func_80037614
#define D_8009B0F4_abs D_8009B0F4
#define D_8009B134_abs D_8009B134
#define func_800378D8 func_80037670
#define func_80037914 func_800376AC
#define func_80037950 func_800376E8
#define func_8003798C func_80037724
#define TextBox_WaitForScriptCompletion func_80037544
#define func_800377C8 func_80037560
#define func_8003767C func_80037414
#define D_8009B112_abs D_8009B112
/* func_80037B40 sits at 0x800378D8 here, but func_800378D8 is itself renamed
 * above, and a #define to it would be rescanned into func_80037670. An
 * intermediate name with an asm label gives it the European symbol instead;
 * the declaration is unprototyped so the source's prototype completes it. */
#define func_80037B40 European_func_800378D8
void European_func_800378D8() asm("func_800378D8");
#define func_8003771C func_800374B4
#include "../duel_effect_state_callbacks.c"
