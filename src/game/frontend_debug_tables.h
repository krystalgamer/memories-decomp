#ifndef MEMORIES_DECOMP_FRONTEND_DEBUG_TABLES_H
#define MEMORIES_DECOMP_FRONTEND_DEBUG_TABLES_H

#include "../types.h"

/* The six frontend debug HUD format strings and the scene index table,
   declared here so the source that defines them and the four that read
   them cannot drift apart. D_80090CDC and D_80090CF4 are declared too,
   although their only reader reaches them from inline asm rather than by
   name, so that the header describes the whole range. */
extern u8 D_80090CB4[0x28];
extern u8 D_80090CDC[0x18];
extern u8 D_80090CF4[0x18];
extern u8 D_80090D0C[0x1C];
extern u8 D_80090D28[0x1C];
extern u8 D_80090D44[0x24];
extern u8 D_80090D68[0x14];

#endif
