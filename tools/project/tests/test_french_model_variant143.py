import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant143Tests(family435.FrenchModelVariant435Tests):
    family = 143
    slot_header_delta = 130
    module_count = 8
    distinct_images = 8
    binding_count = 24
    tail_start = 0x1020
    spans = ((4, 0xFF4),)
    helpers = ((4, 4080, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (481, 496)), (9, (234, 546)))
    entry_anchors = {
        4: 0x27BDFEA8, 0xE0: 0xA7C00000, 0xE4: 0xA460FFFC,
        0xE8: 0xA460FFFE, 0xEC: 0xA4600000,
        0x810: 0xA6C00002, 0x814: 0xA6C20002,
        0xAE8: 0x9582068E,
        0xFEC: 0x03E00008, 0xFF0: 0x27BD0158,
        0xFF4: 4096, 0xFF8: 4096, 0xFFC: 4096, 0x1000: 0,
    }



    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(
                f"D_{base + 0x1004:X} = 0x{base + 0x1004:X}; // type:u8 size:0x1C defined:true",
                symbols)
            self.assertIn(f"[0x1004, data, overlays/{module['name']}/image_view]", layout.read_text())
            self.assertIn(
                f"D_{base + 0xFF4:X} = 0x{base + 0xFF4:X}; // type:u32 size:0x10 defined:true",
                symbols)
            suffix = "_slot1" if module["name"].endswith("1") else ""
            self.assertIn(f"[0xFF4, .rodata, overlays/french_model_variant/variant143_entry{suffix}]",
                          layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (160,192,48,128,128,128,1,44,100,15,200,100,100,30,30,12,0,0,160,6,180),
            1: (208,96,32,128,128,128,21,55,20,15,200,20,100,40,18,12,110,0,160,3,150),
            2: (224,224,160,128,128,128,24,32,100,8,70,50,100,30,30,14,0,0,120,6,60),
            3: (224,224,160,128,128,128,21,16,50,8,100,60,150,30,30,14,0,0,120,4,80),
        }
        arguments = {496: 0, 546: 1, 481: 2, 234: 3}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = arguments[int(row["model"])]
                self.assertEqual(int(row["command_word"]), 74000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 74000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                descriptor = struct.unpack_from("<8B13h", data, 0x1020 + 34 * argument)
                self.assertEqual(descriptor, descriptors[argument])
                self.assertTrue(0 < descriptor[7] <= 55)
                self.assertTrue(all(descriptor[index] > 0 for index in (10, 11, 13, 14, 15)))
                observed.add(argument)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1020I", data[4:0xFF4])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 42)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x698 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model143Config)": 34, "sizeof(Model143State)": 0x698,
            "sizeof(Model143Face)": 12, "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8,
            "sizeof(VECTOR)": 16, "sizeof(POLY_F3)": 20, "sizeof(POLY_FT4)": 40,
            "sizeof(CVECTOR)": 4, "sizeof(GsIMAGE)": 28,
        }
        for typename, fields in (
            ("Model143Config", (
                ("red", 0), ("green", 1), ("blue", 2), ("sprite_red", 3),
                ("sprite_green", 4), ("sprite_blue", 5), ("part", 6), ("count", 7),
                ("length", 8), ("half_width", 10), ("spread", 12), ("height_spread", 14),
                ("sprite_half_size", 16), ("travel_duration", 18), ("hold_duration", 20),
                ("sprite_duration", 22), ("rotation_x", 24), ("rotation_y", 26),
                ("rotation_z", 28), ("spacing", 30), ("delay", 32),
            )),
            ("Model143State", (
                ("config", 0), ("positions", 4), ("vertices", 0x204), ("targets", 0x22C),
                ("rotations", 0x42C), ("faces", 0x62C), ("colors", 0x674),
                ("texture", 0x68C), ("elapsed", 0x690), ("completed", 0x694),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 41)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant143_entry.h")
