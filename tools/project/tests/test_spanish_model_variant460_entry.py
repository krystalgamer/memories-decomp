import re
import struct
import unittest

from tools.project.tests import test_spanish_model_variant460 as family460
from tools.project.tests.test_french_model_variant435 import ROOT


class SpanishModelVariant460EntryTests(unittest.TestCase):
    family = 460
    config_name = "sles_03951"
    module_prefix = "spanish"
    setUp = family460.SpanishModelVariant460Tests.setUp
    legal_images = family460.SpanishModelVariant460Tests.legal_images
    register_writes = staticmethod(family460.SpanishModelVariant460Tests.register_writes)
    direct_stores = staticmethod(family460.SpanishModelVariant460Tests.direct_stores)

    def test_readonly_original_pointer_home_reaches_only_two_helpers(self):
        calls = {0xB54: 0xCFC, 0xB94: 0x17EC}
        for module, data in self.legal_images():
            base = int(module["load_address"], 0)
            word = lambda pc: struct.unpack_from("<I", data, pc)[0]
            for pc, offset in calls.items():
                self.assertEqual(word(pc), 0x0C000000 | ((base + offset) >> 2 & 0x3FFFFFF))
            self.assertEqual(word(0x840), 0x08000000 | ((base + 0xBE4) >> 2 & 0x3FFFFFF))
            for pc, expected in ((0xC, 0xAFA400D0), (0x10, 0x0080B021),
                                 (0xB50, 0x8FA400D0), (0xB90, 0x8FA400D0),
                                 (0xB58, 0), (0xB98, 0)):
                self.assertEqual(word(pc), expected)
            stores = self.direct_stores(data, 4, 0xCFC, 29)
            self.assertEqual([pc for pc, off, size in stores if off < 212 and 208 < off + size], [0xC])
            definitions = {r: set(self.register_writes(data, 4, 0xCFC, r)) for r in (4, 22)}
            self.assertFalse(any(pc < 0xC for pc in definitions[4]))
            pending, visited, reaching = [(4, None, None)], set(), {}
            while pending:
                pc, arg, root = pending.pop()
                self.assertTrue(4 <= pc < 0xCFC and pc % 4 == 0)
                if (pc, arg, root) in visited:
                    continue
                visited.add((pc, arg, root))
                if pc in definitions[4]:
                    arg = pc
                if pc in definitions[22]:
                    root = pc
                instruction = word(pc)
                op = instruction >> 26
                if op in (1, 2, 3, 4, 5, 6, 7) or instruction == 0x03E00008:
                    if pc + 4 in definitions[4]:
                        arg = pc + 4
                    if pc + 4 in definitions[22]:
                        root = pc + 4
                    if pc in calls:
                        reaching.setdefault(pc, set()).add((arg, root))
                    if instruction == 0x03E00008:
                        continue
                    if op == 2:
                        targets = [(((base + pc + 4) & 0xF0000000) | ((instruction & 0x3FFFFFF) << 2)) - base]
                    elif op == 3:
                        targets, arg = [pc + 8], None
                    else:
                        displacement = (instruction & 65535) - (65536 if instruction & 32768 else 0)
                        targets = [pc + 8, pc + 4 + displacement * 4]
                    pending.extend((target, arg, root) for target in targets)
                else:
                    pending.append((pc + 4, arg, root))
            self.assertEqual(reaching, {0xB54: {(0xB50, 0x10)}, 0xB94: {(0xB90, 0x10)}})
            local = {pc: (word(pc) & 0x3FFFFFF) * 4 + 0x80000000 - base
                     for pc in range(4, 0xCFC, 4) if word(pc) >> 26 == 3
                     and base <= (word(pc) & 0x3FFFFFF) * 4 + 0x80000000 < base + 20480}
            self.assertEqual(local, calls)

    def test_complete_register_lifetimes_frame_and_projection_spills(self):
        lifetimes = (
            (16, [0x80, 0xC4, 0x110, 0x1BC, 0x20C, 0x210, 0x2C0, 0x374, 0x59C, 0x740,
                  0x988, 0xA68, 0xA8C, 0xBB4, 0xCF0]),
            (17, [0xFC, 0x1A0, 0x2A0, 0x8FC, 0xA48, 0xA54, 0xA90, 0xCEC]),
            (18, [0x7C, 0xB4, 0xF8, 0x170, 0x23C, 0x5B4, 0x6D4, 0x8E4, 0x9F4, 0xCE8]),
            (19, [0x10C, 0x17C, 0x1E0, 0x4E0, 0x52C, 0x550, 0x8E8, 0x9FC, 0xCE4]),
            (20, [0x100, 0x160, 0x288, 0x570, 0x77C, 0x7D0, 0x8F0, 0xA2C, 0xCE0]),
            (21, [0x28, 0x3B0, 0x8EC, 0xA08, 0xCDC]), (22, [0x10, 0xCD8]),
            (23, [0x30, 0x410, 0x598, 0x900, 0x9F0, 0xCD4]),
            (30, [0x20, 0x304, 0xCD0]), (29, [4, 0xCF8]),
        )
        for _, data in self.legal_images():
            for register, expected in lifetimes:
                self.assertEqual(self.register_writes(data, 4, 0xCFC, register), expected)
            stores = self.direct_stores(data, 4, 0xCFC, 29)
            self.assertEqual([(pc, off, size) for pc, off, size in stores if off + size > 208],
                             [(0xC, 208, 4), (0x64, 212, 4)])
            self.assertTrue(all(0 <= off and off + size <= 216 for _, off, size in stores))
            self.assertFalse(any(off < 128 and 112 < off + size for _, off, size in stores))
            for begin, sites in ((128, [0x34, 0x758]), (132, [0x3C, 0x7E8]), (136, [0x44]),
                                 (140, [0x6C]), (144, [0x74]), (212, [0x64])):
                self.assertEqual([pc for pc, off, size in stores if off < begin + 4 and begin < off + size], sites)

    def test_packet_footprints_and_eight_part_update_loops(self):
        anchors = {
            4: 0x27BDFF30, 0xCF8: 0x27BD00D0, 0x64: 0xAFA500D4,
            0x20: 0x26DE2B18, 0x28: 0x26D52BB4, 0x48: 0x26D82B80,
            0x304: 0x27DE0034, 0x3B0: 0x26B50028,
            0x408: 0x8FA40090, 0x414: 0x8FA40090, 0x420: 0x8FA40090,
            0x7C: 0x00009021, 0x80: 0x02C08021, 0xAC: 0x90450010,
            0xB4: 0x26520001, 0xB8: 0xAE022E84, 0xBC: 0x2A420008, 0xC4: 0x26100004,
            0x100: 0x24142C3C, 0x160: 0x26940020, 0x170: 0x26520001,
            0x17C: 0x26730004, 0x1A0: 0x26310020, 0x1B4: 0x2A420008, 0x1BC: 0x26100010,
            0x590: 0x2A820008, 0x598: 0x26F702E8, 0x754: 0x2739009C, 0x770: 0x2A020010,
            0x7C4: 0x28820011, 0x7D8: 0x2A820002, 0x7E0: 0x27180378,
            0x948: 0x8C430024, 0x94C: 0x24020001, 0x950: 0x1462001A, 0x958: 0x12400018,
            0x964: 0x02752021, 0x96C: 0x000211C0, 0x974: 0x00021303,
            0x98C: 0x000211C0, 0x994: 0x00021303, 0x9A4: 0x000211C0, 0x9AC: 0x00021303,
            0x9E0: 0x00122940, 0x9E4: 0x24A52C3C, 0x9F0: 0x26F70004, 0x9F4: 0x26520001,
            0x9FC: 0x26730320, 0xA08: 0x26B50514, 0xA2C: 0x26940020, 0xA40: 0x2A420008,
            0xA48: 0x26310010, 0xA50: 0x27A50070, 0xA54: 0x27B10074,
            0xA68: 0x27B00078, 0xAA8: 0x27A5007C,
        }
        for _, data in self.legal_images():
            for pc, value in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], value)
            for register, regions, offsets, halves, size in (
                    (30, ((0x2B0, 0x304), (0x304, 0x358)),
                     (12, 13, 24, 25, 36, 37, 48, 49), (14, 26), 52),
                    (21, ((0x358, 0x3B0), (0x3B0, 0x408)),
                     (12, 13, 20, 21, 28, 29, 36, 37), (14, 22), 40)):
                for begin, end in regions:
                    fields = {(off, width) for _, off, width in self.direct_stores(data, begin, end, register)}
                    self.assertEqual(fields, {(off, 1) for off in offsets} | {(off, 2) for off in halves})
                    self.assertLessEqual(max(off + width for off, width in fields), size)
        for start, count, stride, end in ((0, 8, 744, 0x1740), (0x1740, 16, 156, 0x2100),
                                          (0x23E8, 2, 888, 0x2AD8), (0x2B18, 2, 52, 0x2B80),
                                          (0x2BB4, 2, 40, 0x2C04), (0x2C3C, 8, 32, 0x2D3C),
                                          (0x2D3C, 8, 16, 0x2DBC), (0x2DBC, 8, 16, 0x2E3C)):
            self.assertEqual(start + count * stride, end)

    def test_actual_descriptors_phase_thresholds_and_owned_calls(self):
        expected = {
            70: (4, 8, 1, 1, 20, 110, 126, 220, 260), 125: (2, 5, 1, 0, 20, 76, 126, 240, 280),
            168: (12, 8, 1, 1, 40, 196, 200, 300, 360), 460: (3, 4, 1, 1, 20, 76, 126, 220, 260),
            469: (8, 8, 1, 2, 20, 110, 296, 320, 400), 704: (9, 8, 1, 1, 20, 220, 296, 400, 540),
            44: (1, 5, 1, 0, 20, 66, 126, 140, 190), 98: (13, 4, 1, 2, 30, 60, 100, 120, 130),
            161: (7, 5, 1, 1, 60, 290, 296, 300, 310), 370: (5, 6, 1, 1, 0, 68, 126, 140, 180),
            400: (6, 4, 1, 0, 180, 190, 204, 260, 300), 458: (11, 8, 1, 0, 0, 48, 96, 300, 340),
            462: (10, 8, 1, 0, 48, 80, 96, 192, 250), 558: (1, 5, 1, 0, 20, 66, 126, 140, 190),
        }
        anchors = {
            0xAD4: 0x8C620038, 0xAE0: 0x0043102B, 0xAE4: 0x10400006, 0xAE8: 0x24020002,
            0xAF4: 0x14620002, 0xAF8: 0x24020003, 0xAFC: 0xAEC22EC4,
            0xB08: 0x8C42003C, 0xB10: 0x0043102B, 0xB24: 0x14620002, 0xB2C: 0xAEC22EC4,
            0xB38: 0x8C430030, 0xB44: 0x0043102B, 0xB48: 0x14400004,
            0xB64: 0x8C43002C, 0xB70: 0x0043102B, 0xB74: 0x14400009,
            0xB84: 0x28420006, 0xB88: 0x10400004, 0xBAC: 0xAEC22E68,
            0xBB8: 0x1203000A, 0xBD8: 0xAEC32E6C, 0xBDC: 0xAEC22E74,
            0xC00: 0x28420006, 0xC14: 0x18400009, 0xC18: 0x2442FF80,
            0xC20: 0xAEC22ED0, 0xC28: 0xAEC02ED0, 0xC34: 0x00021140,
            0xC44: 0x2462FFFE, 0xC48: 0x2C420003, 0xC50: 0x24180004,
            0xC60: 0x24190001, 0xC6C: 0xAEC22EC4, 0xC80: 0x28620040,
            0xC84: 0x10400010, 0xC98: 0xAEC22E78, 0xC9C: 0x28420040,
            0xCA4: 0x24020040, 0xCA8: 0xAEC22E78, 0xCB4: 0xAEC22EC4, 0xCC0: 0x24180002,
        }
        for module, data in self.legal_images():
            row = self.instances[module["name"]]
            command = int(row["command_word"])
            self.assertEqual(command // 1000, 626)
            descriptor = 0x29C8 + command % 1000 * 64
            self.assertTrue(0x28CC <= descriptor and descriptor + 64 <= len(data))
            self.assertEqual((command % 1000, *struct.unpack_from("<3i5I", data, descriptor + 32)),
                             expected[int(row["model"])])
            for pc, value in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], value)
            base = int(module["load_address"], 0)
            bindings = (ROOT / module["linker_symbols"]).read_text()
            addresses = {int(value, 0) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
            self.assertEqual(len(addresses), 34)
            calls = [(pc, 0x80000000 | ((struct.unpack_from("<I", data, pc)[0] & 0x3FFFFFF) << 2))
                     for pc in range(4, 0xCFC, 4) if struct.unpack_from("<I", data, pc)[0] >> 26 == 3]
            self.assertEqual(len(calls), 55)
            for _, target in calls:
                self.assertIn(target, addresses | {base + 0xCFC, base + 0x17EC})
            for target, sites in ((0x8005BF24, [0xBC0, 0xBD4]),
                                  (0x8008A428, [0x108, 0x16C, 0x8F8, 0x9F8]),
                                  (0x80087868, [0xA78, 0xAAC]), (0x80082EE8, [0x2B0, 0x308, 0x40C])):
                self.assertEqual([pc for pc, destination in calls if destination == target], sites)
