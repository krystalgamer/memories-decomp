from __future__ import annotations

import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
CALLER = REPOSITORY / "src/game/func_8005E808.c"
COPIER = REPOSITORY / "src/game/model_slot_support.c"


@unittest.skipUnless(os.name == "posix" and shutil.which("cc"), "requires a POSIX host C compiler")
class KeyframeCoordinateExtentTests(unittest.TestCase):
    def run_witness(self, narrow_buffer: bool) -> subprocess.CompletedProcess[str]:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        copier = re.search(
            r"void Model_CopySlotU16Values\([^{}]+\)\s*\{[^{}]+\}",
            COPIER.read_text(),
        )
        self.assertIsNotNone(copier)
        copier_source = copier.group(0)
        self.assertEqual(
            re.findall(r"out\[(\d+)\]\s*=", copier_source),
            ["0", "1", "2", "3"],
        )
        with tempfile.TemporaryDirectory(
            prefix="keyframe-extent-", dir=REPOSITORY / "tmp"
        ) as temporary:
            directory = Path(temporary)
            caller = CALLER
            if narrow_buffer:
                text = CALLER.read_text()
                self.assertEqual(text.count("s16 pos[4];"), 1)
                text = text.replace("s16 pos[4];", "s16 pos[3];")
                text = re.sub(
                    r'^#include "([^"]+)"',
                    lambda match: '#include "' + str(
                        (CALLER.parent / match.group(1)).resolve()
                    ) + '"',
                    text,
                    flags=re.MULTILINE,
                )
                caller = directory / "narrow-caller.c"
                caller.write_text(text)
            witness = directory / "witness.c"
            witness.write_text(
                r"""
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/types.h"
#include "src/game/model_copy_slot_u16_values.h"

static void checked_copy(s32 slot, u16 *out, size_t extent);
#define Model_CopySlotU16Values(slot, out) \
    checked_copy((slot), (out), __builtin_object_size((out), 0))
"""
                + '#include "' + str(caller) + '"\n'
                + r"""
#undef Model_CopySlotU16Values

/* Only the copier's input field is needed by this host fixture. */
typedef struct { u16 field_DD0[4]; } ModelSlot;
static ModelSlot D_800F2C40[2] = {
    {{0xFFFE, 0xFFFD, 0xFFFC, 0x1357}},
    {{0x8000, 0xFFFF, 0, 0x2468}}
};
SVECTOR D_800F5768[CAMERA_POSE_VECTOR_COUNT];
u8 *D_8009B074;
static unsigned copies;
static unsigned roots;
"""
                + copier_source
                + r"""

static void checked_copy(s32 slot, u16 *out, size_t extent)
{
    if (extent != 4 * sizeof(u16)) {
        fprintf(stderr, "coordinate extent: %zu\n", extent);
        exit(7);
    }
    if (slot < 0 || slot >= 2) exit(8);
    out[3] = (u16)~D_800F2C40[slot].field_DD0[3];
    Model_CopySlotU16Values(slot, out);
    if (*(volatile u16 *)(out + 3) != D_800F2C40[slot].field_DD0[3]) exit(9);
    copies++;
}

int func_80058DD8(int slot)
{
    return slot >= 0 && slot < 2;
}

void func_8005FB30(u8 *data)
{
    (void)data;
}

void func_8005EBF4(Key *cur, s32 k, s32 scale, s32 den, s16 *out)
{
    (void)cur; (void)k; (void)scale; (void)den; (void)out;
    exit(10);
}

long SquareRoot0(long value)
{
    static const long expected[] = {14, 1073676290};
    if (roots >= 2 || value != expected[roots]) exit(11);
    roots++;
    return 0;
}

int main(void)
{
    ModelKeyframeTimingView key = {0};
    const s16 channels[2][4] = {
        {1, 1, 1, 0x80},
        {0, 0, 0, 0x81}
    };
    memcpy(key.pad_00, channels, sizeof(channels));
    key.field_20 = -1;
    D_8009B074 = (u8 *)&key;
    D_800F5768[1].vx = -1;
    func_8005E808((u8 *)&key);
    if (copies != 2 || roots != 2 || key.field_26 != 1) return 12;
    if (key.field_22 != 2 || key.field_24 != 0) return 13;
    puts("two four-halfword copies; signed coordinate arithmetic preserved");
    return 0;
}
"""
            )
            executable = directory / "witness"
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            compilation = subprocess.run(
                [
                    shutil.which("cc"), "-std=gnu99", "-O2",
                    "-fno-strict-aliasing", "-Werror=incompatible-pointer-types",
                    "-I", str(REPOSITORY), str(witness), "-o", str(executable),
                ],
                cwd=REPOSITORY, env=environment,
                capture_output=True, text=True,
            )
            self.assertEqual(
                compilation.returncode, 0, compilation.stdout + compilation.stderr
            )
            return subprocess.run(
                [str(executable)], cwd=REPOSITORY, env=environment,
                capture_output=True, text=True,
            )

    def test_ready_slots_copy_four_halfwords_with_signed_coordinates(self) -> None:
        result = self.run_witness(narrow_buffer=False)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("two four-halfword copies", result.stdout)

    def test_three_halfword_buffer_is_rejected(self) -> None:
        result = self.run_witness(narrow_buffer=True)
        self.assertEqual(result.returncode, 7, result.stdout + result.stderr)
        self.assertIn("coordinate extent: 6", result.stderr)


if __name__ == "__main__":
    unittest.main()
