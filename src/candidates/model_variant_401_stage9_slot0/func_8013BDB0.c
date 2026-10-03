#include "../../types.h"
#include "../../overlays/model_variant/variant415_wings.h"

/* CANDIDATE, NOT MATCHING: USA header-415 path-band renderer.
 * gcc_2_7_2_cdk_g0 emits the retail 0x59C bytes (359 instructions) and
 * opcode sequence, but 52 words still differ in allocation/scheduling.
 * Retail assembly remains linked. Header 565 is the slot-1 counterpart;
 * neither image may be promoted without a complete-image match.
 *
 * Five paths reuse three 0x74-byte bands at the start of the context.
 * Each path uses a matrix translation at 0x2F94 + path * 32, a direction
 * at 0x3020 + path * 16, and two signed angle-component arrays at 0x3070
 * and 0x307A with stride 2. Each band's two columns are projected separately
 * with nonnegative growth / 1024; rows a and c supply one textured quad.
 * The middle row is still projected, as in retail. Signed depth and GTE
 * flag tests precede the u16 depth conversion passed to GsSortPoly.
 *
 * Only the fifth path grows the levels by step * 64. The final column of
 * the third band advances the repeat count, requests a staggered restart,
 * or advances phase 2 to 3. Restart occurs after projection and before
 * drawing, and resets levels to -column * 256 - band * 512.
 *
 * A direct reset assignment and one work pointer produce 359 instructions
 * with opcode distance 2. The single-iteration reset block and separate
 * reset_work view below recover the opcode sequence. Keep these source
 * forms when reproducing the pinned fingerprint. Setup reordering, index
 * caching, typed path arrays and declaration permutations did not match.
 * No fixed registers, inline assembly or replacement opcodes are used.
 */
void func_8013BDB0(u8 *ctx)
{
    SVECTOR rot;
    VECTOR scale;
    MATRIX m;
    MATRIX ls;
    GsCOORDINATE2 coord;
    PSXLONG flag[3][2];
    PSXLONG p;
    u8 *work;
    GsOT *ot;
    POLY_GT4 *poly;
    u8 *reset_work;
    Variant415Wing *band;
    s16 n;
    s16 i;
    s16 j;
    s32 angle;
    s32 size;
    band = (Variant415Wing *)ctx;
    work = ctx;
    poly = (POLY_GT4 *)(work + 0x2B90);
    ot = func_80058F10();
    for (n = 0; n < 5; n++) {
        angle = ratan2(MODEL_VARIANT_HALF(work + n * 2, 0x307A), MODEL_VARIANT_HALF(work + n * 2, 0x3070)) + 0x800;
        do { band = (Variant415Wing *)work; } while (0);
        for (i = 0; i < 3; i++, band++) {
            for (j = 0; j < 2; j++) {
                size = band->level[j];
                if (size < 0) size = 0;
                rot.vx = 0;
                rot.vy = 0;
                rot.vz = angle;
                m.t[0] = MODEL_VARIANT_WORD(work + n * 32, 0x2F94) + MODEL_VARIANT_WORD(work + n * 16, 0x3020) * size / 1024;
                m.t[1] = MODEL_VARIANT_WORD(work + n * 32, 0x2F98) + MODEL_VARIANT_WORD(work + n * 16, 0x3024) * size / 1024;
                m.t[2] = MODEL_VARIANT_WORD(work + n * 32, 0x2F9C) + MODEL_VARIANT_WORD(work + n * 16, 0x3028) * size / 1024;
                scale.vx = 4096;
                scale.vy = 4096;
                scale.vz = 4096;
                RotMatrix(&rot, &m);
                coord.coord = m;
                coord.super = 0;
                coord.flg = 0;
                GsGetLs(&coord, &ls);
                GsSetLsMatrix(&ls);
                ReadRotMatrix(&ls);
                RotMatrix(&rot, &ls);
                ScaleMatrix(&ls, &scale);
                SetRotMatrix(&ls);
                band->otz[j] = RotTransPers3(&band->a[j], &band->b[j], &band->c[j], &band->sa[j], &band->sb[j], &band->sc[j], &p, &flag[i][j]);
                if (band->level[j] < 1024 && n + 1 == 5) {
                    band->level[j] += MODEL_VARIANT_WORD(work, 0x30A8) << 6;
                    if (band->level[j] >= 1024) {
                        band->level[j] = 1024;
                        if (MODEL_VARIANT_WORD(work, 0x30E8) < 2) MODEL_VARIANT_WORD(work, 0x30E8) = 2;
                        if (i + 1 == 3 && j == 1) {
                            if (MODEL_VARIANT_WORD(work, 0x30D8) < 3) {
                                MODEL_VARIANT_WORD(work, 0x30D8)++;
                                MODEL_VARIANT_WORD(work, 0x30DC) = 1;
                            } else if (MODEL_VARIANT_WORD(work, 0x30E8) == 2) {
                                MODEL_VARIANT_WORD(work, 0x30E8) = 3;
                            }
                        }
                    }
                }
            }
        }
        band = (Variant415Wing *)work;
        reset_work = work;
        if (MODEL_VARIANT_WORD(reset_work, 0x30DC) == 1 && n + 1 == 5) {
            for (i = 0; i < 3; i++, band++) {
                for (j = 0; j < 2; j++) {
                    band->level[j] = -(j << 8) - (i << 9);
                }
            }
            MODEL_VARIANT_WORD(reset_work, 0x30DC) = 0;
        }
        band = (Variant415Wing *)work;
        for (i = 0; i < 3; i++, band++) {
            poly->r0 = 255;
            poly->g0 = 255;
            poly->b0 = 255;
            poly->r1 = 16;
            poly->g1 = 16;
            poly->b1 = 16;
            poly->r2 = 255;
            poly->g2 = 255;
            poly->b2 = 255;
            poly->r3 = 16;
            poly->g3 = 16;
            poly->b3 = 16;
            for (j = 0; j < 1; j++) {
                poly->x0 = band->sa[j];
                poly->y0 = band->sa[j] >> 16;
                poly->x1 = band->sa[j + 1];
                poly->y1 = band->sa[j + 1] >> 16;
                poly->x2 = band->sc[j];
                poly->y2 = band->sc[j] >> 16;
                poly->x3 = band->sc[j + 1];
                poly->y3 = band->sc[j + 1] >> 16;
                if (0 <= band->otz[j] && flag[i][j] >= 0) GsSortPoly(poly, ot, (u16)band->otz[j]);
            }
        }
    }
}
