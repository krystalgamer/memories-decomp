/*
 * Model_PrepareSharedPrimitiveCaches (0x80067220-0x80067354).
 *
 * Projects a shared HMD block's vertices into the vertop2 cache (SXY2 plus
 * an IR0 << 16 | SZ3 word, or all ones when RTPS overflowed) and, in lit
 * modes, lights its shared normals into the nortop2 colour cache.
 *
 * The original was written by hand: no stack frame, the argument read from
 * $a0 and the next block returned in $v0, and both loops load the next
 * vector while the GTE works on the current one. This is plain C over the
 * stock Psy-Q inline_c.h macros, built with the gcc_2_8_1_o2_massaged
 * profile: GCC's -O2 output is rewritten into the original instructions by
 * tools/project/asm_massage.py with the edit file in
 * shared_primitive_cache_prepare.massage/. See notes/asm-massage.md and
 * notes/shared-primitive-renderers.md.
 */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/inline_c.h"
#include "model.h"
#include "model_handler_state.h"
#include "primitive_handler_modes.h"
#include "shared_primitive_renderers.h"

u32 *Model_PrepareSharedPrimitiveCaches(GsARGUNIT_SHARED *arg)
{
    ModelSharedCacheBlock *block = (ModelSharedCacheBlock *)arg->primp;
    u32 rgb = gModel_dwPolyGt3Template & 0xFFFFFF;
    s8 mode = (s8)gModel_bCurrentDrawMode;
    /* The colour cache holds two words per normal; a nonzero cache base
     * selects the second. */
    u32 offset = gModel_dwLitColorCacheBase;
    u32 count;

    gte_ldrgb(&rgb);
    if (offset != 0) {
        offset = sizeof(u32);
    }
    count = block->vertex_count;
    if (count != 0) {
        SVECTOR *vertex = &arg->vertop[block->vertex_index];
        ModelSharedCacheEntry *entry =
            (ModelSharedCacheEntry *)&arg->vertop2[block->vertex_cache_index];

        do {
            gte_ldv0(vertex);
            count--;
            vertex++;
            gte_rtps_b();
            /* Rejected until the projection is accepted. */
            entry->words[1] = 0xFFFFFFFF;
            if (mode != 1) {
                s32 flag;

                gte_stflg(&flag);
                if (flag < 0) {
                    goto next;
                }
            }
            {
                u32 p, sz;

                gte_stsxy2(&entry->words[0]);
                gte_stdp(&p);
                gte_stsz(&sz);
                entry->words[1] = p << 16 | sz;
            }
        next:
            entry++;
        } while (count != 0);
    }
    if (mode != 0) {
        count = block->normal_count;
        if (count != 0) {
            SVECTOR *normal = &arg->nortop[block->normal_index];
            u32 *colour = (u32 *)((u8 *)&arg->nortop2[block->colour_cache_index] + offset);

            do {
                gte_ldv0(normal);
                count--;
                normal++;
                gte_nccs_b();
                gte_strgb(colour);
                colour += 2;
            } while (count != 0);
        }
    }
    return (u32 *)(block + 1);
}
