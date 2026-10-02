import hashlib
import re
import struct
import unittest

from tools.project.tests import test_french_model_variant431 as french
from tools.project.tests import test_spanish_model_variant460 as instructions
from tools.project.tests.test_french_model_variant435 import ROOT
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant431Tests(french.FrenchModelVariant431Tests):
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

    _sheet_images = legal_images

    def test_original_context_reaches_all_four_helpers(self):
        expected_calls = {0xD34: 0x1338, 0xD60: 0x2444, 0xD94: 0x17C8, 0xDB4: 0xF1C}
        for module, data in self.legal_images():
            base = int(module["load_address"], 0)
            word = lambda pc: struct.unpack_from("<I", data, pc)[0]
            self.assertEqual(word(0xC), 0xAFA400F0)
            self.assertEqual(word(0x10), 0x0080B021)
            for pc in expected_calls:
                self.assertEqual(word(pc - 4), 0x8FA400F0)
                self.assertEqual(word(pc + 4), 0)
                self.assertEqual(word(pc), 0x0C000000 | ((base + expected_calls[pc]) >> 2 & 0x3FFFFFF))
            stores = self.direct_stores(data, 4, 0xF1C, 29)
            self.assertEqual([(pc, off, size) for pc, off, size in stores if off + size > 240],
                             [(0xC, 240, 4), (0x80, 244, 4)])
            definitions = {r: set(self.register_writes(data, 4, 0xF1C, r)) for r in (4, 22)}
            pending, visited, reaching = [(4, None, None)], set(), {}
            while pending:
                pc, root, arg = pending.pop()
                self.assertTrue(4 <= pc < 0xF1C and pc % 4 == 0)
                if (pc, root, arg) in visited:
                    continue
                visited.add((pc, root, arg))
                if pc in definitions[22]:
                    root = pc
                if pc in definitions[4]:
                    arg = pc
                instruction = word(pc)
                op = instruction >> 26
                if op in (1, 2, 3, 4, 5, 6, 7) or instruction == 0x03E00008:
                    if pc + 4 in definitions[22]:
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
            self.assertEqual(reaching, {pc: {(0x10, pc - 4)} for pc in expected_calls})
            calls = [(pc, 0x80000000 | ((word(pc) & 0x3FFFFFF) << 2))
                     for pc in range(4, 0xF1C, 4) if word(pc) >> 26 == 3]
            symbols = ROOT / module["linker_symbols"]
            addresses = {int(value, 0) for value in re.findall(r"= (0x[0-9A-F]+);", symbols.read_text())}
            self.assertEqual(len(addresses), 36)
            self.assertEqual(len(calls), 69)
            self.assertEqual(len({target for _, target in calls if target in addresses}), 25)
            for _, target in calls:
                self.assertIn(target, addresses | {base + offset for offset in expected_calls.values()})
            self.assertEqual({pc: target - base for pc, target in calls if base <= target < base + 20480},
                             expected_calls)

    def test_complete_entry_register_lifetimes_and_pointer_homes(self):
        lifetimes = {
            16: [0x9C, 0xF0, 0x124, 0x190, 0x1E0, 0x1E4, 0x270, 0x304, 0x594, 0x704, 0x7C8, 0x950, 0x9A0, 0xA54, 0xAAC, 0xC38, 0xCE0, 0xDD4, 0xF10],
            17: [0x128, 0x168, 0x24C, 0x530, 0x5F4, 0x6D8, 0x73C, 0x94C, 0x998, 0xA50, 0xAA4, 0xC40, 0xCC8, 0xF0C],
            18: [0x130, 0x144, 0x1FC, 0x534, 0x5D0, 0x6CC, 0x794, 0x944, 0xA38, 0xA48, 0xB2C, 0xC34, 0xD0C, 0xF08],
            19: [0x12C, 0x138, 0x1B4, 0x3FC, 0x468, 0x4C4, 0x52C, 0x58C, 0x648, 0x6D0, 0x738, 0x93C, 0x9D0, 0xA40, 0xB10, 0xC58, 0xC78, 0xF04],
            20: [0x1CC, 0x4B4, 0x6C0, 0x6D4, 0x734, 0xC4C, 0xC70, 0xF00],
            21: [0x208, 0x520, 0xC68, 0xEFC],
            22: [0x10, 0xEF8],
            23: [0x3C, 0x2AC, 0x4AC, 0x660, 0x6C8, 0x75C, 0x7AC, 0x8F4, 0x948, 0x9A4, 0xA4C, 0xAB0, 0xC64, 0xEF4],
            29: [4, 0xF18],
            30: [0x98, 0xE0, 0x120, 0x148, 0x234, 0x478, 0x4EC, 0xC30, 0xC80, 0xEF0],
        }
        homes = {
            128: [0x18, 0x4A4], 132: [0x20, 0x934], 136: [0x28, 0x9F4],
            140: [0x30, 0xB18], 144: [0x40, 0x7A8], 148: [0x84, 0x674],
            152: [0x50], 156: [0x58], 160: [0x90],
        }
        for _, data in self.legal_images():
            for register, sites in lifetimes.items():
                self.assertEqual(self.register_writes(data, 4, 0xF1C, register), sites)
            for pc, word in {4: 0x27BDFF10, 0xF18: 0x27BD00F0, 0x80: 0xAFA500F4,
                             0xC24: 0x27A50070, 0xC28: 0x27A60074, 0xC2C: 0x27A70078,
                             0xC90: 0x27A5007C, 0xCB8: 0x27A60074, 0xCD4: 0x27A70078,
                             0xC64: 0x97B70070, 0xC68: 0x97B50072, 0xCEC: 0x97A2007C,
                             0xCF0: 0x87A3007E, 0xCFC: 0xA6421870, 0xD00: 0xA643187A}.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], word)
            stores = self.direct_stores(data, 4, 0xF1C, 29)
            for home, sites in homes.items():
                self.assertEqual([pc for pc, off, size in stores if off < home + 4 and home < off + size], sites)
            self.assertFalse(any(off < 80 and 16 < off + size for _, off, size in stores))

    def test_complete_helper_register_lifetimes(self):
        rows = (
            (0xF1C, 0x1338, (
                [0x10B0, 0x10D8, 0x1138, 0x122C, 0x132C], [0xF60, 0x1328],
                [0xFA0, 0x12E0, 0x1324], [0x114C, 0x1320], [0xF2C, 0x131C],
                [0xF24, 0x1304, 0x1318], [0x1134, 0x1248, 0x1314],
                [0x102C, 0x1040, 0x1310], [0xF1C, 0x1334], [0x1020, 0x103C, 0x130C])),
            (0x1338, 0x17C8, (
                [0x137C, 0x1768, 0x17BC], [0x1350, 0x17B8], [0x13FC, 0x16A4, 0x17B4],
                [0x1348, 0x1774, 0x17B0], [0x1340, 0x17AC], [0x13EC, 0x1698, 0x17A8],
                [0x1378, 0x17A4], [0x1380, 0x1760, 0x17A0], [0x1338, 0x17C4],
                [0x1374, 0x1764, 0x179C])),
            (0x2444, 0x28C4, (
                [0x2490, 0x286C, 0x28B8], [0x2484, 0x28B4], [0x2688, 0x27C4, 0x28B0],
                [0x244C, 0x28AC], [0x268C, 0x27D0, 0x28A8], [0x2488, 0x2868, 0x28A4],
                [0x2458, 0x2870, 0x28A0], [0x248C, 0x289C], [0x2444, 0x28C0],
                [0x2494, 0x2864, 0x2898])),
            (0x28C4, 0x2BCC, (
                [0x2914, 0x2B80, 0x2BC0], [0x28E0, 0x2BBC], [0x2978, 0x2B30, 0x2BB8],
                [0x296C, 0x2B24, 0x2BB4], [0x28CC, 0x2BB0], [0x2908, 0x2BAC],
                [0x28D8, 0x2B88, 0x2BA8], [0x2910, 0x2BA4], [0x28C4, 0x2BC8],
                [0x290C, 0x2BA0])),
            (0x2BCC, 0x2F48, (
                [0x2C18, 0x2F08, 0x2F3C], [0x2BE4, 0x2F38], [0x2BD4, 0x2F34],
                [0x2C40, 0x2E3C, 0x2F30], [0x2C34, 0x2E30, 0x2F2C], [0x2C0C, 0x2F28],
                [0x2C08, 0x2F04, 0x2F24], [0x2BDC, 0x2F14, 0x2F20],
                [0x2BCC, 0x2F44], [0x2C14, 0x2F1C])),
        )
        for _, data in self.legal_images():
            for start, end, writes in rows:
                self.assertEqual(len(writes), 10)
                for register, sites in zip((16, 17, 18, 19, 20, 21, 22, 23, 29, 30), writes):
                    self.assertEqual(self.register_writes(data, start, end, register), sites)

    def test_helper_packet_fields_and_projection_homes(self):
        line_fields = {(0, 4)} | {(offset, 1) for offset in range(12, 18)}
        quad_fields = {(offset + color, 1) for offset in (4, 12, 20, 28) for color in range(3)}
        textured_fields = {(offset + color, 1) for offset in (4, 16, 28, 40) for color in range(3)}
        rows = (
            (0xF1C, 0x1338, 296, 0xF60, 0x26B11730, line_fields, (0x1184,)),
            (0x1338, 0x17C8, 264, 0x1350, 0x26911610, quad_fields, (0x14CC, 0x15D0)),
            (0x2444, 0x28C4, 272, 0x2484, 0x26711668, textured_fields, (0x26C8,)),
            (0x28C4, 0x2BCC, 272, 0x28E0, 0x26911730, line_fields, (0x2A3C,)),
            (0x2BCC, 0x2F48, 264, 0x2BE4, 0x26511730, line_fields, (0x2D30,)),
        )
        for _, data in self.legal_images():
            word = lambda pc: struct.unpack_from("<I", data, pc)[0]
            for start, end, frame, packet_pc, packet_word, fields, projections in rows:
                self.assertEqual(word(start), 0x27BD0000 | (-frame & 65535))
                self.assertEqual(word(end - 4), 0x27BD0000 | frame)
                self.assertEqual(word(packet_pc), packet_word)
                self.assertEqual({(off, size) for _, off, size in self.direct_stores(data, start, end, 17)}, fields)
                for pc in projections:
                    self.assertEqual(word(pc), 0x27A200D0)
                    self.assertEqual(word(pc + 8), 0x27A200D4)

    def test_actual_descriptor_dispatch_and_sixth_sheet_cursor(self):
        if not (ROOT / "game/spain/DATA/MODEL.MRG").exists():
            self.skipTest("legal Spanish MODEL input required")
        with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as archive:
            archive.seek((351 * 276 + 275) * 2048 + 0x110)
            command, = struct.unpack("<i", archive.read(4))
        self.assertEqual(command, 597000)
        for _, data in self.legal_images():
            descriptor = 0x3044 + command % 1000 * 104
            self.assertEqual(hashlib.sha256(data[descriptor:descriptor + 104]).hexdigest(),
                             "7bb7ea728ab8ac81745e398ab1d777efd65011aa6f78fb25cfe878b9518aa2eb")
            self.assertEqual(list(data[descriptor + 4:descriptor + 9]), [22, 17, 9, 2, 14])
            self.assertEqual(struct.unpack_from("<15I", data, descriptor + 44),
                             (*range(10, 311, 30), 0, 0, 0, 0))
            for pc, expected in {
                0xD18: 0x8C430014, 0xD24: 0x0043102B, 0xD28: 0x14400004,
                0xD44: 0x8C43002C, 0xD50: 0x0043102B, 0xD54: 0x14400004,
                0xD68: 0x8EC218D8, 0xD70: 0x00021080, 0xD78: 0x8C63002C,
                0xD84: 0x0043102B, 0xD88: 0x14400004,
                0xDA4: 0x28420002, 0xDA8: 0x14400004,
                0xC70: 0x26940020, 0xCC8: 0x26310010, 0xD04: 0x2BC20005,
                0xEA0: 0x2C620040, 0xEB8: 0xAEC218AC, 0xEC8: 0xAEC218AC,
                0x2450: 0x266804E0, 0x2480: 0xAFA800D8, 0x24A0: 0x8FA900D8,
                0x24A8: 0x8D2200F4, 0x24F4: 0x16AA0017, 0x24FC: 0x86621764,
                0x2864: 0x27DE0020, 0x2868: 0x26B50001,
                0x286C: 0x26100098, 0x2870: 0x26D60098, 0x2874: 0x2AA20006,
                0x2884: 0x25290120, 0x2890: 0xAFA900D8,
            }.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], expected)
            stores = self.direct_stores(data, 0x2444, 0x28C4, 29)
            self.assertEqual([pc for pc, off, size in stores if off < 220 and 216 < off + size],
                             [0x2480, 0x2890])
        self.assertEqual(0x4E0 + 5 * 0x120, 0xA80)
        self.assertEqual(0x4E0 + 5 * 0x120 + 0xF4, 0xB74)
        self.assertEqual(0xA80 + 0x98 + 0x5C, 0xB74)
        self.assertEqual(0x13B0 + 5 * 116, 0x15F4)


if __name__ == "__main__":
    unittest.main()
