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
