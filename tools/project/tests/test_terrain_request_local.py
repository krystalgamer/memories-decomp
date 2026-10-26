"""Guard Terrain's real initialization prefix and byte-readback boundary."""

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
#define DUEL_TERRAIN_SCALAR_IN_DATA
#include "src/game/duel_terrain_boost.h"
#include "src/game/duel_side_state.h"
#include "src/game/duel_effect_request.h"
#include "src/game/duel_action_lock.h"
#include "src/game/sound.h"
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef struct { u8 before[16]; DuelSideState sides[2]; u8 after[16]; } GuardedSides;
typedef struct { u8 before[16]; DuelEffectRequest request; u8 after[16]; } GuardedRequest;
typedef struct { u8 before[16]; DuelEffectRequest *scene; u8 after[16]; } GuardedHandle;
extern GuardedSides side_block;
extern GuardedRequest request_block;
extern GuardedHandle handle_block;
#define gDuelEffect_pTrackedRequest handle_block.scene
"""
WITNESS = HEADERS + r"""
GuardedSides side_block;
GuardedRequest request_block;
GuardedHandle handle_block, expected_handle;
DuelSideState *gDuel_pActiveSideState;
s16 gDuel_wEffectCardID;
u8 gDuel_bTerrain;
static DuelEffectRequest other;
static u8 expected_sides[sizeof(side_block)], expected_request[sizeof(request_block)];
static u32 allocations, sounds, error, terrain, rank_value;
void initialize_terrain(void);
static void put16(u8 *p, u32 value) { p[0] = value; p[1] = value >> 8; }
DuelEffectRequest *DuelEffect_AllocateRequest(s32 id)
{
    if (id != 0xA || gDuel_bTerrain != terrain ||
        gDuel_pActiveSideState->rank.field_0A != rank_value) error = 1;
    allocations++;
    return &request_block.request;
}
void SD_SEPlayFull(u32 id)
{
    if (id != 0x13 || allocations != 1 ||
        handle_block.scene != &request_block.request ||
        request_block.request.phase != (s32)terrain - 1) error = 2;
    sounds++;
}
int main(void)
{
    u32 side, value, i, counter, payload, cases = 0;
    payload = (u8 *)&request_block.request.phase - (u8 *)&request_block;
    for (side = 0; side < 2; side++) {
        gDuel_pActiveSideState = &side_block.sides[side];
        counter = (u8 *)&side_block.sides[side].rank.field_0A - (u8 *)&side_block;
        for (value = 0; value < 65536; value++) {
            for (i = 0; i < sizeof(side_block); i++)
                ((u8 *)&side_block)[i] = i * 37 + value;
            ((u8 *)&side_block)[counter] = value >> 8;
            for (i = 0; i < sizeof(side_block); i++)
                expected_sides[i] = ((u8 *)&side_block)[i];
            rank_value = ((value >> 8) + 1) & 0xFF;
            expected_sides[counter] = rank_value;
            for (i = 0; i < sizeof(request_block); i++) {
                ((u8 *)&request_block)[i] = i * 19 + value;
                expected_request[i] = ((u8 *)&request_block)[i];
            }
            for (i = 0; i < sizeof(handle_block); i++)
                ((u8 *)&handle_block)[i] = i * 13 + value;
            handle_block.scene = value & 1 ? &other : 0;
            for (i = 0; i < sizeof(handle_block); i++)
                ((u8 *)&expected_handle)[i] = ((u8 *)&handle_block)[i];
            expected_handle.scene = &request_block.request;
            terrain = ((value & 0xFF) + 0x100 - 0x49) & 0xFF;
            put16(expected_request + payload, terrain - 1);
            gDuel_wEffectCardID = value;
            gDuel_bTerrain = value >> 8;
            allocations = sounds = error = 0;
            initialize_terrain();
            if (error || allocations != 1 || sounds != 1 || gDuel_bTerrain != terrain) return 1;
            for (i = 0; i < sizeof(side_block); i++)
                if (((u8 *)&side_block)[i] != expected_sides[i]) return 1;
            for (i = 0; i < sizeof(request_block); i++)
                if (((u8 *)&request_block)[i] != expected_request[i]) return 1;
            for (i = 0; i < sizeof(handle_block); i++)
                if (((u8 *)&handle_block)[i] != ((u8 *)&expected_handle)[i]) return 1;
            cases++;
        }
    }
    return cases == 131072 ? 0 : 2;
}
"""


class TerrainRequestLocalTests(unittest.TestCase):

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_initialization_prefix_and_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_action_lock.c").read_text()
        match = re.search(
            r"(?s)if \(DuelEffect_MarkInitialized\(\) == 0\) \{(?P<init>.*?)"
            r"\}\s*f = gDuel_wCardEffectFlags;", source,
        )
        self.assertIsNotNone(match)
        fragment = match["init"]
        for optimization, mutation in (
            ("-O0", None), ("-O2", None),
            ("-O2", "lost-byte-readback"), ("-O2", "wrong-request"),
            ("-O2", "missing-publication"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = fragment
                if mutation == "lost-byte-readback":
                    code = code.replace("n = gDuel_bTerrain - 1;", "n = v - 1;")
                elif mutation == "wrong-request":
                    code = code.replace("DuelEffect_AllocateRequest(0xA)", "DuelEffect_AllocateRequest(0xB)")
                elif mutation == "missing-publication":
                    code = code.replace("gDuelEffect_pTrackedRequest = e;", "")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, fragment)
                with tempfile.TemporaryDirectory(prefix="terrain-request-local-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(
                        HEADERS + "\nvoid initialize_terrain(void) {\n"
                        " DuelSideState *r; DuelEffectRequest *e; s32 v, n;\n"
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
