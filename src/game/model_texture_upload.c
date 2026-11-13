#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "model_texture_upload.h"
#include "model_scene_runtime.h"

#ifndef MODEL_TEXTURE_PACK_DECL
#define MODEL_TEXTURE_PACK_DECL u32
#endif

#if !defined(VERSION_EUROPE) || \
    defined(VERSION_EUROPE_MODEL_TEXTURE_PACK_PAGE_CLUT)
MODEL_TEXTURE_PACK_DECL ModelTexture_PackPageClut(
    int side, int blend_mode, GsIMAGE *params)
{
    register int offset;
    u32 high;
    u32 low;

#ifdef MODEL_TEXTURE_PACK_SIDE_BIAS
    side -= MODEL_TEXTURE_PACK_SIDE_BIAS;
#endif
    side &= 1;
    offset = side << 8;
    params->px -= 0x280;
    params->px += offset;
    if (params->cy < 0x100) {
#ifndef VERSION_EUROPE
        params->cx -= 0x80;
#endif
        if (params->cy == 8) {
            params->cy = side + 0xF2;
        }
    } else {
        params->cx -= 0x280;
        params->cx += offset;
    }
    /* The (u16) reads are required for the match: they give the halfword
     * pmode load and the lhu zero-extension of py/cy. */
    high = (((u16)params->pmode & 3) << 7) | ((blend_mode & 3) << 5) |
           ((((u16)params->py & 0x100) << 16) >> 20) |
           ((params->px & 0x3FF) >> 6) |
           (((u16)params->py & 0x200) << 2);
    low = ((u16)params->cy << 6) |
          ((params->cx >> 4) & 0x3F);
    return (high << 16) | (low & 0xFFFF);
}
#endif

#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_MODEL_TEXTURE_LOAD_TIM)
s32 ModelTexture_LoadTim(GsIMAGE *data, char *path, s32 destination_mode, s32 blend_mode, s32 pixel_x,
                         s32 pixel_y, s32 clut_x, s32 clut_y)
{
    RECT bounds;
    s32 high;
    s32 low;
    ModelTextureTimWords *words = (ModelTextureTimWords *)data;

    high = 0;
    low = high;

    if (HostFile_ReadAll(path, (char *)0x80400000) > 0) {
        GsGetTimInfo((unsigned long *)0x80400004, data);
        if (destination_mode < 2) {
            if (words->pixel_origin == 0) {
                data->px = 0xC0;
                data->py = 0x100;
            }
            if (words->clut_origin == 0) {
#ifdef VERSION_EUROPE
                data->cx = 0x280;
#else
                data->cx = 0x200;
#endif
                data->cy = 0xF2;
            }
            if (pixel_x >= 0) {
                data->px = pixel_x;
            } else {
                data->px += (destination_mode & 1) << 8;
            }
            if (pixel_y >= 0) {
                data->py = pixel_y;
            }
            if (clut_x >= 0) {
                data->cx = clut_x;
            }
            if (clut_y >= 0) {
                data->cy = clut_y;
            } else {
                data->cy += destination_mode;
            }
        } else if (destination_mode >= 3) {
#ifdef VERSION_EUROPE
            ModelTexture_PackPageClut(destination_mode, blend_mode, data);
#else
            ModelTexture_PackPageClut(destination_mode - 3, blend_mode, data);
#endif
        }

        bounds.x = data->px;
        bounds.y = data->py;
        bounds.w = data->pw;
        bounds.h = data->ph;
        while (IsIdleGPU(3) != 0) {
        }
        while (LoadImage2(
            &bounds, (u32 *)data->pixel
        ) != 0) {
        }
        bounds.x = data->cx;
        bounds.y = data->cy;
        bounds.w = data->cw;
        bounds.h = data->ch;
        while (IsIdleGPU(3) != 0) {
        }
        while (LoadImage2(
            &bounds, (u32 *)data->clut
        ) != 0) {
        }
        while (IsIdleGPU(3) != 0) {
        }
        /* (u16) reads as in ModelTexture_PackPageClut, required for the match. */
        high = (((u16)data->pmode & 3) << 7) | ((blend_mode & 3) << 5) |
               ((((u16)data->py & 0x100) << 16) >> 20) |
               ((data->px & 0x3FF) >> 6) |
               (((u16)data->py & 0x200) << 2);
        low = ((u16)data->cy << 6) |
              ((data->cx >> 4) & 0x3F);
    }

    return (high << 16) | (low & 0xFFFF);
}
#endif
