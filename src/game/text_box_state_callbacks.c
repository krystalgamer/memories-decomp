#include "../types.h"
#include "text_box_state_callbacks.h"

/* Initialized data at 0x80090E64: the text-box state callback table.
 *
 * TextBox_BuildStep indexes this table with the low five bits of state byte
 * 0x51. The image contains exactly eighteen entries; the masked index can
 * exceed that bound, so the public declaration remains unsized.
 */

void Dialog_UpdateChoice(u8 *);
void func_8003741C(u8 *);
void func_800374A8(u8 *);
void func_800375A4(u8 *);
void func_8003767C(u8 *);
void func_8003771C(u8 *);
void func_800377AC(u8 *);
void func_800377C8(u8 *);
struct Obj;
void func_8003787C(struct Obj *);
void func_800378D8(u8 *);
void func_80037914(u8 *);
void func_80037950(u8 *);
void func_8003798C(u8 *);
void func_800379C4(u8 *);
void func_800379F8(u8 *);
void func_80037A58(u8 *);
void func_80037B40(u8 *);

TextBoxStateCallback D_80090E64[] = {
    Dialog_UpdateChoice,
    Dialog_UpdateChoice,
    func_8003741C,
    func_800374A8,
    func_800375A4,
    func_8003767C,
    func_800377C8,
    (TextBoxStateCallback)func_8003787C,
    func_800378D8,
    func_80037950,
    func_800377AC,
    func_8003771C,
    func_8003798C,
    func_800379C4,
    func_80037914,
    func_800379F8,
    func_80037A58,
    func_80037B40,
};
