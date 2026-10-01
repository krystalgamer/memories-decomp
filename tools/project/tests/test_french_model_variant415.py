import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant415Tests(family435.FrenchModelVariant435Tests):
    family = 415
    source_family = 398
    module_count = 10
    distinct_images = 10
    binding_count = 36
    tail_start = 0x2FB8
    spans = ((4, 0x12B4), (0x12B4, 0x18A4), (0x18A4, 0x2024),
             (0x2024, 0x2508), (0x2508, 0x2A34), (0x2A34, 0x2FB8))
    helpers = ((0x18A4, 1920, "bands", "func_8013C8AC"),
               (0x2024, 1252, "sheets", "func_8013D02C"),
               (0x2508, 1324, "webs", "func_8013D514"),
               (0x2A34, 1412, "curtains", "func_8013DA44"))
    reachable_helpers = {0x18A4, 0x2024, 0x2A34}
    local_call_targets = {0x12B4, 0x18A4, 0x2024, 0x2A34}
    models_by_stage = ((9, (102, 282, 288, 642, 645)),)
    descriptor_table = 0x30B4
    minimum_context = 0x1A44
    commands = {581000, 581001, 581004}
    curtain_start = 0x13B4
    curtain_ends = (0x1814, 0x16FC)
    curtain_init_bound = (0x8B4, 0x2AC20004)
    curtain_draw_bound = (0x2F54, 0x29820003)
    entry_anchors = {
        0xC: 0x00809821, 0x14: 0x0260F021, 0x1C: 0x27D906A8,
        0x28: 0xAFB90084, 0x94: 0xAFBE0094, 0xC4: 0x00191840,
        0xC8: 0x00791821, 0xCC: 0x00031900, 0xD4: 0xAFC31A00,
        0x8CC: 0x8FB80084, 0x8D4: 0x27030090, 0x8E0: 0x27300020,
        0x8E4: 0x272F0040, 0x8E8: 0x272E0060, 0xA00: 0x28820004,
        0xA7C: 0x27390098, 0xA84: 0xAC60FFF8, 0xA94: 0x2AC20002,
        0xA9C: 0x24630098, 0xAA0: 0x8FB80094, 0xAA4: 0x0000B021,
        0xAAC: 0x2714019C, 0xB8C: 0x263000C0, 0xBC8: 0x26310008,
        0xBE4: 0x2A620006, 0xC1C: 0x27180030, 0xC38: 0x2B020004,
        0xC8C: 0x271801A0, 0xCA0: 0xAE80FFFC, 0xCB8: 0xAE82FFF8,
        0xCBC: 0x2AC20003, 0xCC4: 0x269401A0, 0x24C8: 0x26100098,
        0x24CC: 0x2AE20002, 0x24D4: 0x26B50098, 0x2878: 0x28420006,
        0x2894: 0x28420004, 0x29D8: 0x265201A0, 0x29E8: 0x252901A0,
        0x29F8: 0x28420003,
        0x18: 0x27D804E0, 0x20: 0xAFB80080, 0x470: 0xA0620144,
        0x4AC: 0xA0620168, 0x4D8: 0x2A620009, 0x4FC: 0x271801C8,
        0x500: 0x18A0FFD2, 0x504: 0xAFB80080, 0x1150: 0x02602021,
        0x18A4: 0x27BDFED8, 0x18D8: 0x866419D2, 0x18DC: 0x866519D0,
        0x18E8: 0x26711854, 0x18EC: 0x8E6319EC, 0x1900: 0x86621A18,
        0x1918: 0x00021182, 0x1944: 0x00021302, 0x1948: 0x267404E0,
        0x195C: 0x27A800C8, 0x1BC8: 0x260200FC, 0x1BD4: 0x26020120,
        0x1BE0: 0x27A200F0, 0x1BFC: 0x260700D8, 0x1C10: 0x0C021E26,
        0x1C2C: 0x28630009, 0x1C34: 0xAE0201A4, 0x1C48: 0x269401C8,
        0x1CD4: 0x92420144, 0x1D1C: 0x92420168, 0x1D74: 0xAE4001A4,
        0x1D84: 0x27A300C8, 0x1D8C: 0xAEA00000, 0x1DA0: 0x964601A4,
        0x1DCC: 0x96020120, 0x1DD8: 0x86020122, 0x1DFC: 0x960200FC,
        0x1E08: 0x860200FE, 0x1EEC: 0x28420008, 0x1F08: 0x269401C8,
        0x1F30: 0x8E631A00, 0x1F38: 0x8C640020, 0x1F3C: 0x8C630024,
        0x1FA0: 0x8CA40028, 0x1FB4: 0x8CA2002C, 0x1FDC: 0xA6621A18,
        0x1FF0: 0xAE621A30,
        0x44: 0x27D813B4, 0x7EC: 0x27110114, 0x83C: 0xA6030088,
        0x864: 0x2A620011, 0x8A8: 0xAE200000, 0x8AC: 0x26310118,
        0x8B4: 0x2AC20004, 0x8B8: 0x27390118, 0xF18: 0xAFC01A2C,
        0xF34: 0xAFC01A30, 0x1128: 0x28420002, 0x1138: 0x02602021,
        0x2A34: 0x27BDFED0, 0x2A40: 0x268C13B4, 0x2A98: 0x269106A8,
        0x2AB0: 0x269318F0, 0x2AD0: 0x8E230088, 0x2B08: 0x25AD0114,
        0x2B80: 0xA6220088, 0x2BC0: 0x2A420011, 0x2DCC: 0x27A200D4,
        0x2E3C: 0x04C00010, 0x2E4C: 0x0440000C, 0x2E58: 0x30C6FFFF,
        0x2E90: 0x2A420010, 0x2F4C: 0x25AD0118, 0x2F54: 0x29820003,
    }

    def test_curtain_initialization_and_draw_bounds(self):
        archive_path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest("legal French MODEL input required")
        self.assertEqual(self.curtain_start + 4 * 280, self.curtain_ends[0])
        self.assertEqual(self.curtain_start + 3 * 280, self.curtain_ends[1])
        with archive_path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for offset, word in (self.curtain_init_bound, self.curtain_draw_bound):
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)

    def test_named_projection_import_keeps_resident_address(self):
        bindings = (family435.ROOT / self.modules[0]["linker_symbols"]).read_text()
        self.assertIn("ratan2 = 0x80089928;", bindings)
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn("ratan2 = 0x80089928; // type:func absolute:true", symbols)
            self.assertNotIn("func_french_80089928", symbols)

    def test_descriptors_and_direct_context_separation(self):
        archive_path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        resident = family435.ROOT / "game/france/SLES_039.48"
        if not archive_path.exists() or not resident.exists():
            self.skipTest("legal French MODEL and resident inputs required")
        pointers = struct.unpack_from("<14I", resident.read_bytes(), 0x800)
        self.assertEqual((pointers[9], pointers[10]), (0x80136000, 0x80176000))
        observed_commands = set()
        with archive_path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(row["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + self.descriptor_table
                self.assertEqual(struct.unpack_from("<I", data, 0xB4)[0],
                                 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xB8)[0],
                                 0x24420000 | (table & 0xFFFF))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110 +
                             (int(row["stage"]) - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(row["command_word"]))
                observed_commands.add(command)
                descriptor = self.descriptor_table + command % 1000 * 48
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 48, len(data))
                widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
                end = self.spans[0][1]
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack(f"<{(end - 4) // 4}I", data[4:end])
                            if word >> 26 in widths and (word >> 21) & 31 == 30
                            and word & 0x8000 == 0]
                self.assertEqual(max(accesses), self.minimum_context)
                context = pointers[9 + slot]
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 4096), (base, 20480)):
                    self.assertTrue(context + max(accesses) <= start or start + size <= context)
        self.assertEqual(observed_commands, self.commands)
