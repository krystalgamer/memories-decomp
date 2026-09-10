#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "func_8004ACE4.h"

void func_8004ACE4(SDSecondaryRecord *entry, int unused)
{
    SpuReverbAttr packet;
    switch (entry->parameter_selector) {
    case SD_SEQUENCE_PARAMETER_REVERB_MODE:
        if (SpuGetReverb() == SPU_ON)
            SpuSetReverb(SPU_OFF);
        if (SpuIsReverbWorkAreaReserved(SPU_CHECK) != 0)
            SpuReserveReverbWorkArea(SPU_OFF);
        packet.mask = SPU_REV_MODE;
        packet.mode = entry->control_value;
        SpuSetReverbModeParam(&packet);
        if (SpuIsReverbWorkAreaReserved(SPU_DIAG) != 0)
            SpuReserveReverbWorkArea(SPU_ON);
        SpuSetReverb(SPU_ON);
        D_8009B458->field_0844 = entry->control_value;
        break;
    case SD_SEQUENCE_PARAMETER_REVERB_DEPTH:
        packet.mask = SPU_REV_DEPTHL | SPU_REV_DEPTHR;
        packet.depth.left = entry->control_value << 8;
        packet.depth.right = entry->control_value << 8;
        SpuSetReverbModeParam(&packet);
        SpuSetReverb(SPU_ON);
        D_8009B458->field_0845 = entry->control_value;
        break;
    }
}
