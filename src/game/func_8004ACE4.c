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
    case 0x0F:
        if (SpuGetReverb() == 1)
            SpuSetReverb(0);
        if (SpuIsReverbWorkAreaReserved(-1) != 0)
            SpuReserveReverbWorkArea(0);
        packet.first = 1;
        packet.second = entry[0x13];
        SpuSetReverbModeParam((SpuReverbAttr *)&packet);
        if (SpuIsReverbWorkAreaReserved(-2) != 0)
            SpuReserveReverbWorkArea(1);
        SpuSetReverb(1);
        D_8009B458->field_0844 = entry[0x13];
        break;
    case 0x10:
        packet.first = 6;
        packet.third = entry[0x13] << 8;
        packet.fourth = entry[0x13] << 8;
        SpuSetReverbModeParam((SpuReverbAttr *)&packet);
        SpuSetReverb(1);
        D_8009B458->field_0845 = entry[0x13];
        break;
    }
}
