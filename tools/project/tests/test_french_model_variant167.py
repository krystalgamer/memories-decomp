import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant167Tests(family435.FrenchModelVariant435Tests):
    family = 167
    slot_header_delta = 130
    module_count = 2
    distinct_images = 2
    binding_count = 22
    tail_start = 0xF20
    spans = ((4, 0xED8),)
    helpers = ((4, 3796, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (71,)),)
    entry_anchors = {
        4: 0x27BDFEF0, 0xA8: 0x27D00204, 0xAC: 0x27D10404,
        0xB8: 0xA7C00606, 0x140: 0x2A620100, 0x168: 0xA7A200B2,
        0x310: 0x24050002, 0x33C: 0x24050001, 0x574: 0x97C2060E,
        0x688: 0x00C0A021, 0x7E8: 0x87C20606, 0x98C: 0x0C0135B1,
        0xA98: 0x04610002, 0xA9C: 0xA6020000, 0xB60: 0xA7C20606,
        0xB9C: 0x97C2060A, 0xE84: 0x24020004, 0xE90: 0x10400003,
        0xEA0: 0xA3C20610, 0xED0: 0x03E00008, 0xED4: 0x27BD0110,
        0xED8: 4096, 0xEDC: 4096, 0xEE0: 4096, 0xEE4: 0,
    }



    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, kind, size in ((0xED8, "u32", 16), (0xEE8, "u8", 56)):
                self.assertIn(
                    f"D_{base + offset:X} = 0x{base + offset:X}; // type:{kind} size:0x{size:X} defined:true",
                    symbols)
            self.assertIn(f"[0xEE8, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        expected = (32,32,32,48,48,48,1300,200,350,90,40,64,90,4,64,20)
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                self.assertEqual(int(row["command_word"]), 98000)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, 98000)
                argument = command % 1000
                self.assertEqual(argument, 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<6B2x10h", data, 0xF20 + 28 * (argument % 100)),
                                 expected)
                self.assertEqual(data[0xF26:0xF28], b"\x19\x00")
                self.assertEqual([struct.unpack_from("<i4hI4hI", data, 0xEE8 + i * 28)
                                  for i in range(2)],
                                 [(8,864,256,32,64,0,640,8,16,1,0),
                                  (9,832,256,32,64,0,656,8,256,1,0)])
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(v, 16) for v in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<949I", data[4:0xED8])
                calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                self.assertEqual(set(calls), addresses)
                self.assertEqual(len(calls), 37)
                self.assertEqual(len(addresses), 22)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x618 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model167Config)": 28, "sizeof(Model167State)": 0x618,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(POLY_FT4)": 40, "sizeof(GsIMAGE)": 28, "sizeof(PSXLONG)": 4,
            "sizeof(((Model167State *)0)->points) / sizeof(SVECTOR)": 64,
            "sizeof(((Model167State *)0)->offsets) / sizeof(s16)": 256,
            "sizeof(((Model167State *)0)->increments) / sizeof(s16)": 256,
            "sizeof(((Model167State *)0)->texture) / sizeof(u32)": 2,
        }
        for typename, fields in (
            ("Model167Config", (
                ("red", 0), ("particle_red", 3), ("unknown_06", 6), ("amplitude", 8),
                ("half_size", 10), ("radius", 12), ("fade_duration", 14), ("unknown_10", 16),
                ("particle_duration", 18), ("particle_delay", 20), ("stagger", 22),
                ("count", 24), ("delay", 26),
            )),
            ("Model167State", (
                ("config", 0), ("points", 4), ("offsets", 516), ("increments", 1028),
                ("phase", 1540), ("scroll", 1542), ("texture", 1544),
                ("started", 1552), ("elapsed", 1556),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 34)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant167_entry.h")
