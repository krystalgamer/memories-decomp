import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant97Tests(family435.FrenchModelVariant435Tests):
    family = 97
    slot_header_delta = 130
    module_count = 4
    distinct_images = 4
    binding_count = 24
    tail_start = 0x11BC
    spans = ((4, 0x1158),)
    helpers = ((4, 4436, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (100,)), (9, (150,)))
    entry_anchors = {
        4: 0x27BDFED8, 0x1150: 0x03E00008, 0x1154: 0x27BD0128,
        0x7D0: 0x00002012, 0x980: 0x00131040, 0x984: 0x86A30008,
        0x988: 0x00531021, 0x9D0: 0x00629021, 0xAE8: 0x00009012,
        0x10B8: 0x0053102A, 0x10D4: 0x00021027, 0x10D8: 0x30440001,
        0x10FC: 0x0053102A, 0x1120: 0xA3C2031C,
        0x1158: 4096, 0x115C: 4096, 0x1160: 4096, 0x1164: 0,
    }



    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, kind, size in ((0x1158, "u32", 16), (0x1168, "u8", 84)):
                self.assertIn(
                    f"D_{base + offset:X} = 0x{base + offset:X}; // type:{kind} size:0x{size:X} defined:true",
                    symbols)
            self.assertIn(f"[0x1168, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        expected = (
            (128,128,128,128,128,128,1,0,700,300,200,300,400,16,8,8,16,40,60,6,2048,108),
            (128,128,128,128,128,128,22,0,400,300,200,400,400,16,16,8,16,40,70,2,48,130),
        )
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = int(row["model"]) == 150
                command = 28000 + argument
                self.assertEqual(int(row["command_word"]), command)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], command)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                descriptor = struct.unpack_from("<8B14h", data, 0x11BC + 36 * argument)
                self.assertEqual(descriptor, expected[argument])
                self.assertTrue(all(descriptor[index] > 0 for index in (11,13,14,16,17,18)))
                self.assertLessEqual(descriptor[13] * 2, 32)
                self.assertLessEqual(descriptor[14], 16)
                self.assertLessEqual(descriptor[15], 8)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(v, 16) for v in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1109I", data[4:0x1158])
                calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                self.assertEqual(set(calls), addresses)
                self.assertEqual(len(calls), 63)
                self.assertEqual(len(addresses), 24)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x324 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model97Config)": 36, "sizeof(Model97State)": 0x324,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(POLY_FT4)": 40, "sizeof(GsIMAGE)": 28,
            "sizeof(((Model97State *)0)->path) / sizeof(SVECTOR)": 32,
            "sizeof(((Model97State *)0)->ring) / sizeof(SVECTOR)": 16,
            "sizeof(((Model97State *)0)->offsets) / sizeof(SVECTOR)": 8,
            "sizeof(((Model97State *)0)->texture) / sizeof(s32)": 3,
            "sizeof(((Model97State *)0)->unknown_104)": 256,
            "sizeof(((Model97State *)0)->unknown_2c4)": 64,
        }
        for typename, fields in (
            ("Model97Config", (
                ("part", 6), ("height", 8), ("radius", 10), ("half_size", 12),
                ("spread", 14), ("lift", 16), ("path_count", 18), ("ring_count", 20),
                ("particle_count", 22), ("beam_duration", 24), ("ring_duration", 26),
                ("particle_duration", 28), ("stagger", 30), ("spin", 32), ("delay", 34),
            )),
            ("Model97State", (
                ("config", 0), ("path", 4), ("ring", 0x204), ("offsets", 0x284),
                ("target", 0x304), ("initialized", 0x30C), ("texture", 0x310),
                ("completed", 0x31C), ("elapsed", 0x320),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 37)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant97_entry.h")
