"""ILP32 projection setup tests; geometry operations and MAC0 are scripted."""

import itertools
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/game/display_object_projection_checks.c"
START = """
.text
.globl _start
_start:
    movl $192, %eax
    movl $0x1f800000, %ebx
    movl $4096, %ecx
    movl $3, %edx
    movl $0x32, %esi
    movl $-1, %edi
    xorl %ebp, %ebp
    int $0x80
    cmpl $0x1f800000, %eax
    jne map_failed
    andl $-16, %esp
    call main
    movl %eax, %ebx
    jmp exit
map_failed:
    movl $77, %ebx
exit:
    movl $1, %eax
    int $0x80
.section .note.GNU-stack,"",@progbits
"""
SHIM = """
#include "src/types.h"
void fixture_read_gte(s32 *value, s32 reg);
#define gte_stopz(p) fixture_read_gte(p, 24)
#define gte_stotz(p) fixture_read_gte(p, 7)
"""


def cases():
    clips = (-2147483648, -1, 0, 1, 2147483647)
    coordinates = (-65537, -32768, 0, 32767, 65536)
    for x, y, clip in itertools.product(coordinates, coordinates, clips):
        yield (0x12345678, x, y, clip, 0x398)
    for value in range(256):
        attribute = value | ((255 - value) << 8) | (((value * 37) & 255) << 16) | (((value * 17) & 255) << 24)
        yield (attribute, coordinates[value % 5], coordinates[(value + 2) % 5], clips[value % 5], 0x398)
    for offset, clip in itertools.product((0x2D0, 0x308, 0x310), clips):
        yield (0x12345678, 160, 120, clip, offset)
    for attribute in (0, 0xFF, 0xFF00, 0xFF0000, 0xFF000000, 0xFFFFFFFF):
        yield (attribute, 160, 120, 1, 0x398)


