#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "display_asset_banks.h"
#include "../unmatched.h"
#ifdef VERSION_EUROPE
#include "high_memory_addresses.h"
#endif

/* The two boot transfer stage callbacks. Main_RunBootSequence queues the boot
   image with Main_LoadBootImageStage (declared as func_800434F4 in
   file_transfer.h) and the boot package with Main_LoadBootPackageStage. */

#if (!defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MAIN_LOAD_BOOT_PACKAGE_STAGE)) && \
    (!defined(VERSION_EUROPE) || defined(VERSION_EUROPE_MAIN_LOAD_BOOT_PACKAGE_STAGE))
void Main_LoadBootPackageStage(FileTransferDescriptor *obj, s32 stage) {
#ifdef VERSION_JAPAN
    /* The Japanese boot package is transferred in three stages rather than
       four: a 32-sector first stage at 0x2C0, and a last stage that uploads
       the image and then points at D_800101D8, folding the US stages 2
       and 3 together. */
    switch (stage) {
    case 0: {
        u32 m;
        s32 w;

        m = 0xFFDDFFFF;
        obj->field_30.h.counter = 0x2C0;
        w = 0x40;
        obj->field_30.h.field_32 = 0;
        obj->w = w;
        D_8009B0F4 &= m;
        m = 0x10000;
        obj->phase_size = m;
        D_8009B0F4 |= m;
        obj->done = 2;
        obj->h = 0x10;
        obj->value_08 = D_8009B118;
        obj->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;
    }

    case 1:
        obj->phase_size = 2 * FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->value_0C = D_8009B118;
        obj->value_08 = D_8009B118;
        obj->done = 1;
        break;

    case 2:
        obj->x = 0x200;
        obj->y = 0xF8;
        obj->w = 0x100;
        obj->h = 8;
        LoadImage2((RECT *)obj, (u32 *G32)D_8009B118);
        obj->phase_size = 3 * FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->value_0C = (s32)D_800101D8;
        obj->value_08 = (s32)D_800101D8;
        obj->done = 1;
        break;
    }
#elif defined(VERSION_EUROPE)
    /* The European boot package takes eight stages: three header stages
       (0x280, 0x2C0 and 0x300 in the counter), the US image stages with the
       image at x 0x280, and three last stages that land in the European
       text banks. */
    switch (stage) {
    case 0: {
        u32 cleared;
        u32 loaded;
        u32 enabled;

        obj->field_30.h.counter = 0x280;
        obj->w = 0x40;
        obj->h = 0x10;
        cleared = D_8009B0F4 & 0xFFDDFFFF;
        D_8009B0F4 = cleared;
        do { loaded = D_8009B0F4; } while (0);
        do { enabled = 0x10000; } while (0);
        obj->field_30.h.field_32 = 0;
        D_8009B0F4 = loaded | enabled;
        obj->done = 2;
        obj->phase_size = 12 * FILE_SECTOR_SIZE;
        obj->value_08 = D_8009B118;
        obj->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;
    }

    case 1: {
        u32 cleared;
        u32 loaded;
        u32 enabled;

        obj->field_30.h.counter = 0x2C0;
        obj->w = 0x40;
        obj->h = 0x10;
        cleared = D_8009B0F4 & 0xFFDDFFFF;
        D_8009B0F4 = cleared;
        do { loaded = D_8009B0F4; } while (0);
        do { enabled = 0x10000; } while (0);
        obj->field_30.h.field_32 = 0;
        D_8009B0F4 = loaded | enabled;
        obj->done = 2;
        obj->phase_size = 13 * FILE_SECTOR_SIZE;
        obj->value_08 = D_8009B118;
        obj->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;
    }

    case 2: {
        u32 cleared;
        u32 loaded;
        u32 enabled;

        obj->field_30.h.counter = 0x300;
        obj->w = 0x40;
        obj->h = 0x10;
        cleared = D_8009B0F4 & 0xFFDDFFFF;
        D_8009B0F4 = cleared;
        do { loaded = D_8009B0F4; } while (0);
        do { enabled = 0x10000; } while (0);
        obj->field_30.h.field_32 = 0;
        D_8009B0F4 = loaded | enabled;
        obj->done = 2;
        obj->phase_size = 13 * FILE_SECTOR_SIZE;
        obj->value_08 = D_8009B118;
        obj->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;
    }

    case 3:
        obj->phase_size = 2 * FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->value_0C = D_8009B118;
        obj->value_08 = D_8009B118;
        obj->done = 1;
        break;

    case 4:
        obj->x = 0x280;
        obj->y = 0xF8;
        obj->w = 0x100;
        obj->h = 8;
        LoadImage2((RECT *)obj, (u32 *G32)D_8009B118);
        obj->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->value_0C = D_8009B118 + 2 * FILE_SECTOR_SIZE;
        obj->value_08 = D_8009B118 + 2 * FILE_SECTOR_SIZE;
        obj->done = 1;
        break;

    case 5:
        obj->x = 0x280;
        obj->y = 0xC0;
        obj->w = 0x10;
        obj->h = 8;
        LoadImage2((RECT *)obj, (u32 *G32)(D_8009B118 + 2 * FILE_SECTOR_SIZE));
        obj->phase_size = 30 * FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->value_0C = (s32)D_800101E0;
        obj->value_08 = (s32)D_800101E0;
        obj->done = 1;
        break;

    case 6:
        obj->phase_size = 32 * FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->value_0C = (s32)D_800101E4;
        obj->value_08 = (s32)D_800101E4;
        obj->done = 1;
        break;

    case 7:
        obj->phase_size = 7 * FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->value_0C = (s32)D_800101E8;
        obj->value_08 = (s32)D_800101E8;
        obj->done = 1;
        break;
    }
#else
    switch (stage) {
    case 0:
        obj->field_30.h.counter = 0x280;
        obj->field_30.h.field_32 = 0;
        obj->w = 0x40;
        obj->h = 0x10;
        D_8009B0F4 &= 0xFFDDFFFF;
        obj->phase_size = 48 * FILE_SECTOR_SIZE;
        D_8009B0F4 |= 0x10000;
        obj->done = 2;
        obj->value_08 = D_8009B118;
        obj->value_0C = D_8009B118 + FILE_SECTOR_SIZE;
        break;

    case 1:
        obj->phase_size = 2 * FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->value_0C = D_8009B118;
        obj->value_08 = D_8009B118;
        obj->done = 1;
        break;

    case 2:
        obj->x = 0x200;
        obj->y = 0xF8;
        obj->w = 0x100;
        obj->h = 8;
        LoadImage2((RECT *)obj, (u32 *G32)D_8009B118);
        obj->phase_size = FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->value_0C = D_8009B118 + 2 * FILE_SECTOR_SIZE;
        obj->value_08 = D_8009B118 + 2 * FILE_SECTOR_SIZE;
        obj->done = 1;
        break;

    case 3:
        obj->x = 0x280;
        obj->y = 0xE8;
        obj->w = 0x10;
        obj->h = 8;
        LoadImage2((RECT *)obj, (u32 *G32)(D_8009B118 + 2 * FILE_SECTOR_SIZE));
        obj->phase_size = 3 * FILE_SECTOR_SIZE;
        D_8009B0F4 &= 0xFFDCFFFF;
        obj->value_0C = (s32)D_800101D8;
        obj->value_08 = (s32)D_800101D8;
        obj->done = 1;
        break;
    }
#endif
}
#endif

