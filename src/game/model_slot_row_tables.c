#include "../types.h"
#include "model.h"
#include "func_8004D914.h"
#include "model_slot_row_tables.h"

#define MODEL_CHANNEL_HALFWORD(cursor) (*(u16 *)(cursor))

/* One model slot's animation channel and row-table helpers: the channel
   decoder, the reset that imports the command list, and the walk that
   consumes it. The three form the complete gcc_2_8_1_g0 run below
   func_8004D914.

   The model loader resets the tables before sending each event through the
   decoder; slot setup later walks the imported list. The reset fills keys
   with 0xFFFF, zeroes rows and maxima, and stores the command list at 0xDD8;
   the walk claims those keys, accumulates rows, and reads the same list. The
   byte cursors reach the part bitfield through ModelSlot::field_BEC. */

/* Channel decoder: walks the header's record count through records of the
   kind *kind selects. For slot half `mode` it rebases two halfwords in each
   record, keeps *best as the running maximum of the record's index
   halfwords, and adds the kind's step to *total; it returns the count, or 0
   for an unknown kind. Every selector arm is written out in both switches,
   the two pairs with equal strides included, and carries its own final
   compare; cross-jumping folds them into retail's shared tails. */
/* Europe also supports mode 2's packed-channel conversion. Its separate
   translation unit keeps explicit maximum and secondary cursors rather
   than applying loop strength reduction. */
