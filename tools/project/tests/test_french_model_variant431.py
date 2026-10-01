import hashlib
import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant431Tests(family435.FrenchModelVariant435Tests):
    family = 431
    source_family = 414
    standalone_helpers = frozenset({"webs", "fan", "sheets"})
    helper_profiles = {"sheets": "gcc_2_8_1_g0_split_no_cse_follow_jumps"}
    module_count = 2
    distinct_images = 2
    binding_count = 36
    tail_start = 0x2F48
    spans = (
        (0x4, 0xF1C),
        (0xF1C, 0x1338),
        (0x1338, 0x17C8),
        (0x17C8, 0x2444),
        (0x2444, 0x28C4),
        (0x28C4, 0x2BCC),
        (0x2BCC, 0x2F48),
    )
    helpers = (
        (0xF1C, 1052, "webs", "func_8013BF1C"),
        (0x1338, 1168, "fan", "func_8013C338"),
        (0x2444, 1152, "sheets", "func_8013D444"),
        (0x28C4, 776, "spokes", "func_8013D8CC"),
        (0x2BCC, 892, "rings", "func_8013DBD8"),
    )
    reachable_helpers = {0xF1C, 0x1338, 0x2444}
    local_call_targets = {0xF1C, 0x1338, 0x17C8, 0x2444}
    models_by_stage = ((7, (401,)),)
    entry_anchors = {
        0x10: 0x80B021,
        0x1C: 0x26D80A80,
        0x20: 0xAFB80084,
        0x24: 0x26D80E10,
        0x28: 0xAFB80088,
        0x2C: 0x26D81170,
        0x30: 0xAFB8008C,
        0x7AC: 0xB821,
        0x7B4: 0x8FB80084,
        0x7BC: 0x27030090,
        0x7C0: 0x8FB80084,
        0x8F4: 0x26F70001,
        0x914: 0xAC60FFF8,
        0x920: 0x24630098,
        0x924: 0x8FB80084,
        0x928: 0x2AE20006,
        0x92C: 0x27180098,
        0x930: 0x1440FFA3,
        0x934: 0xAFB80084,
        0x938: 0x8FB80088,
        0x940: 0xAFA000B0,
        0x944: 0x2712008C,
        0x950: 0x8FB00088,
        0x968: 0xA6020000,
        0x994: 0xA6030040,
        0x9A0: 0x26100008,
        0x9B4: 0x2AE20008,
        0x9D8: 0x27180001,
        0x9E4: 0xAFB800B0,
        0x9E8: 0x8FB80088,
        0x9F0: 0x27180090,
        0x9F4: 0xAFB80088,
        0x9F8: 0xA242FFF4,
        0xA18: 0xAE400000,
        0xA24: 0xAE43FFFC,
        0xA28: 0x8FB800B0,
        0xA30: 0x2B020006,
        0xA34: 0x1440FFC4,
        0xA3C: 0x8FB8008C,
        0xA44: 0xAFA000B0,
        0xA48: 0x2712008C,
        0xA54: 0x8FB0008C,
        0xA74: 0xA6030000,
        0xAA0: 0xA6020040,
        0xAAC: 0x26100008,
        0xABC: 0x2AE20008,
        0xAD8: 0x27180001,
        0xADC: 0xAFB800B0,
        0xAE0: 0xA242FFF8,
        0xB04: 0xAE43FFFC,
        0xB08: 0xAE400000,
        0xB0C: 0x8FB8008C,
        0xB14: 0x27180090,
        0xB18: 0xAFB8008C,
        0xB1C: 0x8FB800B0,
        0xB24: 0x2B020004,
        0xB28: 0x1440FFC8,
    }

    entry_anchors.update({
        0xC: 0xAFA400F0, 0x84: 0xAFB60094, 0xB0: 0x00181040,
        0xB4: 0x00581021, 0xB8: 0x00021080, 0xBC: 0x00581021,
        0xC0: 0x000210C0, 0xC8: 0xAEC218B0, 0x4A8: 0x8FB80094,
        0x4B4: 0x2714019C, 0x4C0: 0x8FB80094, 0x4CC: 0xAFB800C0,
        0x534: 0x8FB200C0, 0x594: 0x265000C0, 0x5D0: 0x26520008,
        0x5EC: 0x2A620006, 0x618: 0x27180030, 0x64C: 0x2B020004,
        0x670: 0x271801A0, 0x688: 0xA282FFE4, 0x68C: 0xA282FFE5,
        0x690: 0xA282FFE6, 0x694: 0x2AE20003, 0x6A4: 0xAE80FFFC,
        0x6B8: 0xAE83FFF8, 0x6C0: 0x269401A0, 0xDA4: 0x28420002,
        0xDA8: 0x14400004, 0xDB0: 0x8FA400F0, 0xDB8: 0x00000000,
        0xF1C: 0x27BDFED8, 0xF24: 0x0080A821, 0xF2C: 0x02A0A021,
        0xF54: 0xA7A000E0, 0xF58: 0x8EA4188C, 0xF5C: 0x8EA51884,
        0xF60: 0x26B11730, 0xF64: 0x0C02264A, 0xF6C: 0x8EA41888,
        0xF78: 0x0C02264A, 0xF80: 0x8EA41774, 0xF84: 0x8EA51770,
        0xF8C: 0x0C02264A, 0xF94: 0x86A4177E, 0xF98: 0x86A5177C,
        0xF9C: 0x0C02264A, 0xFA0: 0x26B20194, 0xFAC: 0x18400003,
        0xFC4: 0x28821801, 0xFCC: 0x24022000, 0xFD0: 0x9243FFEC,
        0xFEC: 0x9242FFED, 0xFF8: 0x00031AC2, 0x100C: 0x9242FFEE,
        0x1050: 0x8E8218E0, 0x1058: 0x28420002, 0x1064: 0x8E821758,
        0x1070: 0x8E82175C, 0x107C: 0x8E821760, 0x1088: 0x86821764,
        0x1094: 0x86821766, 0x10A0: 0x86821768, 0x114C: 0x00039900,
        0x1150: 0x266200C0, 0x1164: 0x00052B43, 0x116C: 0x00803021,
        0x1174: 0x3C025000, 0x1178: 0xAE220000, 0x1184: 0x27A200D0,
        0x118C: 0x27A200D4, 0x1198: 0x00A03821, 0x11A8: 0x0C021E56,
        0x11C4: 0xA220000C, 0x11E0: 0xA228000F, 0x11F0: 0xA220000F,
        0x11FC: 0xA229000C, 0x1200: 0x04C0000A, 0x1208: 0x8FA200D4,
        0x1210: 0x04400006, 0x1224: 0x30C6FFFF, 0x1238: 0x28420006,
        0x1254: 0x28420004, 0x1268: 0x28622000, 0x1274: 0x8E8218A8,
        0x127C: 0x00021200, 0x1290: 0x8E8318E0, 0x1294: 0x24020005,
        0x12A8: 0xAE420000, 0x12AC: 0xAE440004, 0x12B4: 0x24030003,
        0x12CC: 0x14840004, 0x12D0: 0x24020006, 0x12D8: 0xAE8218E0,
        0x12DC: 0xAE420000, 0x12E0: 0x265201A0, 0x12FC: 0x28420003,
        0x1304: 0x26B501A0,
    })

    def test_web_sdk_alias_preserves_existing_address(self):
        paths = [family435.ROOT / self.modules[0]["linker_symbols"]]
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            paths.append(layout.with_name(layout.stem + "_symbols.txt"))
        for path in paths:
            text = path.read_text()
            self.assertIn("ratan2 = 0x80089928;", text)
            self.assertNotIn("func_french_80089928 =", text)

    def test_web_command_descriptor_and_entry_guard(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(row["slot"])
                self.assertEqual(int(row["record"]), 351)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x3044
                self.assertEqual(struct.unpack_from("<I", data, 0xA0)[0],
                                 0x3C030000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xAC)[0],
                                 0x24630000 | (table & 0xFFFF))
                archive.seek((351 * 276 + 275) * 2048 + 0x110)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(row["command_word"]))
                self.assertEqual(command, 597000)
                self.assertLessEqual(0x3044 + command % 1000 * 104 + 104, len(data))
                self.assertEqual(struct.unpack_from("<I", data, 0xDA4)[0], 0x28420002)
                self.assertEqual(struct.unpack_from("<I", data, 0xDA8)[0], 0x14400004)
                self.assertEqual(struct.unpack_from("<I", data, 0xDB0)[0], 0x8FA400F0)
                self.assertEqual(struct.unpack_from("<I", data, 0xDB4)[0],
                                 0x0C000000 | (((base + 0xF1C) >> 2) & 0x3FFFFFF))
                self.assertEqual(3 * 416, 0x4E0)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + 0x18F4 <= start or start + size <= context)

    def _sheet_images(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                yield module, data

    def test_sheet_extent_and_unused_terminal_companion_read(self):
        self.assertEqual(0xA80 + 6 * 0x98, 0xE10)
        self.assertEqual(0x4E0 + 5 * 0x120, 0xA80)
        self.assertEqual(0x4E0 + 5 * 0x120 + 0xF4, 0xA80 + 0x98 + 0x5C)
        anchors = {
            0x460: 0xAC6200F4, 0x464: 0x24C6FF00, 0x46C: 0x2A620005,
            0x490: 0x24A50120, 0x498: 0x2BC20005, 0x49C: 0x27180120,
            0x914: 0xAC60FFF8, 0x920: 0x24630098, 0x928: 0x2AE20006,
            0x2444: 0x27BDFEF0, 0x2450: 0x266804E0, 0x2458: 0x26760A80,
            0x24A8: 0x8D2200F4, 0x24B0: 0x18400002, 0x24F4: 0x16AA0017,
            0x2874: 0x2AA20006, 0x2884: 0x25290120,
        }
        for module, data in self._sheet_images():
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                 (module["name"], hex(offset)))

    def test_sheet_entry_descriptor_gate_precedes_companion_advancement(self):
        for module, data in self._sheet_images():
            base = int(module["load_address"], 0)
            for offset, word in {
                0xC: 0xAFA400F0, 0xD3C: 0x8EC218B0, 0xD44: 0x8C43002C,
                0xD48: 0x8EC218A0, 0xD50: 0x0043102B, 0xD54: 0x14400004,
                0xD5C: 0x8FA400F0, 0xD64: 0, 0x1B20: 0x8E6218A8,
                0x1B28: 0x000211C0, 0x1B30: 0xAEA200F4, 0x1B54: 0xAEA200F4,
            }.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                 (module["name"], hex(offset)))
            for call, target in ((0xD60, 0x2444), (0xD94, 0x17C8)):
                self.assertEqual(struct.unpack_from("<I", data, call)[0],
                                 0x0C000000 | ((base + target) >> 2 & 0x3FFFFFF))

    def test_sheet_projection_sorting_and_terminal_phase_updates(self):
        anchors = {
            0x2484: 0x26711668, 0x25DC: 0x24030400, 0x25E0: 0xAFA30030,
            0x25E4: 0xAFA30034, 0x25E8: 0xAFA30038,
            0x26C8: 0x27A200D0, 0x26D0: 0x27A200D4,
            0x277C: 0x04600011, 0x2784: 0x8FA200D4, 0x278C: 0x0440000D,
            0x2794: 0x12A90008, 0x27A4: 0x8D420104, 0x27AC: 0x28420400,
            0x27C0: 0x3066FFFF, 0x27D8: 0x16AB0022, 0x27E0: 0x8E6318E0,
            0x280C: 0x00021240, 0x2814: 0xAE020000, 0x284C: 0x00021140,
            0x2854: 0x1C400003, 0x2858: 0xAE020000,
            0x285C: 0xAE000000, 0x2860: 0xAE7518E0,
        }
        for module, data in self._sheet_images():
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                 (module["name"], hex(offset)))

    def test_fan_initializer_extent_and_packet(self):
        self.assertEqual(0x13B0 + 5 * 0x74, 0x15F4)
        self.assertEqual(0x1610 + 36, 0x1634)
        anchors = {
            0x34: 0x26D813B0, 0x40: 0xAFB80090, 0x4C: 0x26D815F4,
            0x54: 0x26D81610, 0x6C4: 0x8FB80090, 0x6CC: 0x27120070,
            0x6DC: 0xA6800000, 0x6FC: 0xA6820008, 0x72C: 0xA6830030,
            0x74C: 0x2A620005, 0x764: 0xA242FFE8, 0x76C: 0xA242FFE9,
            0x778: 0xA242FFEC, 0x784: 0xAE40FFF0, 0x790: 0xAE400000,
            0x794: 0x26520074, 0x79C: 0x2AE20005, 0x7A0: 0x27180074,
            0x1338: 0x27BDFEF8, 0x1348: 0x269313B0, 0x1350: 0x26911610,
            0x137C: 0x26901410, 0x1768: 0x26100074, 0x176C: 0x2BC20005,
            0x1774: 0x26730074,
        }
        for module, data in self._sheet_images():
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                 (module["name"], hex(offset)))

    def test_fan_entry_gate_and_bounded_descriptor_index(self):
        for module, data in self._sheet_images():
            base = int(module["load_address"], 0)
            self.assertEqual(struct.unpack_from("<I", data, 0x3044 + 0x14)[0], 0)
            self.assertEqual(struct.unpack_from("<11I", data, 0x3044 + 0x2C),
                             tuple(range(10, 311, 30)))
            self.assertLessEqual(0x2C + 11 * 4, 104)
            self.assertEqual(struct.unpack_from("<I", data, 0xD34)[0],
                             0x0C000000 | ((base + 0x1338) >> 2 & 0x3FFFFFF))
            for offset, word in {
                0xB44: 0xAEC018D8, 0xD10: 0x8EC218B0, 0xD18: 0x8C430014,
                0xD24: 0x0043102B, 0xD28: 0x14400004, 0xD30: 0x8FA400F0,
                0xD38: 0, 0x16A8: 0x8E8218D8, 0x16B4: 0x00621821,
                0x16B8: 0x8C62002C, 0x16C0: 0x2442FFF8, 0x16C4: 0x0062182B,
                0x1B74: 0x24020005, 0x1B78: 0x15420015,
                0x1B84: 0x24020004, 0x1B88: 0x15620011,
                0x1B90: 0x8E6318D8, 0x1B98: 0x2862000A,
                0x1B9C: 0x10400005, 0x1BA0: 0x24620001, 0x1BA8: 0xAE6218D8,
            }.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                 (module["name"], hex(offset)))

    def test_fan_projection_done_flag_and_angle(self):
        anchors = {
            0x1384: 0xAFA200D8, 0x1388: 0x8E82189C, 0x13A8: 0x968218DC,
            0x13B4: 0x8EE21794, 0x13C0: 0x8EE21798, 0x13CC: 0x8EE2179C,
            0x14CC: 0x27A200D0, 0x14D4: 0x27A200D4,
            0x156C: 0x04C00009, 0x157C: 0x04400005, 0x1588: 0x30C6FFFF,
            0x1670: 0x04C00009, 0x1680: 0x04400005, 0x168C: 0x30C6FFFF,
            0x16D8: 0x28621000, 0x16E4: 0x8E020010, 0x16EC: 0x1440000D,
            0x16FC: 0x00021280, 0x1714: 0xAE020000, 0x1720: 0xAE020010,
            0x1734: 0x8E030010, 0x173C: 0x14620008, 0x174C: 0x00021200,
            0x1754: 0x1C400002, 0x175C: 0xAE000000, 0x1760: 0x26F70020,
            0x1780: 0x00021840, 0x1784: 0x00621821,
            0x178C: 0x00031980, 0x1794: 0xAE8218DC,
            0x1BA4: 0x24080001, 0x1BB0: 0xAFA80140, 0x1C14: 0x15420029,
            0x1C90: 0x266313B0, 0x1C98: 0xAC600070,
            0x1CAC: 0x28420005, 0x1CB0: 0x1440FFF9, 0x1CB4: 0x24630074,
        }
        for module, data in self._sheet_images():
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                 (module["name"], hex(offset)))
