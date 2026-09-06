#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "card_constants.h"

typedef struct {
    u8 pad_00[0x40];
    u16 field_40;
} DuelCardStatObject;

extern s32 gDuel_adwCardStats[];

extern void *func_8004002C(void);
extern DuelCardStatObject *func_800400AC(void *, s32);
extern void func_80040510();
extern void func_800428EC(DuelCardStatObject *, s32);
extern void func_80042918(DuelCardStatObject *);

void func_80031574(s32 index, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
{
    register s32 table_index __asm__("$16") = index;
    register s32 x __asm__("$17") = arg1;
    register s32 y __asm__("$18") = arg2;
    register DuelCardStatObject *object __asm__("$19");
    register s32 field_18 __asm__("$20") = arg3;
    register s32 bits __asm__("$2");
    s32 kind;

    object = func_800400AC(func_8004002C(), 1);
    func_80040510(object, x, y, 0x10, 0x10, 0, 0xC8, 0xB, 0x260, 0xFC);
    __asm__ volatile(
        "lui $2,%%hi(gDuel_adwCardStats)\n\t"
        "addiu $2,$2,%%lo(gDuel_adwCardStats)\n\t"
        "addiu %1,%1,-1\n\t"
        "sll %1,%1,2\n\t"
        "addu %1,%1,$2\n\t"
        "lw %0,0(%1)\n\t"
        "nop"
        : "=r"(bits), "+r"(table_index));
    kind = (bits >> CARD_STAT_TYPE_SHIFT) & CARD_STAT_TYPE_MASK;
    switch (kind) {
    case CARD_TYPE_MAGIC:
    case CARD_TYPE_EQUIP:
        object->field_40 += 0x10;
        break;
    case CARD_TYPE_TRAP:
        object->field_40 += 0x20;
        break;
    case CARD_TYPE_RITUAL:
        object->field_40 += 0x30;
        break;
    }
    func_80042918(object);
    func_800428EC(object, 10);
    __asm__ volatile(
        "lui $5,0xF7FF\n\t"
        "addu $2,%0,$0\n\t"
        "sw $0,0x44($2)\n\t"
        "sh %1,0x18($2)\n\t"
        "lw $3,0x50($sp)\n\t"
        "lw $4,4($2)\n\t"
        "ori $5,$5,0xFFFF\n\t"
        "sh $3,0x1A($2)\n\t"
        "lui $3,%%hi(func_800313E8)\n\t"
        "addiu $3,$3,%%lo(func_800313E8)\n\t"
        "sw $3,0x24($2)\n\t"
        "lhu $3,8($2)\n\t"
        "and $4,$4,$5\n\t"
        "sw $4,4($2)\n\t"
        "andi $3,$3,0xFFF7\n\t"
        "sh $3,8($2)"
        :
        : "r"(object), "r"(field_18)
        : "$2", "$3", "$4", "$5", "memory");
    (void)arg4;
}

void func_800316F0(u8 *arg0, s32 arg1, u8 *arg2, s32 arg3)
{
    s32 i;

    arg0[0xF] = 0x70;
    i = arg3 - 1;
    if (i >= 0) {
        do {
            u8 value = arg2[i];

            if (value < 0xA) {
                arg0[0xE] = value * 8 - 0x80;
                GsSortFastSprite((GsSPRITE *)arg0, (GsOT *)arg1, 0);
            }
            *(u16 *)(arg0 + 4) += 8;
            i--;
        } while (i >= 0);
    }
}
