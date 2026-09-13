#ifndef MEMORIES_DECOMP_DISPLAY_ASSET_BANKS_H
#define MEMORIES_DECOMP_DISPLAY_ASSET_BANKS_H

#include "../types.h"

/* Reused display-resource bank. Transfer callbacks fill it, and resident
 * result/menu code and overlays pass it to func_800428A8 as the resource
 * argument. It is a byte-addressed bank, not a DisplayObject allocation;
 * the callers do not establish a common full extent. */
extern u8 D_801AF000[];

#endif
