"""ILP32 witnesses for the real view-delta TU and its overlapping COMMON names.

Only valid model records with positive unsigned-halfword moduli are exercised.
The independent oracle checks signed arithmetic, update gates, the complete
eight-byte snapshot copy, and preservation of all other guarded storage.
"""

from __future__ import annotations

import os
from pathlib import Path
import platform
import re
import shlex
import shutil
import subprocess
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
SOURCE = REPOSITORY / "src/game/func_8004E7B0.c"
SEMANTIC_YAW_MISMATCH = 20
EXPECTED_CASES = 1 + 4 * 4 * 65536 + 65535 + 16 * 16 * 3 * 7 + 24 * 24 * 9 * 3
START = """
.text
.globl _start
_start:
    and $-16, %esp
    call main
    mov %eax, %ebx
    mov $1, %eax
    int $0x80
.section .note.GNU-stack,"",@progbits
"""

ALIASES = {
    "D_8009AF88": "graphics_storage+16",
    "D_8009AF8E": "graphics_storage+22",
    "D_8009AF90": "graphics_storage+24",
    "D_8009B478": "snapshot_storage+16",
    "D_8009B47A": "snapshot_storage+18",
    "D_8009B47C": "snapshot_storage+20",
    "D_8009B480": "snapshot_storage+24",
    "D_8009B482": "snapshot_storage+26",
    "D_8009B484": "snapshot_storage+28",
}

