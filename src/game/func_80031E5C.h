#ifndef MEMORIES_DECOMP_FUNC_80031E5C_H
#define MEMORIES_DECOMP_FUNC_80031E5C_H

#include "../types.h"

/* Puts the build-deck screen's two counters -- the record's +0x5A9C and
 * +0x5AA0 words -- into the D_801D5608 staging pair, then opens the text box
 * that prints them and takes its display object out of screen space. Called
 * after every add or remove, which is what refreshes the count on screen. */
void func_80031E5C(u8 *record);

#endif
