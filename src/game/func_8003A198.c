#include "../types.h"
#include "func_8003A198.h"

int func_8003A198(unsigned char*b,int x,int y,int z){u16 *p;p=(u16*)(b+x*2);if(*p){p=(u16*)(b+*p+y*2);if(*p){p=(u16*)(b+*p+z*2);if(*p)return 1;}}return 0;}
