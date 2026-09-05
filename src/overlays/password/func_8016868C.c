#include "../../types.h"

typedef struct {
    u8 unk0[0xC];
    s16 x;
    s16 y;
    u8 unk10;
    u8 flags;
    u8 unk12[0xA];
} PasswordNode;

typedef struct {
    u8 unk0[0x24];
    PasswordNode *nodes;
    u8 unk28[0x3C];
} PasswordEntry;

extern PasswordEntry D_800EB0F8[];

PasswordNode *func_8016868C(s32 index, s32 x, s32 y)
{
    PasswordEntry *base;
    PasswordNode *node;

    base = D_800EB0F8;
    node = base[index].nodes;
    for (;;) {
        if (!(node->flags & 0x80)) {
            return (PasswordNode *)0;
        }
        if (node->x == x && node->y == y) {
            return node;
        }
        node++;
    }
}
