"""Check stored resource-object types without importing the full object schema."""

import os
from pathlib import Path
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"


@unittest.skipUnless(COMPILER.is_file(), "requires supported GCC 2.8.1")
class DuelResourceObjectTypeTests(unittest.TestCase):
    def test_stored_pointees_and_rejected_opaque_slots(self):
        with tempfile.TemporaryDirectory(
            prefix="resource-object-types-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            environment = dict(os.environ, TMPDIR=str(directory))
            for member in ("object_00", "object_04"):
                for pointee, accepted in (
                    ("struct DisplayObject", True), ("void", False),
                ):
                    with self.subTest(member=member, pointee=pointee):
                        source = directory / "probe.c"
                        source.write_text(
                            '#include "../../src/game/duel_effect_resource_record.h"\n'
                            f"{pointee} **slot(DuelEffectResourceRecord *r) {{\n"
                            f"    return &r->{member};\n"
                            "}\n"
                            "void *generic_release_view(DuelEffectResourceRecord *r) {\n"
                            "    return r->object_00;\n"
                            "}\n"
                            "typedef char extent[sizeof(DuelEffectResourceRecord) == 0x40 ? 1 : -1];\n"
                            "typedef char primary_width[sizeof(((DuelEffectResourceRecord *)0)"
                            "->object_00) == 4 ? 1 : -1];\n"
                            "typedef char secondary_width[sizeof(((DuelEffectResourceRecord *)0)"
                            "->object_04) == 4 ? 1 : -1];\n"
                            "typedef char source_x[(u32)&((DuelEffectResourceRecord *)0)"
                            "->src_x == 0x28 ? 1 : -1];\n"
                            "typedef char source_y[(u32)&((DuelEffectResourceRecord *)0)"
                            "->src_y == 0x2A ? 1 : -1];\n"
                            "typedef char attack_offset[(u32)&((DuelEffectResourceRecord *)0)"
                            "->attack == 0x32 ? 1 : -1];\n"
                            "typedef char defense_offset[(u32)&((DuelEffectResourceRecord *)0)"
                            "->defense == 0x34 ? 1 : -1];\n"
                            "typedef char attack_width[sizeof(((DuelEffectResourceRecord *)0)"
                            "->attack) == 2 ? 1 : -1];\n"
                            "typedef char defense_width[sizeof(((DuelEffectResourceRecord *)0)"
                            "->defense) == 2 ? 1 : -1];\n"
                            "typedef char attack_signed[((__typeof__"
                            "(((DuelEffectResourceRecord *)0)->attack))-1) < 0 ? 1 : -1];\n"
                            "typedef char defense_signed[((__typeof__"
                            "(((DuelEffectResourceRecord *)0)->defense))-1) < 0 ? 1 : -1];\n"
                            "typedef char attack_adjustment_offset[(u32)&((DuelEffectResourceRecord *)0)"
                            "->attack_adjustment == 0x36 ? 1 : -1];\n"
                            "typedef char defense_adjustment_offset[(u32)&((DuelEffectResourceRecord *)0)"
                            "->defense_adjustment == 0x38 ? 1 : -1];\n"
                            "typedef char attack_adjustment_width[sizeof(((DuelEffectResourceRecord *)0)"
                            "->attack_adjustment) == 2 ? 1 : -1];\n"
                            "typedef char defense_adjustment_width[sizeof(((DuelEffectResourceRecord *)0)"
                            "->defense_adjustment) == 2 ? 1 : -1];\n"
                            "typedef char attack_adjustment_signed[((__typeof__"
                            "(((DuelEffectResourceRecord *)0)->attack_adjustment))-1) < 0 ? 1 : -1];\n"
                            "typedef char defense_adjustment_signed[((__typeof__"
                            "(((DuelEffectResourceRecord *)0)->defense_adjustment))-1) < 0 ? 1 : -1];\n"
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

    def test_constructor_and_release_keep_actual_pool_objects(self):
        setup = (ROOT / "src/game/duel_effect_resource_setup.c").read_text()
        self.assertIn("DisplayObject *object;", setup)
        self.assertEqual(
            setup.count(
                "object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(),"
            ), 2,
        )
        self.assertIn("entry->object_04 = object;", setup)
        self.assertIn("primary = object;", setup)
        self.assertIn("entry->object_00 = primary;", setup)
        self.assertIn("secondary->field_54 = primary;", setup)
        release = (ROOT / "src/game/func_80029528.c").read_text()
        expected = (
            "DisplayObject_ReleaseIfPresent(entry->object_00);",
            "DisplayObject_ReleaseIfPresent(entry->object_04);",
            "entry->object_04 = 0;",
            "entry->object_00 = 0;",
        )
        offsets = [release.index(statement) for statement in expected]
        self.assertEqual(offsets, sorted(offsets))


if __name__ == "__main__":
    unittest.main()
