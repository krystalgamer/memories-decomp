#include "../types.h"

typedef struct {
    s16 f0;
    s16 f2;
    s16 f4;
    u8 pad_06[8];
    s16 fE;
} Obj;

/* SXY2 as written back by swc2 $14: screen x in the low half, screen y in the
   high half. */
typedef struct {
    u16 x;
    s16 y;
} Projected;

extern Obj D_800F2848;
extern u16 D_8009AF20;
extern s16 D_8009B200;
extern s16 D_8009B202;
extern u8 D_800FE148[];
extern void func_80017130(void);
extern void SetGeomScreen(long);
extern void SetGeomOffset(long, long);
extern void func_8001352C(void);
extern void GsSetLsMatrix(void *);

void func_800178BC(void)
{
    Projected p;
    s32 y;

    func_80017130();
    SetGeomScreen(D_800F2848.fE);
    SetGeomOffset(0xA0, 0x6C);
    D_800F2848.f0 = 0x14E;
    D_800F2848.f4 = 0x3FE;
    D_800F2848.f2 = D_8009AF20;
    func_8001352C();
    GsSetLsMatrix(D_800FE148);
    __asm__ volatile(
        "lui $2, 0x1F80\n"
        "ori $2, $2, 0x03E0\n"
        "addiu $3, $0, 0x3E8\n"
        "sh $3, 0($2)\n"
        "sh $0, 2($2)\n"
        "sh $3, 4($2)\n"
        "lwc2 $0, 0($2)\n"
        "lwc2 $1, 4($2)\n"
        "nop\n"
        "nop\n"
        ".word 0x4A180001\n" /* rtps */
        "addiu $2, $sp, 0x10\n"
        "swc2 $14, 0($2)\n"
        : "=m"(p) : : "$2", "$3"
    );
    /* Taking y through an s32 keeps the sign extension in the load, which is
       what makes it lh rather than the lhu the x read gets. */
    y = p.y;
    D_8009B200 = p.x - 0xA0;
    D_8009B202 = y - 0x6C;
    SetGeomOffset(0, 0);
    func_80017130();
}
