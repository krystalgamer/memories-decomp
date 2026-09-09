#include "../types.h"
#include "func_80036D3C.h"

  void Text_SetCursorOffset(u8 *o){int v; unsigned int *p;v=func_80036D3C(o);p=&((unsigned int*)o)[*(s8*)(o+0x58)];*p=(*p&0xFFFF0000)|(v&0xFFFF);}
