#include "../../types.h"

typedef struct {
    u8 pad[3];
    u8 len;
    u8 r;
    u8 g;
    u8 b;
    u8 code;
    s16 x0;
    s16 y0;
    s16 x1;
    s16 y1;
    s16 x2;
    s16 y2;
    s16 x3;
    s16 y3;
} MainMenuQuad;

extern void *D_800E9D94;
extern void func_8005B260(void *, void *, int, int);

void func_80184454(int column)
{
    MainMenuQuad quad;
    int left = column * 160;
    int right = left + 0xA0;

    quad.len = 5;
    quad.code = 0x28;
    quad.r = 0x40;
    quad.g = 0x40;
    quad.b = 0x40;
    quad.x0 = left;
    quad.y0 = 0;
    quad.x1 = right;
    quad.y1 = 0;
    quad.x2 = left;
    quad.y2 = 0xF0;
    quad.x3 = right;
    quad.y3 = 0xF0;
    func_8005B260(&quad, D_800E9D94, 0x1F, 2);
}
