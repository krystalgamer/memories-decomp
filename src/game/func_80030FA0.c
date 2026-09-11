#include "../types.h"
#include "frontend_debug_tables.h"
#include "frontend_debug_state.h"
#include "func_80030FA0.h"
#define D_8009B26C_AS_SCALAR_DATA
#include "../unmatched.h"

extern u8 gFreeDuel_bReturnFlags __attribute__((section(".data")));

void func_80030FA0(void)
{
    u8 *p = D_80090D68;
    s32 i = D_8009B2F1;
    u8 v;

    gFreeDuel_bReturnFlags = 0;
    v = p[i];
    D_8009B2B2 = 0;
    D_8009B26C = v;
}
