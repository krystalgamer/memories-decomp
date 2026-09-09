#define D_8009B268_IN_DATA
#define D_8009B26D_IN_DATA
#include "../types.h"
#include "main_services.h"

extern u8 D_8009B26C[];

void func_8002FA28(void)
{
    D_8009B268 = 1;
    D_8009B26D = 5;
    D_8009B26C[0] = 8;
}
