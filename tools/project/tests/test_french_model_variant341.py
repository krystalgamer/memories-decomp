import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant341Tests(family435.FrenchModelVariant435Tests):
    family = 341
    source_family = 324
    module_count = 4
    distinct_images = 4
    binding_count = 36
    tail_start = 0x2D58
    spans = ((4, 0xF38), (0xF38, 0x1354), (0x1354, 0x17C0),
             (0x17C0, 0x22F8), (0x22F8, 0x26C4), (0x26C4, 0x29D8), (0x29D8, 0x2D58))
    helpers = ((0xF38, 1052, "webs", "func_8013BF3C"),
               (0x22F8, 972, "draw", "func_8013D2F8"),
               (0x26C4, 788, "spokes", "func_8013D6C8"),
               (0x29D8, 896, "rings", "func_8013D9E0"))
    reachable_helpers = {0xF38, 0x22F8}
    local_call_targets = {0xF38, 0x1354, 0x17C0, 0x22F8}
    models_by_stage = ((7, (7, 552)),)
    entry_anchors = {
        0x0C: 0x00809021, 0x14: 0x0240B021, 0x20: 0x26D806CC, 0x24: 0xAFB80084,
        0x28: 0x26D80764, 0x2C: 0xAFB80088, 0x30: 0x26D80AC4, 0x34: 0xAFB8008C,
        0x38: 0x26D80D04, 0xA8: 0x00181880, 0xAC: 0x00781821,
        0xB0: 0x00031880, 0xB8: 0xAEC30F2C, 0x888: 0xAC60FFF8,
        0x898: 0x24630098, 0x89C: 0x27180098, 0x8A0: 0x1AE0FFA4,
        0x924: 0x2AE20008, 0x960: 0x27180090, 0x9A0: 0x2B020006,
        0x9A8: 0x26520090, 0xA84: 0x27180090, 0xA94: 0x2B020004,
        0xA9C: 0x26520090, 0xAD4: 0xAEC00F50, 0xD88: 0x02402021,
        0x430: 0x2714019C, 0x510: 0x265000C0, 0x54C: 0x26520008,
        0x568: 0x2A620006, 0x594: 0x27180030, 0x5C8: 0x2B020004,
        0x5EC: 0x271801A0, 0x604: 0xA282FFE4, 0x610: 0x2AE20003,
        0x620: 0xAE80FFFC, 0x634: 0xAE83FFF8, 0x63C: 0x269401A0,
        0xDD4: 0x02402021, 0xF38: 0x27BDFED8, 0xF7C: 0x26B10EB0,
        0xFBC: 0x26B20194, 0xFE0: 0x28821801, 0x106C: 0x8E820F50,
        0x1080: 0x8E820ED8, 0x10A4: 0x86820EE4, 0x116C: 0x266200C0,
        0x11A0: 0x27A200D0, 0x11A8: 0x27A200D4, 0x11FC: 0xA228000F,
        0x121C: 0x18C0000A, 0x122C: 0x18400006, 0x1240: 0x30C6FFFF,
        0x1254: 0x28420006, 0x1270: 0x28420004, 0x1290: 0x8E820F24,
        0x1298: 0x00021200, 0x12C8: 0xAE440004, 0x12E8: 0x14840004,
        0x12F4: 0xAE820F50, 0x12FC: 0x265201A0, 0x1318: 0x28420003,
    }

    def test_selected_descriptor_and_context_load_separation(self):
        path = family435.ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(row["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<I", data)[0], int(row["header"]))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(row["command_word"]))
                table = base + 0x2E54
                self.assertEqual(struct.unpack_from("<I", data, 0x98)[0],
                                 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x9C)[0],
                                 0x24420000 | (table & 0xFFFF))
                descriptor = 0x2E54 + command % 1000 * 20
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 20, len(data))
                self.assertEqual(3 * 416, 0x4E0)
                self.assertEqual(0x6CC + 152, 0x764)
                self.assertEqual(0x764 + 6 * 144, 0xAC4)
                self.assertEqual(0xAC4 + 4 * 144, 0xD04)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + 0xF64 <= start or start + size <= context)
