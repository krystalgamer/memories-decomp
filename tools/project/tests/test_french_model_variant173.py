import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant173Tests(family435.FrenchModelVariant435Tests):
    family = 173
    slot_header_delta = 130
    module_count = 12
    distinct_images = 12
    binding_count = 23
    tail_start = 0xFD8
    spans = ((4, 0xEB0),)
    helpers = ((4, 3756, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (392,)), (9, (8, 113, 350, 386, 448)))
    entry_anchors = {
        4: 0x27BDFEC8, 0xEA8: 0x03E00008, 0xEAC: 0x27BD0138,
        0xEB0: 4096, 0xEB4: 4096, 0xEB8: 4096, 0xEBC: 0,
    }



    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0xEC0:X} = 0x{base + 0xEC0:X}; // type:u8 size:0x118 defined:true", symbols)
            self.assertIn(f"D_{base + 0xEB0:X} = 0x{base + 0xEB0:X}; // type:u32 size:0x10 defined:true", symbols)
            slot = int(module["name"][-1])
            source = "overlays/french_model_variant/variant173_entry" + ("_slot1" if slot else "")
            self.assertIn(f"[0xEB0, .rodata, {source}]", layout.read_text())
            self.assertIn(f"[0xEC0, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (48, 48, 48, 4, 100, 300, 100, 40, 20, 30, 6, 256, 1, 120, 640),
            1: (16, 48, 16, 2, 100, 300, 100, 40, 20, 30, 6, 256, 1, 120, 640),
            2: (48, 48, 48, 5, 100, 300, 100, 40, 20, 30, 6, 256, 1, 70, 640),
            103: (16, 0, 16, 13, 300, 500, 150, 20, 10, 10, 2, 128, 1, 150, 160),
            104: (32, 32, 36, 15, 250, 400, 450, 40, 20, 30, 2, 256, 2, 120, 160),
            105: (16, 16, 19, 18, 250, 450, 450, 40, 20, 30, 2, 256, 2, 120, 160),
        }
        arguments = {(7, 392): 103, (9, 8): 0, (9, 113): 104,
                     (9, 350): 2, (9, 386): 1, (9, 448): 105}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot = int(row["slot"])
                argument = arguments[int(row["stage"]) - slot, int(row["model"])]
                self.assertEqual(int(row["command_word"]), 104000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110
                             + (int(row["stage"]) - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 104000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                observed.add(argument)
                descriptor = struct.unpack_from("<4B11h", data, 0xFD8 + 26 * (argument % 100))
                self.assertEqual(descriptor, descriptors[argument])
                self.assertTrue(0 < descriptor[10] <= descriptor[11] <= 256)
                if argument < 100:
                    self.assertEqual(descriptor[11] // descriptor[10], 42)
                    self.assertEqual(descriptor[11] % descriptor[10], 4)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                self.assertIn("GsGetLwUnit = 0x8008A428;", bindings)
                self.assertIn("ccos = 0x800868A8;", bindings)
                self.assertIn("csin = 0x80086B38;", bindings)
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<939I", data[4:0xEB0])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 45)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 10 * 2048)):
                    self.assertTrue(context + 0x1244 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model173Config)": 26, "sizeof(Model173State)": 0x1244,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(GsIMAGE)": 28, "sizeof(POLY_FT4)": 40, "sizeof(POLY_F4)": 24,
        }
        for typename, fields in (
            ("Model173Config", (("half_size", 4), ("spread", 6), ("gather_duration", 10),
                                ("travel_duration", 12), ("burst_duration", 14),
                                ("group_size", 16), ("count", 18), ("spacing", 20),
                                ("delay", 22), ("unknown_18", 24))),
            ("Model173State", (("positions", 4), ("offsets", 0x804), ("center", 0x1004),
                               ("texture", 0x100C), ("completed", 0x1034),
                               ("elapsed", 0x1238), ("mode", 0x123C), ("frames", 0x1240))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 26)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant173_entry.h")
