import struct
import unittest

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant445Tests(family435.FrenchModelVariant435Tests):
    resident_name = "SLES_039.48"
    family = 445
    source_family = 428
    source_directories = {"bands": "spanish_model_variant"}
    standalone_helpers = frozenset({"bands"})
    module_count = 12
    distinct_images = 12
    binding_count = 36
    tail_start = 0x3594
    spans = ((4, 0x1034), (0x1034, 0x17A8), (0x17A8, 0x1C8C),
             (0x1C8C, 0x21F0), (0x21F0, 0x24F8), (0x24F8, 0x2874),
             (0x2874, 0x2BD8), (0x2BD8, 0x3594))
    helpers = ((0x1034, 1908, "bands", "func_8013C034"),
               (0x17A8, 1252, "sheets", "func_8013C7AC"),
               (0x1C8C, 1380, "webs", "func_8013CC94"),
               (0x21F0, 776, "spokes", "func_8013D1FC"),
               (0x24F8, 892, "rings", "func_8013D508"),
               (0x2874, 868, "quad", "func_8013D888"),
               (0x2BD8, 2492, "spiral", "func_8013DBF0"))
    reachable_helpers = {0x1034, 0x17A8, 0x1C8C, 0x2BD8}
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


class FrenchModelBand445DescriptorTests(unittest.TestCase):
    anchors = {
        0x1034: 0x27BDFED8, 0x103C: 0x00809821, 0x1068: 0x86641562,
        0x106C: 0x86651560, 0x1078: 0x26711418, 0x107C: 0x8E63157C,
        0x1090: 0x866315B0, 0x10CC: 0x26740AB0, 0x10E0: 0x27A800C8,
        0x10E4: 0xAFA800FC, 0x1104: 0x001280C0, 0x1138: 0x26020048,
        0x1164: 0x26100090, 0x118C: 0x8E62153C, 0x1198: 0x8E621540,
        0x11A4: 0x8E621544, 0x11B0: 0x8E631550, 0x11B4: 0x8E6215B4,
        0x11F8: 0x8E631554, 0x1240: 0x8E631558, 0x1338: 0x24C50048,
        0x1340: 0x24C60090, 0x1348: 0x00108080, 0x134C: 0x260200FC,
        0x1358: 0x26020120, 0x1364: 0x27A200F0, 0x1374: 0x000310C0,
        0x1378: 0x00431021, 0x137C: 0x00021080, 0x1380: 0x260700D8,
        0x1384: 0x8FA800FC, 0x1398: 0xAFA2001C, 0x13B0: 0x28630009,
        0x13B8: 0xAE0201A4, 0x13CC: 0x269401C8, 0x1508: 0x27A300C8,
        0x1510: 0xAEA00000, 0x1550: 0x96020120, 0x155C: 0x86020122,
        0x1580: 0x960200FC, 0x158C: 0x860200FE, 0x1690: 0x8E6315BC,
        0x16B4: 0x8E631590, 0x16B8: 0x8E621580, 0x1760: 0xA66215B0,
        0x1774: 0xAE6215BC, 0x18: 0x26D80AB0, 0x1C: 0xAFB80084,
        0x48: 0x26D71418, 0x488: 0x8FB80084, 0x48C: 0x24061000,
        0x490: 0x2704019E, 0x494: 0x8FA30084, 0x49C: 0xAC86FFEE,
        0x4A0: 0xAC86FFF2, 0x4A4: 0xAC86FFF6, 0x4B8: 0xA0620144,
        0x4CC: 0xA0620145, 0x4E0: 0xA0620146, 0x4F4: 0xA0620168,
        0x508: 0xA0620169, 0x51C: 0xA062016A, 0x520: 0x2A620009,
        0x528: 0x24630004, 0x530: 0xA480FFFE, 0x534: 0xAC800002,
        0x538: 0xA4800000, 0x540: 0x248401C8, 0x544: 0x271801C8,
        0x548: 0x18A0FFD2, 0x54C: 0xAFB80084, 0xEBC: 0x8EC215BC,
        0xEC4: 0x18400003, 0xED0: 0x02602021, 0x16BC: 0x8C640020,
        0x16C0: 0x8C630024, 0x16D0: 0x0043001B, 0x1724: 0x8CA40028,
        0x1738: 0x8CA2002C, 0x1744: 0x0062001B,
    }

    def test_band_layout_and_actual_descriptor_timings(self):
        family = FrenchModelVariant445Tests()
        family.setUp()
        archive_path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest("legal French MODEL input required")
        expected = {
            611000: (24, 32, 94, 116),
            611001: (148, 160, 360, 390),
            611002: (48, 64, 240, 300),
            611003: (20, 28, 120, 140),
            611004: (90, 98, 160, 170),
        }
        commands = set()
        with archive_path.open("rb") as archive:
            for module in family.modules:
                row = family.instances[module["name"]]
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for offset, word in self.anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                self.assertEqual(struct.unpack_from("<I", data, 0xECC)[0],
                                 0x0C000000 | ((base + 0x1034) >> 2 & 0x3FFFFFF))
                table = base + 0x3690
                self.assertEqual(struct.unpack_from("<I", data, 0x9C)[0],
                                 0x3C030000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xA0)[0],
                                 0x24630000 | (table & 0xFFFF))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110 +
                             (int(row["stage"]) - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(row["command_word"]))
                commands.add(command)
                descriptor = 0x3690 + command % 1000 * 52
                self.assertGreaterEqual(descriptor, family.tail_start)
                self.assertLessEqual(descriptor + 52, len(data))
                timing = struct.unpack_from("<4I", data, descriptor + 0x20)
                self.assertEqual(timing, expected[command])
                self.assertGreater(timing[1], timing[0])
                self.assertGreater(timing[3], timing[2])
                layout = family435.ROOT / module["layout"]
                for path in (layout.with_name(layout.stem + "_symbols.txt"),
                             family435.ROOT / module["linker_symbols"]):
                    text = path.read_text()
                    self.assertIn("rcos = 0x800866F8;", text)
                    self.assertNotIn("func_french_800866F8", text)
        self.assertEqual(commands, set(expected))


