#include "../types.h"
#include "script_command_table.h"

/* Initialized data at 0x80090C50: the 23 scene-script command handlers.
 *
 * Script_RunTick masks the command word with 0x1F before indexing, while
 * TextBox_BuildStep can redispatch the current low byte directly. The image
 * contains exactly these entries, so the declaration remains unsized rather
 * than implying every masked value is valid.
 */

void func_8002E3DC(void);
void func_8002E470(void);
void func_8002E5AC(void);
void func_8002E918(void);
void func_8002E6B8(void);
void Script_OpShowImage(void);
void Script_OpViewportTween(void);
void Script_UpdateViewportTween(void);
void func_8002EB48(void);
void func_8002EB78(void);
void Script_OpSound(void);
void func_8002EDB0(void);
void func_8002EE20(void);
void func_8002EE94(void);
void func_8002F430(void);
void func_8002F438(void);
void func_8002F440(void);
void func_8002F630(void);
void func_8002F930(void);
void func_8002F94C(void);
void func_8002F968(void);
void func_8002F9D4(void);
void func_8002FA28(void);

ScriptCommandHandler D_80090C50[] = {
    func_8002E3DC,
    func_8002E470,
    func_8002E5AC,
    func_8002E918,
    func_8002E6B8,
    Script_OpShowImage,
    Script_OpViewportTween,
    Script_UpdateViewportTween,
    func_8002EB48,
    func_8002EB78,
    Script_OpSound,
    func_8002EDB0,
    func_8002EE20,
    func_8002EE94,
    func_8002F430,
    func_8002F438,
    func_8002F440,
    func_8002F630,
    func_8002F930,
    func_8002F94C,
    func_8002F968,
    func_8002F9D4,
    func_8002FA28,
};
