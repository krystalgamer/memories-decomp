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
    "duel_field_effect_steps.c", "func_80024E58.c",
    "duel_scene_card_placement.c", "duel_ritual_effect.c",
    "duel_card_effects.c", "func_80018FEC.c", "duel_scene_battle.c",
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
                        f"{slot_type}handle(void) {{ return &D_8009B17C; }}\n"
                        "u8 *(*byte_allocator)(s32) = DuelEffect_AllocateRequest;\n"
                        "struct PartialEffect;\n"
                        "struct PartialEffect *partial(void) {\n"
                        " return (struct PartialEffect *)D_8009B17C;\n"
                        "}\n"
                        "u8 *bytes(void) { return (u8 *)D_8009B17C; }\n"
                        "void *generic(void) { return D_8009B17C; }\n"
                        "typedef char handle_width[sizeof(D_8009B17C) == 4 ? 1 : -1];\n"
                        "typedef char request_extent[sizeof(DuelEffectRequest) == 0x20 ? 1 : -1];\n"
                        "typedef char progress[(u32)&((DuelEffectRequest *)0)->field_1D"
                        " == 0x1D && sizeof(((DuelEffectRequest *)0)->field_1D) == 1 ? 1 : -1];\n"
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

    def test_allocator_owner_and_unscaled_live_clients(self):
        pool = (ROOT / "src/game/duel_effect_object_pool.c").read_text()
        self.assertIn("DuelEffectRequest *p = DuelEffect_FindFreeRequest();", pool)
        self.assertIn("return (u8 *)p;", pool)
        header = (ROOT / "src/game/duel_effect_request.h").read_text()
        self.assertIn("extern DuelEffectRequest *G32 D_8009B17C;", header)
        writes = []
        for relative in CLIENTS:
            source = (ROOT / "src/game" / relative).read_text()
            self.assertNotRegex(source, r"\bD_8009B17C\s*\[")
            writes.extend(re.findall(r"\bD_8009B17C\s*=\s*([^;]+);", source))
        self.assertEqual(len(writes), 14)
        self.assertEqual(writes.count("0"), 1)
        battle = (ROOT / "src/game/duel_scene_battle.c").read_text()
        self.assertEqual(battle.count("D_8009B17C = req;"), 3)
        self.assertNotIn("DUEL_EFFECT_REQUEST_BYTES", battle)
        exit_scene = (ROOT / "src/game/func_80018FEC.c").read_text()
        self.assertEqual(exit_scene.count("D_8009B17C->field_1D == 0"), 2)
        source = (ROOT / "src/game/duel_card_effects.c").read_text()
        self.assertIn("D_8009B17C->field_1D != 0", source)
        source = (ROOT / "src/game/duel_field_effect_steps.c").read_text()
        self.assertIn("D_8009B17C->field_1D != 0", source)
        self.assertIn("D_8009B17C->field_1D >= 2", source)


if __name__ == "__main__":
    unittest.main()
