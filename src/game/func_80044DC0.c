#include "../types.h"
#include "func_80044DC0.h"
#include "sound.h"

/* Sibling of init_local_flags_and_notify (func_80044F58): builds the same
   4-byte CdMix volume/id packet, but here f0/f1-or-f2 are scaled DSP-style
   from a0 instead of copied verbatim. g_SDValue's fields 0x514/0x515 are
   per-channel volume scalars; f42 and f533 combine with (a0+1) to derive a
   shared 0-8191 "shift" factor, which each channel's scalar is then
   multiplied against and rescaled (>>7) into its packet byte. f48 picks
   which of f1/f2 carries the scaled value (the other gets 0), matching
   init_local_flags_and_notify's f48-based slot selection. Reloads
   g_SDValue through a fresh alias for the trailing f510 write, same
   pattern as project_d8009b45c_reload_pointer_trick. */
struct Local4 {
    u8 f0;
    u8 f1;
    u8 f2;
    u8 f3;
};

extern void func_8007CDC0(struct Local4 *a0);

void func_80044DC0(s16 a0) {
    SDValue *p = g_SDValue;
    s32 prod1 = (a0 + 1) * p->mix_scale;
    s32 prod2 = p->mix_multiplier * (prod1 >> 8);
    s32 shift = ((u32)(u16)prod2) >> 3;
    struct Local4 local;
    s16 s0 = a0;

    local.f0 = (shift * p->channel_volume[0]) >> 7;
    local.f3 = 0;

    if (p->output_type == 0) {
        local.f1 = 0;
        local.f2 = (shift * p->channel_volume[1]) >> 7;
    } else {
        local.f1 = (shift * p->channel_volume[1]) >> 7;
        local.f2 = 0;
    }

    func_8007CDC0(&local);

    g_SDValue->cd_volume = s0;
}
