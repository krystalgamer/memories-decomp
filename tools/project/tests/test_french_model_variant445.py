import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant445Tests(family435.FrenchModelVariant435Tests):
    resident_name = "SLES_039.48"
    family = 445
    source_family = 428
    module_count = 12
    distinct_images = 12
    binding_count = 36
    tail_start = 0x3594
    spans = ((4, 0x1034), (0x1034, 0x17A8), (0x17A8, 0x1C8C),
             (0x1C8C, 0x21F0), (0x21F0, 0x24F8), (0x24F8, 0x2874),
             (0x2874, 0x2BD8), (0x2BD8, 0x3594))
    helpers = ((0x17A8, 1252, "sheets", "func_8013C7AC"),
               (0x1C8C, 1380, "webs", "func_8013CC94"),
               (0x21F0, 776, "spokes", "func_8013D1FC"),
               (0x24F8, 892, "rings", "func_8013D508"),
               (0x2874, 868, "quad", "func_8013D888"))
    reachable_helpers = {0x17A8, 0x1C8C}
    local_call_targets = {0x1034, 0x17A8, 0x1C8C, 0x2BD8}
    models_by_stage = ((7, (187, 596)), (9, (239, 361, 368, 478)))
    entry_anchors = {0x0C: 0x00809821, 0x14: 0x0260B021, 0x20: 0x26D80C78,
                     0x28: 0x26D80DA8, 0x30: 0x26D81108, 0x38: 0x26D81348,
                     0x65C: 0x27180090, 0x820: 0x27180098, 0x838: 0x2AE20002,
                     0xB30: 0x27180090, 0xB70: 0x2B020006, 0xC58: 0x27180090,
                     0xC68: 0x2B020004, 0xC98: 0xAEC015B8, 0xCB0: 0xAEC015BC}
    entry_anchors.update({
        0x24: 0xAFB80088, 0x40: 0x26D805D0, 0x5C: 0xAFB80098,
        0xB0: 0x00181040, 0xB4: 0x00581021, 0xB8: 0x00021080,
        0xBC: 0x00581021, 0xC0: 0x00021080, 0xC8: 0xAEC21590,
        0x670: 0x8FB80088, 0x678: 0x27030090, 0x684: 0x27100020,
        0x688: 0x270F0040, 0x68C: 0x270E0060, 0x828: 0xAC60FFF8,
        0x840: 0x24630098, 0x844: 0x8FB80098, 0x850: 0x2714019C,
        0x930: 0x265000C0, 0x96C: 0x26520008, 0x988: 0x2A620006,
        0x9B4: 0x27180030, 0x9E8: 0x2B020004, 0xA10: 0xA282FFE4,
        0xA38: 0x271801A0, 0xA50: 0xAE80FFFC, 0xA68: 0xAE82FFF8,
        0xA6C: 0x2AE20003, 0xA74: 0x269401A0,
        0x1C94: 0x00809821, 0x1C98: 0x26680C78, 0x1C9C: 0x266905D0,
        0x1CC4: 0xAFA800D0, 0x1CCC: 0xAFA900D4, 0x1CF8: 0x26711514,
        0x1D1C: 0x26720764, 0x1F1C: 0x0003B100, 0x1F24: 0x26C200C0,
        0x1F34: 0x00052B43, 0x1F40: 0x3C025000, 0x1F44: 0xAE220000,
        0x1FC8: 0x04C0000A, 0x1FD8: 0x04400006, 0x2000: 0x28420006,
        0x201C: 0x28420004, 0x2048: 0x8E631588, 0x2050: 0x000310C0,
        0x2054: 0x00431023, 0x2058: 0x00021140, 0x2068: 0x8FA800D0,
        0x2070: 0x8D020088, 0x2078: 0x28420801, 0x2098: 0x8D220088,
        0x2194: 0x265201A0, 0x21A4: 0x252901A0, 0x21B4: 0x28420003,
    })

    def test_web_projection_imports_keep_resident_addresses(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            for name, address in (("RotTransPers3", 0x80087898), ("ratan2", 0x80089928)):
                self.assertIn(f"{name} = 0x{address:X};", bindings)
                self.assertIn(f"{name} = 0x{address:X}; // type:func absolute:true", symbols)
                self.assertNotIn(f"func_french_{address:X}", symbols)

    def test_web_descriptors_and_direct_context_separation(self):
        archive_path = family435.ROOT / "game" / self.region / "DATA/MODEL.MRG"
        resident = family435.ROOT / "game" / self.region / self.resident_name
        if not archive_path.exists() or not resident.exists():
            self.skipTest(f"legal {self.region} MODEL and resident inputs required")
        pointers = struct.unpack_from("<14I", resident.read_bytes(), 0x800)
        self.assertEqual((pointers[9], pointers[10]), (0x80136000, 0x80176000))
        commands = set()
        with archive_path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(row["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x3690
                self.assertEqual(struct.unpack_from("<I", data, 0x9C)[0],
                                 0x3C030000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xA0)[0], 0x24630000 | (table & 0xFFFF))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110 +
                             (int(row["stage"]) - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(row["command_word"]))
                commands.add(command)
                descriptor = 0x3690 + command % 1000 * 52
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 52, len(data))
                widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack(f"<{(0x1034 - 4) // 4}I", data[4:0x1034])
                            if word >> 26 in widths and (word >> 21) & 31 == 22 and word & 0x8000 == 0]
                self.assertEqual(max(accesses), 0x15D0)
                for start, size in ((pointers[slot], 96 * 2048), (pointers[3 + slot], 4096), (base, 20480)):
                    self.assertTrue(pointers[9 + slot] + 0x15D0 <= start or start + size <= pointers[9 + slot])
        self.assertEqual(commands, set(range(611000, 611005)))
