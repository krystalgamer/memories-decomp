import json
from pathlib import Path
import re
import shutil
import subprocess
import unittest


ROOT = Path(__file__).resolve().parents[3]


class ScriptImageLocalizedIdsTests(unittest.TestCase):
    def preprocessed_source(self, source: str) -> str:
        compiler = shutil.which("cc")
        if compiler is None:
            self.skipTest("a C preprocessor is required")
        result = subprocess.run(
            [compiler, "-E", "-P", str(ROOT / source)],
            cwd=ROOT, check=True, capture_output=True, text=True,
        )
        return re.sub(r"\s+", " ", result.stdout)

    def test_existing_regions_keep_volatile_owner_without_localized_remapping(self) -> None:
        for source in (
            "src/game/script_image_objects.c",
            "src/game/japanese/script_image_objects.c",
            "src/game/european/script_image_objects.c",
        ):
            with self.subTest(source=source):
                text = self.preprocessed_source(source)
                self.assertIn(
                    "volatile ScriptImageObjectSet *owner, s32 value) {", text,
                )
                self.assertNotIn("if (value == 0x10)", text)
                self.assertNotIn("if (value == 0x11)", text)

    def test_spanish_remaps_both_ids_before_recording_the_owner(self) -> None:
        text = self.preprocessed_source("src/game/spanish/script_image_objects.c")
        self.assertIn("volatile ScriptImageObjectSet *owner, s32 value) {", text)
        start = text.index("if (value == 0x10)")
        end = text.index("if (owner)", start)
        remap = text[start:end]
        first, second = remap.split("else if (value == 0x11)")
        for language in range(1, 5):
            for arm, image in ((first, 0x40 + language * 2),
                               (second, 0x41 + language * 2)):
                self.assertIn(
                    f"if (D_8009C02B == {language}) {{ value = 0x{image:X}; }}",
                    arm,
                )
        self.assertIn("((ScriptImageObjectSet *)owner)->image_id = value;", text[end:])
        self.assertIn("base + index * stride + 0x29E8", text)

    def test_spanish_group_keeps_all_four_exact_contiguous_extents(self) -> None:
        manifest = json.loads((ROOT / "config/sles_03951/matching_c.json").read_text())
        group = [
            e for e in manifest["functions"]
            if e["source"] == "src/game/spanish/script_image_objects.c"
        ]
        self.assertEqual(
            [(int(e["address"], 0), int(e["size"], 0)) for e in group],
            [(0x8002DFE8, 0x134), (0x8002E11C, 0x188),
             (0x8002E2A4, 0x54), (0x8002E2F8, 0xC8)],
        )
        self.assertEqual({e["profile"] for e in group}, {"gcc_2_8_1_g0"})


if __name__ == "__main__":
    unittest.main()
