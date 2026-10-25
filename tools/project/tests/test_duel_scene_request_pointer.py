"""Check the allocator-backed scene handle and retained partial/API boundaries."""

import os
from pathlib import Path
import re
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
CLIENTS = (
    "duel_field_effects.c", "duel_effect_apply_terrain.c",
    "duel_scene_card_placement.c",
    "duel_card_effects.c", "duel_draw_resolution.c", "duel_scene_battle.c",
)


class DuelSceneRequestPointerTests(unittest.TestCase):
    @unittest.skipUnless(COMPILER.is_file(), "requires supported GCC 2.8.1")
    def test_exact_stored_handle_and_rejected_byte_or_opaque_slots(self):
        with tempfile.TemporaryDirectory(
            prefix="scene-request-type-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            environment = dict(os.environ, TMPDIR=str(directory))
            for slot_type, accepted in (
                ("DuelEffectRequest **", True),
                ("u8 **", False),
                ("void **", False),
            ):
                with self.subTest(slot_type=slot_type):
                    source = directory / "probe.c"
                    source.write_text(
                        '#include "../../src/game/duel_effect_request.h"\n'
                        f"{slot_type}handle(void) {{ return &gDuelEffect_pTrackedRequest; }}\n"
                        "DuelEffectRequest *(*request_allocator)(s32) = DuelEffect_AllocateRequest;\n"
                        "struct PartialEffect;\n"
                        "struct PartialEffect *partial(void) {\n"
                        " return (struct PartialEffect *)gDuelEffect_pTrackedRequest;\n"
                        "}\n"
                        "u8 *bytes(void) { return (u8 *)gDuelEffect_pTrackedRequest; }\n"
                        "void *generic(void) { return gDuelEffect_pTrackedRequest; }\n"
                        "typedef char handle_width[sizeof(gDuelEffect_pTrackedRequest) == 4 ? 1 : -1];\n"
                        "typedef char request_extent[sizeof(DuelEffectRequest) == 0x20 ? 1 : -1];\n"
                        "typedef char progress[(u32)&((DuelEffectRequest *)0)->progress"
                        " == 0x1D && sizeof(((DuelEffectRequest *)0)->progress) == 1 ? 1 : -1];\n"
                    )
                    result = subprocess.run(
                        [
                            str(COMPILER), "-S", "-O2", "-G0", "-Werror",
                            "-o", str(directory / "probe.s"), str(source),
                        ],
                        cwd=ROOT, env=environment, text=True,
                        capture_output=True, timeout=60,
                    )
                    if accepted:
                        self.assertEqual(result.returncode, 0, result.stderr)
                    else:
                        self.assertNotEqual(result.returncode, 0, result.stderr)
                        self.assertIn("incompatible pointer type", result.stderr)



if __name__ == "__main__":
    unittest.main()
