import re
import struct
import unittest

from tools.project.tests import test_spanish_model_variant338 as family338
from tools.project.tests import test_spanish_model_variant460 as lifetimes460


class SpanishModelVariant338EntryTests(unittest.TestCase):
    family = 338
    setUp = family338.SpanishModelVariant338Tests.setUp
    register_writes = staticmethod(lifetimes460.SpanishModelVariant460Tests.register_writes)
    direct_stores = staticmethod(lifetimes460.SpanishModelVariant460Tests.direct_stores)

    def legal_images(self):
        path = family338.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                yield module, archive.read(20480)

    def test_original_context_reaches_unconditional_ribbon_and_gated_helpers(self):
        calls = {0x9F4: 0xBA0, 0xA1C: 0x16D8, 0xA38: 0x1ED4}
        for module, data in self.legal_images():
            base = int(module["load_address"], 0)
            definitions = set(self.register_writes(data, 4, 0xBA0, 19))
            for pc, offset in calls.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0],
                                 0x0C000000 | ((base + offset) >> 2 & 0x3FFFFFF))
            self.assertEqual(struct.unpack_from("<I", data, 0x844)[0],
                             0x08000000 | ((base + 0xA88) >> 2 & 0x3FFFFFF))
            self.assertEqual(struct.unpack_from("<I", data, 0x9DC)[0], 0x02602021)
            self.assertEqual(self.register_writes(data, 0x9DC, 0x9FC, 4), [0x9DC])
            for pc, expected in ((0xA20, 0x02602021), (0xA3C, 0x02602021),
                                 (0xA04, 0x8C430020), (0xA08, 0x8EC21F2C),
                                 (0xA10, 0x0043102B), (0xA14, 0x14400003),
                                 (0xA2C, 0x28420002)):
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], expected)
            pending, visited, reaching = [(4, None)], set(), {}
            while pending:
                pc, definition = pending.pop()
                self.assertTrue(4 <= pc < 0xBA0 and pc % 4 == 0)
                if (pc, definition) in visited:
                    continue
                visited.add((pc, definition))
                word, = struct.unpack_from("<I", data, pc)
                op = word >> 26
                if pc in definitions:
                    definition = pc
                if op in (1, 2, 3, 4, 5, 6, 7) or word == 0x03E00008:
                    if pc + 4 in definitions:
                        definition = pc + 4
                    if pc in calls:
                        reaching.setdefault(pc, set()).add(definition)
                    if word == 0x03E00008:
                        continue
                    if op == 2:
                        targets = [(((base + pc + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)) - base]
                    elif op == 3:
                        targets = [pc + 8]
                    else:
                        displacement = (word & 65535) - (65536 if word & 32768 else 0)
                        targets = [pc + 8, pc + 4 + displacement * 4]
                    pending.extend((target, definition) for target in targets)
                else:
                    pending.append((pc + 4, definition))
            self.assertEqual(reaching, {pc: {0xC} for pc in calls})

    def test_register_lifetimes_frame_and_argument_home(self):
        lifetimes = (
            (22, [0x14, 0xB7C]), (19, [0xC, 0x14C, 0x5B0, 0x7A4, 0x7D4, 0xB88]),
            (18, [0x1C, 0x784, 0xB8C]), (20, [0x38, 0x454, 0xB84]),
            (21, [0x30, 0x390, 0xB80]), (23, [0x158, 0x5D8, 0xB78]),
            (30, [0x28, 0x2D8, 0xB74]), (29, [4, 0xB9C]),
        )
        for _, data in self.legal_images():
            for register, expected in lifetimes:
                self.assertEqual(self.register_writes(data, 4, 0xBA0, register), expected)
            stores = self.direct_stores(data, 4, 0xBA0, 29)
            self.assertEqual([(pc, off, size) for pc, off, size in stores if off + size > 208],
                             [(0x5C, 212, 4)])
            self.assertTrue(all(0 <= off and (off + size <= 208 or (pc, off, size) == (0x5C, 212, 4))
                                for pc, off, size in stores))
            self.assertFalse(any(off < 128 and 112 < off + size for _, off, size in stores))
            for begin, sites in ((128, [0x48, 0x7EC]), (132, [0x64]), (136, [0x6C])):
                self.assertEqual([pc for pc, off, size in stores if off < begin + 4 and begin < off + size], sites)

    def test_six_packet_footprints_projection_and_phase_anchors(self):
        anchors = {
            4: 0x27BDFF30, 0xB9C: 0x27BD00D0, 0x5C: 0xAFA500D4,
            0x84: 0x8FB900D4, 0x840: 0x97B800D4,
            0x28: 0x26DE1DAC, 0x2D8: 0x27DE0034, 0x390: 0x26B50028,
            0x454: 0x26940028, 0x7C8: 0x28820011, 0x7DC: 0x2A620002,
            0x7E4: 0x27390378, 0x7EC: 0xAFB90080,
            0x974: 0x27A40050, 0x978: 0x27A50070, 0x97C: 0x27B10074,
            0x984: 0x02203021, 0x990: 0x27B00078, 0x99C: 0x02003821,
            0x9A0: 0x0C021E1A, 0x9A8: 0x02203021, 0x9AC: 0x02003821,
            0x9B4: 0x97B00070, 0x9B8: 0x87B10072, 0x9C4: 0x27A40058,
            0x9D0: 0x27A5007C, 0x9D4: 0x0C021E1A, 0x9F0: 0xA6C21F0C,
            0xA50: 0xAEC21F28, 0xA5C: 0x1203000A, 0xA7C: 0xAEC31F2C,
            0xA80: 0xAEC21F34, 0xA84: 0xAED01F30, 0xAA4: 0x28420006,
            0xAB8: 0x18400009, 0xABC: 0x2442FF80, 0xAC4: 0xAEC21F74,
            0xACC: 0xAEC01F74, 0xAD8: 0x00021140, 0xADC: 0xAEC21F74,
            0xAE8: 0x2462FFFE, 0xAEC: 0x2C420003, 0xAF4: 0x24190004,
            0xB04: 0x24180001, 0xB10: 0xAEC21F68, 0xB24: 0x28620040,
            0xB28: 0x10400010, 0xB3C: 0xAEC21F38, 0xB40: 0x28420040,
            0xB48: 0x24020040, 0xB4C: 0xAEC21F38, 0xB50: 0x24020007,
            0xB58: 0xAEC21F68, 0xB64: 0x24190002,
        }
        for _, data in self.legal_images():
            for pc, expected in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], expected)
            for register, regions, offsets, halves, size in (
                    (30, ((0x280, 0x2D8), (0x2D8, 0x334)),
                     (12, 13, 24, 25, 36, 37, 48, 49), (14, 26), 52),
                    (21, ((0x334, 0x390), (0x390, 0x3EC)),
                     (12, 13, 20, 21, 28, 29, 36, 37), (14, 22), 40),
                    (20, ((0x3EC, 0x454), (0x454, 0x4B4)),
                     (12, 13, 20, 21, 28, 29, 36, 37), (14, 22), 40)):
                for begin, end in regions:
                    fields = {(off, width) for _, off, width in self.direct_stores(data, begin, end, register)}
                    self.assertEqual(fields, {(off, 1) for off in offsets} | {(off, 2) for off in halves})
                    self.assertLessEqual(max(off + width for off, width in fields), size)

    def test_descriptor_vertex_mode_and_actual_owned_calls(self):
        expected = {34: (0, 22, 196, 1, 40), 164: (6, 10, 0, 0, 30),
                    165: (4, 14, 0, 0, 0), 210: (5, 2, 0, 0, 20),
                    424: (7, 5, 0, 0, 40), 443: (1, 20, 0, 0, 50),
                    459: (2, 12, 0, 0, 20), 609: (5, 2, 0, 0, 20)}
        for module, data in self.legal_images():
            row = self.instances[module["name"]]
            command = int(row["command_word"])
            self.assertEqual(command // 1000, 504)
            offset = 0x2808 + command % 1000 * 52
            self.assertTrue(0x270C <= offset and offset + 52 <= len(data))
            self.assertEqual((command % 1000, data[offset + 12],
                              struct.unpack_from("<H", data, offset + 18)[0],
                              *struct.unpack_from("<iI", data, offset + 28)), expected[int(row["model"])])
            for pc, word in (
                    (0xF0, 0x8CC3001C), (0xF8, 0x1462000D), (0x100, 0x86C41F78),
                    (0x104, 0x90C5000C), (0x108, 0x94C60012), (0x110, 0x26C71EE4),
                    (0x114, 0x8EC21EE4), (0x118, 0x8EC31EE8), (0x11C, 0x8EC41EEC),
                    (0x120, 0xAEC21ED8), (0x124, 0xAEC31EDC), (0x12C, 0xAEC41EE0),
                    (0x138, 0x26C51EC4), (0x8EC, 0x8CC3001C), (0x8F4, 0x1462000D),
                    (0x900, 0x90C5000C), (0x904, 0x94C60012), (0x90C, 0x26C71EE4),
                    (0x91C, 0xAEC21ED8), (0x920, 0xAEC31EDC), (0x928, 0xAEC41EE0),
                    (0x934, 0x26C51EC4)):
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], word)
            bindings = (family338.ROOT / module["linker_symbols"]).read_text()
            addresses = {int(value, 0) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
            self.assertEqual(len(addresses), 35)
            base = int(module["load_address"], 0)
            calls = [(pc, 0x80000000 | ((struct.unpack_from("<I", data, pc)[0] & 0x3FFFFFF) << 2))
                     for pc in range(4, 0xBA0, 4) if struct.unpack_from("<I", data, pc)[0] >> 26 == 3]
            self.assertEqual(len(calls), 57)
            for _, target in calls:
                self.assertIn(target, addresses | {base + off for off in (0xBA0, 0x16D8, 0x1ED4)})
            for target, sites in ((0x8005BF24, [0xA64, 0xA78]), (0x8005C4D8, [0x10C, 0x908]),
                                  (0x8005CB58, [0x1E0, 0x1F4, 0x204, 0x214, 0x224, 0x234])):
                self.assertEqual([pc for pc, destination in calls if destination == target], sites)
