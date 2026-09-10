/*
 * Runs the complete eleven-state per-player model dispatcher. Current best
 * under gcc_2_8_1_g8_no_split: 341/341 instructions, encoding-keyed opcode
 * distance 24, and 291 differing linked words.
 *
 * Seven explicit register assignments preserve the incoming player in $s0,
 * dispatch value in $v0, and case-8 roles in $v1/$s2/$s1/$s5/$s3. With most
 * linked words still different, this is a structural candidate, not a near
 * match justified by those assignments.
 *
 * Each call brackets dispatch with VSync timing and a diagnostic printf,
 * advances the per-player state byte, and covers all eleven target cases.
 * Residual differences are concentrated in cases 0 and 3, jump-table dispatch
 * scheduling, and the common tail; case 8 has the target instruction count
 * and register roles.
 */
#include "../types.h"
#include "../game/model_graphics_state.h"
#include "../psyq/libetc.h"
#include "../psyq/stdio.h"
#include "../game/script_state.h"

extern u8 D_800F2C40[];
extern u8 D_80010538[];
#define HIGH_MEMORY_ADDRESSES_MODEL_PREFIX
#include "../game/high_memory_addresses.h"

extern void func_8004CB0C(s32 a0, s32 a1, s32 a2, s32 a3);
extern void func_8004D75C(s32 a0);
#include "../game/func_8004D914.h"
extern void func_8005A4C4(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4);
extern void func_8005A468(s32 a0, s32 a1);
extern void func_800590DC(s32 a0);
#include "../game/func_800582C0.h"
extern void func_8005F198(s32 a0);
extern void func_8004DC38(s32 a0, s32 a1, s32 a2, s32 a3);
extern void func_80048D08(s32 a0, void *a1);

