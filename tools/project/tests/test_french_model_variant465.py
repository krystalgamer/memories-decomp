import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant465Tests(family435.FrenchModelVariant435Tests):
    family = 465
    source_family = 448
    module_count = 4
    distinct_images = 4
    binding_count = 37
    tail_start = 0x4354
    spans = (
        (0x4, 0x124C),
        (0x124C, 0x166C),
        (0x166C, 0x256C),
        (0x256C, 0x2AB4),
        (0x2AB4, 0x3014),
        (0x3014, 0x331C),
        (0x331C, 0x3698),
        (0x3698, 0x39FC),
        (0x39FC, 0x4354),
    )
    helpers = (
        (0x2AB4, 1376, "webs", "func_8013DB58"),
        (0x3014, 776, "spokes", "func_8013E0BC"),
        (0x331C, 892, "rings", "func_8013E3C8"),
        (0x3698, 868, "quad", "func_8013E748"),
    )
    reachable_helpers = set()
    local_call_targets = {0x124C, 0x166C, 0x256C}
    models_by_stage = ((9, (108, 573)),)
    entry_anchors = {
        0xC: 0x809021,
        0x14: 0x240B021,
        0x28: 0x26D811F0,
        0x2C: 0xAFB8008C,
        0x30: 0x26D81318,
        0x34: 0xAFB80090,
        0x38: 0x26D81678,
        0x3C: 0xAFB80094,
        0x40: 0x26D818B8,
        0x44: 0xAFB80098,
        0x62C: 0x8FB8008C,
        0x634: 0xAFA000B4,
        0x638: 0x27040090,
        0x63C: 0x8FB8008C,
        0x7E4: 0xAC80FFF8,
        0x7F4: 0xA021,
        0x800: 0x9821,
        0x818: 0xAC400098,
        0x81C: 0xAC4000E0,
        0x820: 0x2A620003,
        0x830: 0x2A820002,
        0x840: 0x29020003,
        0x84C: 0x8FB800B4,
        0x854: 0x27180001,
        0x858: 0xAFB800B4,
        0x85C: 0x8FB8008C,
        0x864: 0x27180128,
        0x868: 0xAFB8008C,
        0x86C: 0x8FB800B4,
        0x874: 0x1B00FF71,
        0x878: 0x24840128,
        0x95C: 0x8FB80098,
        0x964: 0xAFA000B4,
        0x968: 0x27110088,
        0x9F8: 0x8FB800B4,
        0xA00: 0x27180001,
        0xA04: 0xAFB800B4,
        0xA58: 0xAE20FFF8,
        0xA64: 0x8FB80098,
        0xA6C: 0x27180090,
        0xA70: 0xAFB80098,
        0xA74: 0x8FB800B4,
        0xA7C: 0x1B00FFBB,
        0xCB4: 0xA021,
        0xCB8: 0x8FB80090,
        0xCC0: 0x2712008C,
        0xCC4: 0x8FB00090,
        0xCE4: 0xA6020000,
        0xD10: 0xA6030040,
        0xD1C: 0x26100008,
        0xD34: 0x2B020008,
        0xD54: 0x26940001,
        0xD64: 0x8FB80090,
        0xD6C: 0x27180090,
        0xD70: 0xAFB80090,
        0xD74: 0xA242FFF4,
        0xD88: 0x2A820006,
        0xD98: 0xAE400000,
        0xDA4: 0xAE43FFFC,
        0xDA8: 0x1440FFC6,
        0xDB0: 0xA021,
        0xDB4: 0x8FB80094,
        0xDBC: 0x2712008C,
        0xDC0: 0x8FB00094,
        0xDE8: 0xA6030000,
        0xE14: 0xA6020040,
        0xE20: 0x26100008,
        0xE38: 0x2B020008,
        0xE48: 0x26940001,
        0xE50: 0xA242FFF8,
        0xE78: 0xAE43FFFC,
        0xE7C: 0xAE400000,
        0xE8C: 0x8FB80094,
        0xE90: 0x2A820004,
        0xE94: 0x27180090,
        0xE98: 0x1440FFC9,
        0xE9C: 0xAFB80094,
        0x48: 0x26D806F8,
        0x54: 0xAFB8009C,
        0xBC: 0x00181040,
        0xC0: 0x00581021,
        0xC4: 0x00021080,
        0xC8: 0x00581021,
        0xCC: 0x00021080,
        0xD4: 0xAEC21B10,
        0xA84: 0x8FB8009C,
        0xA88: 0xAFA000B4,
        0xA90: 0x2715019C,
        0xAA8: 0xAFB800C8,
        0xB70: 0x263000C0,
        0xBAC: 0x26310008,
        0xBC8: 0x2A620006,
        0xBF4: 0x27180030,
        0xC14: 0x2A820004,
        0xC3C: 0xA2A2FFE4,
        0xC60: 0xA2A3FFE5,
        0xC6C: 0x271801A0,
        0xC8C: 0xA2A3FFE6,
        0xC9C: 0xAEA2FFF8,
        0xCA8: 0x2B020003,
        0xCB0: 0x26B501A0,
        0x2ABC: 0x00809821,
        0x2AC0: 0x266811F0,
        0x2AC4: 0x266906F8,
        0x2B0C: 0x26721A84,
        0x2B4C: 0x2671088C,
        0x2E00: 0x18400004,
        0x2E24: 0x28420006,
        0x2E40: 0x28420004,
        0x2E74: 0x000310C0,
        0x2E78: 0x00431023,
        0x2E7C: 0x00021140,
        0x2E94: 0x8D220088,
        0x2EBC: 0x8D420088,
        0x2FB8: 0x263101A0,
        0x2FC8: 0x252901A0,
        0x2FD8: 0x28420003,
    }

    def test_web_binding_keeps_existing_resident_address(self):
        bindings = (family435.ROOT / self.modules[0]["linker_symbols"]).read_text()
        self.assertIn("ratan2 = 0x80089928;", bindings)
        self.assertNotIn("func_french_80089928", bindings)
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn("ratan2 = 0x80089928; // type:func absolute:true", symbols)
            self.assertNotIn("func_french_80089928", symbols)

    def test_selected_web_descriptors_and_context_separation(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        resident = family435.ROOT / "game/france/SLES_039.48"
        if not path.exists() or not resident.exists():
            self.skipTest("legal French MODEL and resident inputs required")
        pointers = struct.unpack_from("<14I", resident.read_bytes(), 0x800)
        self.assertEqual((pointers[9], pointers[10]), (0x80136000, 0x80176000))
        with path.open("rb") as archive:
            for module in self.modules:
                instance = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(instance["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x4450
                self.assertEqual(struct.unpack_from("<I", data, 0xA8)[0],
                                 0x3C030000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xAC)[0],
                                 0x24630000 | (table & 0xFFFF))
                archive.seek((int(instance["record"]) * 276 + 275) * 2048 + 0x110 +
                             (int(instance["stage"]) - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(instance["command_word"]))
                self.assertEqual(command, 631000)
                descriptor = 0x4450 + command % 1000 * 52
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 52, len(data))
                widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack("<1170I", data[4:0x124C])
                            if word >> 26 in widths and (word >> 21) & 31 == 22
                            and word & 0x8000 == 0]
                self.assertEqual(max(accesses), 0x1B50)
                context = pointers[9 + slot]
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + max(accesses) <= start or start + size <= context)
