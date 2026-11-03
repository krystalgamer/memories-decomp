#ifndef MEMORIES_DECOMP_MAIN_MENU_MODULE_STATE_H
#define MEMORIES_DECOMP_MAIN_MENU_MODULE_STATE_H

#include "../../types.h"
#include "frontend.h"
#include "value_setup.h"
#include "trade_helpers.h"

/* Bytes in the zero-initialized state at 0x80184558-0x80185CD4 that no
 * consumer has been seen to name. */
extern u8 *D_80184564;
extern u8 D_8018459E;
extern u8 D_8018459F;
extern DisplayObject *D_801845A8;
extern DisplayObject *D_801845AC;
extern u8 D_801845BF;
extern u8 *D_801845E4;
extern u8 *D_801845E8;
extern u16 D_80185CD2;

#endif