void func_80056828(s32 a0) {
    register s32 player asm("s0") = a0;
    u8 *rec = D_800F2C40 + player * 0xE20;
    u32 state;
    u32 idx;
    void **table;
    register s32 v0 asm("$2");
    s32 sem1;

    sem1 = VSync(1);
    state = rec[0xE14];
    if (state != 0 && state != 0xFF) {
        v0 = state;
        idx = v0 - 1;
        if (idx < 0xB) {
            table = (void **) (D_80010538 + 0x11D4);
            asm volatile("" ::
                "g"(&&L0), "g"(&&L1), "g"(&&L2), "g"(&&L3), "g"(&&L4),
                "g"(&&L5), "g"(&&L6), "g"(&&L7), "g"(&&L8), "g"(&&L9));
            asm volatile("" :: "g"(&&L10));
            goto *table[idx];

        L0:
            {
                s32 a1;
                s32 a2 = 0xC000;
                if (player == 0) {
                    a1 = (s32)D_80010000[0].payload_bases[0];
                } else if (player == 1) {
                    a1 = (s32)D_80010000[0].payload_bases[1];
                } else {
                    a1 = (s32)D_80010000[0].payload_bases[2];
                }
                v0 = *(s32 *) a1;
                if (v0 != 0) {
                    func_8004CB0C(player, a1, v0, -1);
                } else {
                    func_8004CB0C(player, a1, a2, -1);
                }
            }
            goto L_tail;

        L1:
            func_8004D75C(player);
            goto L_tail;

        L2:
            func_8004D914(player);
            goto L_tail;

        L3:
            {
                u8 *p = D_800F2C40 + player * 0xE20;
                u8 v1;
                u16 t1;
                u8 t0;
                u8 *a3;
                s32 a1;
                s32 a2;
                u16 v1b;
                u16 w;
                s32 sh;
                s32 sh2;
                s32 sh3;
                u8 b;

                if (player < 2) {
                    v1 = p[0xE1B];
                    if (0 < v1) {
                        t1 = 0xFFFF;
                        t0 = v1;
                        a3 = p;
                        a1 = 0;
                        a2 = 0;
                        while (a1 < t0) {
                            w = *(u16 *) (a3 + 0x33C);
                            if (w != t1) {
                                sh = a1;
                                if (sh < 0) {
                                    sh += 7;
                                }
                                sh >>= 3;
                                b = a3[0xBEC + sh];
                                sh2 = sh << 3;
                                sh3 = a1 - sh2;
                                if ((b >> sh3) & 1) {
                                    a2 += 0x14;
                                } else {
                                    a2 += 0xC;
                                }
                            }
                            a1 += 1;
                            a3 += 2;
                        }
                        v0 = *(s32 *) (p + 0xDE0);
                        v1b = *(u16 *) (p + 0xE02);
                        v0 += a2;
                        *(s32 *) (p + 0xDF0) = v0;
                        v0 += v1b * 4;
                        *(s32 *) (p + 0xDF4) = v0;
                    } else {
                        v0 = *(s32 *) (p + 0xDE0);
                        v1b = *(u16 *) (p + 0xE02);
                        *(s32 *) (p + 0xDF0) = v0;
                        v0 += v1b * 4;
                        *(s32 *) (p + 0xDF4) = v0;
                    }
                }
                func_8005A4C4((s32)p, 0, 0, 0,
                              player == 1 ? 0x800 : 0);
            }
            goto L_tail;

        L4:
            {
                u8 *rec2 = D_800F2C40 + player * 0xE20;
                u8 *p = rec2 + 0x1E0;
                u8 count;
                s32 a1;
                s16 a3;
                s32 t0;
                s32 *obj;
                u16 t;
                u8 tb;

                count = rec2[0xE1B];
                if (count != 0) {
                    a1 = 0;
                    a3 = -1;
                    t0 = 0x10;
                    while (1) {
                        obj = *(s32 **) p;
                        *(s16 *) ((u8 *) obj + 8) = a3;
                        obj = *(s32 **) p;
                        t = *(u16 *) ((u8 *) obj + 0x18);
                        *(u16 *) ((u8 *) obj + 0x16) = t;
                        obj = *(s32 **) p;
                        *(s16 *) ((u8 *) obj + 0xA) = a3;
                        obj = *(s32 **) p;
                        tb = *(u8 *) ((u8 *) obj + 0x1A);
                        *(u8 *) ((u8 *) obj + 0xC) = tb;
                        obj = *(s32 **) p;
                        *(u16 *) ((u8 *) obj + 0x10) = 0;
                        obj = *(s32 **) p;
                        a1 += 1;
                        *(u8 *) ((u8 *) obj + 0xD) = (u8) t0;
                        count = rec2[0xE1B];
                        if (!(a1 < count)) {
                            break;
                        }
                        p += 4;
                    }
                }
            }
            goto L_tail;

        L5:
            rec[0xE1F] = 1;
            func_800590DC(player);
            rec[0xE1F] = 0;
            goto L_tail;

        L6:
            {
                u8 *p = D_800F2C40 + player * 0xE20;
                u8 v1 = p[0xE0D];
                u8 v1b = p[0xE16];
                s32 a2 = v1 * 2;
                s32 v0;
                if (v1b == 0x3C) {
                    func_8005A468(player, -a2);
                    v0 = (player < 2);
                } else if (v1b < 0x3D) {
                    if (v1b == 0x23) {
                        func_8005A468(player, 0);
                        v0 = (player < 2);
                    } else {
                        v0 = (player < 2);
                    }
                } else if (v1b == 0x3E) {
                    func_8005A468(player, a2);
                    v0 = (player < 2);
                } else {
                    v0 = (player < 2);
                }
                if (!v0) {
                    rec[0xE1F] = 1;
                }
            }
            goto L_tail;

        L7:
            func_800582C0(player, rec[0xE0C], *(u16 *) (rec + 0xE0A));
            goto L_tail;

        L8:
            {
                register u8 *base asm("$3") = D_800F2C40;
                register u8 *p asm("$18") = base + player * 0xE20;
                register s32 s1 asm("$17");
                void *fn;
                s32 a2;
                s32 a1;
                register u16 s5 asm("$21");
                s32 idx8;
                register u8 *s3 asm("$19");
                u8 *o;

                v0 = *(s32 *) (p + 0xD10);
                s1 = *(s32 *) (p + 0xDE8);
                D_8009AFA0 = (u8) player;
                if (v0 >= 0) {
                    if (player == 0) {
                        fn = (void *) (D_80010000[0].primary_modules[0] + 4);
                    } else {
                        fn = (void *) (D_80010000[0].primary_modules[1] + 4);
                    }
                    func_8005F198(1);
                    a2 = *(s32 *) (p + 0xD10);
                    a1 = (a2 / 1000) * 1000;
                    ((void (*)(s32, s32, s32)) fn)(s1, a2 - a1, a2);
                    func_8005F198(0);
                }
                s1 = p[0xBF5];
                p[0xE0E] = 6;
                v0 = p[0xE1B];
                s5 = *(u16 *) (p + 0xE06);
                if (v0 != 0) {
                    idx8 = 0;
                    s3 = p;
                    do {
                        a2 = s3[0xBF5];
                        if (s1 != 0) {
                            o = *(u8 **) (s3 + 0x1E0);
                            o[0xC] = (u8) s1;
                            a2 = s1;
                        }
                        func_8004DC38((s32) p, idx8, a2, s5);
                        v0 = p[0xE1B];
                        idx8 += 1;
                        s3 += 4;
                    } while (idx8 < v0);
                }
                if (s1 != 0) {
                    p[0xBF5] = (u8) s1;
                }
            }
            goto L_tail;

        L9:
            if (rec[0xE1D] == 0) {
                v0 = player << 11;
                func_80048D08(player, D_801A8000 + v0);
            }
            goto L_tail;

        L10:
            v0 = 1;
            if (rec[0xE1D] == 0) {
                rec[0xE1E] = 1;
            }
            rec[0xE1F] = (u8) v0;
            goto L_tail;
        }
    }

L_tail:
    v0 = VSync(1);
    v0 = printf((char *) (D_80010538 + 0x105C), rec[0xE14], v0 - sem1);
    if (rec[0xE1F] == 0) {
        v0 = rec[0xE14] + 1;
    } else {
        v0 = 0xFF;
    }
    rec[0xE14] = (u8) v0;
L_end:
    return;
}
