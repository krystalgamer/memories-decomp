from __future__ import annotations

import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
SOURCE = REPOSITORY / "src/game/model_interpolate_transform.c"


@unittest.skipUnless(os.name == "posix" and shutil.which("cc"), "requires a POSIX host C compiler")
class AnimationAngleWrapTests(unittest.TestCase):
    def run_witness(self, mutation: str | None = None) -> subprocess.CompletedProcess[str]:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        source = SOURCE.read_text()
        start = source.index("    if (dur == 0x10) {")
        depth = 0
        end = None
        for position in range(source.index("{", start), len(source)):
            if source[position] == "{":
                depth += 1
            elif source[position] == "}":
                depth -= 1
                if depth == 0:
                    end = position + 1
                    break
        self.assertIsNotNone(end)
        block = source[start:end]
        if mutation == "zero-direction":
            self.assertIn("if (w > 0)", block)
            block = block.replace("if (w > 0)", "if (w >= 0)", 1)
        elif mutation == "half-turn-boundary":
            self.assertIn("d >= MODEL_ANGLE_WRAP_THRESHOLD", block)
            block = block.replace(
                "d >= MODEL_ANGLE_WRAP_THRESHOLD",
                "d >= MODEL_ANGLE_HALF_TURN", 1,
            )
        elif mutation is not None:
            raise ValueError(mutation)

        model = (REPOSITORY / "src/game/model.h").read_text()
        definitions = []
        for name in ("MODEL_ANGLE_FULL_TURN", "MODEL_ANGLE_HALF_TURN", "MODEL_ANGLE_WRAP_THRESHOLD"):
            match = re.search(r"^#define " + name + r" .+$", model, re.MULTILINE)
            self.assertIsNotNone(match)
            definitions.append(match.group(0))
        witness = (
            '#include <stdio.h>\n#include "src/types.h"\n'
            + "\n".join(definitions)
            + r"""
static s32 reference(s32 current, s32 previous, s32 duration)
{
    s32 delta = (s16)current - (s16)previous;
    if (duration == 16 &&
        (delta >= MODEL_ANGLE_WRAP_THRESHOLD || delta <= -MODEL_ANGLE_WRAP_THRESHOLD)) {
        s32 angle = (s16)previous;
        return angle <= 0 ? angle + MODEL_ANGLE_FULL_TURN : angle - MODEL_ANGLE_FULL_TURN;
    }
    return previous;
}

static void actual(s32 a, s32 b, s32 c, s32 rx, s32 ry, s32 rz, s32 dur, s32 *out)
{
    s32 w;
"""
            + block
            + r"""
    out[0] = rx;
    out[1] = ry;
    out[2] = rz;
}

int main(void)
{
    static const s32 deltas[] = {-2050, -2049, -2048, -1, 0, 1, 2048, 2049, 2050};
    u32 current;
    u32 cases = 0;
    unsigned offset;
    s32 duration;
    for (current = 0; current < 65536; current++) {
        for (offset = 0; offset < sizeof(deltas) / sizeof(deltas[0]); offset++) {
            u16 previous = current + deltas[offset];
            for (duration = 15; duration <= 16; duration++) {
                s32 out[3];
                actual(current, previous, current ^ 0x8000,
                       previous, current, previous ^ 0x8000, duration, out);
                if (out[0] != reference(current, previous, duration) ||
                    out[1] != reference(previous, current, duration) ||
                    out[2] != reference(current ^ 0x8000, previous ^ 0x8000, duration)) {
                    fprintf(stderr, "wrap mismatch: %u %u %d\n", current, previous, duration);
                    return 1;
                }
                cases++;
            }
        }
    }
    printf("%u\n", cases);
    return 0;
}
"""
        )
        with tempfile.TemporaryDirectory(prefix="animation-wrap-", dir=REPOSITORY / "tmp") as temporary:
            directory = Path(temporary)
            path = directory / "witness.c"
            binary = directory / "witness"
            path.write_text(witness)
            subprocess.run(
                ["cc", "-std=c99", "-O2", "-I", str(REPOSITORY), str(path), "-o", str(binary)],
                check=True, capture_output=True, text=True, timeout=30,
            )
            return subprocess.run([str(binary)], capture_output=True, text=True, timeout=30)

    def test_actual_wrap_block_preserves_boundaries_and_duration_gate(self):
        result = self.run_witness()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stdout, "1179648\n")

    def test_zero_angle_direction_negative_control(self):
        result = self.run_witness("zero-direction")
        self.assertEqual(result.returncode, 1)
        self.assertIn("wrap mismatch:", result.stderr)

    def test_exact_half_turn_negative_control(self):
        result = self.run_witness("half-turn-boundary")
        self.assertEqual(result.returncode, 1)
        self.assertIn("wrap mismatch:", result.stderr)
