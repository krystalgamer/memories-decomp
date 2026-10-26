#include "../types.h"
#include "model_texture_upload.h"
#include "model.h"
#include "model_set_slot_shadow_enabled.h"

void ModelTexture_UploadTimFile(char *path, int b, int c, int d, int e, int f, int g)
{
    /* The 32-byte retail scratch is one word larger than GsIMAGE. */
    s32 local[8];

    ModelTexture_LoadTim((GsIMAGE *)local, path, b, c, d, e, f, g);
}

/* Copies the image header onto the stack before forwarding it, so the
   callee can't alias the caller's copy. */
u32 ModelTexture_PackPageClutCopy(s32 a0, s32 a1, GsIMAGE *src) {
    GsIMAGE buf = *src;
    return ModelTexture_PackPageClut(a0, a1, &buf);
}

int Model_SetSlotShadowEnabled(int index, int value)
{
    register ModelSlot *entry;
    register int old;

    {
        register ModelSlot *base = gModel_aSlots;

        entry = &((ModelSlot *)0)[index];

        entry = (ModelSlot *)((unsigned int)entry + (unsigned int)base);
    }

    old = entry->shadow_enabled;
    if (value >= 0) {
        entry->shadow_enabled = value;
    }
    return old;
}
