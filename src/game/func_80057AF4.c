#include "../types.h"

typedef struct {
    u16 length;
    u8 pad02[0x74];
} AnimEntry;

typedef struct {
    u8 pad000[0x1E0];
    u8 *parts[58];
    u16 map[11][58];
    AnimEntry entries[9];
    u8 padBEA[2];
    u8 bits[8];
    u8 queued;
    u8 anim;
    u8 played;
    u8 padBF7[0x1E1];
    u16 *src_table;
    u8 *base;
    u8 *dst;
    u8 padDE4[0x14];
    u16 field_DF8;
    u8 padDFA[4];
    u8 field_DFE;
    u8 padDFF[0xF];
    u8 mode;
    u8 current;
    u8 pending;
    u8 padE11[0xA];
    u8 count;
    u8 padE1C[3];
    u8 active;
} AnimSlot;

extern AnimSlot D_800F2C40[];

extern void func_8005B620(u8 *, u8 *, s32);
extern void func_800597C8(s32, s32, s32);
void func_80057AF4(s32, s32, s32);

void func_80057AF4(s32 index, s32 anim, s32 flag) {
    AnimSlot *m;
    u8 **parts;
    u8 *dst;
    u8 *base;
    u16 *src;
    s32 i;
    s32 prev;
    /* Three allocation pins. Global allocation ranks the word count (16
       references over 38 instructions) above the slot pointer (44 over 153),
       so unpinned the count takes $s1 and the slot pointer $s2, and every
       later choice shifts with them; retail has the pointer in $s1 and the
       count sharing $s2 with the index. Pinning the count restores that and
       the map entry and the sign-adjust temporary then follow it back to $v1
       and $v0. All three are load-bearing: releasing them costs 53, 3 and 16
       differing words respectively. */
    register s32 entry __asm__("$3");
    register s32 count __asm__("$18");
    register s32 t __asm__("$2");

    m = &D_800F2C40[index];
    dst = m->dst;
    base = m->base;
    parts = m->parts;
    if (m->active == 0) {
        return;
    }
    switch (anim) {
    case -1:
        m->pending = 0;
        for (i = 0; i < m->count; parts++, i++) {
            *(u16 *)(*parts + 8) = 0xFFFF;
        }
        break;
    case 0:
        if (m->current == 0) {
            return;
        }
        for (i = 0; i < m->count; i++) {
            entry = m->map[m->current][i];
            src = (u16 *)((u8 *)m->src_table + entry * 4);
            if (entry != 0xFFFF) {
                t = i;
                if (i < 0) {
                    t = i + 7;
                }
                count = 3;
                t >>= count;
                if ((m->bits[t] >> (i - (t << count))) & 1) {
                    count = 5;
                }
                func_8005B620(base + src[0] * 4, dst, count);
                t = count * 4;
                dst += t;
            }
        }
        m->current = 0;
        break;
    default:
        prev = m->current;
        if (anim != m->field_DFE + 3) {
            m->mode = 2;
        }
        if (m->entries[anim].length == 0) {
            return;
        }
        if (prev != 0) {
            if (flag == 0) {
                return;
            }
            m->played = m->queued;
            func_80057AF4(index, 0, 0);
        }
        if (anim == m->field_DFE + 3 || m->field_DF8 == 0x309) {
            m->mode = 7;
        }
        m->current = anim;
        if (prev == anim) {
            func_800597C8(index, 0, 0);
            return;
        }
        m->pending = 1;
        for (i = 0; i < m->count; parts++, i++) {
            entry = m->map[m->current][i];
            src = (u16 *)((u8 *)m->src_table + entry * 4);
            if (entry != 0xFFFF) {
                t = i;
                if (i < 0) {
                    t = i + 7;
                }
                count = 3;
                t >>= count;
                if ((m->bits[t] >> (i - (t << count))) & 1) {
                    count = 5;
                }
                *(u16 *)(*parts + 8) = ((u8 *)src - (u8 *)m->src_table) >> 2;
                func_8005B620(dst, base + src[0] * 4, count);
                t = count * 4;
                dst += t;
            }
        }
        break;
    }
}
