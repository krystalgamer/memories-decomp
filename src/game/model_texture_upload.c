#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "model_texture_upload.h"
#include "file_query_wrappers.h"

u32 func_80058A7C(int side, int mode, GsIMAGE *params)
{
    register int offset;
    u32 high;
    u32 low;

    side &= 1;
    offset = side << 8;
    *(u16 *)&params->px -= 0x280;
    *(u16 *)&params->px += offset;
    if (params->cy < 0x100) {
        *(u16 *)&params->cx -= 0x80;
        if (params->cy == 8) {
            params->cy = side + 0xF2;
        }
    } else {
        *(u16 *)&params->cx -= 0x280;
        *(u16 *)&params->cx += offset;
    }
    high = ((*(u16 *)&params->pmode & 3) << 7) | ((mode & 3) << 5) |
           (((*(u16 *)&params->py & 0x100) << 16) >> 20) |
           ((*(u16 *)&params->px & 0x3FF) >> 6) |
           ((*(u16 *)&params->py & 0x200) << 2);
    low = (*(u16 *)&params->cy << 6) |
          ((*(u16 *)&params->cx >> 4) & 0x3F);
    return (high << 16) | (low & 0xFFFF);
}

s32 func_80058B4C(GsIMAGE *data, s32 arg1, s32 mode, s32 arg3, s32 x, s32 y,
                  s32 z, s32 w)
{
    u16 bounds[4];
    s32 high;
    s32 low;

    high = 0;
    low = high;

    if (func_800598E4(arg1, 0x80400000) > 0) {
        GsGetTimInfo((unsigned long *)0x80400004, data);
        if (mode < 2) {
            if (*(s32 *)&data->px == 0) {
                data->px = 0xC0;
                data->py = 0x100;
            }
            if (*(s32 *)&data->cx == 0) {
                data->cx = 0x200;
                data->cy = 0xF2;
            }
            if (x >= 0) {
                data->px = x;
            } else {
                *(u16 *)&data->px =
                    *(u16 *)&data->px + ((mode & 1) << 8);
            }
            if (y >= 0) {
                data->py = y;
            }
            if (z >= 0) {
                data->cx = z;
            }
            if (w >= 0) {
                data->cy = w;
            } else {
                *(u16 *)&data->cy = *(u16 *)&data->cy + mode;
            }
        } else if (mode >= 3) {
            func_80058A7C(mode - 3, arg3, data);
        }

        bounds[0] = *(u16 *)&data->px;
        bounds[1] = *(u16 *)&data->py;
        bounds[2] = data->pw;
        bounds[3] = data->ph;
        while (IsIdleGPU(3) != 0) {
        }
        while (LoadImage2(
            (RECT *)bounds, (u32 *)data->pixel
        ) != 0) {
        }
        bounds[0] = *(u16 *)&data->cx;
        bounds[1] = *(u16 *)&data->cy;
        bounds[2] = data->cw;
        bounds[3] = data->ch;
        while (IsIdleGPU(3) != 0) {
        }
        while (LoadImage2(
            (RECT *)bounds, (u32 *)data->clut
        ) != 0) {
        }
        while (IsIdleGPU(3) != 0) {
        }
        high = ((*(u16 *)&data->pmode & 3) << 7) | ((arg3 & 3) << 5) |
               (((s32)(*(u16 *)&data->py & 0x100) << 16) >> 20) |
               ((*(u16 *)&data->px & 0x3FF) >> 6) |
               ((*(u16 *)&data->py & 0x200) << 2);
        low = (*(u16 *)&data->cy << 6) |
              ((*(u16 *)&data->cx >> 4) & 0x3F);
    }

    return (high << 16) | (low & 0xFFFF);
}
