#include "../types.h"
#include "display_object_layout.h"

typedef void (*ObjFn)(u8 *);
typedef struct Slot70 {
    s16 unk0;
    s16 unk2;
    u8 pad4[4];
    u16 flags;
    u8 unkA[0x1A];
    ObjFn unk24;
    u8 pad28[DISPLAY_OBJECT_RECORD_SIZE - 0x28];
} Slot70;
extern u8 D_800EFE38[];
extern u8 D_800F2878[];
extern Slot70 D_800EFE48[];
extern void func_8004020C(s32);

void func_800402A0(Slot70 *arg0, s32 arg1) {
    u16 saved = arg0->flags;
    s32 v;

    func_8004020C((s32)arg0);
    v = *(s16 *)(D_800EFE38 + arg1 * 2);
    if (v < 0) {
        *(u16 *)(D_800F2878 + arg1 * 2) = arg0->unkA[0];
        arg0->unk2 = -1;
        arg0->unk0 = -1;
    } else {
        D_800EFE48[v].unk0 = arg0->unkA[0];
        arg0->unk2 = *(u16 *)(D_800EFE38 + arg1 * 2);
    }
    arg0->unk0 = -1;
    *(u16 *)(D_800EFE38 + arg1 * 2) = arg0->unkA[0];
    arg0->flags = saved;
}
