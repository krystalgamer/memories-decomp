import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant132Tests(family435.FrenchModelVariant435Tests):
    family = 132
    slot_header_delta = 130
    module_count = 2
    distinct_images = 2
    binding_count = 26
    tail_start = 0xEE4
    spans = ((4, 0xEB8),)
    helpers = ((4, 3764, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (416,)),)
    entry_anchors = {
        4: 0x27BDFD20, 0xF8: 0x000211C3, 0xFC: 0x00021840,
        0x100: 0x00621821, 0x114: 0x2A62002A, 0x130: 0x34426061,
        0x1E8: 0x2A620055, 0x204: 0x0013A200, 0x2AC: 0x2A620011,
        0x310: 0xA5400516, 0x398: 0x0C016FC9, 0x45C: 0x8542051E,
        0x5E8: 0x00021400, 0x81C: 0x86430018, 0xD74: 0x0C022706,
        0xDAC: 0x30840020, 0xE60: 0x8FAA0288, 0xE70: 0x14400004,
        0xEB0: 0x03E00008, 0xEB4: 0x27BD02E0,
        0xEB8: 4096, 0xEBC: 4096, 0xEC0: 4096, 0xEC4: 0,
    }



    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, kind, size in ((0xEB8, "u32", 16), (0xEC8, "u8", 28)):
                self.assertIn(
                    f"D_{base + offset:X} = 0x{base + offset:X}; // type:{kind} size:0x{size:X} defined:true",
                    symbols)
            self.assertIn(f"[0xEC8, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        expected = (128,128,128,255,255,255,128,128,128,100,400,600,150,50,12,3,32,16,30,60,282)
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                self.assertEqual(int(row["command_word"]), 63000)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, 63000)
                argument = command % 1000
                self.assertEqual(argument, 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                descriptor = struct.unpack_from("<9Bx12h", data, 0xEE4 + 34 * argument)
                self.assertEqual(descriptor, expected)
                self.assertEqual(struct.unpack_from("<i4hI4hI", data, 0xEC8),
                                 (9,832,256,64,64,0,640,8,256,1,0))
                count = descriptor[16]
                self.assertEqual(count, 32)
                self.assertEqual(max(i * (128 // count) for i in range(count)), 124)
                self.assertEqual(count * (128 // count), 128)
                self.assertEqual(42 + 85, 127)
                self.assertEqual(max(j + 18 for j in range(16)), 33)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(v, 16) for v in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<941I", data[4:0xEB8])
                calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                self.assertEqual(set(calls), addresses)
                self.assertEqual(len(calls), 43)
                self.assertEqual(len(addresses), 25)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x528 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model132Config)": 34, "sizeof(Model132State)": 0x528,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(POLY_FT4)": 40, "sizeof(POLY_F4)": 24, "sizeof(DISPENV)": 20,
            "sizeof(GsIMAGE)": 28, "sizeof(DVECTOR)": 4, "sizeof(PSXLONG)": 4,
            "sizeof(((Model132State *)0)->path) / sizeof(SVECTOR)": 128,
            "sizeof(((Model132State *)0)->ring) / sizeof(SVECTOR)": 34,
            "sizeof(((Model132State *)0)->position) / sizeof(u16)": 4,
            "sizeof(((Model132State *)0)->texture) / sizeof(s32)": 1,
        }
        for typename, fields in (
            ("Model132Config", (
                ("red", 0), ("flash_red", 3), ("ring_red", 6), ("unknown_09", 9),
                ("radius", 10), ("curve_width", 12), ("path_height", 14),
                ("half_size", 16), ("ring_height", 18), ("radial_count", 20),
                ("ring_count", 22), ("particle_count", 24), ("particle_duration", 26),
                ("flash_duration", 28), ("ring_duration", 30), ("delay", 32),
            )),
            ("Model132State", (
                ("config", 0), ("path", 4), ("ring", 1028), ("position", 1300),
                ("texture", 1308), ("elapsed", 1312), ("started", 1316),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 38)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant132_entry.h")
