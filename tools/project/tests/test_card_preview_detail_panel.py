"""Check the named renderer's seven-region ownership and opaque callback ABI."""

import csv
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
NAME = "CardPreview_DrawDetailPanel"
REGIONS = (
    ("slus_01411", 0x80028B08, 0x600, False),
    ("slpm_86398", 0x800288E8, 0x600, False),
    ("sles_03947", 0x80028AC8, 0x674, True),
    ("sles_03948", 0x80028C0C, 0x674, True),
    ("sles_03949", 0x80028C0C, 0x674, True),
    ("sles_03950", 0x80028C0C, 0x674, True),
    ("sles_03951", 0x80028C0C, 0x674, True),
)
COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"


class CardPreviewDetailPanelTests(unittest.TestCase):
    def test_seven_region_symbol_and_compiler_owners(self):
        for config, address, size, european in REGIONS:
            with self.subTest(config=config):
                directory = ROOT / "config" / config
                with (directory / "functions.csv").open(newline="") as handle:
                    rows = list(csv.DictReader(handle))
                matches = [row for row in rows if int(row["address"], 0) == address]
                self.assertEqual(len(matches), 1)
                self.assertEqual(
                    (matches[0]["name"], matches[0]["status"], matches[0]["module"]),
                    (NAME, "matching_c", "game"),
                )
                self.assertEqual(int(matches[0]["size"], 0), size)
                functions = json.loads((directory / "matching_c.json").read_text())["functions"]
                matches = [row for row in functions if int(row["address"], 0) == address]
                self.assertEqual(len(matches), 1)
                source = "src/game/" + ("european/" if european else "") + "card_preview_draw_detail_panel.c"
                self.assertEqual(
                    (matches[0]["source"], matches[0]["profile"], int(matches[0]["size"], 0)),
                    (source, "gcc_2_8_1_g8_split", size),
                )
                self.assertEqual(sum(row["source"] == source for row in functions), 1)
                self.assertTrue((ROOT / source).is_file())
                self.assertRegex(
                    (directory / "symbols.txt").read_text(),
                    rf"(?m)^{NAME}\s*=\s*0x{address:08X};.*type:func",
                )
                if config != "slus_01411":
                    self.assertIn(
                        source.removeprefix("src/").removesuffix(".c"),
                        (directory / "split.yaml").read_text(),
                    )

    def test_producer_link_and_preserved_callback_boundaries(self):
        source = (ROOT / "src/game/card_preview_draw_detail_panel.c").read_text()
        self.assertIn("void CardPreview_DrawDetailPanel(DisplayObject *obj, s32 arg1)", source)
        for text in (
            "win = (DisplayObject *)obj->field_54;",
            "DisplayObject_SubmitPacket(PRM, packet, arg1, arg, origin);",
            "do { sb = 0x00090009; } while (0);",
            "win->flags = flags;",
            "origin = (DisplayObjectPacketOrigin *)0x1F800398;",
            "PRM = (SpritePrim *)0x1F800320;",
            "packet = (POLY_FT4 *)0x1F800344;",
        ):
            self.assertIn(text, source)
        constructor = (ROOT / "src/game/duel_effect_resource_setup.c").read_text()
        self.assertIn("object->field_4C = (s32)CardPreview_DrawDetailPanel;", constructor)
        self.assertIn("secondary->field_54 = primary;", constructor)
        wrapper = (ROOT / "src/game/european/card_preview_draw_detail_panel.c").read_text()
        self.assertIn('#include "../card_preview_draw_detail_panel.c"', wrapper)
        self.assertNotIn("#define CardPreview_DrawDetailPanel", wrapper)
        self.assertNotIn(
            "#define CardPreview_DrawDetailPanel",
            (ROOT / "src/game/european/duel_effect_resource_setup.c").read_text(),
        )
        with (ROOT / "notes/semantic-symbol-map.csv").open(newline="") as handle:
            matches = [row for row in csv.DictReader(handle) if row["name"] == NAME]
        self.assertEqual(len(matches), 1)
        self.assertEqual((matches[0]["kind"], matches[0]["address"], matches[0]["confidence"]),
                         ("function", "0x80028B08", "confirmed"))
        for old in ("src/game/func_80028B08.c", "src/game/european/func_80028B08.c"):
            self.assertFalse((ROOT / old).exists())

    @unittest.skipUnless(COMPILER.is_file(), "requires supported GCC 2.8.1")
    def test_opaque_callback_contract(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="detail-panel-contract-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for parameters, accepted in (
                ("DisplayObject *, s32", True), ("u8 *, s32", False), ("DisplayObject *", False),
            ):
                with self.subTest(parameters=parameters):
                    source = directory / "probe.c"
                    source.write_text(
                        '#include "../../src/game/card_preview_callbacks.h"\n'
                        f"void (*checked)({parameters}) = CardPreview_DrawDetailPanel;\n"
                    )
                    result = subprocess.run(
                        [str(COMPILER), "-S", "-O2", "-G0", "-Werror",
                         "-o", str(directory / "probe.s"), str(source)],
                        cwd=ROOT, env=dict(os.environ, TMPDIR=str(directory)),
                        text=True, capture_output=True, timeout=60,
                    )
                    if accepted:
                        self.assertEqual(result.returncode, 0, result.stderr)
                    else:
                        self.assertNotEqual(result.returncode, 0, result.stderr)
                        self.assertIn("incompatible pointer type", result.stderr)


if __name__ == "__main__":
    unittest.main()
