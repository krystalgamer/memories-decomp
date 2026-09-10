#ifndef MEMORIES_DECOMP_FRONTEND_DEBUG_TABLES_H
#define MEMORIES_DECOMP_FRONTEND_DEBUG_TABLES_H

#include "../types.h"

/* The six frontend debug HUD format strings and the scene index table,
   declared here so the source that defines them and the four that read
   them cannot drift apart. D_80090CDC and D_80090CF4 are declared too,
   although their only reader, func_80030998, is generated assembly rather
   than C, so that the header describes the whole range. */
extern u8 D_80090CB4[0x28];
extern u8 D_80090CDC[0x18];
extern u8 D_80090CF4[0x18];
extern u8 D_80090D0C[0x1C];
extern u8 D_80090D28[0x1C];
extern u8 D_80090D44[0x24];
extern u8 D_80090D68[0x14];

/* func_80030FA0 reads it as the index into D_80090D68 (`s32 i =
   D_8009B2F1;` at func_80030FA0.c:11, `v = p[i];` at :15); func_800300C8
   reads it as `s32 index = D_8009B2F1;` (src/candidates/func_800300C8.c:20).
   func_80031084
   is the only C writer: it adds 0xA when the sum is below 0x14
   (func_80031084.c:44-46), subtracts 0xA when the difference is not negative
   (:48-50), steps it by -1 with wraps to 0x13 and 9 (:54-65) and by +1 with
   wraps to 0xA and 0 (:70-81), stores 0x13 (:89-90) and stores
   `D_8009B2F1 + 1` into D_8009B2EB (:112). s8 because all three units
   declared it s8 when they matched and :48 tests `D_8009B2F1 - 0xA >= 0`;
   retail loads it lb at func_80030FA0.s:5 and func_80031084.s:59/:68/:82/
   :109/:141, and every access is %gp_rel, so this is the plain declaration.
   No prototype takes &D_8009B2F1. Initial value not read. */
extern s8 D_8009B2F1;

#endif
