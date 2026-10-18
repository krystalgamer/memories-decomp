import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant131Tests(family435.FrenchModelVariant435Tests):
    family = 131
    slot_header_delta = 130
    module_count = 12
    distinct_images = 12
    binding_count = 24
    tail_start = 0x1604
    spans = ((4, 0x15BC),)
    helpers = ((4, 5560, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (183, 273, 486, 490, 540, 637)),)
    entry_anchors = {
        4: 0x27BDFD88, 0x15B4: 0x03E00008, 0x15B8: 0x27BD0278,
        0x15BC: 4096, 0x15C0: 4096, 0x15C4: 4096, 0x15C8: 0,
    }



    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(
                f"D_{base + 0x15CC:X} = 0x{base + 0x15CC:X}; // type:u8 size:0x38 defined:true",
                symbols)
            self.assertIn(f"[0x15CC, data, overlays/{module['name']}/image_view]", layout.read_text())
            self.assertIn(
                f"D_{base + 0x15BC:X} = 0x{base + 0x15BC:X}; // type:u32 size:0x10 defined:true",
                symbols)
            suffix = "_slot1" if module["name"].endswith("1") else ""
            self.assertIn(f"[0x15BC, .rodata, overlays/french_model_variant/variant131_entry{suffix}]",
                          layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (128, 128, 128, 128, 128, 128, 15, 0, 200, 100, 60, 76, 240, 80),
            1: (128, 128, 128, 128, 128, 128, 1, 0, 300, 150, 60, 90, 400, 120),
            2: (128, 128, 128, 128, 128, 128, 13, 0, 150, 150, -60, 80, 240, 40),
            3: (32, 128, 32, 64, 128, 64, 28, 0, 150, 150, 50, 90, 320, 60),
            5: (16, 128, 8, 96, 128, 32, 8, 0, 100, 70, 30, 60, 250, 70),
        }
        arguments = {183: 2, 273: 5, 486: 0, 490: 3, 540: 1, 637: 5}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = arguments[int(row["model"])]
                self.assertEqual(int(row["command_word"]), 62000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 62000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<8B6h", data, 0x1604 + 20 * argument),
                                 descriptors[argument])
                observed.add(argument)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1390I", data[4:0x15BC])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 71)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x18C <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model131Config)": 20, "sizeof(Model131State)": 0x18C,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(GsIMAGE)": 28, "sizeof(POLY_FT4)": 40, "sizeof(DVECTOR)": 4,
        }
        for typename, fields in (
            ("Model131Config", (("part", 6), ("radius", 8), ("particle_half_size", 10),
                                ("spin", 12), ("fade", 14), ("duration", 16), ("delay", 18))),
            ("Model131State", (("ring", 4), ("origin", 0x74), ("particles", 0x7C),
                               ("texture", 0x17C), ("elapsed", 0x184),
                               ("command_group", 0x188), ("completed", 0x189))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 22)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant131_entry.h")
