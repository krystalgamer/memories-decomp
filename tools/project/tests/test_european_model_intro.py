import csv
import json
from pathlib import Path
import re
import sys
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/overlays/spanish_model_intro"
sys.path.insert(0, str(ROOT / "tools/project"))

import overlay_sources
import progress


class SpanishModelIntroTests(unittest.TestCase):
    config = ROOT / "config/sles_03951/overlays"
    region = "spain"
    prefix = "spanish"
    load_inventories = staticmethod(progress.load_spanish_overlay_inventories)

    def test_loader_slice_and_archive_are_registered(self) -> None:
        modules = json.loads((self.config.parent / "overlays.json").read_text())["modules"]
        module = next(m for m in modules if m["name"] == f"{self.prefix}_model_intro")
        self.assertEqual(module["archive"], f"game/{self.region}/DATA/SU.MRG")
        self.assertEqual(module["sector_offset"], 1767)
        self.assertEqual(module["sector_count"], 16)
        self.assertEqual(module["load_address"], "0x80180000")
        self.assertEqual(module["sha256"],
                         "f5fa6a720f1a584e229f63e8e8e6bc9c6a9bdb3ad37ec279de38df6ba3380ce1")
        loader = (ROOT / "src/game/european/model_intro_controller.c").read_text()
        for alias in (
            "#define MODEL_INTRO_PACKAGE_START_SECTOR 0x6E7",
            "#define func_801807B0 func_80180420",
            "#define func_80181C4C func_80180004",
            "#define func_80180A24 func_8018019C",
        ):
            self.assertIn(alias, loader)

    def test_complete_group_preserves_definition_order(self) -> None:
        expected = [(0x80180004, 408), (0x8018019C, 644), (0x80180420, 128),
                    (0x801804A0, 16), (0x801804B0, 288)]
        rows = json.loads((self.config / "model_intro_matching_c.json").read_text())["functions"]
        self.assertEqual([(int(r["address"], 0), int(r["size"], 0)) for r in rows], expected)
        self.assertEqual({r["source"] for r in rows},
                         {"src/overlays/spanish_model_intro/runtime.c"})
        self.assertEqual({r["profile"] for r in rows}, {"gcc_2_8_1_g0_split"})
        source = (SOURCE / "runtime.c").read_text()
        self.assertEqual(
            re.findall(r"^(?:void|s32) (func_[0-9A-F]+)\(", source, re.MULTILINE),
            [f"func_{address:X}" for address, _ in expected],
        )
        with (self.config / "model_intro_functions.csv").open() as handle:
            inventory = list(csv.DictReader(handle))
        self.assertEqual([(int(r["address"], 0), int(r["size"], 0)) for r in inventory], expected)
        self.assertEqual({r["status"] for r in inventory}, {"matching_c"})
        self.assertEqual(len(overlay_sources.c_segments(ROOT, self.config / "model_intro.yaml")), 1)

    def test_local_storage_is_not_an_absolute_alias(self) -> None:
        symbols = (self.config / "model_intro_symbols.txt").read_text()
        aliases = (self.config / "model_intro_linker_symbols.txt").read_text()
        for name, size in (("D_801805D0", "0x80"), ("D_80180650", "0x1"),
                           ("D_80180654", "0x18"), ("D_8018066C", "0x4"),
                           ("D_80180670", "0x4")):
            self.assertRegex(symbols, rf"{name} = 0x{name[2:]}; //[^\n]*size:{size}\b")
            self.assertNotIn(name + " =", aliases)
        self.assertIn("DuelEffect_HasActiveEntry = 0x800372E0;", aliases)
        self.assertIn("D_800EB0F8 = 0x800F0850;", aliases)
        self.assertIn("D_8009B0D8 = 0x8009C43C;", aliases)

    def test_canonical_contracts_and_widths_remain(self) -> None:
        source = (SOURCE / "runtime.c").read_text()
        header = (SOURCE / "runtime.h").read_text()
        self.assertNotIn("extern ", source)
        self.assertNotIn("asm", source)
        self.assertNotIn("TextBox_DestroyIndex(", header)
        self.assertIn("void TextBox_DestroyIndex(s32 index);",
                      (ROOT / "src/game/text_box_lifecycle.h").read_text())
        for contract in (
            "D_80180654[i].delay -= (u16)D_8009B0D8;",
            "*(u8 *)&D_8018066C->field_0C = i;",
            "object->field_4C = (s32)func_801804B0;",
            "packet->r0 = packet->g0 = packet->b0 = (u8)object->field_0C;",
            "D_80180654[i].phase = 16;",
        ):
            self.assertIn(contract, source)
        self.assertIn("s16 delay;", header)
        self.assertIn("extern u32 D_80180670;", header)

    def test_opaque_tail_is_not_counted_as_c(self) -> None:
        layout = (self.config / "model_intro.yaml").read_text()
        self.assertIn(f"- [0x674, data, overlays/{self.prefix}_model_intro/opaque_tail]", layout)
        self.assertIn("- [0x8000]", layout)
        counts = self.load_inventories(ROOT)["model_intro"]
        self.assertEqual(counts["matching_c_function_count"], 5)
        self.assertEqual(counts["matching_c_bytes"], 1484)
        self.assertEqual(32768 - 0x674, 31116)

    def test_resident_bindings_follow_regional_owners(self) -> None:
        aliases = dict(re.findall(
            r"^(\w+) = (0x[0-9A-F]+);",
            (self.config / "model_intro_linker_symbols.txt").read_text(), re.MULTILINE,
        ))
        with (self.config.parent / "functions.csv").open() as handle:
            inventory = {int(row["address"], 0): row for row in csv.DictReader(handle)}
        resident_aliases = dict(re.findall(
            r"^(\w+) = (0x[0-9A-F]+);",
            (self.config.parent / "link_symbols.ld").read_text(), re.MULTILINE,
        ))
        self.assertEqual(len(aliases), 12)
        for name, address in aliases.items():
            with self.subTest(symbol=name):
                if name.startswith("D_"):
                    self.assertEqual(resident_aliases[name], address)
                else:
                    row = inventory[int(address, 0)]
                    canonical = "func_800370E0" if name == "DuelEffect_HasActiveEntry" else name
                    self.assertEqual(row["name"], canonical)
                    self.assertEqual(row["status"], "matching_c")
        manifest = json.loads((self.config.parent / "matching_c.json").read_text())
        self.assertEqual(sum(
            row["source"] == "src/game/european/model_intro_controller.c"
            for row in manifest["functions"]
        ), 4)


class FrenchModelIntroTests(SpanishModelIntroTests):
    config = ROOT / "config/sles_03948/overlays"
    region = "france"
    prefix = "french"
    load_inventories = staticmethod(progress.load_french_overlay_inventories)


if __name__ == "__main__":
    unittest.main()