#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_MODEL_SLOT_DECODER)
s32 func_8004D134(s32 mode, u16 *kind, u8 *ctx, s32 *best, s32 *total)
{
    u8 *hdr;
    u8 *rec;
    u8 *base;
    u8 *p;
    u8 *q;
    s32 count;
    s32 n;
    s32 off1;
    s32 off2;
    s32 stride;
    s32 step;
    s32 add1;
    s32 add2;
    s32 f;
#ifdef VERSION_EUROPE
    s32 v;
#else
    u32 v;
#endif
    u32 adjusted;
    /* x and y carry the second rebase's row and sum as well as the running
       maximum and its candidate; one pseudo each keeps them in $a0/$a2. */
    s32 x;
#ifndef VERSION_EUROPE
    s32 y;
#endif
    u32 k;

#ifdef VERSION_EUROPE
#define MODEL_SECOND_OFFSET v
#else
#define MODEL_SECOND_OFFSET off2
#endif
    hdr = *(u8 *G32 *)ctx;
    base = *(u8 *G32 *)(ctx + 0x14);
    count = *(u16 *)(hdr + 2);
    rec = base + *(s32 *)(hdr + 4) * 4;
    n = count;
#ifdef VERSION_EUROPE
    if ((u32)mode >= 3) {
#else
    if ((u32)mode >= 2) {
#endif
        return 0;
    }
#ifdef VERSION_EUROPE
    off2 = *kind;
    switch ((u32)off2) {
#else
    k = *kind;
    switch (k) {
#endif
    case 9:
        off1 = 6;
        MODEL_SECOND_OFFSET = 2;
        stride = 0x14;
        step = 0x20;
        break;
    case 0x209:
        off1 = 0xA;
        MODEL_SECOND_OFFSET = 6;
        stride = 0x18;
        step = 0x20;
        break;
    case 0xD:
        off1 = 6;
        MODEL_SECOND_OFFSET = 2;
        stride = 0x18;
        step = 0x28;
        break;
    case 0x20D:
        off1 = 0xA;
        MODEL_SECOND_OFFSET = 6;
        stride = 0x1C;
        step = 0x28;
        break;
    case 0x11:
        off1 = 6;
        MODEL_SECOND_OFFSET = 2;
        stride = 0x18;
        step = 0x28;
        break;
    case 0x211:
        off1 = 0xA;
        MODEL_SECOND_OFFSET = 6;
        stride = 0x1C;
        step = 0x28;
        break;
    case 0x15:
        off1 = 6;
        MODEL_SECOND_OFFSET = 2;
        stride = 0x1C;
        step = 0x34;
        break;
    case 0x215:
        off1 = 0xA;
        MODEL_SECOND_OFFSET = 6;
        stride = 0x20;
        step = 0x34;
        break;
    default:
        return 0;
    }

    if (--n != -1) {
        add1 = (mode << 2) - 0xA;
        add2 = (mode << 4) + 0x3BD8;
#ifdef VERSION_EUROPE
        p = rec + 0x1C;
        q = rec + MODEL_SECOND_OFFSET;
#endif
        do {
#ifdef VERSION_EUROPE
            ctx = rec + off1;
            off2 = MODEL_CHANNEL_HALFWORD(ctx);
            v = ((u32)off2 >> 7) & 3;
            if (mode < 2) {
                off2 += add1;
                MODEL_CHANNEL_HALFWORD(ctx) = off2;
                if (v >= 3) {
                    MODEL_CHANNEL_HALFWORD(ctx) = off2 & 0xFF7F;
                }
                if (v < 2) {
                    u32 packed;

                    v = MODEL_CHANNEL_HALFWORD(q);
                    off2 = ((u32)v >> 6) & 0xF;
                    off2 += 0xD0;
                    off2 += mode << 4;
                    adjusted = off2;
                    packed = adjusted << 6;
                    v &= 0x3F;
                    packed |= v;
                    MODEL_CHANNEL_HALFWORD(q) = packed;
                }
            } else if (v < 2) {
                v = MODEL_CHANNEL_HALFWORD(q);
                off2 = (u32)v >> 6;
                if (off2 < 0x100) {
                    v = (v & 0x3F) << 4;
                    adjusted = off2 & 0xF;
                    off2 = adjusted + 0xD0;
                    if (v >= 0x200) {
                        off2 = adjusted + 0xF0;
                    }
                    v = (v & 0xFF) + 0x280;
                    {
                        /* Keep the packed result distinct from the index. */
                        s32 result = off2 << 6;

                        x = v >> 4;
                        result |= x & 0x3F;
                        MODEL_CHANNEL_HALFWORD(q) = result;
                    }
                }
            }
#else
            if (mode < 2) {
                p = rec + off1;
                v = MODEL_CHANNEL_HALFWORD(p);
                adjusted = v + add1;
                MODEL_CHANNEL_HALFWORD(p) = adjusted;
                f = (v >> 7) & 3;
                if (f >= 3) {
                    MODEL_CHANNEL_HALFWORD(p) = adjusted & 0xFF7F;
                }
                if (f < 2) {
                    q = rec + off2;
                    v = MODEL_CHANNEL_HALFWORD(q);
                    y = v + add2;
                    MODEL_CHANNEL_HALFWORD(q) = y;
                    x = v >> 6;
                    if (x >= 0x10) {
                        y = (y & 0x3F) + 0x10;
                        MODEL_CHANNEL_HALFWORD(q) = y;
                        MODEL_CHANNEL_HALFWORD(q) =
                            y | ((x % 0x10) << 6);
                    }
                }
            }
#endif
            if (best != 0) {
#ifdef VERSION_EUROPE
#define MODEL_CURRENT_MAX off2
#define MODEL_MAX_CANDIDATE v
#else
#define MODEL_CURRENT_MAX x
#define MODEL_MAX_CANDIDATE y
#endif
#ifdef VERSION_EUROPE
#define MODEL_MAX_VALUE(offset) (*(u16 *)(p + (offset) - 0x1C))
#else
#define MODEL_MAX_VALUE(offset) (*(u16 *)(rec + (offset)))
#endif
#ifdef VERSION_EUROPE
                off2 = *kind;
                switch ((u32)off2) {
#else
                k = *kind;
                switch (k) {
#endif
                case 9:
                    MODEL_CURRENT_MAX = *best;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0xC);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *best = MODEL_CURRENT_MAX;
                    break;
                case 0x209:
                    MODEL_CURRENT_MAX = *best;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x10);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *best = MODEL_CURRENT_MAX;
                    break;
                case 0xD:
                    MODEL_CURRENT_MAX = *best;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0xC);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *best = MODEL_CURRENT_MAX;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x10);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                        *best = MODEL_MAX_CANDIDATE;
                    }
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x14);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *best = MODEL_CURRENT_MAX;
                    break;
                case 0x20D:
                    MODEL_CURRENT_MAX = *best;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x10);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *best = MODEL_CURRENT_MAX;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x14);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                        *best = MODEL_MAX_CANDIDATE;
                    }
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x18);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *best = MODEL_CURRENT_MAX;
                    break;
                case 0x11:
                    MODEL_CURRENT_MAX = *best;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0xE);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *best = MODEL_CURRENT_MAX;
                    break;
                case 0x211:
                    MODEL_CURRENT_MAX = *best;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x12);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *best = MODEL_CURRENT_MAX;
                    break;
                case 0x15:
                    MODEL_CURRENT_MAX = *best;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0xA);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *best = MODEL_CURRENT_MAX;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x10);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                        *best = MODEL_MAX_CANDIDATE;
                    }
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x14);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *best = MODEL_CURRENT_MAX;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x18);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *best = MODEL_CURRENT_MAX;
                    break;
                case 0x215:
                    MODEL_CURRENT_MAX = *best;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0xE);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *best = MODEL_CURRENT_MAX;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x14);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                        *best = MODEL_MAX_CANDIDATE;
                    }
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x18);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *best = MODEL_CURRENT_MAX;
                    MODEL_MAX_CANDIDATE = MODEL_MAX_VALUE(0x1C);
                    if (MODEL_CURRENT_MAX < MODEL_MAX_CANDIDATE) {
                        MODEL_CURRENT_MAX = MODEL_MAX_CANDIDATE;
                    }
                    *best = MODEL_CURRENT_MAX;
                    break;
                }
