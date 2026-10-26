"""Check the resident outline owners and the evidence behind their names."""

import csv
import json
import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest

from tools.project.tests.test_library_display_objects import START


ROOT = Path(__file__).resolve().parents[3]
HEADERS = r"""
#include "src/types.h"
#include "src/game/display_object.h"
#include "src/game/display_object_core.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
"""
FADE_WITNESS = HEADERS + r"""
typedef struct { u8 before[16]; DisplayObject object; u8 after[16]; } GuardedObject;
static GuardedObject block;
static u8 expected[sizeof(block)];
static u32 releases, bad_pointer;
void run_fade(DisplayObject *o);
void DisplayObject_ReleaseIfPresent(void *object)
{
    if (object != &block.object) bad_pointer = 1;
    releases++;
}
static void put16(u8 *p, u32 value) { p[0] = value; p[1] = value >> 8; }
int main(void)
{
    u32 value, i, half, signed_release, offset, colour, cases = 0, step;
    offset = (u8 *)&block.object.field_60 - (u8 *)&block;
    colour = (u8 *)&block.object.rgb + 2 - (u8 *)&block;
    for (value = 0; value < 65536; value++) {
        for (i = 0; i < sizeof(block); i++)
            ((u8 *)&block)[i] = i * 37 + value;
        put16((u8 *)&block + offset, value);
        for (i = 0; i < sizeof(block); i++) expected[i] = ((u8 *)&block)[i];
        half = (value - 16) & 0xFFFF;
        signed_release = half == 0 || (half & 0x8000) != 0;
        put16(expected + offset, half);
        if (!signed_release) expected[colour] = half;
        releases = bad_pointer = 0;
        run_fade(&block.object);
        if (bad_pointer || releases != signed_release) return 1;
        for (i = 0; i < sizeof(block); i++)
            if (((u8 *)&block)[i] != expected[i]) return 2;
        cases++;
    }
    for (i = 0; i < sizeof(block); i++) ((u8 *)&block)[i] = 0;
    put16((u8 *)&block + offset, 0x100);
    releases = bad_pointer = 0;
    for (step = 1; step <= 16; step++) {
        run_fade(&block.object);
        if (bad_pointer || releases != (step == 16)) return 3;
    }
    return cases == 65536 ? 0 : 4;
}
"""


class LibraryCardOutlineTests(unittest.TestCase):
    def test_regional_owners_share_one_grouped_unit(self):
        pairs = {
            "slus_01411": (0x8002A9C0, 0x8002ABB4),
            "slpm_86398": (0x8002A7A0, 0x8002A994),
            "sles_03947": (0x8002AA0C, 0x8002AC00),
            "sles_03948": (0x8002AB50, 0x8002AD44),
            "sles_03949": (0x8002AB50, 0x8002AD44),
            "sles_03950": (0x8002AB50, 0x8002AD44),
            "sles_03951": (0x8002AB50, 0x8002AD44),
        }
        owners = (
            ("Library_DrawFadingCardOutline", 0x1F4),
            ("Library_CreateFadingCardOutline", 0xF0),
        )
        profile = "gcc_2_8_1_g8_split"
        wrappers = {"slus_01411": "", "slpm_86398": "japanese/"}
        for region, addresses in pairs.items():
            directory = ROOT / "config" / region
            manifest = json.loads((directory / "matching_c.json").read_text())
            with (directory / "functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            self.assertEqual(addresses[1] - addresses[0], 0x1F4)
            source = f"src/game/{wrappers.get(region, 'european/')}library_runtime.c"
            for address, (name, size) in zip(addresses, owners):
                with self.subTest(region=region, function=name):
                    entry, = [
                        item for item in manifest["functions"]
                        if int(item["address"], 0) == address
                    ]
                    self.assertEqual(entry["source"], source)
                    self.assertEqual(int(entry["size"], 0), size)
                    self.assertEqual(entry["profile"], profile)
                    self.assertEqual(
                        sum(item["source"] == source for item in manifest["functions"]), 9
                    )
                    row, = [
                        item for item in inventory if int(item["address"], 0) == address
                    ]
                    self.assertEqual(row["name"], name)
                    self.assertEqual(row["module"], "game")
                    self.assertEqual(row["status"], "matching_c")
        for address in ("8002A9C0", "8002ABB4"):
            for suffix in ("c", "h"):
                self.assertFalse((ROOT / f"src/game/func_{address}.{suffix}").exists())


    @unittest.skipUnless(
        platform.system() == "Linux"
        and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"),
        "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_fade_prefix_and_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        draw = (ROOT / "src/game/library_runtime.c").read_text()
        prefix = re.search(
            r"(?ms)^    n = \*\(u16 \*\)&o->field_60 - 0x10;.*?^    \}",
            draw,
        )
        colour = re.search(
            r"(?m)^    \(\(u8 \*\)&o->rgb\)\[2\] = \*\(u8 \*\)&o->field_60;",
            draw,
        )
        self.assertIsNotNone(prefix)
        self.assertIsNotNone(colour)
        fragment = prefix.group() + "\n" + colour.group()
        variants = (
            ("-O0", None, 0),
            ("-O2", None, 0),
            ("-O2", "double-step", 2),
            ("-O2", "unsigned-release", 1),
            ("-O2", "wrong-colour-byte", 2),
        )
        for optimization, mutation, expected in variants:
            with self.subTest(optimization=optimization, mutation=mutation):
                code = fragment
                if mutation == "double-step":
                    code = code.replace(" - 0x10;", " - 0x20;")
                elif mutation == "unsigned-release":
                    code = code.replace("if ((s16)n <= 0)", "if (n <= 0)")
                elif mutation == "wrong-colour-byte":
                    code = code.replace("&o->rgb)[2]", "&o->rgb)[0]")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, fragment)
                with tempfile.TemporaryDirectory(
                    prefix="library-outline-fade-", dir=ROOT / "tmp"
                ) as name:
                    directory = Path(name)
                    source = directory / "operations.c"
                    source.write_text(
                        HEADERS + "\nvoid run_fade(DisplayObject *o) { s32 n;\n"
                        + code + "\n}\n"
                    )
                    witness = directory / "witness.c"
                    witness.write_text(FADE_WITNESS)
                    start = directory / "start.S"
                    start.write_text(START)
                    binary = directory / "witness"
                    environment = dict(os.environ, TMPDIR=str(directory))
                    result = subprocess.run(
                        [
                            shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                            "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                            "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                            "-Werror=incompatible-pointer-types", "-I", str(ROOT),
                            str(source), str(witness), str(start), "-o", str(binary),
                        ],
                        cwd=ROOT, env=environment, text=True,
                        capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run(
                        [str(binary)], cwd=ROOT, env=environment, timeout=180
                    )
                    self.assertEqual(result.returncode, expected)


if __name__ == "__main__":
    unittest.main()
