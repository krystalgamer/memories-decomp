/* func_80012DB4 @ 0x80012DB4 in SLUS_014.11 — 0xA8, 42 instructions.
 *
 * My attempt: the C I believe is correct. Self-contained, so it builds with
 * nothing but the compiler; see build.sh for the exact invocation, and
 * func_80012DB4.s for the original game assembly.
 *
 * Everything here matches the game except one instruction: maspsx pads the
 * `beq` delay slot with `nop` where ASPSX moves the following `li $v0, 1`
 * into it. Built it is 172 bytes against the game's 168.
 *
 * D_8009B0C1 is defined rather than declared on purpose. The assembler only
 * resolves a small global gp-relative when the translation unit defines it,
 * and that is what makes the store a single %gp_rel instruction carrying the
 * load-delay nop at 0x80012DEC. Build with --use-comm-section so it stays a
 * COMMON. */

typedef unsigned char u8;
typedef unsigned short u16;
typedef signed int s32;

/* Psy-Q libgpu / libetc entry points; prototypes only, never called here. */
extern void DrawSync(s32 mode);
extern s32 VSync(s32 mode);

extern u16 D_8009B098;
extern volatile u8 D_8009B0C0;
extern volatile s32 D_8009B0C8;
u8 D_8009B0C1;
extern s32 D_8009B0D8;
extern volatile unsigned char D_8009AFA3 __attribute__((section(".data")));
extern unsigned char D_8009AFA4 __attribute__((section(".data")));
extern s32 D_8009B0CC;

void func_80012DB4(void)
{
    s32 v;

    if ((D_8009B098 & 0x8000) == 0) {
        DrawSync(0);
    }
    while (D_8009B0C8 < D_8009B0C0) {
    }
    v = D_8009B0C8;
    D_8009B0C1 = v;
    if ((v & 0xFF) != 0) {
        D_8009B0C1 = 1;
    }
    D_8009B0D8 = D_8009B0C1 + 1;
    if (D_8009AFA4 != 0) {
        D_8009AFA3 = 2;
    } else {
        D_8009AFA3 = D_8009B0D8;
    }
    D_8009AFA4 = 0;
    D_8009B0C8 = -1;
    VSync(0);
    D_8009B0CC = D_8009B0CC + 1;
}
