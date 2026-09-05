#include "../../types.h"

typedef struct {
    u8 unk0[0x8];
    u16 flags;
    u8 unkA[0x26];
    s16 x;
    s16 y;
    u8 unk34[0x35];
    u8 kind;
} PasswordNode;

extern s32 gPassword_nDigitIndex;
extern PasswordNode *gPassword_pDigitCursorWidget;

void func_80169F38(PasswordNode *obj)
{
    switch (obj->kind) {
    case 0:
        obj->flags |= 0x40;
        if (gPassword_nDigitIndex >= 7) {
            obj->flags &= ~0x40;
        }
        obj->x = 0x129;
        obj->y = 0x68;
        break;
    case 1:
        obj->x = gPassword_pDigitCursorWidget->x + 5;
        obj->y = 0x78;
        break;
    case 2:
        obj->flags |= 0x40;
        if (gPassword_nDigitIndex <= 0) {
            obj->flags &= ~0x40;
        }
        obj->x = 0xA9;
        obj->y = 0x68;
        break;
    case 3:
        obj->x = gPassword_pDigitCursorWidget->x + 5;
        obj->y = 0x58;
        break;
    }
}
