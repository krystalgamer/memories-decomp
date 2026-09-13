"""ILP32 witnesses for the ring effect, with controlled opaque SDK helpers.

The fixture owns only the measured work prefix, not a whole model module.
Projection/trigonometry results are call witnesses, not GTE emulation. Query
stubs touch only the eight-byte adjustment prefix; RotTrans writes exactly
three translation words, never VECTOR.pad beyond MATRIX.t.
"""

from __future__ import annotations

import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
SOURCE = REPOSITORY / "src/game/func_8006C37C.c"
START = """
.text
.globl _start
_start:
    mov (%esp), %eax
    lea 4(%esp), %edx
    and $-16, %esp
    sub $8, %esp
    push %edx
    push %eax
    call main
    mov %eax, %ebx
    mov $1, %eax
    int $0x80
.section .note.GNU-stack,"",@progbits
"""

WITNESS = r"""
s32 func_8006C37C(RingWork *state, s32 command);
#define CHECK(c, n) do { if (!(c)) return (n); } while (0)

u16 D_80091604[6] = {0x2020,0x0020,0x012C,0x0014,0x000A,0};
static struct { u32 before; RingWork work; u32 after; } allocation;
static RingWork *work = &allocation.work;
static RingSettings alternate_settings;
static MATRIX source_matrix, expected_matrix;
static GsOT ordering_table;
static ModelEffectAdjustment dimensions;
static s32 frame, error, count, calls_slot, calls_frame, calls_memset;
static s32 calls_cos, calls_sin, calls_matrix, calls_average, calls_submit;
static s32 draw_mode, expected_scale, depth_mode, flag_index, flag_value, mutation;
static s32 initial_phase, expected_fade_at_pop, expected_growth_at_pop, executions;
static s32 last_return, last_average, current_triangle;
static u8 expected_center[3], expected_rim[3];
static void *zeroed[2];
static MATRIX *saved_pointer, *new_pointer;
static long *translation_pointer;
static POLY_G3 *triangle_pointer;
typedef struct { s32 kind, a, b; } Event;
static Event events[256];
static const s32 cosine_values[4] = {4096,2049,-4096,-2049};
static const s32 primary_sines[4] = {0,4096,-1,-4096};
static const s32 secondary_sines[4] = {4096,-4096,2049,-2049};
static const s32 draw_sines[4] = {0,17,-17,-4096};
static const s32 average_values[8] = {0,1,-1,-2,-3,131074,65535,2147483647};
static const s32 average_indices[8] = {0,0,0,-1,-1,1,32767,65535};

static void clear(void *pointer, u32 size)
{
    volatile u8 *p = pointer;
    while (size--) *p++ = 0;
}
static void event(s32 kind, s32 a, s32 b)
{
    if (count >= 256) { error = 1; return; }
    events[count].kind = kind; events[count].a = a; events[count].b = b;
    count++;
}
void *memset(void *pointer, int value, unsigned int size)
{
    volatile u8 *bytes = pointer;
    u32 i;
    event(1, value, size);
    if (calls_memset >= 2 || value || size != 8) error = 2;
    else zeroed[calls_memset] = pointer;
    calls_memset++;
    for (i = 0; i < size; i++) bytes[i] = value;
    return pointer;
}
s32 func_80058DCC(void)
{
    event(2, calls_slot, 0);
    return calls_slot++;
}
s32 func_80058E1C(void)
{
    event(3, frame, 0);
    calls_frame++;
    return frame;
}
void func_80057E20(s32 index, ModelEffectAdjustment *out)
{
    event(4, index, 0);
    if (index != 1) error = 3;
    out->x = dimensions.x; out->y = dimensions.y;
    out->z = dimensions.z; out->max = dimensions.max;
}
int ccos(int angle)
{
    s32 i = calls_cos++;
    event(5, angle, 0);
    if (draw_mode || i >= 66 || angle != (i % 33) * 128 ||
        calls_sin != i * 2) error = 4;
    return cosine_values[(i % 33) & 3];
}
int csin(int angle)
{
    s32 i = calls_sin++;
    event(6, angle, 0);
    if (draw_mode) {
        if (i || angle != (initial_phase & 3) * 1024) error = 5;
        return draw_sines[initial_phase & 3];
    }
    if (i >= 132 || angle != ((i / 2) % 33) * 128 ||
        calls_cos != i / 2 + 1) error = 6;
    return i & 1 ? secondary_sines[((i / 2) % 33) & 3] :
                   primary_sines[((i / 2) % 33) & 3];
}
void *func_80058F10(void)
{
    event(7, 0, 0);
    if (mutation == 2) work->settings = &alternate_settings;
    return &ordering_table;
}
void SetPolyG3(POLY_G3 *p)
{
    event(8, 0, 0); triangle_pointer = p;
    setPolyG3(p);
}
void SetPolyG4(POLY_G4 *p)
{
    event(9, 0, 0);
    if ((void *)p == (void *)triangle_pointer) error = 7;
    setPolyG4(p);
}
void PushMatrix(void) { event(10, 0, 0); }
void *func_80059220(void)
{
    event(11, 0, 0);
    return &source_matrix;
}
static s32 equal_matrix(const MATRIX *left, const MATRIX *right)
{
    const u8 *a = (const u8 *)left, *b = (const u8 *)right;
    s32 i;
    for (i = 0; i < 32; i++) if (a[i] != b[i]) return 0;
    return 1;
}
void GsSetLsMatrix(MATRIX *p)
{
    s32 i = calls_matrix++;
    event(12, i, 0);
    if (i < 2) {
        if (!i) saved_pointer = p;
        if (p == &source_matrix || p != saved_pointer ||
            !equal_matrix(p, &expected_matrix)) error = 8;
        if (!i && mutation == 3) source_matrix.t[0] = 7777;
    } else {
        if (i != 2 || p != new_pointer || p->m[0][0] != 222 ||
            p->t[0] != 321 || p->t[1] != -654 || p->t[2] != 987) error = 9;
        if (mutation == 7) { work->fade = 1; work->growth = 7; }
    }
}
void Model_CopySlotU16Values(s32 index, u16 *out)
{
    s32 i;
    event(13, index, 0);
    if (index != 1 || out != zeroed[1]) error = 10;
    for (i = 0; i < 4; i++) if (out[i]) error = 11;
    out[0] = 11; out[1] = (u16)-22; out[2] = 33; out[3] = 44;
}
void RotTrans(SVECTOR *position, VECTOR *translation, long *flag)
{
    event(14, 0, 0);
    if (position != zeroed[1] || position->vx != 11 || position->vy != -22 ||
        position->vz != 33) error = 12;
    translation_pointer = (long *)translation;
    translation_pointer[0] = 321;
    translation_pointer[1] = -654;
    translation_pointer[2] = 987;
    *flag = 0x55;
}
MATRIX *RotMatrix(SVECTOR *angles, MATRIX *matrix)
{
    s32 row, column;
    event(15, 0, 0); new_pointer = matrix;
    if (angles != zeroed[0] || angles->vx || angles->vy || angles->vz ||
        (long *)&matrix->t[0] != translation_pointer ||
        matrix->t[0] != 321 || matrix->t[1] != -654 || matrix->t[2] != 987) error = 13;
    for (row = 0; row < 3; row++) for (column = 0; column < 3; column++)
        matrix->m[row][column] = 100 + row * 3 + column;
    return matrix;
}
MATRIX *ScaleMatrix(MATRIX *matrix, VECTOR *scale)
{
    s32 row, column;
    event(16, scale->vx, 0);
    if (matrix != new_pointer || scale->vx != expected_scale ||
        scale->vy != expected_scale || scale->vz != expected_scale || scale->pad != 0)
        error = 14;
    for (row = 0; row < 3; row++) for (column = 0; column < 3; column++)
        if (matrix->m[row][column] != 100 + row * 3 + column) error = 15;
    matrix->m[0][0] = 222;
    return matrix;
}
void RotTransPersN(SVECTOR *vertices, DVECTOR *screen, u16 *depths,
                   u16 *perspective, u16 *flags, long n)
{
    s32 i;
    event(17, n, 0);
    if (vertices != work->vertices || n != 67) { error = 16; return; }
    for (i = 0; i < 67; i++) {
        if (vertices[i].vx != i || vertices[i].vy != -i || vertices[i].vz != i * 2)
            error = 17;
        screen[i].vx = 1000 + i * 3; screen[i].vy = -1000 - i * 5;
        depths[i] = 65000 + i;
        perspective[i] = 0x1234;
        flags[i] = i == flag_index ? flag_value : 0;
    }
    if (mutation == 4) { work->fade = 3; work->growth = 4; }
}
long AverageZ3(long a, long b, long c)
{
    s32 i = calls_average++;
    event(18, i, 0);
    if (i >= 32 || a != 65000 || b != 65001 + i || c != 65002 + i) error = 18;
    current_triangle = i;
    last_average = depth_mode ? average_values[i & 7] : 200;
    return last_average;
}
void func_8005B260(u32 *source, GsOT *ot, s32 index, s32 flags)
{
    POLY_G3 *p = (POLY_G3 *)source;
    s32 i = current_triangle, expected_index = depth_mode ? average_indices[i & 7] : 100;
    event(19, i, index); calls_submit++;
    if (p != triangle_pointer || ot != &ordering_table || index != expected_index ||
        flags != 1 || expected_index < 0 || (flag_index >= 0 && flag_index < 3 && (flag_value & 32)))
        error = 19;
    if (((u8 *)p)[3] != 6 || p->code != 0x30 ||
        p->x0 != 1000 || p->y0 != -1000 ||
        p->x1 != 1003 + i * 3 || p->y1 != -1005 - i * 5 ||
        p->x2 != 1006 + i * 3 || p->y2 != -1010 - i * 5 ||
        p->r0 != expected_center[0] || p->g0 != expected_center[1] || p->b0 != expected_center[2] ||
        p->r1 != expected_rim[0] || p->g1 != expected_rim[1] || p->b1 != expected_rim[2] ||
        p->r2 != expected_rim[0] || p->g2 != expected_rim[1] || p->b2 != expected_rim[2]) error = 20;
}
void PopMatrix(void)
{
    event(20, 0, 0);
    if (work->phase != initial_phase || work->fade != expected_fade_at_pop ||
        work->growth != expected_growth_at_pop) error = 21;
    if (mutation == 1) frame = 9;
    if (mutation == 5) { work->growth = -5; work->phase = 255; }
    if (mutation == 6) work->fade = 1;
}

static void setup(s32 drawing)
{
    s32 i;
    clear(&allocation, sizeof(allocation));
    allocation.before = 0xABCDEF01; allocation.after = 0x76543210;
    work->field_220 = 0x11223344;
    work->color.cd = 0xA5;
    work->pad_22D[0] = 0xC1; work->pad_22D[1] = 0xC2; work->pad_22D[2] = 0xC3;
    work->settings = (RingSettings *)D_80091604;
    alternate_settings = *work->settings;
    alternate_settings.fade_divisor = 7; alternate_settings.growth_divisor = 3;
    work->color.r = 153; work->color.g = 255; work->color.b = 238;
    work->fade = expected_fade_at_pop = 20;
    work->growth = expected_growth_at_pop = 10;
    work->phase = initial_phase = 0;
    for (i = 0; i < 67; i++) {
        work->vertices[i].vx = i; work->vertices[i].vy = -i; work->vertices[i].vz = 2 * i;
    }
    clear(events, sizeof(events));
    for (i = 0; i < 32; i++) ((u8 *)&source_matrix)[i] = ((u8 *)&expected_matrix)[i] = i + 17;
    frame = 2; expected_scale = 4096;
    expected_center[0] = 153; expected_center[1] = 255; expected_center[2] = 238;
    expected_rim[0] = expected_rim[1] = expected_rim[2] = 32;
    dimensions.x = 400; dimensions.y = 100; dimensions.z = 200; dimensions.max = 0x1234;
    draw_mode = drawing; depth_mode = 0; flag_index = -1; flag_value = 0;
    count = error = calls_slot = calls_frame = calls_memset = calls_cos = calls_sin = 0;
    calls_matrix = calls_average = calls_submit = mutation = 0;
    saved_pointer = new_pointer = 0; translation_pointer = 0; triangle_pointer = 0;
}
static void run(s32 command) { executions++; last_return = func_8006C37C(work, command); }
static s32 preserved(void)
{
    return !error && allocation.before == 0xABCDEF01 && allocation.after == 0x76543210 &&
        work->field_220 == 0x11223344 && work->color.cd == 0xA5 &&
        work->pad_22D[0] == 0xC1 && work->pad_22D[1] == 0xC2 && work->pad_22D[2] == 0xC3 &&
        calls_memset == 2 && calls_slot == 2 && calls_frame == 1 &&
        zeroed[0] != zeroed[1];
}

static s32 init_cases(void)
{
    static const s16 axes[14][3] = {
        {-32768,-1,-10},{239,10,20},{240,240,240},{241,10,10},{10,241,10},{10,10,241},
        {400,400,1},{1,400,400},{400,1,400},{560,1,1},{561,1,1},{32767,1,1},
        {-1,-1,-1},{0,0,0}
    };
    static const s16 radius[14] = {300,300,300,301,301,301,500,500,500,700,700,700,300,300};
    static const s16 half_radius[14] = {150,150,150,150,150,150,250,250,250,350,350,350,150,150};
    static const s16 z_full[14] = {37,37,37,37,37,37,62,62,62,87,87,87,37,37};
    static const s16 z_half[14] = {18,18,18,18,18,18,31,31,31,43,43,43,18,18};
    static const s16 inner[14] = {240,240,240,240,240,240,400,400,400,560,560,560,240,240};
    static const s16 inner_z[14] = {30,30,30,30,30,30,50,50,50,70,70,70,30,30};
    static const s16 inner_half_z[14] = {15,15,15,15,15,15,25,25,25,35,35,35,15,15};
    static const s32 commands[] = {0,398,999,1234};
    static const u8 colors[4][3] = {{102,102,102},{153,255,238},{255,255,255},{50,153,170}};
    s32 a, c, ring, i;
    for (a = 0; a < 14; a++) for (c = 0; c < 4; c++) {
        setup(0); dimensions.x = axes[a][0]; dimensions.y = axes[a][1]; dimensions.z = axes[a][2];
        run(commands[c]);
        CHECK(preserved() && last_return == 0 && work->settings == (RingSettings *)D_80091604 &&
              work->fade == 20 && work->growth == 0 && work->phase == 0, 30);
        CHECK(calls_cos == 66 && calls_sin == 132 && count == 204 &&
              events[0].kind == 1 && events[1].kind == 1 && events[2].kind == 2 &&
              events[3].kind == 3 && events[4].kind == 2 && events[5].kind == 4, 31);
        CHECK(work->color.r == colors[c][0] && work->color.g == colors[c][1] &&
              work->color.b == colors[c][2] &&
              work->vertices[0].vx == radius[a] / 3 &&
              work->vertices[0].vy == radius[a] / 3 && work->vertices[0].vz == 0, 32);
        for (ring = 0; ring < 2; ring++) for (i = 0; i < 33; i++) {
            s32 r = ring ? inner[a] : radius[a], h = ring ? inner[a] / 2 : half_radius[a];
            s32 z = ring ? inner_z[a] : z_full[a], zh = ring ? inner_half_z[a] : z_half[a];
            s32 phase = i & 3;
            SVECTOR *v = &work->vertices[1 + ring * 33 + i];
            CHECK(v->vx == (phase == 0 ? r : phase == 1 ? h : phase == 2 ? -r : -h) &&
                  v->vy == (phase == 0 || phase == 2 ? 0 : phase == 1 ? r : -r) &&
                  v->vz == (phase == 0 ? z : phase == 1 ? -z : phase == 2 ? zh : -zh), 33);
        }
    }
    return 0;
}

static s32 draw_events(void)
{
    static const s32 prefix[] = {1,1,2,3,7,8,9,10,11,12,6,2,13,12,14,15,16,12,17};
    s32 i, cursor = 19, drawn = 0, blocked = flag_index >= 0 && flag_index < 3 && (flag_value & 32);
    if (!preserved() || calls_sin != 1 || calls_cos != 0 || calls_matrix != 3 || calls_average != 32)
        return 0;
    for (i = 0; i < 19; i++) if (events[i].kind != prefix[i]) return 0;
    for (i = 0; i < 32; i++) {
        if (events[cursor].kind != 18 || events[cursor++].a != i) return 0;
        if (!blocked && (!depth_mode || average_indices[i & 7] >= 0)) {
            if (events[cursor].kind != 19 || events[cursor++].a != i) return 0;
            drawn++;
        }
    }
    return calls_submit == drawn && count == cursor + 1 && events[cursor].kind == 20;
}

static s32 scale_and_phase(void)
{
    static const s32 growth[] = {0,1,3,10};
    static const u8 phase[] = {0,1,2,3,255};
    static const s32 scales[4][4] = {{0,1,-1,-256},{409,410,408,153},{1228,1229,1227,972},{4096,4097,4095,3840}};
    static const u8 center[4][3] = {{0,0,0},{15,25,23},{45,76,71},{153,255,238}};
    static const u8 rim[] = {0,3,9,32};
    s32 g, p, i;
    for (g = 0; g < 4; g++) for (p = 0; p < 5; p++) {
        setup(1); frame = 0;
        work->growth = expected_growth_at_pop = growth[g];
        work->phase = initial_phase = phase[p];
        expected_scale = scales[g][phase[p] & 3];
        for (i = 0; i < 3; i++) { expected_center[i] = center[g][i]; expected_rim[i] = rim[g]; }
        run(-1);
        CHECK(draw_events() && last_return == 0 && work->growth == growth[g] &&
              work->phase == (u8)(phase[p] + 1), 40);
    }
    return 0;
}

static s32 colors_and_fade(void)
{
    static const s32 fade[] = {0,1,7,19,20,-1};
    static const s32 growth[] = {7,7,3,9,10,7};
    static const s32 scales[] = {2867,2867,1228,3686,4096,2867};
    static const u8 center[6][3] = {{0,0,0},{4,8,7},{15,26,24},{130,217,203},{153,255,238},{252,248,249}};
    static const u8 rim[] = {0,0,3,27,32,0};
    s32 f, i;
    for (f = 0; f < 6; f++) {
        setup(1); frame = 0;
        work->fade = expected_fade_at_pop = fade[f];
        work->growth = expected_growth_at_pop = growth[f]; expected_scale = scales[f];
        for (i = 0; i < 3; i++) { expected_center[i] = center[f][i]; expected_rim[i] = rim[f]; }
        run(-2);
        CHECK(draw_events() && work->fade == fade[f] && work->growth == growth[f] &&
              last_return == (f == 0 || f == 5 ? 2 : 0), 50);
    }
    return 0;
}

static s32 flags_and_depth(void)
{
    static const s32 indices[] = {-1,0,1,2,3,66,1,2};
    static const s32 values[] = {0,32,32,32,32,32,1,0x2000};
    s32 d, f;
    for (d = 0; d < 2; d++) for (f = 0; f < 8; f++) {
        setup(1); depth_mode = d; flag_index = indices[f]; flag_value = values[f];
        run(-1);
        CHECK(draw_events() && calls_submit == (f >= 1 && f <= 3 ? 0 : d ? 24 : 32), 60);
    }
    return 0;
}

static s32 updates(void)
{
    static const s32 frames[] = {0,1,2,17,255,256,258};
    static const s32 deltas[] = {0,1,2,17,255,0,2};
    static const s32 commands[] = {-2,-1,-3};
    s32 f, c;
    for (f = 0; f < 7; f++) for (c = 0; c < 3; c++) {
        setup(1); frame = frames[f];
        work->growth = expected_growth_at_pop = 7; expected_scale = 2867;
        expected_center[0] = 107; expected_center[1] = 178; expected_center[2] = 166;
        expected_rim[0] = expected_rim[1] = expected_rim[2] = 22;
        run(commands[c]);
        CHECK(draw_events() && work->phase == 1, 70);
        if (c == 0)
            CHECK(work->fade == 20 - deltas[f] && work->growth == 7 &&
                  last_return == (deltas[f] >= 20 ? 2 : 0), 71);
        else CHECK(work->fade == 20 && work->growth == (deltas[f] >= 3 ? 10 : 7 + deltas[f]) &&
                   last_return == 0, 72);
    }
    return 0;
}

static s32 opaque_mutations(void)
{
    s32 m;
    /* These mutations probe opaque-call reloads; they are not producer-domain
       claims. Negative growth is injected only after the scale calculation. */
    for (m = 1; m <= 7; m++) {
        setup(1); mutation = m;
        if (m == 4) { expected_fade_at_pop = 3; expected_growth_at_pop = 4; }
        if (m == 7) {
            expected_fade_at_pop = 1; expected_growth_at_pop = 7;
            expected_center[0] = 4; expected_center[1] = 8; expected_center[2] = 7;
            expected_rim[0] = expected_rim[1] = expected_rim[2] = 0;
        }
        run(m == 6 ? -2 : -1);
        CHECK(draw_events(), 80);
        if (m == 1) CHECK(work->growth == 10 && calls_frame == 1 && frame == 9, 81);
        if (m == 2) CHECK(work->settings == &alternate_settings && work->growth == 10, 82);
        if (m == 3) CHECK(source_matrix.t[0] == 7777, 83);
        if (m == 4) CHECK(work->fade == 3 && work->growth == 6, 84);
        if (m == 5) CHECK(work->growth == 0 && work->phase == 0, 85);
        if (m == 6) CHECK(work->fade == -1 && last_return == 2, 86);
        if (m == 7) CHECK(work->fade == 1 && work->growth == 9, 87);
    }
    return 0;
}

int main(int argc, char **argv)
{
    static const s32 cases[] = {56,20,6,16,21,7};
    s32 result, scenario;
    if (sizeof(void *) != 4 || sizeof(long) != 4 || sizeof(RingWork) != 0x230 ||
        sizeof(RingSettings) != 12 || sizeof(ModelEffectAdjustment) != 8 ||
        sizeof(MATRIX) != 32 || sizeof(VECTOR) != 16 || sizeof(SVECTOR) != 8 ||
        sizeof(POLY_G3) != 28 || argc != 2) return 100;
    scenario = argv[1][0] - '0';
    switch (scenario) {
    case 0: result = init_cases(); break;
    case 1: result = scale_and_phase(); break;
    case 2: result = colors_and_fade(); break;
    case 3: result = flags_and_depth(); break;
    case 4: result = updates(); break;
    case 5: result = opaque_mutations(); break;
    default: return 101;
    }
    if (result) return result;
    return executions == cases[scenario] ? 0 : 102;
}
"""


