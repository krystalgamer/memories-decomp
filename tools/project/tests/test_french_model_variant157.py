import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant157Tests(family435.FrenchModelVariant435Tests):
    family = 157
    slot_header_delta = 130
    module_count = 4
    distinct_images = 4
    binding_count = 28
    tail_start = 0x1084
    spans = ((4, 0x1058),)
    helpers = ((4, 4180, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (131, 145)),)
    entry_anchors = {
        4: 0x27BDFD38, 0x1050: 0x03E00008, 0x1054: 0x27BD02C8,
        0x1058: 4096, 0x105C: 4096, 0x1060: 4096, 0x1064: 0,
    }



    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, kind, size in ((0x1058, "u32", 16), (0x1068, "u8", 28)):
                self.assertIn(
                    f"D_{base + offset:X} = 0x{base + offset:X}; // type:{kind} size:0x{size:X} defined:true",
                    symbols)
            self.assertIn(f"[0x1068, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (192,192,192,128,128,128,96,0,128,10,500,150,1200,60,30,30,60,7,50),
            1: (192,128,192,128,128,128,96,0,128,10,600,150,1200,160,50,30,60,7,44),
        }
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = {131: 0, 145: 1}[int(row["model"])]
                self.assertEqual(int(row["command_word"]), 88000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 88000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                descriptor = struct.unpack_from("<10B9h", data, 0x1084 + 28 * argument)
                self.assertEqual(descriptor, descriptors[argument])
                self.assertGreater(descriptor[10], 2 * descriptor[11])
                self.assertTrue(all(descriptor[index] > 0 for index in (11,12,13,14,16,17)))
                observed.add(argument)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(v, 16) for v in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1045I", data[4:0x1058])
                calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                self.assertEqual(set(calls), addresses)
                self.assertEqual(len(calls), 50)
                self.assertEqual(len(addresses), 27)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x820 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model157Config)": 28, "sizeof(Model157State)": 0x820,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(DVECTOR)": 4, "sizeof(POLY_G3)": 28, "sizeof(POLY_F4)": 24,
            "sizeof(POLY_FT4)": 40, "sizeof(GsIMAGE)": 28,
            "sizeof(DVECTOR[34])": 136, "sizeof(u16[34])": 68, "sizeof(SVECTOR[192])": 1536,
        }
        for typename, fields in (
            ("Model157Config", (
                ("red", 0), ("green", 1), ("blue", 2), ("sprite_red", 3),
                ("sprite_green", 4), ("sprite_blue", 5), ("trail_red", 6),
                ("trail_green", 7), ("trail_blue", 8), ("unknown_09", 9),
                ("radius", 10), ("half_size", 12), ("fall_distance", 14),
                ("ring_duration", 16), ("fall_duration", 18), ("unknown_14", 20),
                ("flash_duration", 22), ("spacing", 24), ("delay", 26),
            )),
            ("Model157State", (
                ("config", 0), ("origin", 4), ("ring", 12), ("starts", 276),
                ("trails", 532), ("texture", 2068), ("completed", 2072),
                ("toggle", 2073), ("elapsed", 2076),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 41)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant157_entry.h")
