import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant465Tests(family435.FrenchModelVariant435Tests):
    resident_name = "SLES_039.48"
    family = 465
    source_family = 448
    standalone_helpers = frozenset({"fan", "orbit"})
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
        (0x124C, 1056, "fan", "func_8013C24C"),
        (0x256C, 1352, "orbit", "func_8013D56C"),
        (0x2AB4, 1376, "webs", "func_8013DB58"),
        (0x3014, 776, "spokes", "func_8013E0BC"),
        (0x331C, 892, "rings", "func_8013E3C8"),
        (0x3698, 868, "quad", "func_8013E748"),
    )
    reachable_helpers = {0x124C, 0x256C}
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

    entry_anchors.update({
        0x60: 0x26D11948, 0x68: 0x26D81964, 0x6C: 0xAFB800A0, 0x90: 0xAFB60080,
        0x98: 0x04A00396, 0x150: 0xAFA000B4, 0x15C: 0x271E0124, 0x234: 0x02202021,
        0x244: 0x0C020B92, 0x258: 0x8FA400A0, 0x25C: 0x0C020BB2, 0x404: 0x00009821,
        0x408: 0x02609021, 0x40C: 0x8FB40080, 0x410: 0x8FB80080, 0x418: 0xA7000000,
        0x41C: 0xA7C0FEDE, 0x424: 0xA7C0FEE0, 0x438: 0x26910008, 0x43C: 0xA6830008,
        0x444: 0xA6200002, 0x45C: 0xA6230004, 0x468: 0x26900090, 0x46C: 0xA6820090,
        0x474: 0xA6000002, 0x478: 0x0220A021, 0x47C: 0x26730001, 0x484: 0xA6020004,
        0x488: 0x2A620011, 0x490: 0x00139200, 0x494: 0x8FB800B4, 0x498: 0x24020080,
        0x49C: 0x27180001, 0x4A0: 0xAFB800B4, 0x4A4: 0x241800FF, 0x4A8: 0xA3D8FFF4,
        0x4AC: 0xA3D8FFF5, 0x4B0: 0xA3D8FFF6, 0x4B4: 0xA3C0FFF8, 0x4B8: 0xA3C2FFF9,
        0x4BC: 0xA3D8FFFA, 0x4C0: 0xA3C0FFFC, 0x4C4: 0xA3C0FFFD, 0x4C8: 0xA3C0FFFE,
        0x4CC: 0xAFC00000, 0x4D0: 0x8FB80080, 0x4D8: 0x27180128, 0x4DC: 0xAFB80080,
        0x4E0: 0x8FB800B4, 0x4E8: 0x1B00FFC6, 0x4EC: 0x27DE0128, 0xED0: 0xAEC01B00,
        0xED8: 0xAEC01B08, 0xEDC: 0xAEC01B3C, 0xFA8: 0xAEC01ABC, 0xFD4: 0xAEC41AB8,
        0xFDC: 0xAEC51AC0, 0x1064: 0x8EC31B10, 0x1070: 0x8C63001C, 0x1074: 0x8EC21B00,
        0x107C: 0x0043102B, 0x1080: 0x14400003, 0x108C: 0x02402021, 0x10F0: 0x8EC31B00,
        0x10F8: 0x00621821, 0x1100: 0xAEC31B00, 0x1104: 0xAEC21B08, 0x124C: 0x27BDFEF0,
        0x1254: 0x0080A021, 0x125C: 0x0280B821, 0x1264: 0x26931948, 0x1288: 0xA7A00028,
        0x128C: 0xA7A0002A, 0x1290: 0xA7A0002C, 0x1294: 0x8E831AB8, 0x1298: 0x26921964,
        0x12A0: 0x8E831ABC, 0x12AC: 0x8E831AC0, 0x12B8: 0x8E830124, 0x12C0: 0xAFA000DC,
        0x12CC: 0x27A800D0, 0x12DC: 0x26910124, 0x1358: 0x0000F021, 0x135C: 0x24160010,
        0x1360: 0x24150008, 0x1364: 0x03C08021, 0x1368: 0x02802021, 0x136C: 0x02962821,
        0x1370: 0x02953021, 0x1374: 0x26670008, 0x137C: 0x26620010, 0x1384: 0x26620018,
        0x138C: 0x27A200D4, 0x1394: 0x0C021E26, 0x139C: 0x9223FFF4, 0x13C0: 0x9223FFF8,
        0x1404: 0x04C00009, 0x1414: 0x04400005, 0x1420: 0x30C6FFFF, 0x1428: 0x24070001,
        0x142C: 0x02952021, 0x1430: 0x02962821, 0x1434: 0x26060090, 0x143C: 0x26070098,
        0x1448: 0x26420008, 0x1450: 0x26420010, 0x1458: 0x26420018, 0x1460: 0x26420020,
        0x1470: 0x0C021E56, 0x1478: 0x9223FFF8, 0x14C0: 0x9223FFFC, 0x1504: 0x04C00009,
        0x1514: 0x04400005, 0x1520: 0x30C6FFFF, 0x1528: 0x24070001, 0x152C: 0x26D60008,
        0x1530: 0x26B50008, 0x1534: 0x27DE0001, 0x1538: 0x2BC20010, 0x1540: 0x26100008,
        0x1544: 0x8EE41B3C, 0x155C: 0x28421000, 0x1568: 0x8EE31B10, 0x156C: 0x8EE21B00,
        0x1570: 0x8C64001C, 0x1574: 0x8C630020, 0x1578: 0x00441023, 0x157C: 0x00021300,
        0x1580: 0x00641823, 0x1584: 0x0043001B, 0x1590: 0x0007000D, 0x15AC: 0xAE220000,
        0x15B8: 0x1500001A, 0x15BC: 0x25020001, 0x15C4: 0xAEE21B3C, 0x15C8: 0x8EE21B10,
        0x15D0: 0x8C430024, 0x15D4: 0x8EE21B00, 0x15DC: 0x0043102B, 0x15F8: 0x8EE21B08,
        0x1600: 0x000211C0, 0x1614: 0xAE200000, 0x1618: 0x14820002, 0x161C: 0x24020003,
        0x1620: 0xAEE21B3C, 0x1624: 0x26310128, 0x162C: 0x26940128, 0x1630: 0x25290001,
        0x1634: 0x1920FF48,
    })

    entry_anchors.update({
        0x534: 0xACE00200, 0x538: 0x24E70004, 0x540: 0x2A820002, 0x618: 0x29020003,
        0x61C: 0x27180208, 0x628: 0x240AFF00, 0x630: 0x24090100, 0x644: 0x27100020,
        0x648: 0x270F0040, 0x64C: 0x270E0060, 0x67C: 0xA70A0000, 0x680: 0xA48AFF72,
        0x684: 0xA480FF74, 0x6A0: 0xA480FFD0, 0x6A4: 0xA5C00002, 0x6AC: 0xA5C00004,
        0x6B8: 0xA489FF78, 0x6BC: 0xA5AA0002, 0x6F4: 0xA489FF80, 0x6F8: 0xA4E90002,
        0x730: 0xA48AFF88, 0x734: 0xA4690002, 0x764: 0x28A20004, 0x778: 0x90420004,
        0x780: 0xA082FFF0, 0x78C: 0x90420005, 0x794: 0xA082FFF1, 0x7A0: 0x90420006,
        0x7A8: 0xA082FFF2, 0x7B4: 0x90420000, 0x7BC: 0xA082FFF4, 0x7C8: 0x90420001,
        0x7D0: 0xA082FFF5, 0x7DC: 0x90420002, 0x7E8: 0xAC80FFFC, 0x7EC: 0xAC800000,
        0x7F0: 0xA082FFF6, 0x828: 0x24630004, 0x838: 0x24A5000C, 0x848: 0x24E70018,
        0x1090: 0x8EC21B3C, 0x1098: 0x28420005, 0x109C: 0x14400003, 0x10A8: 0x02402021,
        0x10C0: 0x02402021, 0x1700: 0x25D60BD8, 0x1C18: 0xAD420200, 0x1C64: 0xAD420200,
        0x1CF4: 0x24020006, 0x1CF8: 0xAF021B3C, 0x1D38: 0x26D60208, 0x256C: 0x27BDFED0,
        0x2574: 0x0080A821, 0x2578: 0x26A80BD8, 0x2580: 0x26B111F0, 0x25AC: 0x26B219BC,
        0x25E0: 0x000A1080, 0x25E4: 0x01621021, 0x25E8: 0x8C420200, 0x25F0: 0x18400104,
        0x2610: 0x8EA21AFC, 0x2618: 0x30420001, 0x2638: 0x8C420098, 0x2644: 0x000298C3,
        0x2648: 0x24420007, 0x27D8: 0x0C021E56, 0x27E0: 0x92230084, 0x284C: 0x92230080,
        0x286C: 0x04C00008, 0x2874: 0x8FA200D4, 0x287C: 0x04400004, 0x2888: 0x0C0210AA,
        0x288C: 0x30C6FFFF, 0x28A0: 0x2BC20004, 0x28C0: 0x8C640098, 0x28C8: 0x28822000,
        0x28D4: 0x8C6200E0, 0x28E4: 0x8EA21B08, 0x28EC: 0x00021280, 0x28F4: 0xAC620098,
        0x2904: 0xAC620098, 0x2910: 0xAC6200E0, 0x2930: 0x18A0000E, 0x2938: 0x8C8300E0,
        0x2948: 0x8EA21B08, 0x2950: 0x00021200, 0x2960: 0x24020002, 0x2964: 0xAC800098,
        0x2968: 0xAC8200E0, 0x2980: 0x8C4200E0, 0x298C: 0x29220024, 0x2998: 0x8EA31B3C,
        0x299C: 0x24020006, 0x29A8: 0x24020007, 0x29AC: 0xAEA21B3C, 0x29C0: 0x000A1300,
        0x29F8: 0x29020003, 0x2A10: 0x2529000C, 0x2A18: 0x256B0800, 0x2A1C: 0x29420002,
        0x2A40: 0x00081280, 0x2A50: 0x25290208, 0x2A68: 0x25080018, 0x2A78: 0x29620003,
    })

    def test_orbit_binding_keeps_existing_resident_address(self):
        bindings = (family435.ROOT / self.modules[0]["linker_symbols"]).read_text()
        self.assertIn("rcos = 0x800866F8;", bindings)
        self.assertNotIn("func_french_800866F8", bindings)
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn("rcos = 0x800866F8; // type:func absolute:true", symbols)
            self.assertNotIn("func_french_800866F8", symbols)

    def test_orbit_entry_phase_gate_and_colors(self):
        path = family435.ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<I", data, 0x10A4)[0],
                                 0x0C000000 | ((base + 0x256C) >> 2 & 0x3FFFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x10BC)[0],
                                 0x0C000000 | ((base + 0x166C) >> 2 & 0x3FFFFFF))
                self.assertEqual(struct.unpack_from("<3I", data, 0x1090),
                                 (0x8EC21B3C, 0, 0x28420005))
                self.assertEqual(data[0x4450:0x4453], bytes((140, 128, 16)))
                self.assertEqual(data[0x4454:0x4457], bytes((224, 180, 160)))

    def test_orbit_selector_and_completion_bounds(self):
        path = family435.ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        self.assertEqual(0xBD8 + 3 * 0x208, 0x11F0)
        self.assertEqual(0x11F0 + 296, 0x1318)
        self.assertEqual(0x98 + 3 * 2 * 3 * 4, 0xE0)
        self.assertEqual(0xE0 + 3 * 2 * 3 * 4, 296)
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for offset, word in {
                    0x25E4: 0x01621021, 0x25E8: 0x8C420200, 0x25F0: 0x18400104,
                    0x28A0: 0x2BC20004, 0x29F8: 0x29020003, 0x2A1C: 0x29420002,
                    0x2A50: 0x25290208, 0x2A78: 0x29620003,
                    0x298C: 0x29220024, 0x2998: 0x8EA31B3C, 0x299C: 0x24020006,
                    0x29A8: 0x24020007, 0x29AC: 0xAEA21B3C,
                    0x1CF4: 0x24020006, 0x1CF8: 0xAF021B3C,
                }.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)

    def test_fan_binding_keeps_existing_resident_address(self):
        bindings = (family435.ROOT / self.modules[0]["linker_symbols"]).read_text()
        self.assertIn("RotTransPers3 = 0x80087898;", bindings)
        self.assertNotIn("func_french_80087898", bindings)
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn("RotTransPers3 = 0x80087898; // type:func absolute:true", symbols)
            self.assertNotIn("func_french_80087898", symbols)

    def test_fan_entry_call_and_selected_timing(self):
        path = family435.ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<I", data, 0x1088)[0],
                                 0x0C000000 | ((base + 0x124C) >> 2 & 0x3FFFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x108C)[0], 0x02402021)
                start, end, shrink = struct.unpack_from("<3I", data, 0x4450 + 0x1C)
                self.assertEqual((start, end, shrink), (40, 140, 220))
                self.assertEqual(end - start, 100)
                self.assertLessEqual(end, shrink)

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
