#include "../../types.h"
#include "location_label.h"
#include "../../game/text_box_runtime.h"
#include "campaign_map.h"

extern void *func_80035BE4(s32, s32, s32, s32, s32, s32);

u8 *CampaignMap_CreateLocationLabel(s32 unused)
{
    u8 *object;

    object = func_80035BE4(0, gCampaignMap_Location + 0x8350, 0x60, 0x18, 0x80, 0xC);
    func_80039A60(object);
    return object;
}
