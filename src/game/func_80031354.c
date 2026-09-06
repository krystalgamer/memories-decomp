#include "../types.h"
#include "display_object_layout.h"

typedef struct {
    u8 pad_00[8];
    u16 flags_08;
} LocalRecord;

extern u8 D_8009B2EB;
extern u16 D_8009B2DE;
extern LocalRecord *D_800EB184[4];
extern LocalRecord *D_8009B2E4;
extern u8 D_800EB010[16];

extern void func_8002FD10(s32);
extern void func_8003134C(void);
extern s32 Rand_GetInterval(s32);
extern void func_8003B378(void *, s32);

void func_80031354(void)
{
    u8 flags = D_8009B2EB;

    if (!(flags & 0x80)) {
        D_8009B2EB = flags | 0x80;
        D_8009B2DE = 0;
        func_8002FD10(0);
    }

    func_8003134C();

    if (D_8009B2DE == 0) {
        LocalRecord *first = D_800EB184[0];
        LocalRecord *second = D_8009B2E4;
        u16 second_flags;

        first->flags_08 |= DISPLAY_OBJECT_FLAG_RENDERABLE;

        second_flags = second->flags_08;
        D_8009B2EB = 0;
        second_flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        second->flags_08 = second_flags;
    } else {
        s32 value = Rand_GetInterval(4);
        func_8003B378(D_800EB010, value);
    }
}