#undef MODEL_MAX_CANDIDATE
#undef MODEL_CURRENT_MAX
#undef MODEL_MAX_VALUE
            }
#ifdef VERSION_EUROPE
            p = p + stride;
            q += stride;
#endif
            rec = rec + stride;
            *total = *total + step;
        } while (--n != -1);
    }
#undef MODEL_SECOND_OFFSET
    return count;
}
#endif

#ifndef MODEL_SLOT_DECODER_ONLY
void func_8004D58C(s32 arg0, u8 *arg1)
{
    ModelSlot *ch;
    u8 *t;
    u8 *c;
    u8 *q;
    u8 *e;
    u8 *p3;
    u8 *p2;
    u8 *g;
    u8 *k;
    u8 *s;
    u8 *u;
    u8 *v;
    s32 ff;
    s32 one;
    s32 i;
    s32 j;
    s32 n;
    s32 m;
    s32 a;
    s32 b;
    s32 w;
    s32 d;
    s32 x;
    s32 y;
    s32 flag;
    s32 qd;

    do {
        do {
            p3 = (u8 *)0;
        } while (0);
    } while (0);
    p2 = (u8 *)0;
    i = 0;
    ff = 0xFFFF;
    n = 0;
    m = 0;
    ch = &D_800F2C40[arg0];
    t = (u8 *)ch;
    c = t;
    ch->field_E06 = 0;
    ch->field_E08 = 0;
    ch->field_DD8 = 0;
    *(s32 *)&ch->field_DDC = 0;
    *(s32 *)&ch->field_DE0 = 0;
    *(s32 *)&ch->field_DE4 = 0;
    ch->field_DF0 = 0;
    /* The reset walks keys and rows with running byte offsets rather than
       indexing ch->field_2C8[i][j] / ch->field_750[i].values[j]. That is not
       a missed cleanup: the indexed form is larger, and the link fails with
       .initialized_data overlapping .text. The walkers are load-bearing.

       The margin is one instruction, which is worth knowing before touching
       anything else here. These three pointer members cost nothing to name:
       they are stored as zero, so the *(s32 *)& that keeps the store a
       non-struct reference is free. The four copies out of the command
       blocks at the end are not -- direct member stores push .text into
       .initialized_data. Their byte-cursor form remains, with each
       displacement derived from the corresponding member instead. */
    do {
        ((ModelSlotRow *)(c + (u32)&((ModelSlot *)0)->field_750))->max = 0;
        j = 0;
        a = n;
        b = m;
        do {
            u = t + a;
            a += 2;
            v = t + b;
            b += 2;
            j++;
            *(u16 *)(v + (u32)&((ModelSlot *)0)->field_2C8) = ff;
            *(s16 *)(u + (u32)&((ModelSlot *)0)->field_750) = 0;
        } while (j < 0x3A);
        n += sizeof(ModelSlotRow);
        m += 0x74;
        i++;
        c += sizeof(ModelSlotRow);
    } while (i < 0xA);
    i = 7;
    q = t + i;
    do {
        q[(u32)&((ModelSlot *)0)->field_BEC] = 0;
        i--;
        q--;
    } while (i >= 0);
    e = *(u8 *G32 *)(arg1 + 0x10);
    if (e == (u8 *)0) {
        return;
    }
    do {
        if (*(s32 *)(e + 8) != 0) {
            w = e[0xF];
            if (w == 3) {
                p3 = *(u8 *G32 *)(e + 4);
            }
            if (w == 2) {
                p2 = *(u8 *G32 *)(e + 4);
            }
        }
        e = *(u8 *G32 *)e;
    } while (e != (u8 *G32)-1);
    if (p3 != (u8 *)0) {
        p3 += 8;
        k = *(u8 *G32 *)p3;
        p3 += 4;
        i = 0;
        if (*(u16 *)k != 0) {
            one = 1;
            g = k;
            do {
                do {
                    do {
                        qd = i / 8;
                    } while (0);
                } while (0);
                s = t + qd;
                d = qd << 3;
                y = s[(u32)&((ModelSlot *)0)->field_BEC];
                flag = *(volatile s32 *)(g + 4) & 0x100;
                if (flag != 0) {
                    x = y | (one << (i - d));
                } else {
                    x = y;
                }
                s[(u32)&((ModelSlot *)0)->field_BEC] = x;
                g += 4;
                i++;
            } while ((u32)i < *(u16 *)k);
        }
        *(s32 *)(t + (u32)&((ModelSlot *)0)->field_DD8) = *(s32 *)p3;
        *(s32 *)(t + (u32)&((ModelSlot *)0)->field_DDC) = *(s32 *)(p3 + 4);
    }
    if (p2 != (u8 *)0) {
        p2 += 4;
        *(s32 *)(t + (u32)&((ModelSlot *)0)->field_DE0) = *(s32 *)p2;
        *(s32 *)(t + (u32)&((ModelSlot *)0)->field_DE4) = *(s32 *)(p2 + 4);
    }
}

