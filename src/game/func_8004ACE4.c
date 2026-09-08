#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

void func_8004ACE4(u8 *entry, int unused)
{
    SpuReverbAttr packet;
    switch (entry[0x11]) {
    case SD_SEQUENCE_PARAMETER_REVERB_MODE:
        if (SpuGetReverb() == SPU_ON)
            SpuSetReverb(SPU_OFF);
        if (SpuIsReverbWorkAreaReserved(SPU_CHECK) != 0)
            SpuReserveReverbWorkArea(SPU_OFF);
        packet.mask = SPU_REV_MODE;
        packet.mode = entry[0x13];
        SpuSetReverbModeParam(&packet);
        if (SpuIsReverbWorkAreaReserved(SPU_DIAG) != 0)
            SpuReserveReverbWorkArea(SPU_ON);
        SpuSetReverb(SPU_ON);
        D_8009B458->field_0844 = entry[0x13];
        break;
    case SD_SEQUENCE_PARAMETER_REVERB_DEPTH:
        packet.mask = SPU_REV_DEPTHL | SPU_REV_DEPTHR;
        packet.depth.left = entry[0x13] << 8;
        packet.depth.right = entry[0x13] << 8;
        SpuSetReverbModeParam(&packet);
        SpuSetReverb(SPU_ON);
        D_8009B458->field_0845 = entry[0x13];
        break;
    }
}