class FrenchModelVariant445SpiralTests(unittest.TestCase):
    family_class = FrenchModelVariant445Tests
    keep_legacy_alias = False
    anchors = {
        0x2C08: 0xAFA40138, 0x2C30: 0x25331418, 0x2C88: 0x8FB50138,
        0x2C98: 0x26BE0014, 0x2DC8: 0xA6020010, 0x2E4C: 0xA6020030,
        0x2E74: 0x28420002, 0x2EAC: 0x2842000C, 0x2EB4: 0x26B5007C,
        0x2EE8: 0x27B70130, 0x2F04: 0x27A800D0, 0x2F14: 0x25720072,
        0x3010: 0xAFB70020, 0x301C: 0xAFA90024,
        0x3020: 0x26A40038, 0x3024: 0x26A50044, 0x302C: 0x27A70134,
        0x3034: 0xAE42FFF6, 0x3058: 0xAE42FFBA, 0x3070: 0xAE42FFDA,
        0x3090: 0xA642FFFC, 0x30AC: 0xA6420000, 0x3120: 0xAE020064,
        0x3150: 0xAE020028, 0x3168: 0xAE020048, 0x3188: 0xA622006C,
        0x31AC: 0xA6220070, 0x31C0: 0x28420002, 0x31E8: 0x2842000C,
        0x31F0: 0x26B5007C, 0x32B0: 0x90C20058, 0x32F8: 0x90C20050,
        0x3340: 0x8CC20064, 0x3348: 0x0440000A, 0x3354: 0x8C4200D0,
        0x335C: 0x04400006, 0x3364: 0x94C60064, 0x34EC: 0x1840FF47,
        0x350C: 0x2842000C, 0x3514: 0x26B5007C, 0x3524: 0x850215A8,
        0x3538: 0x8D021588, 0x3548: 0xA50215A8, 0x3560: 0xA50215A8,
    }

    def test_spiral_storage_and_retail_accesses(self):
        family = self.family_class()
        family.setUp()
        archive_path = family435.ROOT / f"game/{family.region}/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest(f"legal {family.region} MODEL input required")
        self.assertEqual(12 * 0x7C, 0x5D0)
        self.assertEqual(0x1418 + 52, 0x144C)
        self.assertEqual(0x80 + 80, 0xD0)
        self.assertEqual(0xD0 + 12 * 2 * 4, 0x130)
        self.assertEqual(0x130 + 4 + 4, 0x138)
        self.assertLessEqual(0x15B0 + 2, 0x15D0)
        with archive_path.open("rb") as archive:
            for module in family.modules:
                base = int(module["load_address"], 0)
                slot = (base - 0x8013B000) // 0x40000
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for offset, word in self.anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                self.assertEqual(struct.unpack_from("<I", data, 0xEB4)[0],
                                 0x0C000000 | ((base + 0x2BD8) >> 2 & 0x3FFFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xEB8)[0], 0x02602021)
                for offset in (0x3030, 0x311C):
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], 0x0C021E1A)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + 0x15D0 <= start or start + size <= context)
                layout = family435.ROOT / module["layout"]
                for path in (layout.with_name(layout.stem + "_symbols.txt"),
                             family435.ROOT / module["linker_symbols"]):
                    self.assertIn("RotTransPers = 0x80087868;", path.read_text())
                    if self.keep_legacy_alias:
                        self.assertIn("func_french_80087868 = 0x80087868;", path.read_text())
                    else:
                        self.assertNotIn("func_french_80087868", path.read_text())

    def test_spiral_view_arrays_and_projection_outputs(self):
        directory = family435.ROOT / "src/overlays/model_variant"
        header = (directory / "variant428_spiral.h").read_text()
        for declaration in ("SVECTOR a[2];", "SVECTOR b[2];", "PSXLONG sa[2];",
                            "PSXLONG sb[2];", "s32 angle[2];", "s32 width[2];",
                            "u8 cb[2][4];", "u8 ca[2][4];", "s32 otz[2];",
                            "s16 ox[2];", "s16 oy[2];"):
            self.assertIn(declaration, header)
        source = (directory / "variant428_spiral.c").read_text()
        self.assertIn("PSXLONG flags[12][2];", source)
        self.assertIn("&p, &flags[i][1]", source)
        self.assertIn("&p, &flags[i][k]", source)
        self.assertIn("&arm->sb[1], &p, &flag", source)
        self.assertIn("&arm->sb[k], &p, &flag", source)
        self.assertEqual(source.count("i < 12"), 3)
