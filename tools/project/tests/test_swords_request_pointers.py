"""Check Swords request ownership, the fixed pointer array and actual expiration."""

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
COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
HEADERS = r"""
#include "src/types.h"
#include "src/game/duel_side_state.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef char side_count[DUEL_SIDE_COUNT == 2 ? 1 : -1];
typedef char handle_extent[sizeof(gDuel_apSwordsEffectObjects) == 8 ? 1 : -1];
typedef struct { u8 before[16]; DuelSideState sides[2]; u8 after[16]; } GuardedSides;
typedef struct { u8 before[16]; DuelEffectRequest requests[2]; u8 after[16]; } GuardedRequests;
typedef struct { u8 before[16]; DuelEffectRequest *slots[2]; u8 after[16]; } GuardedHandles;
extern GuardedSides side_block;
extern GuardedRequests request_block;
extern GuardedHandles handle_block;
#define gDuel_apSwordsEffectObjects handle_block.slots
"""
WITNESS = HEADERS + r"""
GuardedSides side_block;
GuardedRequests request_block;
GuardedHandles handle_block;
DuelSideState *gDuel_pActiveSideState;
u8 gDuel_bActiveSide;
static u8 expected_sides[sizeof(side_block)];
static u8 expected_requests[sizeof(request_block)];
static u8 expected_handles[sizeof(handle_block)];
void expire_swords(void);
static void put16(u8 *p, u32 value) { p[0] = value; p[1] = value >> 8; }
int main(void)
{
    u32 side, value, i, counter, request, pointer, next, cases = 0;
    for (side = 0; side < 2; side++) {
        gDuel_bActiveSide = side;
        gDuel_pActiveSideState = &side_block.sides[side];
        counter = (u8 *)&side_block.sides[side].swords_turns_remaining - (u8 *)&side_block;
        request = (u8 *)&request_block.requests[side].phase - (u8 *)&request_block;
        pointer = (u8 *)&handle_block.slots[side] - (u8 *)&handle_block;
        for (value = 0; value < 256; value++) {
            for (i = 0; i < sizeof(side_block); i++)
                ((u8 *)&side_block)[i] = i * 37 + value;
            for (i = 0; i < sizeof(request_block); i++)
                ((u8 *)&request_block)[i] = i * 19 + value;
            for (i = 0; i < sizeof(handle_block); i++)
                ((u8 *)&handle_block)[i] = i * 13 + value;
            ((u8 *)&side_block)[counter] = value;
            for (i = 0; i < 2; i++) handle_block.slots[i] = &request_block.requests[i];
            for (i = 0; i < sizeof(side_block); i++) expected_sides[i] = ((u8 *)&side_block)[i];
            for (i = 0; i < sizeof(request_block); i++) expected_requests[i] = ((u8 *)&request_block)[i];
            for (i = 0; i < sizeof(handle_block); i++) expected_handles[i] = ((u8 *)&handle_block)[i];
            if (value != 0) {
                next = (value - 1) & 0xFF;
                expected_sides[counter] = next;
                if (next == 0 || (next & 0x80)) {
                    expected_sides[counter] = 0;
                    put16(expected_requests + request, 0xFFFD - side);
                    for (i = 0; i < 4; i++) expected_handles[pointer + i] = 0;
                }
            }
            expire_swords();
            for (i = 0; i < sizeof(side_block); i++)
                if (((u8 *)&side_block)[i] != expected_sides[i]) return 1;
            for (i = 0; i < sizeof(request_block); i++)
                if (((u8 *)&request_block)[i] != expected_requests[i]) return 1;
            for (i = 0; i < sizeof(handle_block); i++)
                if (((u8 *)&handle_block)[i] != expected_handles[i]) return 1;
            cases++;
        }
    }
    return cases == 512 ? 0 : 2;
}
"""


class SwordsRequestPointerTests(unittest.TestCase):
    @unittest.skipUnless(COMPILER.is_file(), "requires supported GCC 2.8.1")
    def test_exact_request_slots_and_fixed_target_extent(self):
        with tempfile.TemporaryDirectory(prefix="swords-pointer-type-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            environment = dict(os.environ, TMPDIR=str(directory))
            for slot_type, accepted in (
                ("DuelEffectRequest **", True),
                ("DuelFieldEffectObject **", False),
                ("u8 **", False),
            ):
                with self.subTest(slot_type=slot_type):
                    source = directory / "probe.c"
                    source.write_text(
                        '#include "../../src/game/duel_side_state.h"\n'
                        f"{slot_type}slot(s32 side) {{ return &gDuel_apSwordsEffectObjects[side]; }}\n"
                        "DuelEffectRequest *(*request_allocator)(s32) = DuelEffect_AllocateRequest;\n"
                        "typedef char count[DUEL_SIDE_COUNT == 2 ? 1 : -1];\n"
                        "typedef char extent[sizeof(gDuel_apSwordsEffectObjects) == 8 ? 1 : -1];\n"
                        "typedef char command[(u32)&((DuelEffectRequest *)0)->phase"
                        " == 0x1A && sizeof(((DuelEffectRequest *)0)->phase) == 2 ? 1 : -1];\n"
                    )
                    result = subprocess.run(
                        [str(COMPILER), "-S", "-O2", "-G0", "-Werror",
                         "-o", str(directory / "probe.s"), str(source)],
                        cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60,
                    )
                    if accepted:
                        self.assertEqual(result.returncode, 0, result.stderr)
                    else:
                        self.assertNotEqual(result.returncode, 0, result.stderr)
                        self.assertIn("incompatible pointer type", result.stderr)


    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_expiration_and_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        phase = (ROOT / "src/game/duel_phase_entry.c").read_text()
        match = re.search(
            r"(?s)if \(gDuel_pActiveSideState->swords_turns_remaining != 0\) \{"
            r".*?gDuel_apSwordsEffectObjects\[gDuel_bActiveSide\] = 0;\s*\}\s*\}",
            phase,
        )
        self.assertIsNotNone(match)
        fragment = match.group()
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "wrong-command"),
            ("-O2", "wrong-clear-side"), ("-O2", "miss-zero"),
            ("-O2", "wide-command"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = fragment
                if mutation == "wrong-command":
                    code = code.replace("0xFFFD - gDuel_bActiveSide", "0xFFFD")
                elif mutation == "wrong-clear-side":
                    code = code.replace(
                        "gDuel_apSwordsEffectObjects[gDuel_bActiveSide] = 0;",
                        "gDuel_apSwordsEffectObjects[gDuel_bActiveSide ^ 1] = 0;",
                    )
                elif mutation == "miss-zero":
                    code = code.replace("if (c <= 0)", "if (c < 0)")
                elif mutation == "wide-command":
                    code = code.replace(
                        "gDuel_apSwordsEffectObjects[gDuel_bActiveSide]->phase =",
                        "*(u32 *)&gDuel_apSwordsEffectObjects[gDuel_bActiveSide]->phase =",
                    )
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, fragment)
                with tempfile.TemporaryDirectory(prefix="swords-expiration-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    source = directory / "operations.c"
                    source.write_text(HEADERS + "\nvoid expire_swords(void) { s8 c;\n" + code + "\n}\n")
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
