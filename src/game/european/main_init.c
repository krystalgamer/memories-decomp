#include "../../types.h"

#define VERSION_EUROPE

#define ResetCallback func_800746D4
#define GsInitVcount func_80085244
#define StopCallback func_800747C8
#define SetMem func_80073B94
#define SetDispMask func_8007F9C8
#define SetVideoMode func_800751A4
#define VSyncCallback func_80074764
#define setjmp func_80090B54

#define Main_VBlankCB func_80012BD4
#define Main_RunBootSequence func_80043C3C
#define Main_RunFrontendLoop func_80043E80

#include "../main_init.c"
