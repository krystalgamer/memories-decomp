import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant107Tests(family435.FrenchModelVariant435Tests):
    family = 107
    slot_header_delta = 130
    module_count = 6
    distinct_images = 6
    binding_count = 25
    tail_start = 0x1198
    spans = ((4, 0x1188),)
    helpers = ((4, 4484, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (402,)), (9, (28, 85)))
    entry_anchors = {
        4: 0x27BDFC88, 0x434: 0x0C016FB5, 0x690: 0x0C021F12,
        0xCF0: 0x0C021F12, 0xCF8: 0x0C020263, 0xD4C: 0x0C020B3A,
        0x1078: 0x0C021C7F, 0x1180: 0x03E00008, 0x1184: 0x27BD0378,
        0x1188: 4096, 0x118C: 4096, 0x1190: 4096, 0x1194: 0,
    }



    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(f"D_{base + 0x1188:X} = 0x{base + 0x1188:X}; "
                          "// type:u32 size:0x10 defined:true", symbols)
            self.assertNotIn("image_view", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        expected = {
            0: (128,192,192,255,255,192,13,0,200,40,200,400,512,30,8,30,25,100),
            1: (255,255,32,128,128,128,22,0,300,50,150,400,1024,30,8,30,20,40),
            3: (255,255,96,128,128,128,27,0,300,70,150,400,1024,30,5,30,20,60),
        }
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = {402: 3, 28: 1, 85: 0}[int(row["model"])]
                command = 38000 + argument
                self.assertEqual(int(row["command_word"]), command)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], command)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                descriptor = struct.unpack_from("<8B10h", data, 0x1198 + 28 * argument)
                self.assertEqual(descriptor, expected[argument])
                self.assertTrue(0 < descriptor[16] <= 25)
                self.assertGreater(descriptor[13], 0)
                self.assertTrue(0 < descriptor[10] < 450)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(v, 16) for v in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1121I", data[4:0x1188])
                calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                self.assertEqual(set(calls), addresses)
                self.assertEqual(len(calls), 41)
                self.assertEqual(len(addresses), 25)
                self.assertIn("GetDispEnv = 0x8008098C;", bindings)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x3B0 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model107Config)": 28, "sizeof(Model107State)": 0x3B0,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(POLY_GT4)": 52, "sizeof(POLY_G4)": 36,
            "sizeof(GsGLINE)": 20, "sizeof(DISPENV)": 20,
            "sizeof(((Model107State *)0)->ring) / sizeof(SVECTOR)": 51,
            "sizeof(((Model107State *)0)->positions) / sizeof(SVECTOR)": 25,
            "sizeof(((Model107State *)0)->unknown_264)": 320,
        }
        for typename, fields in (
            ("Model107Config", (
                ("part", 6), ("radius", 8), ("thickness", 10), ("near_z", 12),
                ("extra_z", 14), ("minimum_scale", 16), ("duration", 18),
                ("stagger", 20), ("unknown_16", 22), ("count", 24), ("delay", 26),
            )),
            ("Model107State", (
                ("config", 0), ("ring", 4), ("positions", 0x19C),
                ("unknown_264", 0x264), ("completed", 0x3A4),
                ("elapsed", 0x3A8), ("started", 0x3AC),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 30)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant107_entry.h")