void func_8004D75C(s32 index)
{
    ModelSlot *ch;
    ModelSlotPart *slot;
    s32 *cmd;
    u32 word;
    s32 row;
    s32 i;
    s32 key;

    ch = &D_800F2C40[index];
    if (ch->field_DD8 == 0) {
        return;
    }
    i = 0;
    if (i < ch->field_E1B) {
        for (; i < ch->field_E1B; i++) {
            slot = ch->field_1E0[i];
            if (slot == 0) {
                break;
            }
            row = 1;
            slot->start_sid = row;
            key = ch->field_1E0[i]->start;
            cmd = &ch->field_DD8[key];
            ch->field_2C8[row][i] = key;
            while (1) {
                word = *cmd;
                if ((s32)word < 0) {
                    row = word >> 16;
                    row = row & 0x7F;
                    if (row == 0) {
                        break;
                    }
                    if (ch->field_2C8[row][i] != 0xFFFF) {
                        cmd++;
                    } else {
                        ch->field_2C8[row][i] = *(u16 *)cmd;
                        cmd = &ch->field_DD8[*(u16 *)cmd];
                    }
                } else {
                    ch->field_750[row].values[i] =
                        ch->field_750[row].values[i] + *((u8 *)cmd + 2);
                    cmd++;
                }
            }
        }
    }
    for (row = 1; row < MODEL_SLOT_ROW_COUNT; row++) {
        ch->field_750[row].max = 0;
        for (i = 0; i < ch->field_E1B; i++) {
            if (ch->field_750[row].max < ch->field_750[row].values[i]) {
                ch->field_750[row].max = ch->field_750[row].values[i];
            }
        }
    }
}

