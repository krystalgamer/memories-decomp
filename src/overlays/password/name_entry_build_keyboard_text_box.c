#include "../../types.h"
#include "name_entry_keyboard.h"

extern void func_8003B6AC(s32, s32);
extern u8 *func_80035BE4(s32, s32, s32, s32, s32, s32);
extern void func_80039A14(u8 *);

void NameEntry_BuildKeyboardTextBox(s32 textOffset)
{
    u8 *object;

    func_8003B6AC(1, 1);
    object = func_80035BE4(1, textOffset + 0xF0, 0x16, 0x18, 0x140, 0xF0);
    object[0x5A] = 0x14;
    object[0x5B] = 0x12;
    func_80039A14(object);
}
