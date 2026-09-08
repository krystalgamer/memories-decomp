#include "../../types.h"
#include "../../game/gpu_packets.h"
#include "trade_helpers.h"

extern void *D_800E9D94;

void MainMenu_DrawTradeColumnOverlay(s32 column)
{
    POLY_F4 quad;
    s32 left = column * 160;
    s32 right = left + 0xA0;

    setPolyF4(&quad);
    quad.r0 = 0x40;
    quad.g0 = 0x40;
    quad.b0 = 0x40;
    quad.x0 = left;
    quad.y0 = 0;
    quad.x1 = right;
    quad.y1 = 0;
    quad.x2 = left;
    quad.y2 = 0xF0;
    quad.x3 = right;
    quad.y3 = 0xF0;
    func_8005B260((u32 *)&quad, (GsOT *)D_800E9D94, 0x1F, 2);
}