WITNESS = r"""
#include "src/types.h"
#include "src/psyq/libgte.h"
#include "src/psyq/libgpu.h"
#include "src/psyq/libgs.h"
#include "src/game/display_object_projection.h"
typedef char ilp32[sizeof(void *) == 4 && sizeof(long) == 4 ? 1 : -1];
typedef struct { u32 attribute; s32 x, y, clip; u32 output; } Case;
static const Case cases[] = {
@CASES@
};
static u8 expected[1024];
static const Case *test;
static u32 current, stage;
#define SCRATCH ((u8 *)0x1F800000)
#define MATRIX_PTR ((MATRIX *)0x1F8002D0)
#define VECTORS ((SVECTOR *)0x1F800308)

static void output(const char *p, u32 size)
{
    __asm__ volatile("int $0x80" : : "a"(4), "b"(1), "c"(p), "d"(size) : "memory", "cc");
}
static void number(u32 value)
{
    char buffer[12], reversed[12];
    u32 i = 0, j = 0;
    do { reversed[i++] = '0' + value % 10; value /= 10; } while (value);
    while (i) buffer[j++] = reversed[--i];
    buffer[j++] = '\n'; output(buffer, j);
}
static void fail(u32 line) __attribute__((noreturn));
static void fail(u32 line)
{
    output("line: ", 6); number(line); output("case: ", 6); number(current);
    __asm__ volatile("int $0x80" : : "a"(1), "b"(73) : "memory");
    __builtin_unreachable();
}
#define CHECK(c) do { if (!(c)) fail(__LINE__); } while (0)
static void expected_write(u32 offset, u32 value, u32 size)
{
    u32 i;
    for (i = 0; i < size; i++) expected[offset + i] = value >> (8 * i);
}
void SetGeomOffset(long x, long y)
{
    struct ProjectionOut *p = (struct ProjectionOut *)(SCRATCH + test->output);
    CHECK(stage++ == 0 && x == test->x && y == test->y);
    CHECK(p->f0 == (s16)x && p->f2 == (s16)y && p->f4 == 0);
}
void SetGeomScreen(long distance)
{ CHECK(stage++ == 1 && distance == 300); }
MATRIX *RotMatrixZYX_gte(SVECTOR *angles, MATRIX *matrix)
{
    u32 i;
    CHECK(stage++ == 2 && angles == VECTORS && matrix == MATRIX_PTR);
    CHECK(angles->vx == (s16)((test->attribute & 255) * 16));
    CHECK(angles->vy == (s16)(((test->attribute >> 8) & 255) * 16));
    CHECK(angles->vz == (s16)(((test->attribute >> 16) & 255) * 16));
    for (i = 0; i < 9; i++) matrix->m[i / 3][i % 3] = (s32)i * 17 - 50;
    return matrix;
}
void GsSetLsMatrix(MATRIX *matrix)
{
    u32 i;
    CHECK(stage++ == 3 && matrix == MATRIX_PTR);
    CHECK(matrix->t[0] == 0 && matrix->t[1] == 0 && matrix->t[2] == 300);
    for (i = 0; i < 9; i++) CHECK(matrix->m[i / 3][i % 3] == (s32)i * 17 - 50);
}
MATRIX *ScaleMatrix(MATRIX *matrix, VECTOR *scale)
{
    return matrix;
}
void SetRotMatrix(MATRIX *matrix)
{
}
void RotAverageNclip3_nom(SVECTOR *a, SVECTOR *b, SVECTOR *c)
{
    CHECK(stage++ == 4 && a == VECTORS && b == VECTORS + 1 && c == VECTORS + 2);
    CHECK(a->vx == 0 && a->vy == 0 && a->vz == 0);
    CHECK(b->vx == 512 && b->vy == 0 && b->vz == 0);
    CHECK(c->vx == 0 && c->vy == 512 && c->vz == 0);
}
void fixture_read_gte(s32 *value, s32 reg)
{
    CHECK(stage++ == 5 && reg == 24);
    CHECK(value != 0 && ((u32)value & 3) == 0);
    *value = test->clip;
}
int main(void)
{
    u32 i, j, vector;
    for (current = 0; current < sizeof(cases) / sizeof(cases[0]); current++) {
        test = &cases[current]; stage = 0;
        for (i = 0; i < 1024; i++) expected[i] = SCRATCH[i] = (i * 37 + 19) & 255;
        expected_write(test->output, test->x, 2);
        expected_write(test->output + 2, test->y, 2);
        expected_write(test->output + 4, 0, 4);
        for (i = 0; i < 3; i++) expected_write(0x308 + i * 2, ((test->attribute >> (8 * i)) & 255) * 16, 2);
        for (i = 0; i < 9; i++) expected_write(0x2D0 + i * 2, (s32)i * 17 - 50, 2);
        expected_write(0x2E4, 0, 4);
        expected_write(0x2E8, 0, 4);
        expected_write(0x2EC, 300, 4);
        for (vector = 0; vector < 3; vector++) {
            for (j = 0; j < 3; j++) {
                expected_write(0x308 + vector * 8 + j * 2,
                    (vector == 1 && j == 0) || (vector == 2 && j == 1) ? 512 : 0, 2);
            }
        }
        CHECK(func_80041E7C(test->attribute, test->x, test->y,
              (struct ProjectionOut *)(SCRATCH + test->output)) == test->clip);
        CHECK(stage == 6);
        for (i = 0; i < 1024; i++) CHECK(SCRATCH[i] == expected[i]);
    }
    output("projection cases: ", 18); number(current);
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires Linux x86, a host compiler, and ILP32 execution",
)
class ProjectionClipHelperTests(unittest.TestCase):
    def build_and_run(self, optimization, mutation=""):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        scratch = ROOT / "tmp/test-projection-clip-helper"
        scratch.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(dir=scratch) as temporary:
            directory = Path(temporary)
            text = SOURCE.read_text()
            self.assertNotIn("libgte_abi_variants.h", text)
            self.assertIn("RotAverageNclip3_nom(v308, v310, v318);", text)
            self.assertIn("SetGeomScreen(MODEL_DEFAULT_PROJECTION);", text)
            self.assertIn("mtx->t[2] = MODEL_DEFAULT_PROJECTION;", text)
            if mutation:
                edits = {
                    "angle-byte": ("(arg0 >> 4)", "(arg0 >> 8)"),
                    "translation": ("mtx->t[2] = MODEL_DEFAULT_PROJECTION;", "mtx->t[2] = 0;"),
                    "corner": ("v308[1].vx = 0x200;", "v308[1].vx = 0x100;"),
                    "output": ("arg3->f4 = 0;", "arg3->f4 = 1;"),
                    "depth-register": ("gte_stopz(p);", "gte_stotz(p);"),
                }
                old, new = edits[mutation]
                self.assertIn(old, text)
                text = text.replace(old, new)
            text = text.replace('#include "../psyq/inline_c.h"', '#include "gte_shim.h"')
            text = re.sub(
                r'^#include "([^"]+)"',
                lambda match: match[0] if match[1] == "gte_shim.h" else
                '#include "' + str((SOURCE.parent / match[1]).resolve()) + '"',
                text, flags=re.MULTILINE,
            )
            (directory / "game.c").write_text(text)
            (directory / "gte_shim.h").write_text(SHIM)
            rows = ",\n".join("{" + ",".join(str(value) for value in case) + "}" for case in cases())
            (directory / "fixture.c").write_text(WITNESS.replace("@CASES@", rows))
            (directory / "start.S").write_text(START)
            flags = [
                "-std=gnu89", "-m32", "-nostdlib", "-fno-pie", "-no-pie",
                "-fno-stack-protector", "-ffreestanding", "-fno-builtin",
                "-fno-strict-aliasing", "-D_LANGUAGE_C",
                "-Werror=implicit-function-declaration", optimization, "-I", str(ROOT),
            ]
            objects = []
            for name in ("game", "fixture", "start"):
                path = directory / (name + (".S" if name == "start" else ".c"))
                obj = directory / (name + ".o")
                result = subprocess.run(["cc", *flags, "-c", str(path), "-o", str(obj)],
                                        capture_output=True, text=True, timeout=60)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                objects.append(str(obj))
            binary = directory / "witness"
            result = subprocess.run(["cc", *flags, *objects, "-o", str(binary)],
                                    capture_output=True, text=True, timeout=60)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            return subprocess.run([str(binary)], capture_output=True, text=True, timeout=30)

    def test_scratchpad_and_geometry_protocol(self):
        self.assertEqual(len(list(cases())), 402)
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.build_and_run(optimization)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(result.stdout, "projection cases: 402\n")

    def test_semantic_mutations_are_rejected(self):
        for mutation in ("angle-byte", "translation", "corner", "output", "depth-register"):
            with self.subTest(mutation=mutation):
                result = self.build_and_run("-O2", mutation)
                self.assertEqual(result.returncode, 73, result.stdout + result.stderr)
                self.assertIn("case: ", result.stdout)


if __name__ == "__main__":
    unittest.main()
