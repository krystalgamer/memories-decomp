#ifndef MEMORIES_DECOMP_HANDWRITTEN_ASM_H
#define MEMORIES_DECOMP_HANDWRITTEN_ASM_H

/*
 * Inline-assembly building blocks for the model primitive and wireframe
 * renderers that were originally written by hand.
 *
 * Those routines have no stack frame. They borrow callee-saved registers and
 * preserve the caller's values in scratch words of the argument block
 * ($a0 + offset), so the save/restore is expressed with the HW_SAVE_REGS* /
 * HW_RESTORE_REGS* macros below. A source using them must also declare each
 * borrowed register as a file-scope register variable, which
 * stops GCC from emitting its own stack-frame save of that register. Other
 * fixed registers ($at, $v0, temporaries) are plain register declarations.
 *
 * GTE commands come from gte_native.h, mostly in the unpadded _B form
 * because the originals interleave independent work into the cycles Psy-Q
 * fills with nops. Plain register transfers need no such change, so the
 * renderers take the vertex loads gte_ldv0/1/2, the screen-XY writes
 * gte_ldsxy0/1/2 and the stores gte_stsxy2, gte_strgb and gte_strgb3 from
 * src/psyq/inline_c.h directly; their expansions pass the same contract.
 *
 * Inline assembly is limited to that save/restore, to GTE operations and to
 * three shapes GCC 2.8.1 cannot emit: a lone lwr merged into a C-loaded word
 * (HW_LWR) and a branch whose delay slot holds a GTE write or a load
 * (HW_GTE_WRITE_BEQ, HW_BEQZ_LOAD). Those branches jump to C labels that GCC
 * does not see as control-flow edges, so only the exact image match vouches
 * for the code around them. Every expansion is checked, with its exact
 * operand signature, by tools/project/handwritten_asm_contract.py, and only
 * profiles that set "handwritten_asm_macros" may use this header.
 */

/* Register numbers: s0..s7 are $16..$23. */
#define HW_SAVE_REGS3(obj, r0, o0, r1, o1, r2, o2)                         \
    __asm__ volatile("sw $" #r0 ", " #o0 "(%0);"                           \
                     "sw $" #r1 ", " #o1 "(%0);"                           \
                     "sw $" #r2 ", " #o2 "(%0)"                            \
                     : : "r"(obj) : "memory")
#define HW_RESTORE_REGS3(obj, r0, o0, r1, o1, r2, o2)                      \
    __asm__ volatile("lw $" #r0 ", " #o0 "(%0);"                           \
                     "lw $" #r1 ", " #o1 "(%0);"                           \
                     "lw $" #r2 ", " #o2 "(%0)"                            \
                     : : "r"(obj) : "memory")
#define HW_SAVE_REGS2(obj, r0, o0, r1, o1)                                 \
    __asm__ volatile("sw $" #r0 ", " #o0 "(%0);"                           \
                     "sw $" #r1 ", " #o1 "(%0)"                            \
                     : : "r"(obj) : "memory")
#define HW_RESTORE_REGS2(obj, r0, o0, r1, o1)                              \
    __asm__ volatile("lw $" #r0 ", " #o0 "(%0);"                           \
                     "lw $" #r1 ", " #o1 "(%0)"                            \
                     : : "r"(obj) : "memory")

/*
 * Keep LABEL in the insn stream without emitting code or data. The address
 * goes into a static that the linker discards, so jump optimisation cannot
 * delete the label. A label directly before the final return stops delay-slot
 * filling, leaving the original "jr ra; nop" epilogue.
 */
#define HW_KEEP_LABEL(label)                                               \
    static void *const label##_keep __attribute__((section(".discard"))) =  \
        &&label

/* Merge the unaligned halfword below an already loaded word (lw + lwr). */
#define HW_LWR(val, offset, p)                                             \
    __asm__ volatile("lwr %0, " #offset "(%1)" : "=r"(val) : "r"(p), "0"(val))

/* GTE registers: VXY/VZ 0-5, SXY0-2 12-14, OTZ 7, MAC0 24, flag $31. */
#define HW_GTE_READ_FLAG(out) __asm__ volatile("cfc2 %0, $31" : "=r"(out))
#define HW_GTE_WRITE(reg, value)                                           \
    __asm__ volatile("mtc2 %0, $" #reg : : "r"(value))
#define HW_GTE_STORE(reg, offset, p)                                       \
    __asm__ volatile("swc2 $" #reg ", " #offset "(%0)" : : "r"(p) : "memory")
#define HW_GTE_READ(reg, out)                                              \
    __asm__ volatile("mfc2 %0, $" #reg : "=r"(out))

/* Flag register read followed by its load-delay nop. */
#define HW_GTE_READ_FLAG_WAIT(out)                                         \
    __asm__ volatile("cfc2 %0, $31;" "nop" : "=r"(out))
/* Data register read followed by its load-delay nop. */
#define HW_GTE_READ_WAIT(reg, out)                                         \
    __asm__ volatile("mfc2 %0, $" #reg ";" "nop" : "=r"(out))

/*
 * if (value == cmp) goto label, with mtc2 value -> reg executed in the
 * branch delay slot either way. GCC cannot fill a slot across inline asm,
 * so the original branch-and-write pair has to be spelled out together.
 */
#define HW_GTE_WRITE_BEQ(reg, value, cmp, label)                           \
    __asm__ volatile(".set\tnoreorder\n"                                   \
                     "beq %0, %1, %2\n"                                     \
                     "mtc2 %0, $" #reg "\n"                                 \
                     ".set\treorder"                                        \
                     : : "r"(value), "r"(cmp), "i"(&&label))
/*
 * if (test == 0) goto label, with out = *(u32 *)(base + offset) executed in
 * the branch delay slot either way (GCC never fills a slot with a load).
 */
#define HW_BEQZ_LOAD(test, label, out, offset, base)                       \
    __asm__ volatile(".set\tnoreorder\n"                                   \
                     "beqz %1, %2\n"                                        \
                     "lw %0, " #offset "(%3)\n"                              \
                     ".set\treorder"                                        \
                     : "=r"(out)                                            \
                     : "r"(test), "i"(&&label), "r"(base))

/* Write SXY0, SXY1, SXY2 into a polygon packet. */
#define HW_GTE_STORE_SXY3(p, o0, o1, o2)                                   \
    __asm__ volatile("swc2 $12, " #o0 "(%0);"                              \
                     "swc2 $13, " #o1 "(%0);"                              \
                     "swc2 $14, " #o2 "(%0)"                               \
                     : : "r"(p) : "memory")

/* Write SXY0, SXY1, SXY2, SXY0 as a closed four-point line. */
#define HW_GTE_STORE_SXY_LOOP(p, o0, o1, o2, o3)                           \
    __asm__ volatile("swc2 $12, " #o0 "(%0);"                              \
                     "swc2 $13, " #o1 "(%0);"                              \
                     "swc2 $14, " #o2 "(%0);"                              \
                     "swc2 $12, " #o3 "(%0)"                               \
                     : : "r"(p) : "memory")

#endif
