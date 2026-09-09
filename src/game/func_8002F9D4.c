#include "../types.h"
#include "script_state.h"

extern unsigned char D_801A8000[];
extern int func_8002EE5C(void);
void func_8002F9D4(void) {
    unsigned char *current = D_8009B290;
    unsigned int offset;
    D_8009B290 = current + 2;
    offset = current[0] | (current[1] << 8);
    if (func_8002EE5C() == 0) D_8009B290 = D_801A8000 + offset;
    D_8009B27C = 0;
}
