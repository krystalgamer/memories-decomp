import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant129Tests(family435.FrenchModelVariant435Tests):
    family = 129
    slot_header_delta = 130
    module_count = 10
    distinct_images = 10
    binding_count = 21
    tail_start = 0x1030
    spans = ((4, 0xFE8),)
    helpers = ((4, 4068, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (219,)), (9, (30, 355, 435, 490)))
    entry_anchors = {
        4: 0x27BDFE90, 0xFE0: 0x03E00008, 0xFE4: 0x27BD0170,
        0xFE8: 4096, 0xFEC: 4096, 0xFF0: 4096, 0xFF4: 0,
    }



    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0xFF8:X} = 0x{base + 0xFF8:X}; // type:u8 size:0x38 defined:true",
                          symbols)
            self.assertIn(f"[0xFF8, data, overlays/{module['name']}/image_view]", layout.read_text())
            self.assertIn(f"D_{base + 0xFE8:X} = 0x{base + 0xFE8:X}; // type:u32 size:0x10 defined:true",
                          symbols)
            suffix = "_slot1" if module["name"].endswith("1") else ""
            self.assertIn(f"[0xFE8, .rodata, overlays/french_model_variant/variant129_entry{suffix}]",
                          layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (64, 32, 0, 128, 128, 128, 8, 0, 200, 20, 200, 400, 512, 300, 60, 240, 8, 30, 28, 100),
            1: (128, 128, 96, 160, 160, 128, 5, 0, 200, 20, 200, 400, 512, 300, 60, 90, 3, 30, 24, 120),
            2: (96, 128, 96, 128, 160, 128, 28, 0, 200, 20, 200, 400, 512, 300, 120, 280, 4, 30, 64, 140),
            3: (96, 96, 128, 128, 160, 128, 9, 0, 200, 20, 200, 400, 512, 300, 120, 320, 6, 30, 48, 90),
            4: (128, 96, 96, 128, 160, 128, 13, 0, 200, 20, 200, 400, 512, 300, 120, 280, 4, 30, 52, 120),
        }
        arguments = {219: 0, 355: 1, 490: 2, 30: 3, 435: 4}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = arguments[int(row["model"])]
                self.assertEqual(int(row["command_word"]), 60000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 60000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<8B12h", data, 0x1030 + 32 * argument),
                                 descriptors[argument])
                observed.add(argument)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1017I", data[4:0xFE8])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 48)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x414 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model129Config)": 32, "sizeof(Model129State)": 0x414,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(GsIMAGE)": 28, "sizeof(POLY_FT4)": 40, "sizeof(POLY_G4)": 36,
        }
        for typename, fields in (
            ("Model129Config", (("part", 6), ("half_size", 8), ("unknown_0A", 10),
                                ("travel_duration", 20), ("flash_duration", 22), ("spacing", 24),
                                ("unknown_1A", 26), ("count", 28), ("delay", 30))),
            ("Model129State", (("positions", 4), ("velocities", 0x204), ("texture", 0x404),
                               ("elapsed", 0x40C), ("completed", 0x410), ("effect_started", 0x411))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 23)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant129_entry.h")