#if (!defined(VERSION_JAPAN) || defined(VERSION_JAPAN_MAIN_LOAD_BOOT_IMAGE_STAGE)) && \
    (!defined(VERSION_EUROPE) || defined(VERSION_EUROPE_MAIN_LOAD_BOOT_IMAGE_STAGE))
#ifdef VERSION_EUROPE
/* The European boot image takes four stages: the header stage counts 0xC0,
   the image goes to (0x280, 0xE0), and a fourth stage reads three sectors
   to D_800101D8. */
void Main_LoadBootImageStage(FileTransferDescriptor *p, s32 mode)
{
    s32 one;
    s32 v;
    s32 w;
    s32 x;
    s32 m;
    s32 d;
    s32 f;

    one = 1;

    if (mode == one) {
        goto m1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto m0;
        }
        return;
    }
    if (mode == 2) {
        goto m2;
    }
    if (mode == 3) {
        goto m3;
    }
    return;

m0:
    m = 0xFFDDFFFF;
    p->field_30.h.counter = 0xC0;
    p->field_30.h.field_32 = 0x100;
    v = D_8009B0F4;
    f = 0x40;
    p->w = f;
    D_8009B0F4 = v & m;
    w = D_8009B0F4;
    m = 0x10000;
    do {
        p->phase_size = m;
        D_8009B0F4 = w | m;
        p->done = 2;
        d = D_8009B118;
        p->h = 0x10;
    } while (0);
    p->value_08 = d;
    p->value_0C = d + FILE_SECTOR_SIZE;
    return;

