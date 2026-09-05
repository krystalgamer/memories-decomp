#include "../types.h"

typedef struct {
    u16 x;
    s16 y;
} ProjectedPair;

typedef struct {
    s16 x;
    s16 y;
} ScreenPair;

extern s16 D_800F2856[];
extern u8 D_800FE148[];
extern u8 D_8009B1D5;
extern u16 D_800908A0[];
extern ScreenPair D_800EA070[];
extern void SetGeomScreen(long);
extern void SetGeomOffset(long, long);
extern void func_800855D0(void *);

/* Projects the thirty coordinate pairs in D_800908A0 through the GTE, one per
   iteration, and writes the biased screen pairs to D_800EA070. Same scratchpad
   and rtps as display_projection.c; the per-iteration form of func_80015DFC.

   The setup after the three calls is written out one statement at a time, in
   the order the target materialises it, because sched1 otherwise emits the two
   table addresses ahead of the counter and the scratchpad address. */
void func_800177C4(void)
{
    ProjectedPair p;
    /* $t0 rather than $t1: with both this and pp allocated from the same class
       the allocator otherwise swaps them, and every scratchpad access differs. */
    register u8 *pad asm("$8");
    ProjectedPair *pp;
    ScreenPair *out;
    u16 *src;
    s32 i;

    SetGeomScreen(D_800F2856[0]);
    SetGeomOffset(0xA0, 0x6C);
    func_800855D0(D_800FE148);
    pad = (u8 *)0x1F8003E0;
    i = 0;
    pp = &p;
    out = D_800EA070;
    src = D_800908A0;
    do {
        s32 y;

        __asm__ volatile(
            "lhu $2, 0(%2)\n"
            "sh $0, 2(%1)\n"
            "sh $2, 0(%1)\n"
            "lhu $2, 2(%2)\n"
            "nop\n"
            "sh $2, 4(%1)\n"
            "lwc2 $0, 0(%1)\n"
            "lwc2 $1, 4(%1)\n"
            "nop\n"
            "nop\n"
            ".word 0x4A180001\n" /* rtps */
            "swc2 $14, 0(%3)\n"
            : "=m"(p) : "r"(pad), "r"(src), "r"(pp) : "$2"
        );
        out->x = p.x - 0x1A;
        y = p.y;
        out->y = y - 0x1E;
        if (D_8009B1D5 != 0) {
            if (i < 0xF) {
                out->y = y - 0x1D;
            }
        } else {
            if (i >= 0xF) {
                out->y = y - 0x1D;
            }
        }
        out++;
        src += 2;
        i++;
    } while (i < 0x1E);
    SetGeomOffset(0, 0);
}
