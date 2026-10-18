import hashlib
import re
import struct
import unittest
from unittest.mock import patch

from tools.project.tests import test_french_model_variant422 as french
from tools.project.tests import test_spanish_model_variant460 as instructions
from tools.project.tests.test_french_model_variant435 import ROOT
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant422Tests(french.FrenchModelVariant422Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    register_writes = staticmethod(instructions.SpanishModelVariant460Tests.register_writes)
    direct_stores = staticmethod(instructions.SpanishModelVariant460Tests.direct_stores)

    def legal_images(self):
        path = ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                yield module, data

    def test_original_context_reaches_both_entry_calls_despite_initialization_reuse(self):
        expected_calls = {0xFA4: 0x1620, 0xFC0: 0x1128}
        for module, data in self.legal_images():
            base = int(module["load_address"], 0)
            word = lambda pc: struct.unpack_from("<I", data, pc)[0]
            self.assertEqual(word(0xC), 0x00809821)
            self.assertEqual(word(0x14), 0x0260B021)
            for pc, target in expected_calls.items():
                self.assertEqual(word(pc + 4), 0x02602021)
                self.assertEqual(word(pc), 0x0C000000 | ((base + target) >> 2 & 0x3FFFFFF))
            definitions = {r: set(self.register_writes(data, 4, 0x1128, r)) for r in (4, 19)}
            pending, visited, reaching = [(4, None, None)], set(), {}
            while pending:
                pc, root, arg = pending.pop()
                self.assertTrue(4 <= pc < 0x1128 and pc % 4 == 0)
                if (pc, root, arg) in visited:
                    continue
                visited.add((pc, root, arg))
                if pc in definitions[19]:
                    root = pc
                if pc in definitions[4]:
                    arg = pc
                instruction = word(pc)
                op = instruction >> 26
                if op in (1, 2, 3, 4, 5, 6, 7) or instruction == 0x03E00008:
                    if pc + 4 in definitions[19]:
                        root = pc + 4
                    if pc + 4 in definitions[4]:
                        arg = pc + 4
                    if pc in expected_calls:
                        reaching.setdefault(pc, set()).add((root, arg))
                    if instruction == 0x03E00008:
                        continue
                    if op == 2:
                        targets = [(((base + pc + 4) & 0xF0000000) | ((instruction & 0x3FFFFFF) << 2)) - base]
                    elif op == 3:
                        targets, arg = [pc + 8], None
                    else:
                        displacement = (instruction & 65535) - (65536 if instruction & 32768 else 0)
                        targets = [pc + 8, pc + 4 + displacement * 4]
                    pending.extend((target, root, arg) for target in targets)
                else:
                    pending.append((pc + 4, root, arg))
            self.assertEqual(reaching, {pc: {(0xC, pc + 4)} for pc in expected_calls})
            calls = [(pc, 0x80000000 | ((word(pc) & 0x3FFFFFF) << 2))
                     for pc in range(4, 0x1128, 4) if word(pc) >> 26 == 3]
            symbols = ROOT / module["linker_symbols"]
            addresses = {int(value, 0) for value in re.findall(r"= (0x[0-9A-F]+);", symbols.read_text())}
            self.assertEqual(len(addresses), 36)
            self.assertEqual(len(calls), 75)
            self.assertEqual(len({target for _, target in calls if target in addresses}), 23)
            for _, target in calls:
                self.assertIn(target, addresses | {base + offset for offset in expected_calls.values()})
            self.assertEqual({pc: target - base for pc, target in calls if base <= target < base + 20480},
                             expected_calls)

    def test_complete_register_write_sets_for_all_nine_functions(self):
        rows = (
            (4, 0x1128, (
                [0x16C, 0x170, 0x21C, 0x448, 0x48C, 0x690, 0x790, 0xA50, 0xBA4, 0xBFC, 0xCA0, 0xD00, 0xEE8, 0xF0C, 0xFE0, 0x111C],
                [0x1B4, 0x204, 0x3F8, 0x4A0, 0x658, 0x770, 0x9F0, 0xA8C, 0xBA8, 0xBF4, 0xCA4, 0xCF8, 0xED4, 0xF10, 0x1118],
                [0x1C8, 0x3F4, 0x4A8, 0x664, 0x6C8, 0x9EC, 0xAB0, 0xBA0, 0xC8C, 0xC9C, 0xD60, 0xE80, 0x1114],
                [0xC, 0x1E0, 0x540, 0x550, 0x660, 0x6C0, 0x974, 0xAE8, 0xB94, 0xC34, 0xC90, 0xD28, 0x1110],
                [0x1EC, 0x3F0, 0x4A4, 0x590, 0x610, 0x65C, 0x6C4, 0x984, 0x9E8, 0xA48, 0xAF0, 0xB9C, 0xC38, 0xC98, 0xD64, 0x110C],
                [0x1F8, 0x650, 0x970, 0xB90, 0x1108], [0x14, 0x1104],
                [0x48, 0x27C, 0x648, 0x9DC, 0x1100], [4, 0x1124], [0x50, 0x31C, 0x9A8, 0x10FC])),
            (0x1128, 0x1620, (
                [0x11E0, 0x124C, 0x140C, 0x14F8, 0x15D0, 0x1614],
                [0x11CC, 0x1200, 0x1254, 0x1498, 0x15BC, 0x1610], [0x1138, 0x160C],
                [0x1170, 0x13A0, 0x14EC, 0x15C0, 0x1608],
                [0x11C8, 0x1250, 0x148C, 0x15C4, 0x1604],
                [0x1188, 0x13A4, 0x13E0, 0x15D8, 0x1600],
                [0x11A0, 0x11B8, 0x1504, 0x15B8, 0x15FC],
                [0x11A8, 0x11C0, 0x11C4, 0x13DC, 0x15D4, 0x15F8],
                [0x1128, 0x161C], [0x1130, 0x1510, 0x15F4])),
            (0x1620, 0x1CFC, (
                [0x16BC, 0x16F4, 0x1728, 0x1778, 0x17AC, 0x1804, 0x18C4, 0x19B0, 0x1A34, 0x1AEC, 0x1CF0],
                [0x1754, 0x17E0, 0x188C, 0x1BBC, 0x1CEC], [0x1674, 0x1CE8],
                [0x16A8, 0x1828, 0x187C, 0x1BAC, 0x1CE4],
                [0x16A4, 0x1840, 0x1848, 0x1BA8, 0x1CE0], [0x168C, 0x1CB0, 0x1CDC],
                [0x1628, 0x1CB8, 0x1CD8], [0x16A0, 0x1824, 0x1844, 0x1BB0, 0x1CD4],
                [0x1620, 0x1CF8], [0x1630, 0x1CD0])),
            (0x1CFC, 0x2408, (
                [0x1DD0, 0x1DD4, 0x1E30, 0x1FF4, 0x1FF8, 0x2014, 0x2054, 0x2098, 0x23FC],
                [0x1DCC, 0x20B0, 0x23F8], [0x1D40, 0x23F4], [0x1D04, 0x23F0],
                [0x1DA0, 0x207C, 0x2080, 0x22EC, 0x23EC], [0x1DB0, 0x23E8],
                [0x1DB4, 0x2050, 0x2088, 0x22C4, 0x23E4], [0x1DAC, 0x23E0],
                [0x1CFC, 0x2404], [0x1DA4, 0x2070, 0x2084, 0x22E0, 0x23DC])),
            (0x2408, 0x28F0, (
                [0x2448, 0x28A4, 0x28E4], [0x2410, 0x28E0], [0x2440, 0x28DC],
                [0x2644, 0x2744, 0x28D8], [0x2620, 0x2734, 0x28D4],
                [0x2614, 0x2730, 0x28D0], [0x25A8, 0x272C, 0x28CC],
                [0x2598, 0x2738, 0x28C8], [0x2408, 0x28EC], [0x2418, 0x28AC, 0x28C4])),
            (0x28F0, 0x2E44, (
                [0x2AE4, 0x2B0C, 0x2B6C, 0x2C4C, 0x2E38], [0x2940, 0x2E34],
                [0x2980, 0x2DE8, 0x2E30], [0x28F8, 0x2E2C],
                [0x29E0, 0x2A68, 0x2A74, 0x2E28], [0x29D8, 0x2A5C, 0x2A70, 0x2E24],
                [0x2B80, 0x2E20], [0x29D4, 0x2A40, 0x2A6C, 0x2E1C],
                [0x28F0, 0x2E40], [0x2B68, 0x2C68, 0x2E18])),
            (0x2E44, 0x3154, (
                [0x2E94, 0x3108, 0x3148], [0x2E60, 0x3144], [0x2EF8, 0x30B8, 0x3140],
                [0x2EEC, 0x30AC, 0x313C], [0x2E4C, 0x3138], [0x2E88, 0x3134],
                [0x2E58, 0x3110, 0x3130], [0x2E90, 0x312C], [0x2E44, 0x3150], [0x2E8C, 0x3128])),
            (0x3154, 0x34D0, (
                [0x31A0, 0x3490, 0x34C4], [0x316C, 0x34C0], [0x315C, 0x34BC],
                [0x31C8, 0x33C4, 0x34B8], [0x31BC, 0x33B8, 0x34B4], [0x3194, 0x34B0],
                [0x3190, 0x348C, 0x34AC], [0x3164, 0x349C, 0x34A8], [0x3154, 0x34CC], [0x319C, 0x34A4])),
            (0x34D0, 0x3834, (
                [0x3510, 0x3800, 0x3828], [0x34E8, 0x3824], [0x34D8, 0x3820], [0x350C, 0x381C],
                [0x34E0, 0x3808, 0x3818], [0x3508, 0x37FC, 0x3814], [0x3504, 0x3810], [],
                [0x34D0, 0x3830], [])),
        )
        for _, data in self.legal_images():
            for start, end, writes in rows:
                self.assertEqual(len(writes), 10)
                for register, expected in zip((16, 17, 18, 19, 20, 21, 22, 23, 29, 30), writes):
                    self.assertEqual(self.register_writes(data, start, end, register), expected)

    def test_all_packet_footprints_and_frames(self):
        colors = {(offset + component, 1) for offset in (4, 16, 28, 40) for component in range(3)}
        uv = {(offset + component, 1) for offset in (12, 24, 36, 48) for component in range(2)}
        xy = {(offset + component, 2) for offset in (8, 20, 32, 44) for component in (0, 2)}
        lines = {(0, 4)} | {(offset, 1) for offset in range(12, 18)}
        quad = {(offset + component, 1) for offset in (4, 12, 20, 28) for component in range(3)}
        rows = (
            (0x1128, 0x1620, 328, 18, colors | uv),
            (0x1620, 0x1CFC, 312, 18, colors | uv | {(26, 2)}),
            (0x1CFC, 0x2408, 256, 18, colors | xy),
            (0x2408, 0x28F0, 272, 18, colors),
            (0x28F0, 0x2E44, 288, 17, lines),
            (0x2E44, 0x3154, 272, 17, lines),
            (0x3154, 0x34D0, 264, 17, lines),
            (0x34D0, 0x3834, 248, 17, quad),
        )
        for _, data in self.legal_images():
            for start, end, frame, packet, fields in rows:
                self.assertEqual(struct.unpack_from("<I", data, start)[0], 0x27BD0000 | (-frame & 65535))
                self.assertEqual(struct.unpack_from("<I", data, end - 4)[0], 0x27BD0000 | frame)
                self.assertEqual({(off, width) for _, off, width in self.direct_stores(data, start, end, packet)}, fields)
            for offset, word in {
                0x1130: 0x0080F021, 0x1138: 0x27D21BDC, 0x1510: 0x02C0F021,
                0x1628: 0x0080B021, 0x1630: 0x02C0F021, 0x1674: 0x26D21CB8,
                0x1CB0: 0x26B501A0, 0x1CB8: 0x26D601A0,
                0x2410: 0x00808821, 0x2440: 0x26321C84, 0x2940: 0x26711D4C,
            }.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)

    def test_incoming_argument_and_separate_color_projection_homes(self):
        for _, data in self.legal_images():
            word = lambda pc: struct.unpack_from("<I", data, pc)[0]
            self.assertEqual(word(4), 0x27BDFF00)
            self.assertEqual(word(0x1124), 0x27BD0100)
            self.assertEqual(word(0x94), 0xAFA50104)
            stores = self.direct_stores(data, 4, 0x1128, 29)
            self.assertEqual([(pc, off, size) for pc, off, size in stores if off + size > 256],
                             [(0x94, 260, 4)])
            for offset, expected in {
                0x1578: 0x27A20100, 0x1580: 0x27A20104,
                0x1958: 0x27A200F8, 0x1960: 0x27A200FC,
                0x155C: 0xAFA20010, 0x1564: 0xAFA20014,
                0x156C: 0xAFA20018, 0x1574: 0xAFA2001C,
                0x158C: 0xAFA20024,
            }.items():
                self.assertEqual(word(offset), expected)
            veil_words = struct.unpack("<439I", data[0x1620:0x1CFC])
            accesses = {word & 65535 for word in veil_words
                        if word >> 26 in (32, 33, 35, 36, 37, 40, 41, 43) and word >> 21 & 31 == 29}
            self.assertFalse(any(208 <= offset < 232 for offset in accesses))
            self.assertTrue({232, 233, 234, 240, 241, 242, 252} <= accesses)

    def test_actual_descriptors_literals_and_retained_scope(self):
        self.assertEqual(len(french.FrenchModelVariant422Tests.entry_anchors), 387)
        sectors = (96, 48, 2, 1, 16, 1, 16, 10, 10, 10, 10, 2, 2, 1, 50, 1)
        self.assertEqual(sum(sectors), 276)
        for module, data in self.legal_images():
            for offset, expected in french.FrenchModelVariant422Tests.entry_anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], expected)
            model = int(module["name"].split("_")[3])
            record = model - 50 * (model >= 350)
            slot = int(module["name"][-1])
            stage = 7 + slot
            self.assertEqual(module["sector_offset"], record * 276 + sum(sectors[:stage]))
            self.assertEqual(int(module["load_address"], 0), 0x8013B000 + slot * 0x40000)
            with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as archive:
                archive.seek((record * 276 + 275) * 2048 + 0x110)
                command, = struct.unpack("<i", archive.read(4))
            self.assertEqual(command, 588000)
            self.assertEqual(struct.unpack_from("<4I", data, 0x386C + 28), (60, 120, 360, 380))
            self.assertEqual(5 * 416, 0x820)
            self.assertEqual(0x820 + 1460, 0xDD4)
            self.assertEqual(0xDD4 + 3 * 416, 0x12B4)
            self.assertEqual(0x12B4 + 456, 0x147C)
            self.assertEqual(0x147C + 2 * 152, 0x15AC)
            self.assertEqual(0x15AC + 6 * 144, 0x190C)
            self.assertEqual(0x190C + 4 * 144, 0x1B4C)
            self.assertEqual(0x1B4C + 144, 0x1BDC)
            self.assertEqual(french.FrenchModelVariant422Tests.local_call_targets, {0x1128, 0x1620})

    def test_missing_spanish_archive_skips_before_open(self):
        with patch.object(type(ROOT), "exists", return_value=False), \
                patch.object(type(ROOT), "open", side_effect=AssertionError("missing archive opened")):
            with self.assertRaisesRegex(unittest.SkipTest, "legal Spanish MODEL input required"):
                self.test_actual_descriptors_literals_and_retained_scope()

    def test_halo_colors_and_veil_wrap_phase_gates(self):
        anchors = {
            0x1280: 0x03B51021, 0x1298: 0x03B51821,
            0x12AC: 0xA06200D0, 0x12B0: 0xA06200D8, 0x12B4: 0xA06200E0,
            0x12D0: 0xA08200E8, 0x12D4: 0xA06200F0, 0x12DC: 0xA06200F8,
            0x12F0: 0xA04B00D8, 0x12F4: 0xA04A00D0, 0x12F8: 0xA04800E0,
            0x12FC: 0xA04900E8, 0x1300: 0xA04A00F0, 0x1304: 0xA04B00F8,
            0x1324: 0x00021140, 0x1338: 0x00021140, 0x1354: 0x8C440028,
            0x1360: 0x0044102B, 0x1374: 0xAE680578, 0x137C: 0xAE69058C,
            0x1380: 0x24A2FC00, 0x1384: 0xAE620578, 0x138C: 0xAE62058C,
            0x139C: 0xAE6205A0, 0x13A4: 0x26B50001, 0x13AC: 0x2AA20005,
            0x13B0: 0x254A0088, 0x1528: 0x240A007F, 0x1530: 0x24080060,
            0x1594: 0x04C00008, 0x15A4: 0x04400004, 0x15B4: 0x30C6FFFF,
            0x196C: 0x86420008, 0x1974: 0x284200A0, 0x199C: 0x24060140,
            0x1A10: 0x24060080, 0x1A20: 0x240601C0,
            0x1BDC: 0x00021880, 0x1BE0: 0x00621821, 0x1BE4: 0x000318C0,
            0x1C00: 0x8C430024, 0x1C0C: 0x0043102B, 0x1C14: 0x2482F800,
            0x1C18: 0x24020800, 0x1C28: 0xAEA20000, 0x1C48: 0x8C430020,
            0x1C54: 0x0043102B, 0x1C60: 0xAFC21E1C, 0x1C70: 0x01420018,
            0x1C80: 0x24020005, 0x1C90: 0x24020001, 0x1C98: 0x24020002,
            0x1CAC: 0xAFC41E1C, 0x1CC0: 0x29820005,
        }
        for _, data in self.legal_images():
            for offset, expected in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], expected)
            self.assertEqual(self.direct_stores(data, 0x1128, 0x1510, 30), [])
            self.assertEqual(self.direct_stores(data, 0x1620, 0x1CFC, 30),
                             [(0x1C60, 0x1E1C, 4), (0x1CAC, 0x1E1C, 4)])
        colors = [{base + index for index in range(5)} for base in (208, 216, 224, 232, 240, 248)]
        self.assertEqual(len(set.union(*colors)), 30)
        self.assertLess(max(set.union(*colors)), 256)


if __name__ == "__main__":
    unittest.main()