/* The relink pass between the two above. Every reach now goes through
   ModelSlot: the command list is ch->field_DD8, the channel count is
   ch->field_E1B, and row 1's key for the current channel is
   ch->field_2C8[1][i].

   The two key reads inside the loops keep a byte-offset sum instead of
   ch->field_2C8[j][i]. Their address has to stay `ch + <running offset> +
   MODEL_SLOT_ROW_KEY_TABLE_OFFSET`, because that way the channel offset and
   the row stride share one induction variable; indexing the member gives the
   compiler a second one, which costs a callee-saved register and two
   instructions. Written left to right so the base is the first addend, which
   is also what the retail sum does. */
void func_8004D914(s32 arg0)
{
    ModelSlot *ch;
    s32 *a;
    s32 *c;
    s32 *t;
    s32 *g;
    s32 i;
    s32 j;
    s32 o;
    s32 w;
    s32 y;
    s32 x;
    u16 *yp;
    s32 k;
    s32 v;
    s32 hi;
    s32 one;
    s32 ff;
    s32 mask;
    s32 bit;

    ch = &D_800F2C40[arg0];
    o = 0;
    if (ch->field_DD8 == 0) {
        return;
    }
    i = 0;
    if (ch->field_E1B == 0) {
        return;
    }

    ff = 0xFFFF;
    mask = 0xFF80FFFF;
    bit = 0x10000;
    one = 1;
    o = i;

    do {
        j = 1;
        k = o + sizeof(ch->field_2C8[0]);
        g = &ch->field_DD8[ch->field_2C8[1][i]];

        do {
            w = *(u16 *)((u8 *)ch + k + MODEL_SLOT_ROW_KEY_TABLE_OFFSET);
            a = &ch->field_DD8[w];
            if (w != ff) {
                t = a - 1;
                while (1) {
                    x = *a;
                    if (x < 0) {
                        hi = (u32)x >> 16;
                        hi = hi & 0x7F;
                        yp = (u16 *)((u8 *)ch
                            + (o + (u32)&((u16 (*)[MODEL_SLOT_PART_COUNT])0)[hi])
                            + MODEL_SLOT_ROW_KEY_TABLE_OFFSET);
                        y = *yp;
                        c = &ch->field_DD8[y];
                        if (hi == 0) {
                            goto zero;
                        }
                        *a = (x & 0xC07FFFFF)
                            | ((((u32)x >> 16) & 0x7F) << 23);
                        if (y != ff) {
                            if (hi >= 2) {
                                if (c != (s32 *)0) {
                                    do {
                                        v = *c;
                                        if (v < 0) {
                                            if ((((u32)v >> 16) & 0x7F)
                                                == hi) {
                                                *c = (v & mask) | bit;
                                            }
                                            if ((*(u16 *)((u8 *)c + 2)
                                                & 0x7F) == one) {
                                                goto hit;
                                            }
                                        }
                                        c++;
                                    } while (c != (s32 *)0);
                                }
                            }
                        }
                    }
cont:
                    a++;
                }
hit:
                *(s16 *)c = g - ch->field_DD8;
                *(s16 *)a = c - ch->field_DD8;
                goto cont;
zero:
                /* Reloaded rather than reusing the base the links above
                   already hold: retail reads the member again here. */
                *(s16 *)t = (a - (s32 *G32)*(volatile s32 *)&ch->field_DD8) - 1;
                *(s16 *)a = (t - ch->field_DD8) + 1;
            }
            j++;
            k += sizeof(ch->field_2C8[0]);
        } while (j < MODEL_SLOT_ROW_COUNT);

        o += 2;
        i++;
    } while (i < ch->field_E1B);
}
#endif
