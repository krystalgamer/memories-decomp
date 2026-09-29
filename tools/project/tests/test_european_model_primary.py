import csv
import json
from pathlib import Path
import re
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from progress import load_french_overlay_inventories, load_spanish_overlay_inventories
from verify_inputs import load_checksum_manifest


class SpanishModelPrimaryTests(unittest.TestCase):
    config = ROOT / "config/sles_03951"
    region = "spain"
    prefix = "spanish"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    records = {8: 8, 116: 116, 141: 141, 150: 150, 167: 167,
               370: 320, 394: 344, 416: 366, 707: 607, 715: 615}
    particle_c_bytes = 13168

    def family(self, module):
        model = int(re.fullmatch(rf"{self.prefix}_model_primary_(\d+)_slot\d", module["name"])[1])
        if model in (150, 167):
            return "copy", 276
        return {8: ("primary62", 1572), 416: ("primary63", 2564),
                141: ("primary64", 2448)}.get(model, ("effect", 216))

    def modules(self):
        manifest = json.loads((self.config / "overlays.json").read_text())
        return [m for m in manifest["modules"] if m["name"].startswith(f"{self.prefix}_model_primary_")]

    def test_loader_slices_cover_both_slots_of_registered_models(self):
        records = self.records
        modules = self.modules()
        self.assertEqual({m["name"] for m in modules}, {
            f"{self.prefix}_model_primary_{model}_slot{slot}" for model in records for slot in (0, 1)
        })
        checksums = load_checksum_manifest(self.config / "files.sha256")
        for module in modules:
            model, slot = map(int, re.fullmatch(
                rf"{self.prefix}_model_primary_(\d+)_slot(\d)", module["name"]).groups())
            self.assertEqual(module["archive"], f"game/{self.region}/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sector_offset"], records[model] * 276 + 220 + slot * 2)
            self.assertEqual(module["sector_count"], 2)
            self.assertEqual(int(module["load_address"], 0), 0x8013A000 + slot * 0x40000)
            self.assertNotIn("duplicate_sector_offsets", module)

    def test_each_layout_selects_one_compiler_owned_entry(self):
        for module in self.modules():
            with self.subTest(module=module["name"]):
                layout = ROOT / module["layout"]
                family, size = self.family(module)
                address = int(module["load_address"], 0) + 4
                suffix = "_slot1" if module["name"].endswith("slot1") else ""
                source = f"src/overlays/spanish_model_primary/{family}{suffix}.c"
                functions = json.loads(layout.with_name(
                    layout.stem + "_matching_c.json").read_text())["functions"]
                self.assertEqual(functions, [{
                    "address": f"0x{address:X}", "profile": "gcc_2_8_1_g0_split",
                    "size": f"0x{size:X}", "source": source,
                }])
                with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                    rows = list(csv.DictReader(handle))
                self.assertEqual(len(rows), 1)
                self.assertEqual(rows[0]["name"], f"func_{address:X}")
                self.assertEqual(rows[0]["status"], "matching_c")
                segments = c_segments(ROOT, layout)
                self.assertEqual(len(segments), 1)
                self.assertEqual(segments[0]["source"], source)

    def test_descriptor_prefixes_have_real_storage_and_tails_are_uncounted(self):
        inventories = self.load_inventories(ROOT)
        total = 0
        for module in self.modules():
            if self.family(module)[0] not in ("copy", "effect"):
                continue
            layout = ROOT / module["layout"]
            copy = any(f"_{model}_" in module["name"] for model in (150, 167))
            data = 0x118 if copy else 0xDC
            tail = data + 48
            symbol = int(module["load_address"], 0) + data
            text = layout.read_text()
            self.assertIn(f"[0x{tail:X}, data, overlays/{module['name']}/opaque_tail]", text)
            self.assertIn("- [0x1000]", text)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{symbol:X} = 0x{symbol:X}; // size:0x30", symbols)
            self.assertNotIn(f"D_{symbol:X} =", (ROOT / module["linker_symbols"]).read_text())
            counts = inventories[layout.stem]
            self.assertEqual(counts["matching_c_function_count"], 1)
            self.assertEqual(counts["matching_c_bytes"], data - 4)
            total += counts["matching_c_bytes"]
        self.assertEqual(total, 3264)

    def test_particle_data_owners_and_unclassified_ranges(self):
        layouts = {
            "primary62": ([(0x628, 16), (0x638, 28), (0x750, 16)],
                          [(0x654, "unknown_gap"), (0x760, "opaque_tail")]),
            "primary63": ([(0xA08, 16), (0xA18, 12)], [(0xA24, "opaque_tail")]),
            "primary64": ([(0x994, 16), (0x9A4, 140), (0xA30, 90)],
                          [(0xA8A, "opaque_tail")]),
        }
        inventories = self.load_inventories(ROOT)
        total = 0
        for module in self.modules():
            family, size = self.family(module)
            if family not in layouts:
                continue
            layout = ROOT / module["layout"]
            text = layout.read_text()
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (ROOT / module["linker_symbols"]).read_text()
            base = int(module["load_address"], 0)
            data, unknown = layouts[family]
            for offset, length in data:
                self.assertRegex(symbols, rf"D_{base+offset:X} = 0x{base+offset:X}; "
                                 rf"// (?:type:u8 )?size:0x{length:X}\b")
                self.assertNotIn(f"D_{base+offset:X} =", bindings)
            for offset, name in unknown:
                if family != "primary64":
                    self.assertIn(f"[0x{offset:X}, data, overlays/{module['name']}/{name}]", text)
            self.assertIn("- [0x1000]", text)
            if family == "primary64":
                self.assertIn("type:u8 size:0x5A", symbols)
                self.assertIn("type:u8 size:0x576 defined:true", symbols)
                self.assertIn(f"[0xA30, data, overlays/{module['name']}/descriptor_storage]", text)
                self.assertNotIn("[0xA8A, data,", text)
                self.assertIn(f"D_{base+0xA8A:X} = 0x{base+0xA8A:X};", symbols)
            counts = inventories[layout.stem]
            self.assertEqual(counts["matching_c_function_count"], 1)
            self.assertEqual(counts["matching_c_bytes"], size)
            total += size
        self.assertEqual(total, self.particle_c_bytes)

    def test_particle_wrappers_and_canonical_texture_contract(self):
        directory = ROOT / "src/overlays/spanish_model_primary"
        for family, offsets in ((62, (0x628, 0x638, 0x750)),
                                (63, (0xA08, 0xA18)),
                                (64, (0x994, 0x9A4, 0xA30))):
            wrapper = (directory / f"primary{family}_slot1.c").read_text()
            self.assertIn("#define func_8013A004 func_8017A004", wrapper)
            self.assertIn(f'#include "primary{family}.c"', wrapper)
            for offset in offsets:
                self.assertIn(f"#define D_{0x8013A000+offset:X} D_{0x8017A000+offset:X}", wrapper)
            header = (directory / f"primary{family}.h").read_text()
            self.assertIn('#include "../../game/model_texture_upload.h"', header)
            self.assertNotIn("u32 func_80059A50(", header)
        self.assertIn("s32 texture;", (directory / "primary62.h").read_text())
        self.assertIn("u32 textures[5];", (directory / "primary64.h").read_text())
        self.assertIn("SVECTOR velocities[3][16];", (directory / "primary63.h").read_text())
        self.assertIn("u32 func_80059A50(s32 slot, s32 mode, GsIMAGE *image);",
                      (ROOT / "src/game/model_texture_upload.h").read_text())

    def test_slot_wrappers_keep_real_slot_specific_symbol_names(self):
        directory = ROOT / "src/overlays/spanish_model_primary"
        for family, data in (("copy", "118"), ("effect", "0DC")):
            wrapper = (directory / f"{family}_slot1.c").read_text()
            self.assertIn("#define func_8013A004 func_8017A004", wrapper)
            self.assertIn(f"#define D_8013A{data} D_8017A{data}", wrapper)
            self.assertIn(f'#include "{family}.c"', wrapper)
        header = (directory / "primary.h").read_text()
        self.assertIn('#include "../../game/model_control.h"', header)
        self.assertIn("u32 tick;", header)
        for path in directory.glob("*.c"):
            self.assertNotRegex(path.read_text(), r"\b(?:asm|__asm__)\b")

    def test_resident_callee_bindings_are_fixed(self):
        configuration = self.config / "overlays"
        for family, expected in (
            ("copy", {"Model_GetActiveSlotIndex": "0x8005BED4", "MoveImage": "0x8007FFD0"}),
            ("effect", {"Model_GetActiveSlotIndex": "0x8005BED4",
                        "Model_GetSlotAnimationIndex": "0x8005BF70",
                        "func_8005D994": "0x8004D9A4"}),
        ):
            text = (configuration / f"model_primary_{family}_linker_symbols.txt").read_text()
            self.assertEqual(dict(re.findall(r"^(\w+) = (0x[0-9A-F]+);", text, re.M)), expected)


