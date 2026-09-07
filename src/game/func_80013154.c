#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/rand.h"
#include "rand_constants.h"

typedef struct {
    s16 v[10];
} ScreenBlock;

/* The init block is a run of byte stores to distinct globals; declared
   volatile so the emitted order is the source order (see Main_Init). The
   six D_8009B14x bytes are addressed %hi/%lo in retail, so they sit outside
   small data. */
extern volatile u8 D_8009B0AD;
extern volatile u8 D_8009B0D0;
extern volatile u8 D_8009B0A8;
extern volatile u8 D_8009B14C __attribute__((section(".data")));
extern volatile u8 D_8009B144 __attribute__((section(".data")));
extern volatile u8 D_8009B14B __attribute__((section(".data")));
extern volatile u8 D_8009B143 __attribute__((section(".data")));
extern volatile u8 D_8009B14A __attribute__((section(".data")));
extern volatile u8 D_8009B142 __attribute__((section(".data")));
extern volatile u8 D_800FE048[];
extern volatile u8 D_8009B0A0;
extern volatile u8 D_8009B0A1;
extern volatile u8 D_8009B0A2;
extern volatile u8 D_8009B0A3;
extern ScreenBlock D_800E9D28;
extern ScreenBlock D_800FE0A8;

extern void func_800856A0(u16, u16, u16, u16);
extern void func_80085DB0(u16, u16, void *);
extern void func_80085740(void);
extern void func_800855B0(s32, s32);
extern void Input_InitPads(void);
extern void MemCardInit(s32);
extern void File_SetPositionTable(void);

/* Graphics and input start-up, called from Main_Init with the work area
 * in $a0. Resets the GPU, sets up a 320x240 display and the display
 * buffers, then initialises the frame flags and the six 0x8009B14x bytes
 * and the display-object slots at D_800FE048. The work area holds two
 * 0x5160-byte frame buffers; each gets its four ordering tables at
 * +0x5110 (lengths 2, 6, 0xC and 6 with their table bases at +0, +0x10,
 * +0x110 and +0x4110) cleared from the last to the first. The screen
 * block at D_800FE0A8 is copied to D_800E9D28, the GTE and the 3D wrappers
 * are initialised with a 300 projection, then the pads, the memory card,
 * the file position table and the random seed. */
void func_80013154(u8 *base)
{
    u8 *buf;
    s32 k;
    s32 off;
    s32 six;
    /* Retail copies the count into $v1 for the two byte stores. As a
       plain local the copy is folded away, since cse rates a narrowing
       subreg of the count cheaper than a fresh pseudo; only a hard
       register the user names is cheaper still. Set once, so it is
       launched right before its stores like the neighbouring constants. */
    register u8 count __asm__("$3");

    ResetGraph(0);
    GsInitGraph(0x140, 0xF0, 4, 1, 0);
    func_800856A0(0, 0, 0x140, 0);
    six = 6;
    buf = base;
    D_8009B0AD = 1;
    D_8009B0D0 = 1;
    D_8009B0A8 = 0;
    D_8009B14C = 1;
    D_8009B144 = 1;
    D_8009B14B = 1;
    D_8009B143 = 1;
    D_8009B14A = 1;
    D_8009B142 = 1;
    D_800FE048[0x18] = 1;
    D_800FE048[0x16] = 1;
    D_800FE048[0x19] = 1;
    D_800FE048[0x1A] = 1;
    D_800FE048[0x1B] = 1;
    count = six;
    D_8009B0A0 = 2;
    D_8009B0A1 = count;
    D_8009B0A2 = 0xC;
    D_8009B0A3 = count;
    /* A goto loop: a real loop would hoist the 2 and the end pointer into
       two more callee-saved registers. The counter reset is the first
       statement of the body so reorg can copy it into the back-branch
       delay slot; the entry then falls through it. */
next:
    k = 3;
    off = 0x514C;
    *(s32 *)(buf + 0x5110) = 2;
    *(u8 **)(buf + 0x5128) = buf + 0x10;
    *(s32 *)(buf + 0x5138) = 0xC;
    *(u8 **)(buf + 0x513C) = buf + 0x110;
    *(u8 **)(buf + 0x5114) = buf;
    *(s32 *)(buf + 0x5124) = six;
    *(s32 *)(buf + 0x514C) = six;
    *(u8 **)(buf + 0x5150) = buf + 0x4110;
    do {
        func_80085DB0(0, k, buf + off);
        off -= 0x14;
        k--;
    } while (k >= 0);
    buf += 0x5160;
    /* Signed compare: retail uses slt, so the pointers compare as s32. */
    if ((s32)buf < (s32)(base + 0xA2C0)) {
        goto next;
    }
    D_800E9D28 = D_800FE0A8;
    InitGeom();
    func_80085740();
    func_800855B0(0, 0);
    SetGeomScreen(0x12C);
    Input_InitPads();
    MemCardInit(1);
    File_SetPositionTable();
    srand(RAND_GRAPHICS_INIT_SEED);
}
