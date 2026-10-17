"""Guard the actual resource capture/stores, not the complete battle controller."""

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
#include "src/game/duel_effect_resource_record.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef char meter_36[(u32)&((DuelEffectResourceRecord *)0)->field_36 == 0x36
                     && sizeof(((DuelEffectResourceRecord *)0)->field_36) == 2 ? 1 : -1];
typedef char meter_38[(u32)&((DuelEffectResourceRecord *)0)->field_38 == 0x38
                     && sizeof(((DuelEffectResourceRecord *)0)->field_38) == 2 ? 1 : -1];
typedef struct {
    u8 before[16];
    DuelEffectResourceRecord records[4];
    u8 after[16];
} GuardedRecords;
extern GuardedRecords test_records;
extern s8 D_8009B1B9;
extern u16 D_8009B1D0;
#define D_800EA0E8 test_records.records
"""
WITNESS = HEADERS + r"""
GuardedRecords test_records;
s8 D_8009B1B9;
u16 D_8009B1D0;
static u8 expected[sizeof(test_records)];
void write_meter(void);
static void put16(u8 *p, u32 value) { p[0] = value; p[1] = value >> 8; }
int main(void)
{
    u32 slot, value, i, offset, cases = 0;
    for (slot = 0; slot < 4; slot++) {
        D_8009B1B9 = slot;
        for (value = 0; value < 65536; value++) {
            for (i = 0; i < sizeof(test_records); i++) {
                ((u8 *)&test_records)[i] = i * 37 + slot * 13 + value;
                expected[i] = ((u8 *)&test_records)[i];
            }
            D_8009B1D0 = value;
            offset = (u8 *)&test_records.records[slot] - (u8 *)&test_records;
            put16(expected + offset + 0x36, value);
            put16(expected + offset + 0x38, value);
            write_meter();
            for (i = 0; i < sizeof(test_records); i++)
                if (((u8 *)&test_records)[i] != expected[i]) return 1;
            cases++;
        }
    }
    return cases == 262144 ? 0 : 2;
}
"""


class BattleResourceMeterTests(unittest.TestCase):
    def test_owner_profile_and_capture_boundary(self):
        manifest = json.loads(
            (ROOT / "config/slus_01411/matching_c.json").read_text()
        )
        entry, = [
            row for row in manifest["functions"]
            if row["source"] == "src/game/duel_scene_battle.c"
        ]
        self.assertEqual(entry["address"], "0x8001F55C")
        self.assertEqual(int(entry["size"], 0), 0x1378)
        self.assertEqual(entry["profile"], "gcc_2_8_1_g8_split_comm")
        source = (ROOT / "src/game/duel_scene_battle.c").read_text()
        self.assertIn("#define D_8009B1B9_AS_SIGNED", source)
        self.assertIn("DuelSideState *view;", source)
        self.assertEqual(source.count("view = &gDuel_aSideStates["), 2)
        self.assertRegex(
            source,
            r"resource = &D_800EA0E8\[D_8009B1B9\];\s*"
            r"arg = D_8009B1D0;\s*"
            r"\*\(u16 \*\)&resource->field_36 = arg;\s*"
            r"\*\(u16 \*\)&resource->field_38 = arg;",
        )
        self.assertNotIn("H(view, 0x36)", source)
        self.assertNotIn("H(view, 0x38)", source)

    @unittest.skipUnless(
        platform.system() == "Linux"
        and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"),
        "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_stores_and_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_scene_battle.c").read_text()
        block = re.search(
            r"(?s)if \(f & 0x40\) \{\s*"
            r"(?:/\*.*?\*/\s*)?DuelEffectResourceRecord \*resource;.*?"
            r"(?P<stores>resource = &D_800EA0E8\[D_8009B1B9\];\s*"
            r"arg = D_8009B1D0;\s*"
            r"\*\(u16 \*\)&resource->field_36 = arg;\s*"
            r"\*\(u16 \*\)&resource->field_38 = arg;)",
            source,
        )
        self.assertIsNotNone(block)
        stores = block["stores"]
        for optimization, mutation in (
            ("-O0", None), ("-O2", None),
            ("-O2", "wrong-slot"), ("-O2", "wrong-member"),
            ("-O2", "wide-second-store"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = stores
                if mutation == "wrong-slot":
                    code = code.replace("[D_8009B1B9]", "[0]")
                elif mutation == "wrong-member":
                    code = code.replace("resource->field_38", "resource->field_36")
                elif mutation == "wide-second-store":
                    code = code.replace(
                        "*(u16 *)&resource->field_38", "*(u32 *)&resource->field_38"
                    )
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, stores)
                with tempfile.TemporaryDirectory(
                    prefix="battle-resource-meter-", dir=ROOT / "tmp"
                ) as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(
                        HEADERS + "\nvoid write_meter(void) {\n"
                        " DuelEffectResourceRecord *resource; s32 arg;\n"
                        + code + "\n}\n"
                    )
                    witness = directory / "witness.c"
                    witness.write_text(WITNESS)
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
                            str(operations), str(witness), str(start), "-o", str(binary),
                        ],
                        cwd=ROOT, env=environment, text=True,
                        capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run(
                        [str(binary)], cwd=ROOT, env=environment, timeout=180
                    )
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
