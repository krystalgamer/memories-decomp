#ifndef MEMORIES_DECOMP_FUNC_800610E0_H
#define MEMORIES_DECOMP_FUNC_800610E0_H

#include "../types.h"

/* Opens the one-line text box in channel 3 (string 10) with a highlight
 * object under column `mode`, 160 pixels apart, and stores `mode` in
 * D_8009B34E. MainMenu_UpdateTradeScreen (now a build-integrated candidate,
 * src/candidates/main_menu/func_801821DC.c) calls it with 0, with 1, and
 * with the other player's index. */
void func_800610E0(int mode);

#endif
