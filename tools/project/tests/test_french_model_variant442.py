import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant442Tests(family435.FrenchModelVariant435Tests):
    family = 442
    source_family = 425
    module_count = 4
    distinct_images = 4
    binding_count = 36
    tail_start = 0x4430
    spans = ((4, 0x1174), (0x1174, 0x1560), (0x1560, 0x1F2C),
             (0x1F2C, 0x2924), (0x2924, 0x2CE8), (0x2CE8, 0x3334),
             (0x3334, 0x3A40), (0x3A40, 0x3D50), (0x3D50, 0x40CC),
             (0x40CC, 0x4430))
    helpers = ((0x2924, 964, "webs", "func_8013D8F8"),
               (0x3334, 1804, "bands", "func_8013E2F4"),
               (0x3A40, 784, "spokes", "func_8013EA00"),
               (0x3D50, 892, "rings", "func_8013ED14"),
               (0x40CC, 868, "quad", "func_8013F094"))
    reachable_helpers = {0x2924}
    local_call_targets = {0x1174, 0x1560, 0x1F2C, 0x2924}
    models_by_stage = ((7, (259, 630)),)
    entry_anchors = {
        0x0C: 0x00809821, 0x14: 0x0260B021,
        0x18: 0x26D81CA0, 0x1C: 0xAFB80080,
        0x20: 0x26D81E68, 0x24: 0xAFB80084,
        0x28: 0x26D81F00, 0x2C: 0xAFB80088,
        0x30: 0x26D82260, 0x34: 0xAFB8008C,
        0x38: 0x26D824A0, 0x3C: 0xAFB80090,
        0x3A8: 0x00002821, 0x3BC: 0x00009821,
        0x3DC: 0xA0620144, 0x418: 0xA0620168,
        0x444: 0x2A620009, 0x44C: 0x24630004,
        0x450: 0x24A50001, 0x460: 0x8FB80080,
        0x468: 0x271801C8, 0x46C: 0x18A0FFD2, 0x470: 0xAFB80080,
        0x588: 0x27180090, 0x598: 0x1B00FFBB,
        0x764: 0x27180098, 0x784: 0x1B00FF8B,
        0xA7C: 0x27180090, 0xABC: 0x2B020006,
        0xCB4: 0x27180090, 0xCC4: 0x2B020004, 0xD04: 0xAEC02740,
        0x478: 0x8FB80090, 0x480: 0xAFA000B0,
        0x514: 0x8FB800B0, 0x51C: 0x27180001, 0x520: 0xAFB800B0,
        0x580: 0x8FB80090, 0x58C: 0xAFB80090, 0x590: 0x8FB800B0,
        0x9BC: 0x8FB80088, 0x9C4: 0xAFA000B4,
        0xBD0: 0x8FB8008C, 0xBD8: 0xAFA000B4,
        0x40: 0x26D80600, 0x8C: 0xAFB60094, 0x798: 0x8FB80094,
        0x7A4: 0x271401FC, 0x880: 0x265000F0, 0x8D4: 0x2A620006,
        0x8F4: 0x27180030, 0x91C: 0x2B020005, 0x974: 0x27180200,
        0x9B0: 0x2B020003, 0x9B8: 0x26940200, 0xFF4: 0x02602021,
        0x292C: 0x00809821, 0x297C: 0x2671266C, 0x29A8: 0x267201F4,
        0x2BBC: 0x28420006, 0x2BD8: 0x28420005, 0x2C90: 0x26520200,
        0x2CAC: 0x28420003, 0x2CB4: 0x26730200,
    }

    def test_selected_web_descriptor_and_context_separation(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                instance = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(instance["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x452C
                self.assertEqual(struct.unpack_from("<I", data, 0xA8)[0],
                                 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xAC)[0],
                                 0x24420000 | (table & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xFF0)[0],
                                 0x0C000000 | (((base + 0x2924) >> 2) & 0x3FFFFFF))
                archive.seek((int(instance["record"]) * 276 + 275) * 2048 + 0x110)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(instance["command_word"]))
                descriptor = 0x452C + command % 1000 * 56
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 56, len(data))
                self.assertEqual(5 * 6 * 8, 0xF0)
                self.assertEqual(3 * 0x200, 0x600)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + 0x2760 <= start or start + size <= context)
