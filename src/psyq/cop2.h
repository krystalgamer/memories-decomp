#ifndef MEMORIES_DECOMP_PSYQ_COP2_H
#define MEMORIES_DECOMP_PSYQ_COP2_H

/*
 * Compiler-allocated scalar COP2 transfers used by software-pipelined GTE
 * code. Keep these templates exact: matching-source validation permits only
 * these forms, not arbitrary statement-level assembly.
 */
#define cop2_load_v0_words(vxy, vz) __asm__ volatile ( \
    "mtc2 %0, $0;" \
    "mtc2 %1, $1" \
    : \
    : "r"(vxy), "r"(vz))

#define cop2_load_rgb_word(rgb) __asm__ volatile ( \
    "mtc2 %0, $6" \
    : \
    : "r"(rgb))

#define cop2_read_flag(flag) __asm__ volatile ( \
    "cfc2 %0, $31;" \
    "nop" \
    : "=r"(flag))

#define cop2_read_ir0(ir0) __asm__ volatile ( \
    "mfc2 %0, $8" \
    : "=r"(ir0))

#define cop2_read_sz3(sz3) __asm__ volatile ( \
    "mfc2 %0, $19" \
    : "=r"(sz3))

#endif