def normalized(text: str, directory: Path) -> str:
    return re.sub(
        r'^#include "([^"]+)"',
        lambda match: '#include "' + str((directory / match[1]).resolve()) + '"',
        text, flags=re.MULTILINE,
    )


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires Linux x86 and a host compiler with ILP32 execution",
)
class ModelRingEffectTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        (REPOSITORY / "tmp").mkdir(exist_ok=True)
        temporary = tempfile.TemporaryDirectory(prefix="ring-effect-test-", dir=REPOSITORY / "tmp")
        cls.addClassCleanup(temporary.cleanup)
        cls.directory = Path(temporary.name)
        cls.witness = cls.directory / "witness.c"
        prefix = SOURCE.read_text().split("s32 func_8006C37C(", 1)[0]
        cls.witness.write_text(normalized(prefix, SOURCE.parent) + WITNESS)
        cls.startup = cls.directory / "start.S"
        cls.startup.write_text(START)
        cls.binaries = {
            optimization: cls.compile(SOURCE, optimization, "actual")
            for optimization in ("-O0", "-O2")
        }

    @classmethod
    def compile(cls, source: Path, optimization: str, label: str) -> Path:
        binary = cls.directory / (label + optimization)
        result = subprocess.run(
            ["cc", "-m32", optimization, "-std=gnu99", "-ffreestanding",
             "-fno-builtin", "-fno-strict-aliasing", "-fno-pie", "-no-pie",
             "-fno-stack-protector", "-nostdlib", f"-I{REPOSITORY}",
             str(source), str(cls.witness), str(cls.startup), "-o", str(binary)],
            cwd=REPOSITORY, env={**os.environ, "TMPDIR": str(cls.directory)},
            capture_output=True, text=True, timeout=60,
        )
        if result.returncode:
            raise AssertionError(result.stderr)
        return binary

    def scenario(self, number: int):
        for optimization, binary in self.binaries.items():
            with self.subTest(optimization=optimization):
                result = subprocess.run(
                    [str(binary), str(number)], cwd=REPOSITORY,
                    capture_output=True, text=True, timeout=10,
                )
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_init_signed_dimensions_clamps_rings_closure_and_colors(self):
        self.scenario(0)

    def test_matrix_pipeline_scale_rounding_and_phase_wrap(self):
        self.scenario(1)

    def test_sequential_color_divisions_and_fade_completion(self):
        self.scenario(2)

    def test_fixed_flag_triple_fan_indices_and_signed_depth_cast(self):
        self.scenario(3)

    def test_captured_byte_frame_and_command_specific_updates(self):
        self.scenario(4)

    def test_synthetic_opaque_mutations_cached_settings_and_reloads(self):
        self.scenario(5)

    def mutant(self, old: str, new: str, scenario: int, expected: range):
        text = SOURCE.read_text()
        self.assertEqual(text.count(old), 1, "semantic mutation must identify one operation")
        source = self.directory / f"mutant-{scenario}.c"
        source.write_text(normalized(text.replace(old, new), SOURCE.parent))
        binary = self.compile(source, "-O2", f"mutant-{scenario}")
        result = subprocess.run(
            [str(binary), str(scenario)], cwd=REPOSITORY,
            capture_output=True, text=True, timeout=10,
        )
        self.assertIn(result.returncode, expected, result.stderr)

    def test_indexed_flags_instead_of_fixed_triple_are_detected(self):
        self.mutant("flags[0] | flags[1] | flags[2]",
                    "flags[0] | flags[i + 1] | flags[i + 2]", 3, range(60, 61))

    def test_wrong_fan_count_is_detected(self):
        self.mutant("while (i < 32)", "while (i < 31)", 3, range(60, 61))

    def test_wrong_fade_command_is_detected(self):
        self.mutant("if (command == -2)", "if (command == -1)", 4, range(70, 73))

    def test_combining_color_divisions_is_detected(self):
        self.mutant(
            "work->color.r * work->fade / settings->fade_divisor *\n"
            "            work->growth / settings->growth_divisor",
            "work->color.r * work->fade * work->growth /\n"
            "            (settings->fade_divisor * settings->growth_divisor)",
            2, range(50, 51),
        )


if __name__ == "__main__":
    unittest.main()