m1:
    p->phase_size = FILE_SECTOR_SIZE;
    D_8009B0F4 = D_8009B0F4 & 0xFFDCFFFF;
    p->value_0C = D_8009B118;
    p->value_08 = D_8009B118;
    goto tail;

m2:
    p->x = 0x280;
    p->y = 0xE0;
    p->w = 0x100;
    p->h = 4;
    LoadImage2((RECT *)p, (u32 *G32)D_8009B118);
    p->value_0C = (u32)D_801AF000;
    p->value_08 = (u32)D_801AF000;
    x = D_8009B0F4;
    p->phase_size = 0x800;
    D_8009B0F4 = x & 0xFFDCFFFF;
    p->done = one;
    return;

m3:
    p->phase_size = 3 * FILE_SECTOR_SIZE;
    D_8009B0F4 = D_8009B0F4 & 0xFFDCFFFF;
    p->value_0C = (u32)D_800101D8;
    p->value_08 = (u32)D_800101D8;

tail:
    p->done = one;
}
#else
void Main_LoadBootImageStage(FileTransferDescriptor *p, s32 mode)
{
    s32 one;
    s32 v;
    s32 w;
    s32 e;
    s32 x;
    s32 m;
    s32 d;
    s32 f;
    s32 hun;

    one = 1;

    if (mode == one) {
        goto m1;
    }
    if (mode < 2) {
        if (mode == 0) {
            goto m0;
        }
        return;
    }
    if (mode == 2) {
        goto m2;
    }
    return;

m0:
    m = 0xFFDDFFFF;
    p->field_30.h.field_32 = 0x100;
    v = D_8009B0F4;
    f = 0x40;
    do {
        p->field_30.h.counter = 0;
    } while (0);
    p->w = f;
    D_8009B0F4 = v & m;
    w = D_8009B0F4;
    m = 0x10000;
    do {
        p->phase_size = m;
        D_8009B0F4 = w | m;
        p->done = 2;
        d = D_8009B118;
        p->h = 0x10;
    } while (0);
    p->value_08 = d;
    p->value_0C = d + FILE_SECTOR_SIZE;
    return;

m1:
    p->phase_size = FILE_SECTOR_SIZE;
    D_8009B0F4 = D_8009B0F4 & 0xFFDCFFFF;
    p->value_0C = D_8009B118;
    p->value_08 = D_8009B118;
    goto tail;

m2:
    do {
        hun = 0x100;
    } while (0);
    /* The RECT halves are stored through the members' addresses as u16.
     * A plain member store, or an s16 cast that fold turns back into one,
     * is an in-struct reference and is reordered against the D_8009B118
     * load; retail keeps these in source order around LoadImage2. */
    *(u16 *)&p->y = 0xF0;
    e = D_8009B118;
    *(u16 *)&p->x = hun;
    *(u16 *)&p->w = hun;
    *(u16 *)&p->h = 4;
    LoadImage2((RECT *)p, (u32 *G32)e);
    p->value_0C = (u32)D_801AF000;
    p->value_08 = (u32)D_801AF000;
    x = D_8009B0F4;
    p->phase_size = 0x800;
    D_8009B0F4 = x & 0xFFDCFFFF;

tail:
    p->done = one;
}
#endif
#endif
