import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant117Tests(family435.FrenchModelVariant435Tests):
    family = 117
    slot_header_delta = 130
    module_count = 6
    distinct_images = 5
    binding_count = 22
    tail_start = 0x1144
    spans = ((4, 0x108C),)
    helpers = ((4, 4232, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (139, 146)), (9, (15,)))
    entry_anchors = {
        4: 0x27BDFEE8, 0xD4: 0xA6A00000,
        0xD8: 0xA640FFFE, 0xE0: 0xA6400000,
        0x4DC: 0x86E20706, 0xC98: 0x1040000A,
        0x1084: 0x03E00008, 0x1088: 0x27BD0118,
        0x108C: 4096, 0x1090: 4096, 0x1094: 4096, 0x1098: 0,
    }



    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(
                f"D_{base + 0x109C:X} = 0x{base + 0x109C:X}; // type:u8 size:0xA8 defined:true",
                symbols)
            self.assertIn(f"[0x109C, data, overlays/{module['name']}/image_view]", layout.read_text())
            self.assertIn(
                f"D_{base + 0x108C:X} = 0x{base + 0x108C:X}; // type:u32 size:0x10 defined:true",
                symbols)

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (128,128,128,1,80,100,80,200,200,32,2,34,30,5,40,4,32,8,0),
            1: (128,128,128,2,100,50,80,200,120,20,2,34,30,10,30,4,32,8,0),
            3: (128,128,128,8,70,30,120,200,200,40,1,34,30,4,100,4,64,1,0),
        }
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = {15: 0, 146: 1, 139: 3}[int(row["model"])]
                self.assertEqual(int(row["command_word"]), 48000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 48000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                descriptor = struct.unpack_from("<4B11h4B", data, 0x1144 + 30 * argument)
                self.assertEqual(descriptor, descriptors[argument])
                self.assertTrue(0 < descriptor[9] <= 40)
                self.assertTrue(all(descriptor[index] > 0 for index in (6, 10, 11, 12, 17)))
                self.assertLess(descriptor[6] + 350, 32768)
                observed.add(argument)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(v, 16) for v in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1058I", data[4:0x108C])
                calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                self.assertEqual(set(calls), addresses)
                self.assertEqual(len(addresses), 22)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x724 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model117Config)": 30, "sizeof(Model117State)": 0x724,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(POLY_FT4)": 40, "sizeof(GsIMAGE)": 28,
        }
        for typename, fields in (
            ("Model117Config", (
                ("size", 4), ("arc_height", 6), ("spread", 8), ("radius", 10),
                ("flash_size", 12), ("count", 14), ("rows", 16),
                ("travel_duration", 18), ("fade_duration", 20), ("spacing", 22),
                ("delay", 24), ("tile_size", 27), ("frames", 28),
            )),
            ("Model117State", (
                ("positions", 4), ("targets", 0x204), ("ends", 0x404),
                ("rows", 0x604), ("frames", 0x644), ("scales", 0x684),
                ("textures", 0x704), ("completed", 0x71C), ("elapsed", 0x720),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 29)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant117_entry.h")
