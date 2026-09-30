import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant414Tests(family435.FrenchModelVariant435Tests):
    resident_name = "SLES_039.48"
    family = 414
    source_family = 397
    module_count = 24
    distinct_images = 22
    binding_count = 36
    tail_start = 0x2DCC
    spans = ((4, 0x1228), (0x1228, 0x1990), (0x1990, 0x1E74),
             (0x1E74, 0x23E4), (0x23E4, 0x26EC), (0x26EC, 0x2A68),
             (0x2A68, 0x2DCC))
    helpers = ((0x1990, 1252, "sheets", "func_8013C994"),
               (0x1E74, 1392, "webs", "func_8013CE7C"),
               (0x23E4, 776, "spokes", "func_8013D3F0"),
               (0x26EC, 892, "rings", "func_8013D6FC"),
               (0x2A68, 868, "quad", "func_8013DA7C"))
    reachable_helpers = {0x1990, 0x1E74}
    local_call_targets = {0x1228, 0x1990, 0x1E74}
    models_by_stage = ((7, (2, 20, 87, 108, 138, 193, 573)),
                       (9, (152, 168, 170, 388, 427)))
    entry_anchors = {0x0C: 0x00809021, 0x14: 0x0240B021, 0x20: 0x26D805FC,
                     0x28: 0x26D8072C, 0x30: 0x26D80A8C, 0x38: 0x26D80CCC,
                     0x6A4: 0x27180090, 0x978: 0x27180098, 0xC8C: 0x27180090,
                     0xDB4: 0x27180090, 0xDB0: 0x2BC20004, 0xDD4: 0xAEC00F74,
                     0x18: 0x26D804E0, 0x7C: 0xAFB60094, 0xA8: 0x001818C0,
                     0xAC: 0x00781821, 0xB0: 0x000318C0, 0xB8: 0xAEC30F54,
                     0x9A4: 0x8FB80094, 0x9B0: 0x2714019C, 0xA90: 0x265000C0,
                     0xAE8: 0x2A620006, 0xB14: 0x27180030, 0xB34: 0x2BC20004,
                     0xB8C: 0x271801A0, 0xBBC: 0xAE82FFF8, 0xBC8: 0x2B020003,
                     0xBD0: 0x269401A0, 0x10AC: 0x02402021, 0x1E7C: 0x00809821,
                     0x1E80: 0x266805FC, 0x1EC8: 0x26720E98, 0x1F08: 0x26710194,
                     0x21F4: 0x28420006, 0x2210: 0x28420004, 0x2264: 0x8D220088,
                     0x228C: 0x8D420088, 0x2388: 0x263101A0, 0x2398: 0x252901A0,
                     0x23A8: 0x28420003}

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
        path = family435.ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        resident = family435.ROOT / f"game/{self.region}/{self.resident_name}"
        if not path.exists() or not resident.exists():
            self.skipTest(f"legal {self.region} MODEL and resident inputs required")
        pointers = struct.unpack_from("<14I", resident.read_bytes(), 0x800)
        self.assertEqual((pointers[9], pointers[10]), (0x80136000, 0x80176000))
        with path.open("rb") as archive:
            for module in self.modules:
                instance = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(instance["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x2EC8
                self.assertEqual(struct.unpack_from("<I", data, 0x98)[0],
                                 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x9C)[0],
                                 0x24420000 | (table & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x10A8)[0],
                                 0x0C000000 | (((base + 0x1E74) >> 2) & 0x3FFFFFF))
                archive.seek((int(instance["record"]) * 276 + 275) * 2048 + 0x110 +
                             (int(instance["stage"]) - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(instance["command_word"]))
                descriptor = 0x2EC8 + command % 1000 * 72
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 72, len(data))
                widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack("<1161I", data[4:0x1228])
                            if word >> 26 in widths and (word >> 21) & 31 == 22
                            and word & 0x8000 == 0]
                self.assertEqual(max(accesses), 0xF8C)
                context = pointers[9 + slot]
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + max(accesses) <= start or start + size <= context)
