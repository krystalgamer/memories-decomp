#include "../types.h"

typedef struct {
    u8 pad_00[0x28];
    u16 x;
    u16 y;
    u8 pad_2C[4];
    u16 screen_x;
    u16 screen_y;
} ProjectionObject;

extern u8 D_800FE148[];
extern void func_800878D0(s32);
extern void func_800878B0(s32, s32);
extern void func_800855D0(void *);

void func_80015D18(ProjectionObject *object)
{
    func_800878D0(0x12C);
    func_800878B0(0xA0, 0x6C);
    func_800855D0(D_800FE148);
    __asm__ volatile(
        "lui $3, 0x1F80\n"
        "lhu $2, 40($16)\n"
        "ori $3, $3, 0x03E0\n"
        "sh $0, 2($3)\n"
        "sh $2, 0($3)\n"
        "lhu $2, 42($16)\n"
        "nop\n"
        "sh $2, 4($3)\n"
        "lwc2 $0, 0($3)\n"
        "lwc2 $1, 4($3)\n"
        "nop\n"
        "nop\n"
        ".word 0x4A180001\n"
        "addiu $2, $16, 48\n"
        "swc2 $14, 0($2)\n"
        : : : "$2", "$3"
    );
    object->screen_x -= 0x20;
    object->screen_y -= 0x1E;
    func_800878B0(0, 0);
}

typedef struct {
    u8 pad_00[0x28];
    s32 position;
    u8 pad_2C[0x44];
} ProjectionEntry;

typedef struct {
    u8 pad_00[0x28];
    s32 position;
    u8 pad_2C[0x3E];
    u8 slot;
} SlotProjectionObject;

extern ProjectionEntry D_800EFE48[];

void func_80015DB8(SlotProjectionObject *object)
{
    ProjectionEntry *entry;

    entry = D_800EFE48 + object->slot;
    object->position = entry->position;
    func_80015D18((ProjectionObject *)object);
}

/* Immediately follows func_80015DB8 in the image and shares its projection
   setup: same 0x1F8003E0 scratchpad pair, same rtps, same write-back through
   swc2 $14. Here the source coordinates come from the object's own target
   record and the projected pair is biased into the object's screen fields. */
typedef struct {
    u16 x;
    s16 y;
} ProjectedPair;

typedef struct {
    void *record;
    u8 pad_04[4];
    s16 screen_x;
    s16 screen_y;
    u8 pad_0C[0xC];
    s8 f18;
} TrackedObject;

extern u8 D_8009B1D5;
/* Declared as an array so it stays out of small data: the target reads it with
   a %hi/%lo pair while D_8009B1D5, a byte, is %gp_rel in the same body. */
extern void *D_800E9D98[];
extern void func_80016784(void *, void *, s32, s32);

void func_80015DFC(TrackedObject *object)
{
    ProjectedPair p;
    s32 y;

    func_800855D0(D_800FE148);
    __asm__ volatile(
        "lw $2, 0($16)\n"
        "lui $3, 0x1F80\n"
        "lhu $2, 0x30($2)\n"
        "ori $3, $3, 0x03E0\n"
        "sh $0, 2($3)\n"
        "sh $2, 0($3)\n"
        "lw $2, 0($16)\n"
        "nop\n"
        "lhu $2, 0x34($2)\n"
        "nop\n"
        "sh $2, 4($3)\n"
        "lwc2 $0, 0($3)\n"
        "lwc2 $1, 4($3)\n"
        "nop\n"
        "nop\n"
        ".word 0x4A180001\n" /* rtps */
        "addiu $2, $sp, 0x10\n"
        "swc2 $14, 0($2)\n"
        : "=m"(p) : : "$2", "$3"
    );
    object->screen_x = p.x - 0x1A;
    y = p.y;
    object->screen_y = y - 0x1E;
    if (D_8009B1D5 != 0) {
        if (object->f18 < 0xF) {
            object->screen_y = y - 0x1D;
        }
    } else {
        if (object->f18 >= 0xF) {
            object->screen_y = y - 0x1D;
        }
    }
    func_80016784(object->record, D_800E9D98[0], object->screen_x,
                  object->screen_y);
    func_800878B0(0xA0, 0x6C);
}
