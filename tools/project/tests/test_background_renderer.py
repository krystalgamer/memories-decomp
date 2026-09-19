"""ILP32 background-renderer witnesses with controlled opaque SDK calls.

These are controller/packet contracts, not a GTE implementation. The fixture
uses the actual SDK layouts and three-slot model backing. It never inspects
uninitialized vertex pads, sprite transforms, or the low 24 packet-tag bits.
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
SOURCE = REPOSITORY / "src/game/func_8004DE24.c"
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
void func_8004DE24(void);
#define WORD(p, n) (*(u16 *)((u8 *)(p) + (n)))
#define CHECK(c, n) do { if (!(c)) return (n); } while (0)

ModelSlot D_800F2C40[MODEL_SLOT_COUNT];
GsCOORDUNIT D_800F56A0;
MATRIX D_800FE148;
u8 *D_8009AF88;
s16 D_8009AF8E, D_8009AF90;
GsOT *D_800E9D90[4];
static GsOT ordering;
static GsCOORDUNIT coordinate;
static u8 record[0xB2], other_record[0xB2];
static s32 error, count, normal_count, sprite_count, polygon_count;
static s32 cosine_count, sine_count, projection_count, depth_mode, mutation, executions;
static s32 height_expected;
static CVECTOR *normal_outputs[13];
static SVECTOR *vertex_base;
static long *projection_base;
typedef struct { s32 kind, a, b; } Event;
static Event events[256];
typedef struct {
    u32 attribute;
    s16 x, y;
    u16 w, h, page, cx, cy, priority;
    u8 u, v, r, g, b;
} SpriteObservation;
static SpriteObservation sprites[20];
static s32 polygon_ids[48];
static const s32 cosine_values[8] = {4096,2049,1,0,-1,-2049,-4096,3073};
static const s32 sine_values[8] = {0,1,2049,4096,-4096,-2049,-1,-3073};
static const s16 expected_ring[8][4][2] = {
    {{1000,0},{1600,0},{2200,0},{2800,0}},
    {{500,0},{800,0},{1100,0},{1400,0}},
    {{0,500},{0,800},{0,1100},{0,1400}},
    {{0,1000},{0,1600},{0,2200},{0,2800}},
    {{0,-1000},{0,-1600},{0,-2200},{0,-2800}},
    {{-500,-500},{-800,-800},{-1100,-1100},{-1400,-1400}},
    {{-1000,0},{-1600,0},{-2200,0},{-2800,0}},
    {{750,-750},{1200,-1200},{1650,-1650},{2100,-2100}}
};
static const u8 input_colors[5][3] = {{128,128,128},{96,64,64},{64,42,42},{32,21,21},{0,0,0}};
static const u8 output_colors[5][3] = {{146,131,135},{114,67,71},{82,45,49},{50,24,28},{18,3,7}};
static const u8 color_order[13] = {0,1,1,2,2,2,2,3,3,3,3,4,4};

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
void func_8004E7B0(s32 value)
{
    event(1, value, 0);
    if (value) error = 2;
    if (mutation == 1) D_8009AF88 = other_record;
}
int GsSetFlatLight(int id, GsF_LIGHT *light)
{
    event(2, id, 0);
    if (id < 0 || id > 2 || light != (GsF_LIGHT *)&D_800F2C40[2].field_D70[id]) error = 3;
    return 0;
}
void GsSetAmbient(long r, long g, long b)
{
    event(3, 0, 0);
    if (r != D_800F2C40[2].field_DA0[0] || g != D_800F2C40[2].field_DA0[1] ||
        b != D_800F2C40[2].field_DA0[2]) error = 4;
}
void GsSetLightMatrix(MATRIX *m)
{
    event(4, 0, 0);
    if (m != &D_800F56A0.matrix) error = 5;
}
void NormalColorCol(SVECTOR *normal, CVECTOR *input, CVECTOR *output)
{
    s32 i = normal_count++, color;
    event(5, i, 0);
    if (i >= 13) { error = 6; return; }
    color = color_order[i];
    if (normal->vx != (i ? 0 : 4096) || normal->vy != (i ? -4096 : 0) ||
        normal->vz || input->r != input_colors[color][0] ||
        input->g != input_colors[color][1] || input->b != input_colors[color][2] ||
        input->cd != 0) error = 7;
    normal_outputs[i] = output;
    output->r = output_colors[color][0];
    output->g = output_colors[color][1];
    output->b = output_colors[color][2];
    output->cd = 0x90 + i;
    if (mutation == 2 && !i) D_8009AF8E = 1920;
}
void GsSortFastSprite(GsSPRITE *sprite, GsOT *ot, unsigned short priority)
{
    SpriteObservation *out;
    s32 i = sprite_count++;
    event(6, i, priority);
    if (i >= 20) { error = 8; return; }
    if (ot != D_800E9D90[3] || priority != (u16)((1u << ot->length) - 1)) error = 9;
    out = &sprites[i];
    out->attribute = sprite->attribute; out->x = sprite->x; out->y = sprite->y;
    out->w = sprite->w; out->h = sprite->h; out->page = sprite->tpage;
    out->cx = sprite->cx; out->cy = sprite->cy; out->priority = priority;
    out->u = sprite->u; out->v = sprite->v;
    out->r = sprite->r; out->g = sprite->g; out->b = sprite->b;
    if (mutation == 3) {
        if (!i) sprite->w = 32767;
        if (i == 1) sprite->w = 64;
    }
    if (mutation == 4 && !i) { record[0xA1] = 1; ordering.length = 5; }
}
void *Model_GetLightSourceMatrix(void)
{
    event(7, 0, 0);
    return &D_800FE148;
}
void GsSetLsMatrix(MATRIX *matrix)
{
    event(8, 0, 0);
    if (matrix != &D_800FE148) error = 10;
}
int rcos(int phase)
{
    s32 i = cosine_count++;
    event(9, phase, 0);
    if (i >= 16 || phase != 256 * (i + 1)) { error = 11; return 0; }
    return cosine_values[i & 7];
}
int rsin(int phase)
{
    s32 i = sine_count++;
    event(10, phase, 0);
    if (i >= 16 || phase != 256 * (i + 1) || cosine_count != i + 1) { error = 12; return 0; }
    return sine_values[i & 7];
}
static s32 depth(s32 segment, s32 strip)
{
    s32 mode = depth_mode == 4 ? segment & 3 : depth_mode;
    if (mode == 3 || mode == 1 && !strip || mode == 2 && strip) return -1;
    return depth_mode == 4 ? 7 : 0;
}
static u32 xy(s32 segment, s32 strip, s32 vertex)
{
    s16 x = 100 + segment * 7 + strip * 50 + vertex;
    s16 y = -100 - segment * 5 - strip * 30 - vertex;
    return (u16)x | ((u32)(u16)y << 16);
}
long RotAverage4(SVECTOR *v0, SVECTOR *v1, SVECTOR *v2, SVECTOR *v3,
                 long *xy0, long *xy1, long *xy2, long *xy3, long *p, long *flag)
{
    SVECTOR *v[4];
    long *out[4];
    s32 call = projection_count++, segment = call / 2, strip = call & 1, i;
    event(11, segment, strip);
    if (call >= 32) { error = 13; return -1; }
    v[0] = v0; v[1] = v1; v[2] = v2; v[3] = v3;
    out[0] = xy0; out[1] = xy1; out[2] = xy2; out[3] = xy3;
    if (!call) { vertex_base = v0; projection_base = p; }
    if (p != projection_base + strip * 2 || flag != p + 1 ||
        cosine_count != segment + 1 || sine_count != segment + 1) error = 14;
    for (i = 0; i < 4; i++) {
        s32 radius = strip * 2 + i / 2;
        s32 pair = i & 1, phase = pair ? segment : segment - 1;
        s32 x = phase < 0 ? 1000 + radius * 600 : expected_ring[phase & 7][radius][0];
        s32 z = phase < 0 ? 0 : expected_ring[phase & 7][radius][1];
        s32 normal_index = (strip ? 9 : 1) + i;
        if (v[i] != vertex_base + strip * 4 + i ||
            v[i]->vx != x || v[i]->vy != height_expected || v[i]->vz != z ||
            out[i] != (long *)((u8 *)normal_outputs[normal_index] + 4)) error = 15;
        *out[i] = xy(segment, strip, i);
    }
    *p = 0x11223344; *flag = 0x55667788;
    return depth(segment, strip);
}
void GsSortPoly(void *pointer, GsOT *ot, unsigned short priority)
{
    POLY_G4 *packet = pointer;
    u8 *bytes = pointer;
    s32 id, vertex, segment = (projection_count - 1) / 2;
    u32 actual_xy[4];
    event(12, segment, 0);
    for (id = 0; id < 3; id++)
        if ((u8 *)normal_outputs[1 + id * 4] == bytes + 4) break;
    if (id == 3 || polygon_count >= 48) { error = 16; return; }
    polygon_ids[polygon_count++] = id;
    if (projection_count != (segment + 1) * 2 || ot != D_800E9D90[3] ||
        priority != (u16)((1u << ot->length) - 1) || bytes[3] != 8 || packet->code != 0x38) error = 17;
    actual_xy[0] = (u16)packet->x0 | ((u32)(u16)packet->y0 << 16);
    actual_xy[1] = (u16)packet->x1 | ((u32)(u16)packet->y1 << 16);
    actual_xy[2] = (u16)packet->x2 | ((u32)(u16)packet->y2 << 16);
    actual_xy[3] = (u16)packet->x3 | ((u32)(u16)packet->y3 << 16);
    for (vertex = 0; vertex < 4; vertex++) {
        s32 color = color_order[1 + id * 4 + vertex];
        s32 strip = id == 2 || id == 1 && vertex >= 2;
        s32 source_vertex = id == 1 ? vertex < 2 ? vertex + 2 : vertex - 2 : vertex;
        CVECTOR *rgb = (CVECTOR *)(bytes + 4 + vertex * 8);
        if (rgb->r != output_colors[color][0] || rgb->g != output_colors[color][1] ||
            rgb->b != output_colors[color][2] ||
            rgb->cd != (vertex ? 0x90 + 1 + id * 4 + vertex : 0x38) ||
            actual_xy[vertex] != xy(segment, strip, source_vertex)) error = 18;
    }
}

static void setup(void)
{
    s32 i;
    clear(D_800F2C40, sizeof(D_800F2C40)); clear(&coordinate, sizeof(coordinate));
    clear(&D_800F56A0, sizeof(D_800F56A0)); clear(&D_800FE148, sizeof(D_800FE148));
    clear(record, sizeof(record)); clear(other_record, sizeof(other_record));
    clear(events, sizeof(events)); clear(sprites, sizeof(sprites));
    clear(normal_outputs, sizeof(normal_outputs));
    clear(&ordering, sizeof(ordering));
    D_800F2C40[2].field_D18 = &coordinate;
    D_800F2C40[2].field_E1F = 1;
    D_800F2C40[2].field_DA0[0] = 100;
    D_800F2C40[2].field_DA0[1] = 200;
    D_800F2C40[2].field_DA0[2] = 300;
    D_8009AF88 = record; D_8009AF90 = 48; D_8009AF8E = 0;
    WORD(record, 0xA6) = 1280; WORD(record, 0xA8) = 256;
    WORD(record, 0xAA) = 0x2AB; WORD(record, 0xAC) = 0x3AB;
    WORD(record, 0xAE) = 0x123; WORD(record, 0xB0) = 0x234;
    for (i = 0; i < 0xB2; i++) other_record[i] = record[i];
    WORD(other_record, 0xA8) = 0x4100; WORD(other_record, 0xAA) = 0x140;
    WORD(other_record, 0xAC) = 0x100; WORD(other_record, 0xAE) = 42; WORD(other_record, 0xB0) = 43;
    D_800E9D90[3] = &ordering; ordering.length = 4;
    count = error = normal_count = sprite_count = polygon_count = 0;
    cosine_count = sine_count = projection_count = depth_mode = mutation = 0;
    height_expected = 300; vertex_base = 0; projection_base = 0;
}
static void run(void) { executions++; func_8004DE24(); }
static s32 lighting(void)
{
    return !error && count >= 7 && events[0].kind == 1 &&
           events[1].kind == 2 && events[1].a == 0 &&
           events[2].kind == 2 && events[2].a == 1 &&
           events[3].kind == 2 && events[3].a == 2 &&
           events[4].kind == 3 && events[5].kind == 4 &&
           events[6].kind == 5 && events[6].a == 0;
}
static s32 ring(void)
{
    s32 segment, index = 0, cursor = 7 + sprite_count, i;
    if (!lighting() || normal_count != 13 || cosine_count != 16 ||
        sine_count != 16 || projection_count != 32) return 0;
    for (i = 1; i < 13; i++, cursor++)
        if (events[cursor].kind != 5 || events[cursor].a != i) return 0;
    if (events[cursor++].kind != 7 || events[cursor++].kind != 8) return 0;
    for (segment = 0; segment < 16; segment++) {
        s32 a = depth(segment, 0), b = depth(segment, 1);
        if (events[cursor].kind != 9 || events[cursor].a != (segment + 1) * 256 ||
            events[cursor + 1].kind != 10 || events[cursor + 2].kind != 11 ||
            events[cursor + 2].b != 0 || events[cursor + 3].kind != 11 ||
            events[cursor + 3].b != 1) return 0;
        cursor += 4;
        if (a >= 0) {
            if (polygon_ids[index++] != 0 || events[cursor++].kind != 12) return 0;
        }
        if (a >= 0 || b >= 0) {
            if (polygon_ids[index++] != 1 || events[cursor++].kind != 12) return 0;
        }
        if (b >= 0) {
            if (polygon_ids[index++] != 2 || events[cursor++].kind != 12) return 0;
        }
    }
    return index == polygon_count && cursor == count;
}

static s32 guards(void)
{
    s32 inactive, channel;
    for (inactive = 0; inactive < 2; inactive++) for (channel = 0; channel < 4; channel++) {
        setup();
        D_800F2C40[2].field_E1F = !inactive;
        D_800F2C40[2].field_DA0[0] = -4096;
        D_800F2C40[2].field_DA0[1] = -4096;
        D_800F2C40[2].field_DA0[2] = -4096;
        if (channel < 3) D_800F2C40[2].field_DA0[channel] = -4095;
        run();
        if (inactive || channel == 3) CHECK(!error && count == 0, 20);
        else CHECK(lighting() && normal_count == 1 && sprite_count == 6, 21);
    }
    return 0;
}

static s32 texture(void)
{
    static const u8 expected_u[3][6] = {
        {4,240,220,200,180,32}, {130,152,174,196,90,112}, {129,108,87,66,45,152}
    };
    static const u16 priorities[] = {0,15,65535};
    static const u8 lengths[] = {0,4,16};
    s32 mode, length, i;
    for (mode = 0; mode < 3; mode++) for (length = 0; length < 3; length++) {
        setup(); WORD(record, 0xA8) = 256 | (mode << 13); ordering.length = lengths[length];
        run();
        CHECK(lighting() && sprite_count == 6 && count == 13 && normal_count == 1, 30);
        for (i = 0; i < 6; i++) {
            SpriteObservation *s = &sprites[i];
            CHECK(s->attribute == (mode ? 0x01000000 : 0) && s->page == 2074 + mode * 128 &&
                  s->cx == 0x123 && s->cy == 0x234 && s->w == 64 && s->h == 85 &&
                  s->x == -32 + 64 * i && s->y == 0 && s->u == expected_u[mode][i] &&
                  s->v == 171 && s->priority == priorities[length] &&
                  s->r == 146 && s->g == 131 && s->b == 135, 31);
        }
    }
    return 0;
}

static s32 clipping(void)
{
    static const s16 heights[] = {128,128,128,128,240,241,256,300,256,256,500,512,200,240,48,300,448,449};
    static const s16 pitches[] = {0,48,-80,-81,0,0,0,0,48,100,-100,-300,-160,-192,0,-250,-300,-300};
    static const s16 expected_h[] = {80,119,0,-1,192,193,208,252,247,247,352,420,-1,0,0,50,148,357};
    static const u8 expected_v[] = {48,9,128,0,48,48,48,48,9,9,148,92,0,240,48,42,92,92};
    s32 i, j;
    for (i = 0; i < 18; i++) {
        setup(); WORD(record, 0xA8) = heights[i]; D_8009AF90 = pitches[i];
        WORD(record, 0xAC) = 9; run();
        CHECK(lighting() && normal_count == 1 && sprite_count == (expected_h[i] < 0 ? 0 : 6), 40);
        for (j = 0; j < sprite_count; j++)
            CHECK(sprites[j].h == expected_h[i] && sprites[j].v == expected_v[i] &&
                  sprites[j].y == 0, 41);
    }
    return 0;
}

static s32 scrolling(void)
{
    static const s16 yaw[] = {-32768,-19,-10,-9,0,9,10,3199,3200,32767};
    static const s16 first[] = {-44,-33,-33,-32,-32,-32,-31,159,160,0};
    static const u8 counts[] = {6,6,6,6,6,6,6,3,3,0};
    s32 i, j;
    for (i = 0; i < 10; i++) {
        setup(); D_8009AF8E = yaw[i]; run();
        CHECK(lighting() && sprite_count == counts[i], 50);
        for (j = 0; j < sprite_count; j++) CHECK(sprites[j].x == first[i] + 64 * j, 51);
    }
    setup(); WORD(record, 0xA6) = 2560; WORD(record, 0xAA) = 0; run();
    CHECK(lighting() && sprite_count == 5 && sprites[0].x == 0 && sprites[4].x == 256 &&
          sprites[0].u == 64 && sprites[1].u == 128 && sprites[2].u == 192 &&
          sprites[3].u == 0 && sprites[4].u == 64, 52);
    return 0;
}

static s32 rings(void)
{
    static const s32 heights[] = {-100,300,32300};
    static const s32 counts[] = {48,32,32,0,28};
    s32 mode, h;
    for (mode = 0; mode < 5; mode++) for (h = 0; h < 3; h++) {
        setup(); record[0xA1] = 1; depth_mode = mode;
        coordinate.matrix.t[1] = heights[h] - 300; height_expected = heights[h];
        run();
        CHECK(ring() && polygon_count == counts[mode], 60);
    }
    setup(); record[0xA1] = 1; D_8009AF90 = -1000; run();
    CHECK(ring() && !sprite_count, 61);
    setup(); record[0xA1] = 2; run();
    CHECK(lighting() && !projection_count && normal_count == 1, 62);
    return 0;
}

static s32 opaque_changes(void)
{
    static const s16 wrapped_x[] = {32,-32737,-33,31,95,159,223,287};
    s32 i;
    setup(); mutation = 1; run();
    CHECK(lighting() && sprite_count == 6 && D_8009AF88 == other_record, 70);
    for (i = 0; i < 6; i++)
        CHECK(sprites[i].attribute == 0x01000000 && sprites[i].page == 277 &&
              sprites[i].cx == 42 && sprites[i].cy == 43 && sprites[i].v == 171 &&
              sprites[i].h == 85 && sprites[0].u == 4, 71);
    setup(); mutation = 2; run();
    CHECK(lighting() && sprite_count == 5 && sprites[0].x == 32 && sprites[4].x == 288, 72);
    setup(); mutation = 3; D_8009AF8E = 1920; run();
    CHECK(lighting() && sprite_count == 8 && sprites[1].w == 32767, 73);
    for (i = 0; i < 8; i++) CHECK(sprites[i].x == wrapped_x[i], 74);
    setup(); mutation = 4; run();
    CHECK(ring() && sprites[0].priority == 15 && sprites[1].priority == 31 &&
          normal_count == 13 && polygon_count == 48, 75);
    return 0;
}

int main(int argc, char **argv)
{
    static const s32 cases[] = {8,9,18,11,17,4};
    s32 scenario, result;
    if (sizeof(void *) != 4 || sizeof(long) != 4 || sizeof(MATRIX) != 32 ||
        sizeof(SVECTOR) != 8 || sizeof(CVECTOR) != 4 || sizeof(POLY_G4) != 36 ||
        sizeof(ModelSlot) != 0xE20 || MODEL_SLOT_COUNT != 3 || argc != 2) return 100;
    scenario = argv[1][0] - '0';
    switch (scenario) {
    case 0: result = guards(); break;
    case 1: result = texture(); break;
    case 2: result = clipping(); break;
    case 3: result = scrolling(); break;
    case 4: result = rings(); break;
    case 5: result = opaque_changes(); break;
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
class BackgroundRendererTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        (REPOSITORY / "tmp").mkdir(exist_ok=True)
        temporary = tempfile.TemporaryDirectory(prefix="background-test-", dir=REPOSITORY / "tmp")
        cls.addClassCleanup(temporary.cleanup)
        cls.directory = Path(temporary.name)
        cls.witness = cls.directory / "witness.c"
        prefix = SOURCE.read_text().split("void func_8004DE24(void)", 1)[0]
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

    def test_valid_backing_early_guards_and_mixed_ambient(self):
        self.scenario(0)

    def test_texture_depth_page_clut_uv_color_and_priority(self):
        self.scenario(1)

    def test_two_clipping_stages_thresholds_negative_y_and_byte_wrap(self):
        self.scenario(2)

    def test_signed_yaw_width_quotient_modulus_and_horizontal_edges(self):
        self.scenario(3)

    def test_ring_normals_colors_vertices_projection_culling_and_copies(self):
        self.scenario(4)

    def test_opaque_sdk_reloads_and_terminating_signed_x_wrap(self):
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

    def test_wrong_texture_page_bit_is_detected(self):
        self.mutant("(H(0xAC) & 0x200) << 2", "(H(0xAC) & 0x200) << 1", 1, range(30, 32))

    def test_wrong_radius_is_detected(self):
        self.mutant("1000 * cosine", "1001 * cosine", 4, range(60, 63))

    def test_wrong_zero_depth_gate_is_detected(self):
        self.mutant("if (first_depth >= 0)", "if (first_depth > 0)", 4, range(60, 63))


if __name__ == "__main__":
    unittest.main()
