#ifndef MEMORIES_DECOMP_GTE_NATIVE_H
#define MEMORIES_DECOMP_GTE_NATIVE_H

/*
 * GTE commands as native COP2 command words.
 *
 * The Psy-Q inline_c.h command macros emit DMPSX placeholder words such as
 * .word 0x000000bf, which Sony's DMPSX post-pass rewrote into the real
 * commands. This project does not emulate that pass and emits the native
 * COP2 command words directly instead. GTE_xxx() keeps the two leading nops
 * of the Psy-Q gte_xxx() form; GTE_xxx_B() is unpadded, like gte_xxx_b(),
 * for code that fills those cycles with independent work.
 */
#define GTE_RTPS() __asm__ volatile("nop;" "nop;" ".word 0x4A180001")
#define GTE_RTPS_B() __asm__ volatile(".word 0x4A180001")
#define GTE_RTPT_B() __asm__ volatile(".word 0x4A280030")
#define GTE_NCCS() __asm__ volatile("nop;" "nop;" ".word 0x4B08041B")
#define GTE_NCCS_B() __asm__ volatile(".word 0x4B08041B")
#define GTE_NCCT() __asm__ volatile("nop;" "nop;" ".word 0x4B18043F")
#define GTE_NCLIP() __asm__ volatile("nop;" "nop;" ".word 0x4B400006")
#define GTE_NCLIP_B() __asm__ volatile(".word 0x4B400006")
#define GTE_AVSZ3_B() __asm__ volatile(".word 0x4B58002D")
#define GTE_AVSZ4_B() __asm__ volatile(".word 0x4B68002E")

#endif
