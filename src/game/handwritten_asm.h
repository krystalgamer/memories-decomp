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
 * GTE commands are emitted without the Psy-Q inline_c.h nop padding because
 * the originals interleave independent work into those cycles.
 *
 * Inline assembly is limited to that save/restore and to GTE operations;
 * every expansion is checked by tools/project/handwritten_asm_contract.py,
 * and only profiles that set "handwritten_asm_macros" may use this header.
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

/*
 * Keep LABEL in the insn stream without emitting code or data. The address
 * goes into a static that the linker discards, so jump optimisation cannot
 * delete the label. A label directly before the final return stops delay-slot
 * filling, leaving the original "jr ra; nop" epilogue.
 */
#define HW_KEEP_LABEL(label)                                               \
    static void *const label##_keep __attribute__((section(".discard"))) =  \
        &&label

/* GTE data registers: VXY/VZ pairs 0-5, SXY0-2 12-14, OTZ 7, flag $31. */
#define HW_GTE_LOAD_V0(p)                                                  \
    __asm__ volatile("lwc2 $0, 0(%0);" "lwc2 $1, 4(%0)" : : "r"(p))
#define HW_GTE_LOAD_V1(p)                                                  \
    __asm__ volatile("lwc2 $2, 0(%0);" "lwc2 $3, 4(%0)" : : "r"(p))
#define HW_GTE_LOAD_V2(p)                                                  \
    __asm__ volatile("lwc2 $4, 0(%0);" "lwc2 $5, 4(%0)" : : "r"(p))

#define HW_GTE_RTPT() __asm__ volatile(".word 0x4A280030")
#define HW_GTE_READ_FLAG(out) __asm__ volatile("cfc2 %0, $31" : "=r"(out))
#define HW_GTE_WRITE(reg, value)                                           \
    __asm__ volatile("mtc2 %0, $" #reg : : "r"(value))
#define HW_GTE_STORE(reg, offset, p)                                       \
    __asm__ volatile("swc2 $" #reg ", " #offset "(%0)" : : "r"(p) : "memory")

/* RTPT, then read the flag register with its load-delay nop. */
#define HW_GTE_RTPT_FLAG(out)                                              \
    __asm__ volatile(".word 0x4A280030;" "cfc2 %0, $31;" "nop" : "=r"(out))
/* RTPS, then read the flag register with its load-delay nop. */
#define HW_GTE_RTPS_FLAG(out)                                              \
    __asm__ volatile(".word 0x4A180001;" "cfc2 %0, $31;" "nop" : "=r"(out))
/* AVSZ4, then read OTZ. */
#define HW_GTE_AVSZ4_OTZ(out)                                              \
    __asm__ volatile(".word 0x4B68002E;" "mfc2 %0, $7" : "=r"(out))
/* AVSZ3, then read OTZ. */
#define HW_GTE_AVSZ3_OTZ(out)                                              \
    __asm__ volatile(".word 0x4B58002D;" "mfc2 %0, $7" : "=r"(out))

/* Write SXY0, SXY1, SXY2, SXY0 as a closed four-point line. */
#define HW_GTE_STORE_SXY_LOOP(p, o0, o1, o2, o3)                           \
    __asm__ volatile("swc2 $12, " #o0 "(%0);"                              \
                     "swc2 $13, " #o1 "(%0);"                              \
                     "swc2 $14, " #o2 "(%0);"                              \
                     "swc2 $12, " #o3 "(%0)"                               \
                     : : "r"(p) : "memory")

#endif
