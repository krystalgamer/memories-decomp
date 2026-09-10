#include "../types.h"
#include "script_command_busy.h"
#include "script_state.h"
#include "script_control_commands.h"

extern u8 D_8009B269[];
extern u8 D_8009B26C[];

void func_8002F930(void)
{
    D_8009B26C[0] = 12;
    D_8009B269[0] = 12;
}

void func_8002F94C(void)
{
    D_8009B26C[0] = 15;
    D_8009B269[0] = 15;
}

void func_8002F968(void)
{
    if (func_8002E3B4() == 0) {
        u8 *p = D_8009B290;
        D_8009B290 = p + 2;
        D_8009B278 = p[0] | (p[1] << 8);
    }
    D_8009B278--;
    if (D_8009B278 <= 0)
        D_8009B27C = 0;
}
