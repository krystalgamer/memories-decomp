import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant476Tests(family435.FrenchModelVariant435Tests):
    family = 476
    source_family = 459
    module_count = 2
    distinct_images = 2
    binding_count = 35
    tail_start = 0x3374
    spans = ((4, 0x135C), (0x135C, 0x170C), (0x170C, 0x20BC),
             (0x20BC, 0x247C), (0x247C, 0x2848), (0x2848, 0x2DD4), (0x2DD4, 0x3374))
    helpers = ((0x135C, 944, "sheet", "func_8013C360"),
               (0x20BC, 960, "webs", "func_8013D064"),
               (0x247C, 972, "curtains", "func_8013D430"),
               (0x2848, 1420, "globe", "func_8013D848"))
    standalone_helpers = frozenset({"globe"})
    reachable_helpers = {0x135C, 0x247C, 0x2848}
    local_call_targets = {0x135C, 0x170C, 0x247C, 0x2848, 0x2DD4}
    models_by_stage = ((9, (712,)),)
    entry_anchors = {
        0x0C: 0x00809821, 0x14: 0x0260F021, 0x24: 0x27D80720, 0x88: 0xAFBE008C,
        0xB8: 0x001918C0, 0xBC: 0x00791823, 0xC0: 0x000318C0, 0xC8: 0xAFC327C0,
        0xB60: 0x26500120, 0xBB8: 0x2A620006, 0xBFC: 0x2B220006,
        0xC58: 0x27180260, 0xC90: 0x2B220003, 0xC98: 0x26B50260,
    }

    entry_anchors.update({
        0x18: 0x27D81A18, 0x1C: 0x27D90F60, 0x20: 0xAFB80084,
        0x724: 0x8FB80084, 0x730: 0x271401A8, 0x758: 0xA6430000,
        0x790: 0xA6430088, 0x7CC: 0xA6430110, 0x7F4: 0x2A620011,
        0x838: 0x271801AC, 0x84C: 0xA680FFF2, 0x850: 0xA682FFF4,
        0x854: 0xAE800000, 0x864: 0xA682FFF0, 0x878: 0xAE82FFF8,
        0x884: 0xAE82FFFC, 0x888: 0x2B220005, 0x890: 0x269401AC,
        0x8B0: 0x27100020, 0x8B4: 0x270F0040, 0x8B8: 0x270E0060,
        0xA58: 0x27390098, 0xA74: 0x24630098, 0x1074: 0x8C82001C,
        0x1080: 0x0062102B, 0x108C: 0x8C82002C, 0x1094: 0x0062102B,
        0x10A4: 0x02602021, 0x1148: 0x8C430030, 0x1154: 0x0043102B,
        0x1158: 0x14400028, 0x117C: 0x02602021, 0x135C: 0x27BDFF00,
        0x1364: 0x00809021, 0x136C: 0x26550F60, 0x1398: 0x265122E8,
        0x13A4: 0x26500FE8, 0x13A8: 0x8E4227AC, 0x13B0: 0x30420001,
        0x13C8: 0x000218C3, 0x13D4: 0xA7A00028, 0x13E0: 0x8E42274C,
        0x13EC: 0x8E422750, 0x13F8: 0x8E422754, 0x1494: 0x0C021CAA,
        0x14B4: 0x0C021DCE, 0x14F4: 0x27A200D0, 0x14FC: 0x27A200D4,
        0x1508: 0x0C021E56, 0x151C: 0x9203FFFD, 0x1534: 0x9203FFFC,
        0x155C: 0x000210C0, 0x157C: 0x9203FFF8, 0x15A8: 0x04600008,
        0x15B0: 0x8FA200D4, 0x15B8: 0x04400004, 0x15C4: 0x0C0210AA,
        0x15C8: 0x3066FFFF, 0x15D0: 0x2A620004, 0x15F0: 0x8E42284C,
        0x1600: 0x8E4327C0, 0x1604: 0x8E4227B0, 0x1608: 0x8C64001C,
        0x160C: 0x8C630020, 0x1614: 0x00021300, 0x161C: 0x0043001B,
        0x1640: 0x24021000, 0x1650: 0xAE42284C, 0x165C: 0x8CA40024,
        0x1670: 0x8E4327B8, 0x1678: 0x00031A40, 0x168C: 0x24022000,
        0x1698: 0x8CA20028, 0x169C: 0x00031B40, 0x16A4: 0x0062001B,
        0x16C8: 0xAE000000, 0x16D0: 0x26100098, 0x16D4: 0x1AE0FF34,
        0x247C: 0x27BDFED0, 0x2484: 0x0080B821, 0x248C: 0x26FE1A18,
        0x24B4: 0x0C0214AA, 0x24C4: 0x26F12704, 0x24DC: 0x26F21BB8,
        0x24E8: 0x188000A8, 0x24F0: 0x0C02198A, 0x251C: 0x86E22758,
        0x2528: 0x86E2275A, 0x2534: 0x86E2275C, 0x2674: 0x93A200F0,
        0x26BC: 0x93A200E8, 0x26D8: 0x24150090, 0x26E4: 0x24140088,
        0x26F0: 0x24130008, 0x2708: 0x03D32821, 0x270C: 0x03D43021,
        0x2730: 0x27A200F8, 0x2738: 0x27A200FC, 0x2740: 0x0C021E56,
        0x274C: 0x04C00008, 0x2754: 0x8FA200FC, 0x275C: 0x04400004,
        0x2768: 0x0C0210AA, 0x276C: 0x30C6FFFF, 0x2780: 0x2AC20010,
        0x2794: 0x28621800, 0x27A0: 0x8EE227B8, 0x27A8: 0x000211C0,
        0x27BC: 0x2462E800, 0x27CC: 0x24420180, 0x27D0: 0x24630320,
        0x27DC: 0x8EE227C0, 0x27E0: 0x8EE327B0, 0x27E4: 0x8C420034,
        0x27EC: 0x0043102B, 0x27F4: 0x24021800, 0x27FC: 0x265201AC,
        0x2804: 0x27DE01AC, 0x280C: 0x29020005,
    })

    entry_anchors.update({
        0x003c: 0x27D00FF8,
        0x0044: 0x27D814E4,
        0x0064: 0x27D926D0,
        0x006c: 0x27D82704,
        0x00a4: 0xA7C22860,
        0x04b4: 0x00021200,
        0x04c0: 0x00029303,
        0x04c8: 0x0002A303,
        0x04e4: 0xA6140002,
        0x04ec: 0x00181303,
        0x04f4: 0xA6020000,
        0x0500: 0x00138A00,
        0x050c: 0xA6020004,
        0x0510: 0x2A620011,
        0x0518: 0x26100008,
        0x0538: 0x000220C3,
        0x0540: 0x24020040,
        0x0544: 0xA32404C8,
        0x0548: 0xA32204C9,
        0x0554: 0xA30204CA,
        0x055c: 0x27180004,
        0x0568: 0x27390100,
        0x0574: 0x27180088,
        0x0580: 0x2B220009,
        0x1140: 0x8FC227C0,
        0x1148: 0x8C430030,
        0x114c: 0x8FC227B0,
        0x1154: 0x0043102B,
        0x1158: 0x14400028,
        0x1168: 0x28420002,
        0x1174: 0xAFC2284C,
        0x1188: 0x28420006,
        0x118c: 0x1040001B,
        0x1198: 0x02602021,
        0x2848: 0x27BDFEE8,
        0x287c: 0x8E242778,
        0x2880: 0x8E252770,
        0x288c: 0x8E242774,
        0x2890: 0x8E252778,
        0x2898: 0x263226D0,
        0x289c: 0x26280FF8,
        0x28a0: 0xAFA800D8,
        0x28a4: 0x86222860,
        0x28a8: 0x00000000,
        0x28ac: 0x14400005,
        0x28b0: 0x00000000,
        0x28b4: 0x96222804,
        0x28d0: 0x00021023,
        0x28d8: 0x86222758,
        0x28e4: 0x8622275A,
        0x28f0: 0x8622275C,
        0x28fc: 0x8E2227FC,
        0x291c: 0x0C021F2E,
        0x2928: 0x0C021D7E,
        0x2930: 0x27A40080,
        0x294c: 0xAFA90084,
        0x297c: 0xAFA000C8,
        0x2984: 0xAFA00080,
        0x2998: 0x28420005,
        0x29ac: 0x2443E000,
        0x29bc: 0x000318C3,
        0x29d0: 0xAE22280C,
        0x29d8: 0x00161200,
        0x29dc: 0x00562023,
        0x29f0: 0x000330C3,
        0x2a08: 0x00021283,
        0x2a0c: 0xA0A204C8,
        0x2a24: 0x00021103,
        0x2a28: 0xA0A204C9,
        0x2a4c: 0xA0A204CA,
        0x2a54: 0x2AC20009,
        0x2a5c: 0x24A50004,
        0x2a80: 0x000210C3,
        0x2ac4: 0x2AC20009,
        0x2ad8: 0x24080088,
        0x2b9c: 0x26420008,
        0x2ba4: 0x26420014,
        0x2bac: 0x26420020,
        0x2bb4: 0x2642002C,
        0x2bbc: 0x27A200D0,
        0x2bc4: 0x27A200D4,
        0x2bdc: 0x04C00008,
        0x2bec: 0x04400004,
        0x2bfc: 0x30C6FFFF,
        0x2c0c: 0x2AE20010,
        0x2c18: 0x27DE0088,
        0x2c1c: 0x26940004,
        0x2c28: 0x2AC20008,
        0x2c38: 0x8E232814,
        0x2c40: 0x28620081,
        0x2c48: 0x24620008,
        0x2c50: 0x28420080,
        0x2c5c: 0xAE202814,
        0x2c68: 0x28620003,
        0x2c7c: 0x28622000,
        0x2c90: 0x00021240,
        0x2ca8: 0xAE2227FC,
        0x2cb0: 0x24020003,
        0x2cc8: 0x000212C0,
        0x2cd4: 0x24421800,
        0x2ce0: 0x8E2427C0,
        0x2ce4: 0x00031980,
        0x2cf0: 0x8C820034,
        0x2cf4: 0x8E2327B0,
        0x2cfc: 0x0043102B,
        0x2d0c: 0xAE22284C,
        0x2d20: 0x00031A00,
        0x2d28: 0xAE2227FC,
        0x2d2c: 0x28424000,
        0x2d38: 0xAE22284C,
        0x2d44: 0xAE22280C,
        0x2d64: 0x1880000A,
        0x2d68: 0xAE2227FC,
        0x2d74: 0x00021140,
        0x2d80: 0xAE22280C,
        0x2d84: 0x24020006,
        0x2d88: 0xAE20280C,
        0x2d98: 0x00021180,
        0x2da0: 0xAE232804,
    })

    def test_helper_sdk_bindings_keep_existing_addresses(self):
        aliases = {"GsSortPoly": 0x800842A8, "GsGetActiveBuff": 0x800852A8,
                   "rsin": 0x80086628, "ReadRotMatrix": 0x800872A8,
                   "SetRotMatrix": 0x80087738, "rcos": 0x800866F8}
        paths = [family435.ROOT / self.modules[0]["linker_symbols"]]
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            paths.append(layout.with_name(layout.stem + "_symbols.txt"))
        for path in paths:
            text = path.read_text()
            for name, address in aliases.items():
                self.assertIn(f"{name} = 0x{address:X};", text)
                self.assertNotIn(f"func_{address:X} =", text)

    def test_sheet_and_curtain_timing_and_entry_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        self.assertEqual(0xF60 + 152, 0xFF8)
        self.assertEqual(0x1A18 + 5 * 428, 0x2274)
        with path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<7I", data, 0x3470 + 0x1C),
                                 (80, 144, 152, 260, 270, 284, 420))
                for call, offset in ((0x10A0, 0x135C), (0x1178, 0x247C), (0x1194, 0x2848)):
                    self.assertEqual(struct.unpack_from("<I", data, call)[0],
                                     0x0C000000 | (((base + offset) >> 2) & 0x3FFFFFF))

    def test_globe_private_view_and_initialization_order(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        header = (directory / "variant476_globe.h").read_text()
        source = (directory / "variant476_globe.c").read_text()
        self.assertIn("SVECTOR points[9][17];", header)
        self.assertIn("u8 color[9][4];", header)
        self.assertEqual(9 * 17 * 8, 0x4C8)
        self.assertEqual(0xFF8 + 0x4C8 + 9 * 4, 0x14E4)
        self.assertEqual(0x26D0 + 52, 0x2704)
        self.assertLess(source.index("globe ="), source.index("ratan2("))
        self.assertEqual(source.count("ratan2("), 2)
        self.assertLess(source.index("RotMatrix("), source.index("ScaleMatrix("))
        self.assertLess(source.index("ScaleMatrix("), source.index("coord.coord = m;"))
        self.assertIn("if (otz >= 0 && flag >= 0)", source)

    def test_globe_descriptor_gate_and_state_endpoints(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<2I", data, 0x3470 + 0x30), (284, 420))
                for call, offset in ((0x1178, 0x247C), (0x1194, 0x2848)):
                    self.assertEqual(struct.unpack_from("<I", data, call)[0],
                                     0x0C000000 | (((base + offset) >> 2) & 0x3FFFFFF))
                for offset, expected in (
                    (0x1154, 0x0043102B), (0x1188, 0x28420006),
                    (0x1198, 0x02602021), (0x2CFC, 0x0043102B),
                    (0x2CA8, 0xAE2227FC), (0x2D28, 0xAE2227FC),
                    (0x2D2C, 0x28424000), (0x2D38, 0xAE22284C),
                    (0x2D44, 0xAE22280C), (0x2D88, 0xAE20280C),
                    (0x2D8C, 0xAE22284C),
                ):
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], expected)

    def test_selected_web_descriptor_and_context_separation(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(row["slot"])
                self.assertEqual(int(row["model"]), 712)
                self.assertEqual(int(row["record"]), 612)
                self.assertEqual(module["sector_offset"], 169112 + slot * 10)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x3470
                self.assertEqual(struct.unpack_from("<I", data, 0xA8)[0],
                                 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xAC)[0],
                                 0x24420000 | (table & 0xFFFF))
                archive.seek((612 * 276 + 275) * 2048 + 0x114)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, 642000)
                self.assertEqual(command, int(row["command_word"]))
                descriptor = 0x3470 + command % 1000 * 56
                self.assertEqual(descriptor, 0x3470)
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 56, len(data))
                self.assertEqual(6 * 6 * 8, 0x120)
                self.assertEqual(3 * 0x260, 0x720)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + 0x2864 <= start or start + size <= context)
