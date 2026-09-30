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
    helpers = ((0x22F8, 972, "draw", "func_8013D2F8"),
               (0x26C4, 788, "spokes", "func_8013D6C8"),
               (0x29D8, 896, "rings", "func_8013D9E0"))
    reachable_helpers = {0x22F8}
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
    }

    def test_selected_descriptor_and_context_load_separation(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
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
                self.assertEqual(0x6CC + 152, 0x764)
                self.assertEqual(0x764 + 6 * 144, 0xAC4)
                self.assertEqual(0xAC4 + 4 * 144, 0xD04)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + 0xF64 <= start or start + size <= context)