WITNESS = r"""
#include "src/types.h"
#include "src/game/model.h"
#include "src/game/model_graphics_state.h"
#include "src/game/func_8004E7B0.h"

extern ModelBytes8 D_8009B478, D_8009B480;
extern s16 D_8009B47A, D_8009B47C, D_8009B482, D_8009B484;

u8 graphics_storage[16 + 36 + 16] __attribute__((aligned(16)));
u8 snapshot_storage[16 + 16 + 16] __attribute__((aligned(16)));
static u8 model_storage[16 + 0xB2 + 16] __attribute__((aligned(16)));
static u8 expected_graphics[sizeof(graphics_storage)];
static u8 expected_snapshot[sizeof(snapshot_storage)];
static u8 expected_model[sizeof(model_storage)];
static u32 cases;

typedef char require_ilp32[
    sizeof(void *) == 4 && sizeof(s32) == 4 && sizeof(s16) == 2 &&
    sizeof(ModelBytes8) == 8 ? 1 : -1];

static void copy(u8 *to, const u8 *from, u32 size)
{
    while (size--) *to++ = *from++;
}

static s32 equal(const u8 *a, const u8 *b, u32 size)
{
    while (size--) if (*a++ != *b++) return 0;
    return 1;
}

static void fill(u8 *to, u32 size, u32 salt)
{
    u32 i;
    for (i = 0; i < size; i++)
        to[i] = (u8)(i * 73 + salt * 19 + (salt >> 8));
}

static s32 halfword(const u8 *p)
{
    u32 value = p[0] | ((u32)p[1] << 8);
    return value < 32768 ? (s32)value : (s32)value - 65536;
}

static void put_halfword(u8 *p, s32 value)
{
    p[0] = (u8)value;
    p[1] = (u8)((u32)value >> 8);
}

static s32 aliases_are_real(void)
{
    /* Volatile locals prevent folding comparisons of distinct C symbols:
     * the linker deliberately overlays snapshot members and block symbols. */
    const void *volatile actual[] = {
        &D_8009AF88, &D_8009AF8E, &D_8009AF90,
        &D_8009B478, &D_8009B47A, &D_8009B47C,
        &D_8009B480, &D_8009B482, &D_8009B484
    };
    const void *volatile expected[] = {
        graphics_storage + 16, graphics_storage + 22, graphics_storage + 24,
        snapshot_storage + 16, snapshot_storage + 18, snapshot_storage + 20,
        snapshot_storage + 24, snapshot_storage + 26, snapshot_storage + 28
    };
    u32 i;
    for (i = 0; i < 9; i++) if (actual[i] != expected[i]) return 0;
    return 1;
}

static void oracle(s32 force, u32 modulus)
{
    s32 current_yaw = halfword(expected_snapshot + 18);
    s32 previous_yaw = halfword(expected_snapshot + 26);
    s32 pitch = halfword(expected_snapshot + 20);
    s32 previous_pitch = halfword(expected_snapshot + 28);
    s32 delta = current_yaw - previous_yaw;
    s32 value, distance;

    if (delta > 2048) delta -= 4096;
    else if (delta < -2048) delta += 4096;
    if (force || delta) {
        /* Signed / and % intentionally preserve C truncation toward zero,
         * including negative remainders and the final halfword truncation. */
        value = delta * 1280 / 384 + halfword(expected_graphics + 22);
        value = (value + (s32)modulus) % (s32)modulus;
        put_halfword(expected_graphics + 22, value);
    }
    if (force || pitch != previous_pitch) {
        if (pitch <= 2048) {
            value = pitch < 1024 ? pitch * 26 / 85 : 313;
        } else {
            distance = pitch > 4096 ? pitch - 4096 : 4096 - pitch;
            value = distance >= 1024 ? -313 : -distance * 26 / 85;
        }
        put_halfword(expected_graphics + 24, value);
    }
    copy(expected_snapshot + 24, expected_snapshot + 16, 8);
}

static s32 check(s32 cy, s32 py, s32 cp, s32 pp,
                 s32 force, u32 modulus, s32 old_yaw, s32 old_pitch)
{
    u8 *record = model_storage + 16;
    const void *volatile actual_record;
    const void *volatile expected_record = record;
    cases++;
    fill(graphics_storage, sizeof(graphics_storage), cases);
    fill(snapshot_storage, sizeof(snapshot_storage), cases + 17);
    fill(model_storage, sizeof(model_storage), cases + 29);
    copy(graphics_storage + 16, (const u8 *)&record, sizeof(record));
    put_halfword(graphics_storage + 22, old_yaw);
    put_halfword(graphics_storage + 24, old_pitch);
    put_halfword(record + 0xA6, (s32)modulus);
    put_halfword(snapshot_storage + 18, cy);
    put_halfword(snapshot_storage + 26, py);
    put_halfword(snapshot_storage + 20, cp);
    put_halfword(snapshot_storage + 28, pp);
    /* Give both non-angle halfwords distinct old/current values. */
    snapshot_storage[24] = snapshot_storage[16] ^ 0xFF;
    snapshot_storage[25] = snapshot_storage[17] ^ 0xFF;
    snapshot_storage[30] = snapshot_storage[22] ^ 0xFF;
    snapshot_storage[31] = snapshot_storage[23] ^ 0xFF;
    actual_record = D_8009AF88;
    if (actual_record != expected_record) return 11;
    copy(expected_graphics, graphics_storage, sizeof(graphics_storage));
    copy(expected_snapshot, snapshot_storage, sizeof(snapshot_storage));
    copy(expected_model, model_storage, sizeof(model_storage));
    oracle(force, modulus);
    func_8004E7B0(force);
    if (!equal(graphics_storage + 22, expected_graphics + 22, 2)) return 20;
    if (!equal(graphics_storage + 24, expected_graphics + 24, 2)) return 21;
    if (!equal(snapshot_storage, expected_snapshot, sizeof(snapshot_storage))) return 22;
    if (!equal(graphics_storage, expected_graphics, sizeof(graphics_storage))) return 23;
    if (!equal(model_storage, expected_model, sizeof(model_storage))) return 24;
    return 0;
}

static void report_cases(void)
{
    char output[32] = "cases=";
    char digits[10];
    u32 value = cases, length = 6, n = 0;
    do { digits[n++] = '0' + value % 10; value /= 10; } while (value);
    while (n) output[length++] = digits[--n];
    output[length++] = '\n';
    __asm__ volatile("int $0x80" : : "a"(4), "b"(1), "c"(output),
                     "d"(length) : "memory", "cc");
}

static const s32 yaw[] = {
    -32768, -4097, -4096, -2049, -2048, -2047, -1, 0,
    1, 2047, 2048, 2049, 4095, 4096, 4097, 32767
};
static const s32 pitch[] = {
    -32768, -4096, -2049, -1024, -1, 0, 1,
    1023, 1024, 1025, 2047, 2048, 2049, 3071, 3072, 3073,
    4095, 4096, 4097, 5119, 5120, 5121, 32766, 32767
};
static const s32 deltas[] = {-4096, -2049, -2048, -2047, 0, 2047, 2048, 2049, 4096};
static const s32 accumulators[] = {-32768, -32767, -1, 0, 1, 32766, 32767};
static const u32 moduli[] = {1, 2, 3, 384, 32768, 65534, 65535};
static const s32 forces[] = {0, 1, -32768};

#define CHECK(cy, py, cp, pp, force, modulus, old_yaw, old_pitch) do { \
    s32 error = check(cy, py, cp, pp, force, modulus, old_yaw, old_pitch); \
    if (error) return error; \
} while (0)

int main(void)
{
    s32 configuration, axis, value, i, j, k, f, m;
    if (!aliases_are_real()) return 10;
    /* Safe mutation witness: 2048 must not wrap, and modulus is nonzero. */
    CHECK(2048, 0, 0, 0, 0, 65535, 0, -32768);
    for (configuration = 0; configuration < 4; configuration++) {
        for (axis = 0; axis < 4; axis++) {
            for (value = -32768; value <= 32767; value++) {
                s32 base = configuration == 0 ? -32768 :
                           configuration == 1 ? 0 :
                           configuration == 2 ? 2048 : 32767;
                CHECK(axis == 0 ? value : base, axis == 1 ? value : base,
                      axis == 2 ? value : base, axis == 3 ? value : base,
                      configuration < 2 ? 0 : forces[configuration - 1],
                      moduli[(configuration + axis) % 7],
                      accumulators[(configuration + axis) % 7],
                      accumulators[(configuration + axis + 3) % 7]);
            }
        }
    }
    for (m = 1; m <= 65535; m++)
        CHECK(yaw[m % 16], yaw[(m / 16) % 16],
              pitch[m % 24], pitch[(m / 24) % 24], forces[m % 3],
              (u32)m, accumulators[m % 7], accumulators[(m / 7) % 7]);
    for (i = 0; i < 16; i++)
        for (j = 0; j < 16; j++)
            for (f = 0; f < 3; f++)
                for (m = 0; m < 7; m++)
                    CHECK(yaw[i], yaw[j], pitch[(i + j) % 24], pitch[j],
                          forces[f], moduli[m], accumulators[m],
                          accumulators[(i + j + f) % 7]);
    for (i = 0; i < 24; i++)
        for (j = 0; j < 24; j++)
            for (k = 0; k < 9; k++)
                for (f = 0; f < 3; f++)
                    CHECK(deltas[k], 0, pitch[i], pitch[j], forces[f],
                          moduli[(i + j + k) % 7], accumulators[(i + k) % 7],
                          accumulators[(j + f) % 7]);
    report_cases();
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host C compiler and ILP32 execution",
)
class ModelViewDeltaTests(unittest.TestCase):
    def run_witness(self, optimization: str, mutation: bool = False):
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        (REPOSITORY / "tmp").mkdir(exist_ok=True)
        with tempfile.TemporaryDirectory(
            prefix="model-view-delta-", dir=REPOSITORY / "tmp"
        ) as temporary:
            directory = Path(temporary)
            source = SOURCE
            if mutation:
                text = SOURCE.read_text()
                predicate = "__builtin_abs(dy) >= 0x801"
                self.assertEqual(text.count(predicate), 1)
                text = text.replace(predicate, "__builtin_abs(dy) >= 0x800")
                text = re.sub(
                    r'^#include "([^"]+)"',
                    lambda match: '#include "'
                    + str((SOURCE.parent / match[1]).resolve())
                    + '"',
                    text,
                    flags=re.MULTILINE,
                )
                source = directory / "wrong-yaw-boundary.c"
                source.write_text(text)
            witness = directory / "witness.c"
            witness.write_text(WITNESS)
            startup = directory / "start.S"
            startup.write_text(START)
            game_object = directory / "game.o"
            witness_object = directory / "witness.o"
            binary = directory / "witness"
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            flags = [
                "cc", "-m32", optimization, "-std=c99", "-ffreestanding",
                "-fno-builtin", "-fno-strict-aliasing", "-fno-pie", "-no-pie",
                "-fno-stack-protector", "-nostdlib",
            ]
            # Keep the actual game TU separate and its tentative definitions
            # intact. COMMON lets the linker aliases supply the real storage.
            commands = [
                flags + ["-fcommon", "-c", str(source), "-o", str(game_object)],
                flags + ["-I", str(REPOSITORY), "-c", str(witness),
                         "-o", str(witness_object)],
                flags + [str(game_object), str(witness_object), str(startup)]
                + [f"-Wl,--defsym,{name}={alias}" for name, alias in ALIASES.items()]
                + ["-o", str(binary)],
            ]
            for command in commands:
                result = subprocess.run(
                    command, capture_output=True, text=True, timeout=60,
                    env=environment, cwd=REPOSITORY,
                )
                self.assertEqual(
                    result.returncode, 0, shlex.join(command) + "\n" + result.stderr
                )
            return subprocess.run(
                [str(binary)], capture_output=True, text=True, timeout=120,
                env=environment, cwd=REPOSITORY,
            )

    def test_actual_view_delta_matches_oracle_at_o0_and_o2(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization)
                self.assertEqual(
                    result.returncode, 0,
                    f"{optimization}: exit {result.returncode}; "
                    "10/11=layout, 20=yaw, 21=pitch, 22=snapshot, "
                    f"23=graphics preservation, 24=model preservation\n{result.stderr}",
                )
                self.assertEqual(result.stdout, f"cases={EXPECTED_CASES}\n")

    def test_wrong_2048_wrap_boundary_is_semantically_rejected(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization, mutation=True)
                self.assertEqual(
                    result.returncode, SEMANTIC_YAW_MISMATCH,
                    f"{optimization}: expected yaw mismatch, not crash or layout "
                    f"failure; exit {result.returncode}\n{result.stderr}",
                )


if __name__ == "__main__":
    unittest.main()
