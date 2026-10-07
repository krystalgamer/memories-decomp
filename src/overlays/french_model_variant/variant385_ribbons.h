#ifndef FRENCH385_RIBBONS_VIEW_H
#define FRENCH385_RIBBONS_VIEW_H
#include "../../types.h"
#include "../model_variant/model_variant.h"

/* MODEL385 work offsets for the fields that variant402_ribbons.h names by
 * their MODEL402 offsets. */
enum {
    RIBBON385_WORK_0058 = 0x056C,
    RIBBON385_WORK_0178 = 0x068C,
    RIBBON385_WORK_0668 = 0x0DEC,
    RIBBON385_WORK_0828 = 0x0FAC,
    RIBBON385_WORK_082C = 0x0FB0,
    RIBBON385_WORK_0830 = 0x0FB4,
    RIBBON385_WORK_083C = 0x0FC0,
    RIBBON385_WORK_0840 = 0x0FC4,
    RIBBON385_WORK_0844 = 0x0FC8,
    RIBBON385_WORK_0850 = 0x0FD4,
    RIBBON385_WORK_0858 = 0x0FDC,
    RIBBON385_WORK_0868 = 0x0FEC,
    RIBBON385_WORK_0874 = 0x0FF8,
    RIBBON385_WORK_087C = 0x1000,
    RIBBON385_WORK_0890 = 0x1014,
    RIBBON385_WORK_08A6 = 0x102A,
    RIBBON385_WORK_08A8 = 0x102C,
    RIBBON385_WORK_08AC = 0x1030,
    RIBBON385_WORK_08C0 = 0x1044,
    RIBBON385_CONFIG_COUNT = 0xC
};

/* One 0x60-byte ribbon of this family: the MODEL402 ribbon with the
 * projection flags kept between the widths and the depths. */
typedef struct {
    SVECTOR a[2];
    PSXLONG sa[2];
    s32 angle[2];
    SVECTOR b[2];
    PSXLONG sb[2];
    s32 width[2];
    PSXLONG flag[2];
    s32 otz[2];
    u16 ox[2];
    u16 oy[2];
    u8 unknown_58[8];
} Ribbon385;
#endif
