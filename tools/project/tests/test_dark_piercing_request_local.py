"""Check the real request initialization prefix, not the full reveal handler."""

import os
from pathlib import Path
import re
import subprocess
import tempfile
import unittest
import platform
import shutil

from tools.project.tests.test_library_display_objects import START


ROOT = Path(__file__).resolve().parents[3]
HEADERS = r"""
#include "src/types.h"
#include "src/game/duel_effect_request.h"
#include "src/game/sound.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef struct { u8 before[16]; DuelEffectRequest request; u8 after[16]; } GuardedRequest;
typedef struct { u8 before[16]; DuelEffectRequest *scene; u8 after[16]; } GuardedHandle;
extern GuardedRequest request_block;
extern GuardedHandle handle_block;
#define gDuelEffect_pTrackedRequest handle_block.scene
"""
WITNESS = HEADERS + r"""
GuardedRequest request_block;
GuardedHandle handle_block, expected_handle;
static DuelEffectRequest other;
static u8 expected_request[sizeof(request_block)];
static u32 allocations, sounds, error;
void initialize_dark(void);
static void put16(u8 *p, u32 value) { p[0] = value; p[1] = value >> 8; }
DuelEffectRequest *DuelEffect_AllocateRequest(s32 id)
{
    if (id != 0x13) error = 1;
    allocations++;
    return &request_block.request;
}
void SD_SEPlayFull(u32 id)
{
    if (id != 0x13 || allocations != 1 ||
        handle_block.scene != &request_block.request ||
        request_block.request.field_00 != 0xA0 ||
        request_block.request.field_02 != 0x68) error = 2;
    sounds++;
}
int main(void)
{
    u32 old_handle, seed, i, base, cases = 0;
    base = (u8 *)&request_block.request - (u8 *)&request_block;
    for (old_handle = 0; old_handle < 2; old_handle++) {
        for (seed = 0; seed < 256; seed++) {
            for (i = 0; i < sizeof(request_block); i++) {
                ((u8 *)&request_block)[i] = i * 37 + seed;
                expected_request[i] = ((u8 *)&request_block)[i];
            }
            for (i = 0; i < sizeof(handle_block); i++)
                ((u8 *)&handle_block)[i] = i * 19 + seed;
            handle_block.scene = old_handle ? &other : 0;
            for (i = 0; i < sizeof(handle_block); i++)
                ((u8 *)&expected_handle)[i] = ((u8 *)&handle_block)[i];
            expected_handle.scene = &request_block.request;
            put16(expected_request + base, 0xA0);
            put16(expected_request + base + 2, 0x68);
            allocations = sounds = error = 0;
            initialize_dark();
            if (error || allocations != 1 || sounds != 1) return 1;
            for (i = 0; i < sizeof(request_block); i++)
                if (((u8 *)&request_block)[i] != expected_request[i]) return 1;
            for (i = 0; i < sizeof(handle_block); i++)
                if (((u8 *)&handle_block)[i] != ((u8 *)&expected_handle)[i]) return 1;
            cases++;
        }
    }
    return cases == 512 ? 0 : 2;
}
"""


class DarkPiercingRequestLocalTests(unittest.TestCase):

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_initialization_prefix_and_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_card_effects.c").read_text()
        function = source[source.index("void DuelEffect_ApplyDarkPiercingLight("):]
        match = re.search(
            r"(?s)if \(DuelEffect_MarkInitialized\(\) == 0\) \{(?P<init>.*?)"
            r"\}\s*flags = gDuel_wCardEffectFlags;", function,
        )
        self.assertIsNotNone(match)
        fragment = match["init"]
        for optimization, mutation in (
            ("-O0", None), ("-O2", None),
            ("-O2", "missing-publication"), ("-O2", "wrong-x"), ("-O2", "wrong-member"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = fragment
                if mutation == "missing-publication":
                    code = code.replace("gDuelEffect_pTrackedRequest = object;", "")
                elif mutation == "wrong-x":
                    code = code.replace("object->field_00 = 0xA0;", "object->field_00 = 0x78;")
                elif mutation == "wrong-member":
                    code = code.replace("object->field_02 = 0x68;", "object->field_04 = 0x68;")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, fragment)
                with tempfile.TemporaryDirectory(prefix="dark-request-local-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(
                        HEADERS + "\nvoid initialize_dark(void) { DuelEffectRequest *object;\n"
                        + code + "\n}\n"
                    )
                    witness = directory / "witness.c"
                    witness.write_text(WITNESS)
                    start = directory / "start.S"
                    start.write_text(START)
                    binary = directory / "witness"
                    environment = dict(os.environ, TMPDIR=str(directory))
                    result = subprocess.run(
                        [shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                         "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                         "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                         "-Werror=incompatible-pointer-types", "-I", str(ROOT),
                         str(operations), str(witness), str(start), "-o", str(binary)],
                        cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run([str(binary)], cwd=ROOT, env=environment, timeout=180)
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
