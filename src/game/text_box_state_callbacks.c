#include "../types.h"
#include "text_box_state_callbacks.h"
#include "dialog_choice_state.h"
#include "duel_effect_entry_control.h"
#include "duel_effect_interaction_states.h"
#include "duel_effect_state_callbacks.h"
#include "func_8003787C.h"

/* Initialized data at 0x80090E64: the text-box state callback table.
 *
 * TextBox_BuildStep indexes this table with the low five bits of state byte
 * 0x51. The image contains exactly eighteen entries; the masked index can
 * exceed that bound, so the public declaration remains unsized.
 */

void Dialog_UpdateChoice(u8 *);

TextBoxStateCallback D_80090E64[] = {
    Dialog_UpdateChoice,
    Dialog_UpdateChoice,
    func_8003741C,
    func_800374A8,
    (TextBoxStateCallback)func_800375A4,
    (TextBoxStateCallback)func_8003767C,
    (TextBoxStateCallback)func_800377C8,
    (TextBoxStateCallback)func_8003787C,
    (TextBoxStateCallback)func_800378D8,
    (TextBoxStateCallback)func_80037950,
    (TextBoxStateCallback)func_800377AC,
    (TextBoxStateCallback)func_8003771C,
    (TextBoxStateCallback)func_8003798C,
    (TextBoxStateCallback)func_800379C4,
    (TextBoxStateCallback)func_80037914,
    (TextBoxStateCallback)func_800379F8,
    (TextBoxStateCallback)func_80037A58,
    (TextBoxStateCallback)func_80037B40,
};