class FrenchModelPrimaryTests(SpanishModelPrimaryTests):
    config = ROOT / "config/sles_03948"
    region = "france"
    prefix = "french"
    load_inventories = staticmethod(load_french_overlay_inventories)
    records = {116: 116, 150: 150, 167: 167, 370: 320, 394: 344, 707: 607, 715: 615}
    particle_c_bytes = 0

    def test_complete_images_reuse_accepted_spanish_sources_without_patching(self):
        spanish = {m["name"]: m for m in SpanishModelPrimaryTests().modules()}
        for module in self.modules():
            original = spanish[module["name"].replace("french_", "spanish_", 1)]
            for key in ("sha256", "archive_sha256", "sector_offset",
                        "sector_count", "load_address"):
                self.assertEqual(module[key], original[key])
            layout = ROOT / module["layout"]
            source_layout = ROOT / original["layout"]
            self.assertEqual(
                layout.with_name(layout.stem + "_matching_c.json").read_text(),
                source_layout.with_name(source_layout.stem + "_matching_c.json").read_text(),
            )

    def test_french_imports_have_resident_owners(self):
        with (self.config / "functions.csv").open() as handle:
            resident = {row["address"]: row for row in csv.DictReader(handle)}
        for name, address in (
            ("Model_GetActiveSlotIndex", "0x8005BED4"),
            ("Model_GetSlotAnimationIndex", "0x8005BF70"),
            ("func_8005D994", "0x8004D9A4"),
        ):
            self.assertEqual(resident[address]["name"], name)
            self.assertEqual(resident[address]["status"], "matching_c")
        self.assertEqual(resident["0x8007FFD0"]["status"], "sdk_asm")
        self.assertIn("MoveImage = 0x8007FFD0;",
                      (self.config / "link_symbols.ld").read_text())
