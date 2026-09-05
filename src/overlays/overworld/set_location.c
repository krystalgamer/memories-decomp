#include "../../types.h"

extern u8 gCampaignMap_Location;
extern u8 gCampaignMap_LocationPrev;
extern u8 D_80169619;
extern u8 D_8016960D;
extern u8 D_801695EC;
extern u8 *D_801695C8;
extern s32 D_801695F8[];
extern u8 *D_801695D8;
extern void *D_800E9DBC;
extern s16 D_800F2856;
extern s32 D_80010000;
extern u8 D_801AF000[];
extern void func_80168258(void);
extern void func_800530C4(void);
extern void func_800533D8(void);
extern void func_80056250(s32, s32, s32, s32);
extern u8 *func_80058F74(s32);
extern void func_8005922C(u8 *, s32 *);
extern void func_80168310(void);
extern void func_80087890(s32, s32, s32);
extern void func_80086810(s32, s32, s32);
extern void func_80035668(s32);
extern s32 func_8004002C(void);
extern u8 *func_800400AC(s32, s32);
extern void func_800428A8(u8 *, s32, s32, s32, s32, s32, s32, s32, u8 *);
extern void func_800428EC(u8 *, s32);
extern void func_801681E8(s32);
extern void func_8016818C(s32);
extern void func_80168050(s32);
extern u8 *func_80168588(s32);
extern s32 func_8002CCA8(s32);
extern void func_8003FF08(s32);
extern void func_80042C08(void);

void CampaignMap_SetLocation(s32 index)
{
    u8 *obj;
    s32 track;
    s32 i;
    s32 location;
    u8 *marker;
    u8 *panel;
    u16 flags;
    s32 colour[3];

    gCampaignMap_Location = index;
    D_80169619 = 0;
    D_8016960D = 0;
    D_801695EC = 0;
    D_801695C8 = 0;
    for (i = 3; i >= 0; i--) {
        D_801695F8[i] = 0;
    }
    func_800530C4();
    func_800533D8();
    func_80056250(2, D_80010000, 0x43000, 0);
    obj = func_80058F74(2);
    colour[0] = 1365;
    colour[1] = 1365;
    colour[2] = 1365;
    func_8005922C(obj, colour);
    func_80168310();
    func_80087890(0, 0, 0);
    func_80086810(6000, 8000, D_800F2856);
    D_800E9DBC = func_80168258;
    func_80035668(0);
    obj = func_800400AC(func_8004002C(), 2);
    func_800428A8(obj, 96, 24, 0, 0, 0, 23, 256, D_801AF000);
    *(u16 *)(obj + 8) = *(u16 *)(obj + 8) | 0x28;
    obj = func_800400AC(func_8004002C(), 6);
    *(s16 *)(obj + 0x30) = 160;
    *(s16 *)(obj + 0x32) = 144;
    *(s16 *)(obj + 0x60) = 128;
    *(s16 *)(obj + 0x48) = 32;
    *(s16 *)(obj + 0x4A) = 192;
    *(s16 *)(obj + 0x44) = 5120;
    *(s16 *)(obj + 0x46) = 4096;
    func_800428EC(obj, -10);
    location = gCampaignMap_Location;
    *(void **)(obj + 0x4C) = func_80042C08;
    D_801695D8 = obj;
    func_801681E8(location);
    func_8016818C(gCampaignMap_Location);
    func_80168050(gCampaignMap_Location);
    gCampaignMap_LocationPrev = gCampaignMap_Location;
    if ((u8)gCampaignMap_Location >= 10) {
        marker = func_80168588(gCampaignMap_Location);
        panel = D_801695D8;
        flags = *(u16 *)(panel + 8);
        D_801695C8 = marker;
        *(u16 *)(panel + 8) = flags & 0xFFBF;
    }
    track = 0x70A0;
    if (func_8002CCA8(71) != 0) {
        track = 0x70B0;
    }
    func_8003FF08(track);
}
