#include "../../types.h"
#include "../../game/display_object_layout.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_lifecycle.h"
#include "shop.h"
#include "../../game/display_object_helpers.h"

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

void Password_UpdateDigitCursor(u8 *object)
{
    s16 remaining;

    object[0x22] = object[0x22] + 1;
    if ((object[0x6C] & 0x40) != 0) {
        if (func_80042B98((DisplayObjectLifecycle *)object) == 0) {
            func_800429D8(object);
            *(s16 *)(object + 0x36) =
                ((*(s16 *)(object + 0x18) - *(s16 *)(object + 0x30)) << 8) /
                *(s16 *)(object + 0x60);
            *(s16 *)(object + 0x38) =
                ((*(s16 *)(object + 0x1A) - *(s16 *)(object + 0x32)) << 8) /
                *(s16 *)(object + 0x60);
        }
        func_80042A78(object);
        remaining = *(u16 *)(object + 0x60) - 1;
        *(s16 *)(object + 0x60) = remaining;
        if (remaining <= 0) {
            *(s32 *)(object + 0x30) = *(s32 *)(object + 0x18);
            object[0x6C] = object[0x6C] & 0x3F;
        }
    }
}

void Password_UpdateDigitCursorDecoration(u8 *object)
{
    PasswordNode *obj = (PasswordNode *)object;

    switch (obj->kind) {
    case 0:
        obj->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        if (gPassword_nDigitIndex >= 7) {
            obj->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
        obj->x = 0x129;
        obj->y = 0x68;
        break;
    case 1:
        obj->x = gPassword_pDigitCursorWidget->x + 5;
        obj->y = 0x78;
        break;
    case 2:
        obj->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        if (gPassword_nDigitIndex <= 0) {
            obj->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
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

void Password_SetDigitCursorTarget(u8 *a)
{
    s32 value = gPassword_nDigitIndex;

    *(s16 *)(a + 0x1A) = 0x63;
    *(s16 *)(a + 0x18) = value * 16 + 0xA3;
}
