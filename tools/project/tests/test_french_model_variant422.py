import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant422Tests(family435.FrenchModelVariant435Tests):
    family = 422
    source_family = 405
    module_count = 4
    distinct_images = 4
    binding_count = 36
    tail_start = 0x3834
    spans = ((4, 0x1128), (0x1128, 0x1620), (0x1620, 0x1CFC),
             (0x1CFC, 0x2408), (0x2408, 0x28F0), (0x28F0, 0x2E44),
             (0x2E44, 0x3154), (0x3154, 0x34D0), (0x34D0, 0x3834))
    helpers = ((0x1CFC, 1804, "bands", "func_8013CD04"),
               (0x2408, 1256, "sheets", "func_8013D410"),
               (0x2E44, 784, "spokes", "func_8013DE54"),
               (0x3154, 892, "rings", "func_8013E168"),
               (0x34D0, 868, "quad", "func_8013E4E8"))
    reachable_helpers = set()
    local_call_targets = {0x1128, 0x1620}
    models_by_stage = ((7, (1, 550)),)
    entry_anchors = {
        0x0C: 0x00809821, 0x14: 0x0260B021,
        0x18: 0x26D812B4, 0x1C: 0xAFB80088,
        0x20: 0x26D8147C, 0x24: 0xAFB8008C,
        0x28: 0x26D815AC, 0x2C: 0xAFB80090,
        0x30: 0x26D8190C, 0x34: 0xAFB80094,
        0x38: 0x26D81B4C, 0x3C: 0xAFB80098,
        0x57C: 0x00002821, 0x590: 0x0000A021,
        0x5B0: 0xA0620144, 0x5EC: 0xA0620168,
        0x618: 0x2A820009, 0x620: 0x24630004,
        0x624: 0x24A50001, 0x634: 0x8FB80088,
        0x63C: 0x271801C8, 0x640: 0x18A0FFD2, 0x644: 0xAFB80088,
        0x75C: 0x27180090, 0x76C: 0x1B00FFBB,
        0x938: 0x27180098, 0x958: 0x2B020002,
        0xC4C: 0x27180090, 0xC68: 0x2A620006,
        0xD70: 0x2A620004, 0xD74: 0x27180090, 0xDB0: 0xAEC01E18,
        0x64C: 0x8FB80098, 0x654: 0xAFA000B8,
        0x6E8: 0x8FB800B8, 0x6F0: 0x27180001, 0x6F4: 0xAFB800B8,
        0x754: 0x8FB80098, 0x760: 0xAFB80098, 0x764: 0x8FB800B8,
        0xB94: 0x00009821, 0xB98: 0x8FB80090,
        0xC90: 0x00009821, 0xC94: 0x8FB80094,
    }

    def test_retained_sheet_descriptor_and_context(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                instance = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(instance["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x386C
                self.assertEqual(struct.unpack_from("<I", data, 0xB4)[0],
                                 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xB8)[0],
                                 0x24420000 | (table & 0xFFFF))
                archive.seek((int(instance["record"]) * 276 + 275) * 2048 + 0x110)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(instance["command_word"]))
                self.assertEqual(command, 588000)
                descriptor = 0x386C + command % 1000 * 48
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 48, len(data))
                self.assertEqual(struct.unpack_from("<2i", data, descriptor + 0x1C), (60, 120))
                for offset, word in {
                    0xC4: 0x00181840, 0xC8: 0x00781821, 0xCC: 0x00031900,
                    0xD4: 0xAEC31DC8, 0x2408: 0x27BDFEF0, 0x2410: 0x00808821,
                    0x2418: 0x263E147C, 0x2440: 0x26321C84, 0x2448: 0x26301504,
                    0x2684: 0x0C021E56, 0x2714: 0x18400005, 0x2728: 0x3046FFFF,
                    0x273C: 0x2AE20004, 0x277C: 0x8E231DC8, 0x2784: 0x8C64001C,
                    0x2788: 0x8C630020, 0x2798: 0x0043001B, 0x28A4: 0x26100098,
                    0x28AC: 0x27DE0098, 0x28B4: 0x29020002,
                }.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                self.assertEqual(0x147C + 2 * 152, 0x15AC)
                self.assertEqual(0x147C + 136, 0x1504)
                self.assertNotIn(0x2408, self.local_call_targets | self.reachable_helpers)
                widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack("<1097I", data[4:0x1128])
                            if word >> 26 in widths and word >> 21 & 31 == 22 and not word & 0x8000]
                self.assertEqual(max(accesses), 0x1E38)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 4096), (base, 20480)):
                    self.assertTrue(context + 0x1E38 <= start or start + size <= context)
