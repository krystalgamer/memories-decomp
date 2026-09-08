#include "../../types.h"
#include "name_entry_frame.h"
#include "../../game/gpu_packets.h"

void NameEntry_DrawSelectionFrame(NameEntrySelectionFrameView *r, GsOT *ot)
{
    LINE_F3 *poly;
    LINE_G2 *line;
    s32 pri;
    s32 x;
    s32 y;
    u32 w;
    u32 h;
    s32 xm1;
    s32 xp3;
    s32 right;

    poly = (LINE_F3 *)0x1F800000;
    line = (LINE_G2 *)0x1F800040;
    /* Keep packed color writes; the constructors fill command bytes afterward. */
    *(u32 *)&poly->r0 = 0x0000FF00;
    *(u32 *)&line->r0 = 0x0000FF00;
    *(u32 *)&line->r1 = 0;
    /* Keep the signed load; narrow only at the packet-submit boundary. */
    pri = r->priority;
    x = r->x;
    y = r->y;
    w = r->width;
    h = r->height;
    setLineF3(poly);
    setLineG2(line);
    xm1 = x - 1;
    xp3 = x + 3;
    poly->x1 = xm1;
    poly->x0 = xm1;
    poly->x2 = xp3;
    poly->y2 = y - 1;
    poly->y1 = y - 1;
    poly->y0 = y + 3;
    func_8005B260((u32 *)poly, ot, (u16)pri, 1);
    right = x + w;
    poly->x1 = right + 1;
    poly->x0 = right + 1;
    poly->x2 = right - 3;
    func_8005B260((u32 *)poly, ot, (u16)pri, 1);
    poly->y2 = y + h + 1;
    poly->y1 = y + h + 1;
    poly->y0 = y + h - 3;
    func_8005B260((u32 *)poly, ot, (u16)pri, 1);
    poly->x1 = xm1;
    poly->x0 = xm1;
    poly->x2 = xp3;
    func_8005B260((u32 *)poly, ot, (u16)pri, 1);
    line->x1 = x + (w >> 1);
    line->x0 = x + (w >> 1);
    line->y0 = y + 2;
    line->y1 = 0;
    func_8005B260((u32 *)line, ot, (u16)pri, 1);
    line->y0 = y + h - 2;
    line->y1 = 192;
    func_8005B260((u32 *)line, ot, (u16)pri, 1);
    line->y1 = y + (h >> 1);
    line->y0 = y + (h >> 1);
    line->x0 = x + 2;
    line->x1 = 0;
    func_8005B260((u32 *)line, ot, (u16)pri, 1);
    line->x0 = right - 2;
    line->x1 = 320;
    func_8005B260((u32 *)line, ot, (u16)pri, 1);
}
