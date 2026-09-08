#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

typedef struct {
    int first;
    int second;
    short third;
    short fourth;
    u8 padC[12];
} Packet;

void func_8004ACE4(u8 *entry, int unused)
{
    Packet packet;
    switch (entry[0x11]) {
    case SD_SEQUENCE_PARAMETER_REVERB_MODE:
        if (SpuGetReverb() == SPU_ON)
            SpuSetReverb(SPU_OFF);
        if (SpuIsReverbWorkAreaReserved(-1) != 0)
            SpuReserveReverbWorkArea(SPU_OFF);
        packet.first = SPU_REV_MODE;
        packet.second = entry[0x13];
        SpuSetReverbModeParam((SpuReverbAttr *)&packet);
        if (SpuIsReverbWorkAreaReserved(-2) != 0)
            SpuReserveReverbWorkArea(SPU_ON);
        SpuSetReverb(SPU_ON);
        D_8009B458->field_0844 = entry[0x13];
        break;
    case SD_SEQUENCE_PARAMETER_REVERB_DEPTH:
        packet.first = SPU_REV_DEPTHL | SPU_REV_DEPTHR;
        packet.third = entry[0x13] << 8;
        packet.fourth = entry[0x13] << 8;
        SpuSetReverbModeParam((SpuReverbAttr *)&packet);
        SpuSetReverb(SPU_ON);
        D_8009B458->field_0845 = entry[0x13];
        break;
    }
}
