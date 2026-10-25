"""Guard the actual Swords request-initialization prefix, not full effect execution."""

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
#include "src/game/duel_side_state.h"
#include "src/game/sound.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef struct { u8 before[16]; DuelEffectRequest request; u8 after[16]; } GuardedRequest;
typedef struct {
    u8 before[16];
    DuelEffectRequest *scene;
    DuelEffectRequest *swords[2];
    u8 after[16];
} GuardedHandles;
extern GuardedRequest request_block;
extern GuardedHandles handle_block;
#define gDuelEffect_pTrackedRequest handle_block.scene
#define gDuel_apSwordsEffectObjects handle_block.swords
"""
WITNESS = HEADERS + r"""
GuardedRequest request_block;
GuardedHandles handle_block, expected_handles;
u8 gDuel_bActiveSide;
static DuelEffectRequest other[2];
static u8 expected_request[sizeof(request_block)];
static u32 allocations, sounds, error;
void initialize_swords(void);
static void put16(u8 *p, u32 value) { p[0] = value; p[1] = value >> 8; }
DuelEffectRequest *DuelEffect_AllocateRequest(s32 id)
{
    if (id != 0x15) error = 1;
    allocations++;
    return &request_block.request;
}
void SD_SEPlayFull(u32 id)
{
    DuelEffectRequest *request = &request_block.request;
    if (id != 0x23 || allocations != 1 || handle_block.scene != request ||
        handle_block.swords[gDuel_bActiveSide ^ 1] != request ||
        !(request->flags & DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING) ||
        request->field_00 != 0xA0 || request->field_02 != 0x78 ||
        request->phase != (gDuel_bActiveSide ^ 1)) error = 2;
    sounds++;
}
int main(void)
{
    u32 side, flags, i, base, cases = 0;
    base = (u8 *)&request_block.request - (u8 *)&request_block;
    for (side = 0; side < 2; side++) {
        gDuel_bActiveSide = side;
        for (flags = 0; flags < 256; flags++) {
            for (i = 0; i < sizeof(request_block); i++)
                ((u8 *)&request_block)[i] = i * 37 + flags;
            request_block.request.flags = flags;
            for (i = 0; i < sizeof(request_block); i++)
                expected_request[i] = ((u8 *)&request_block)[i];
            for (i = 0; i < sizeof(handle_block); i++)
                ((u8 *)&handle_block)[i] = i * 19 + flags;
            handle_block.scene = &other[side];
            for (i = 0; i < 2; i++) handle_block.swords[i] = &other[i];
            for (i = 0; i < sizeof(handle_block); i++)
                ((u8 *)&expected_handles)[i] = ((u8 *)&handle_block)[i];
            expected_handles.scene = &request_block.request;
            expected_handles.swords[side ^ 1] = &request_block.request;
            put16(expected_request + base, 0xA0);
            put16(expected_request + base + 2, 0x78);
            put16(expected_request + base + 0x1A, side ^ 1);
            expected_request[base + 0x1C] = flags | 0x20;
            allocations = sounds = error = 0;
            initialize_swords();
            if (error || allocations != 1 || sounds != 1) return 1;
            for (i = 0; i < sizeof(request_block); i++)
                if (((u8 *)&request_block)[i] != expected_request[i]) return 1;
            for (i = 0; i < sizeof(handle_block); i++)
                if (((u8 *)&handle_block)[i] != ((u8 *)&expected_handles)[i]) return 1;
            cases++;
        }
    }
    return cases == 512 ? 0 : 2;
}
"""


class SwordsLocalRequestTests(unittest.TestCase):

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_initialization_prefix_and_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_field_effects.c").read_text()
        function = source[source.index("void DuelEffect_ApplySwords("):]
        match = re.search(
            r"(?s)if \(DuelEffect_MarkInitialized\(\) == 0\) \{(?P<init>.*?)"
            r"\} else if", function,
        )
        self.assertIsNotNone(match)
        fragment = match["init"]
        for optimization, mutation in (
            ("-O0", None), ("-O2", None),
            ("-O2", "wrong-side"), ("-O2", "lost-flags"), ("-O2", "wrong-payload"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = fragment
                if mutation == "wrong-side":
                    code = code.replace(
                        "gDuel_apSwordsEffectObjects[side]", "gDuel_apSwordsEffectObjects[side ^ 1]"
                    )
                elif mutation == "lost-flags":
                    code = code.replace(
                        "object->flags |= DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING;",
                        "object->flags = DUEL_EFFECT_REQUEST_FLAG_NONBLOCKING;",
                    )
                elif mutation == "wrong-payload":
                    code = code.replace("object->field_00 = 0xA0;", "object->field_00 = 0x78;")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, fragment)
                with tempfile.TemporaryDirectory(prefix="swords-local-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    source = directory / "operations.c"
                    source.write_text(
                        HEADERS + "\nvoid initialize_swords(void) {\n"
                        " DuelEffectRequest *object; s32 side, field_side;\n" + code + "\n}\n"
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
                         str(source), str(witness), str(start), "-o", str(binary)],
                        cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run([str(binary)], cwd=ROOT, env=environment, timeout=180)
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
