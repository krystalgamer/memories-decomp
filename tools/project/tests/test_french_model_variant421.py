import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant421Tests(family435.FrenchModelVariant435Tests):
    family = 421
    source_family = 404
    module_count = 12
    distinct_images = 12
    binding_count = 36
    tail_start = 0x406C
    spans = ((4, 0x11D0), (0x11D0, 0x1860), (0x1860, 0x247C),
             (0x247C, 0x2C28), (0x2C28, 0x310C), (0x310C, 0x367C),
             (0x367C, 0x398C), (0x398C, 0x3D08), (0x3D08, 0x406C))
    helpers = ((0x2C28, 1252, "sheets", "func_8013DCA4"),
               (0x310C, 1392, "webs", "func_8013E18C"),
               (0x367C, 784, "spokes", "func_8013E700"),
               (0x398C, 892, "rings", "func_8013EA14"),
               (0x3D08, 868, "quad", "func_8013ED94"))
    reachable_helpers = {0x2C28, 0x310C}
    local_call_targets = {0x11D0, 0x1860, 0x247C, 0x2C28, 0x310C}
    models_by_stage = ((7, (84, 162)), (9, (88, 114, 184, 369)))
    entry_anchors = {0x0C: 0x00809021, 0x14: 0x0240B021, 0x20: 0x26D80FD8,
                     0x28: 0x26D81108, 0x30: 0x26D81468, 0x38: 0x26D816A8,
                     0x61C: 0x27180090, 0x8F0: 0x27180098, 0xC04: 0x27180090,
                     0xDB0: 0x27180090, 0x910: 0x2B020002, 0xC20: 0x2BC20006,
                     0xDAC: 0x2BC20004, 0xDE0: 0xAEC01958, 0xDFC: 0xAEC01960,
                     0x40: 0x26D804E0, 0x84: 0xAFB60094, 0xB0: 0x00181900,
                     0xB4: 0x00781823, 0xB8: 0x00031880, 0xC0: 0xAEC31938,
                     0x91C: 0x8FB80094, 0x928: 0x2714019C, 0x940: 0xAFB800BC,
                     0xA08: 0x265000C0, 0xA44: 0x26520008, 0xA60: 0x2A620006,
                     0xA8C: 0x27180030, 0xAAC: 0x2BC20004, 0xAD4: 0xA282FFE4,
                     0xB04: 0x271801A0, 0xB34: 0xAE82FFF8, 0xB40: 0x2B020003,
                     0xB48: 0x269401A0, 0x1030: 0x02402021, 0x3114: 0x00809821,
                     0x3118: 0x26680FD8, 0x3160: 0x26721874, 0x31A0: 0x26710194,
                     0x348C: 0x28420006, 0x34A8: 0x28420004, 0x34FC: 0x8D220088,
                     0x3524: 0x8D420088, 0x3620: 0x263101A0, 0x3630: 0x252901A0,
                     0x3640: 0x28420003}

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
                table = base + 0x4168
                self.assertEqual(struct.unpack_from("<I", data, 0xA0)[0],
                                 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xA4)[0],
                                 0x24420000 | (table & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x102C)[0],
                                 0x0C000000 | (((base + 0x310C) >> 2) & 0x3FFFFFF))
                archive.seek((int(instance["record"]) * 276 + 275) * 2048 + 0x110 +
                             (int(instance["stage"]) - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(instance["command_word"]))
                descriptor = 0x4168 + command % 1000 * 60
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 60, len(data))
                widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack("<1139I", data[4:0x11D0])
                            if word >> 26 in widths and (word >> 21) & 31 == 22
                            and word & 0x8000 == 0]
                self.assertEqual(max(accesses), 0x1978)
                context = pointers[9 + slot]
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + max(accesses) <= start or start + size <= context)
