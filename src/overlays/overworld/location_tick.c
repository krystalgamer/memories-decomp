#include "../../types.h"

extern u8 gCampaignMap_Location;
extern u8 gCampaignMap_LocationPrev;
extern u8 gCampaignMap_aLocationTable[];
extern u8 D_801695EC;
extern u8 D_8016960D;
extern u8 *D_801695C8;
extern u8 D_800E9ECE;
extern u8 D_800E9ECF;
extern u16 D_800F2848;
extern u8 D_8009B26C;
extern u8 D_8009B27A;
extern s32 func_80168AA8(void);
extern void func_80168050(s32);
extern u8 *CampaignMap_CreateLocationLabel(s32);
extern u8 *CampaignMap_CreateLocationMarker(s32);
extern void func_8004036C(u8 *);
extern void func_80168004(void);
extern void Fade_InitOut(void);
extern void func_8003FF58(s32);
extern void func_8001352C(void);
extern s32 CampaignMap_PickExit(void);

void func_80168FCC(void)
{
    u8 *table;
    u8 *record;
    s32 exit;

    if (D_801695EC != 0) {
        if (func_80168AA8() != 0) {
            return;
        }
        D_801695EC = 0;
        func_80168050(gCampaignMap_Location);
        CampaignMap_CreateLocationLabel(gCampaignMap_Location);
        if (gCampaignMap_Location >= 10) {
            if (D_801695C8 == 0) {
                D_801695C8 =
                    CampaignMap_CreateLocationMarker(gCampaignMap_Location);
            }
            table = gCampaignMap_aLocationTable;
            record = table + gCampaignMap_Location * 66;
            *(s16 *)(D_801695C8 + 0x30) = *(u16 *)(record + 12);
            *(s16 *)(D_801695C8 + 0x32) = *(u16 *)(record + 14);
        } else {
            func_8004036C(D_801695C8);
            D_801695C8 = 0;
        }
    }
    if (D_8016960D != 0) {
        if ((D_8016960D & 0x80) == 0) {
            D_8016960D = D_8016960D | 0x80;
            Fade_InitOut();
            D_800E9ECF = 2;
            func_80168004();
            func_8004036C(D_801695C8);
            func_8003FF58(4);
        }
        D_800F2848 = D_800F2848 - 2;
        func_8001352C();
        if ((D_800E9ECE & 0x80) == 0) {
            D_8009B26C = 2;
            D_8009B27A = gCampaignMap_Location + 32;
        }
        return;
    }
    gCampaignMap_LocationPrev = gCampaignMap_Location;
    exit = CampaignMap_PickExit();
    if (exit < 0) {
        return;
    }
    if ((exit & 0x8000) != 0) {
        D_8016960D = 1;
        return;
    }
    gCampaignMap_Location = exit;
    if ((u8)exit < 10) {
        func_8004036C(D_801695C8);
        D_801695C8 = 0;
    }
    func_80168004();
    D_801695EC = 1;
}
