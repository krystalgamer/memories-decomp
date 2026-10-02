import hashlib
import re
import struct
import unittest

from tools.project.tests import test_spanish_model_variant402 as family402
from tools.project.tests import test_spanish_model_variant460 as instructions
from tools.project.tests.test_french_model_variant435 import ROOT


class SpanishModelVariant402EntryTests(unittest.TestCase):
    family = 402
    setUp = family402.SpanishModelVariant402Tests.setUp
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

    def test_original_context_reaches_only_band_dispatch(self):
        for module, data in self.legal_images():
            base = int(module["load_address"], 0)
            word = lambda pc: struct.unpack_from("<I", data, pc)[0]
            self.assertEqual(word(0x6A8), 0x08000000 | ((base + 0x944) >> 2 & 0x3FFFFFF))
            self.assertEqual(word(0x8E4), 0x02402021)
            definitions = set(self.register_writes(data, 4, 0xA5C, 18))
            pending, visited, reaching = [(4, None)], set(), set()
            while pending:
                pc, definition = pending.pop()
                self.assertTrue(4 <= pc < 0xA5C and pc % 4 == 0)
                if (pc, definition) in visited:
                    continue
                visited.add((pc, definition))
                if pc in definitions:
                    definition = pc
                instruction = word(pc)
                op = instruction >> 26
                if op in (1, 2, 3, 4, 5, 6, 7) or instruction == 0x03E00008:
                    if pc + 4 in definitions:
                        definition = pc + 4
                    if pc == 0x8E0:
                        reaching.add(definition)
                    if instruction == 0x03E00008:
                        continue
                    if op == 2:
                        targets = [(((base + pc + 4) & 0xF0000000) | ((instruction & 0x3FFFFFF) << 2)) - base]
                    elif op == 3:
                        targets = [pc + 8]
                    else:
                        displacement = (instruction & 65535) - (65536 if instruction & 32768 else 0)
                        targets = [pc + 8, pc + 4 + displacement * 4]
                    pending.extend((target, definition) for target in targets)
                else:
                    pending.append((pc + 4, definition))
            self.assertEqual(reaching, {0xC})
            calls = [(pc, 0x80000000 | ((word(pc) & 0x3FFFFFF) << 2))
                     for pc in range(4, 0xA5C, 4) if word(pc) >> 26 == 3]
            bindings = (ROOT / module["linker_symbols"]).read_text()
            addresses = {int(value, 0) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
            self.assertEqual(len(addresses), 35)
            self.assertEqual(len(calls), 57)
            for _, target in calls:
                self.assertIn(target, addresses | {base + 0x1B38})
            self.assertEqual([(pc, target - base) for pc, target in calls if base <= target < base + 20480],
                             [(0x8E0, 0x1B38)])
            for target, sites in ((0x8005C028, [0xAC, 0xC4, 0xDC]), (0x8005BF24, [0x920, 0x934]),
                                  (0x8008A428, [0x11C, 0x74C, 0x864]), (0x80087868, [0x7BC, 0x7F0])):
                self.assertEqual([pc for pc, destination in calls if destination == target], sites)

    def test_complete_register_lifetimes_and_incoming_command_home(self):
        lifetimes = (
            (16, [0x1A0, 0x1A4, 0x248, 0x2E8, 0x440, 0x59C, 0x5E8, 0x7AC, 0x7D0, 0x914, 0xA50]),
            (17, [0x38, 0x224, 0x2E0, 0x598, 0x5F0, 0x798, 0x7D4, 0xA4C]),
            (18, [0xC, 0x174, 0x594, 0x5EC, 0xA48]), (19, [0x18C, 0x590, 0x624, 0xA44]),
            (20, [0x1C0, 0x58C, 0x618, 0xA40]), (21, [0x1C, 0x580, 0xA3C]),
            (22, [0x30, 0x32C, 0x42C, 0x564, 0x584, 0x61C, 0xA38]),
            (23, [0x28, 0x284, 0xA34]), (30, [0x14, 0xA30]), (29, [4, 0xA58]),
        )
        for _, data in self.legal_images():
            for register, sites in lifetimes:
                self.assertEqual(self.register_writes(data, 4, 0xA5C, register), sites)
            for pc, value in ((4, 0x27BDFF38), (0xA58, 0x27BD00C8), (0x60, 0xAFA500CC),
                              (0xC, 0x00809021), (0x14, 0x0240F021)):
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], value)
            stores = self.direct_stores(data, 4, 0xA5C, 29)
            self.assertEqual([(pc, off, size) for pc, off, size in stores if off + size > 200],
                             [(0x60, 204, 4)])
            self.assertTrue(all(0 <= off and (off + size <= 200 or (pc, off, size) == (0x60, 204, 4))
                                for pc, off, size in stores))
            self.assertFalse(any(off < 128 and 112 < off + size for _, off, size in stores))
            for begin, sites in ((128, [0x68]), (132, [0x40, 0x638]), (136, [0x48]),
                                 (140, [0x6C]), (144, [0x74]), (204, [0x60])):
                self.assertEqual([pc for pc, off, size in stores if off < begin + 4 and begin < off + size], sites)

    def test_five_packet_footprints_and_band_aliases(self):
        for _, data in self.legal_images():
            for pc, value in ((0x28, 0x27D706A8), (0x30, 0x27D60778), (0x284, 0x26F70034),
                              (0x32C, 0x26D60028), (0x44, 0x27D80710), (0x4C, 0x27D90744),
                              (0x384, 0x8FA4008C), (0x390, 0x8FA4008C), (0x39C, 0x8FA4008C),
                              (0x3B4, 0x8FA40090), (0x3C4, 0x8FB90090)):
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], value)
            for register, regions, offsets, halves, size in (
                    (23, ((0x234, 0x284), (0x284, 0x2D4)),
                     (12, 13, 24, 25, 36, 37, 48, 49), (14, 26), 52),
                    (22, ((0x2D4, 0x32C), (0x32C, 0x384)),
                     (12, 13, 20, 21, 28, 29, 36, 37), (14, 22), 40)):
                for begin, end in regions:
                    fields = {(off, width) for _, off, width in self.direct_stores(data, begin, end, register)}
                    self.assertEqual(fields, {(off, 1) for off in offsets} | {(off, 2) for off in halves})
                    self.assertLessEqual(max(off + width for off, width in fields), size)
            self.assertEqual(self.register_writes(data, 0x3B4, 0x3FC, 4), [0x3B4])
            self.assertEqual(self.register_writes(data, 0x3C4, 0x3FC, 25), [0x3C4])
            fields = {(off, width) for reg in (4, 25)
                      for _, off, width in self.direct_stores(data, 0x3B4, 0x3FC, reg)}
            self.assertEqual(fields, {(off, 1) for off in (12, 13, 24, 25, 36, 37, 48, 49)} | {(14, 2), (26, 2)})
        for start, count, stride, end in ((0x58, 2, 144, 0x178), (0x438, 2, 280, 0x668),
                                          (0x6A8, 2, 52, 0x710), (0x778, 2, 40, 0x7C8)):
            self.assertEqual(start + count * stride, end)

    def test_actual_descriptor_signed_part_loop_and_distinct_phase_tail(self):
        anchors = {
            0x5AC: 0x00021102, 0x5BC: 0x00021102, 0x5D8: 0x00031982,
            0x5FC: 0x00031982, 0x600: 0x2402FF40, 0x614: 0xAE74FFFC, 0x618: 0x2694F800,
            0x7F8: 0x97A2007C, 0x7FC: 0xAFC00890, 0x800: 0x00501023,
            0x804: 0xA7C2084C, 0x808: 0x87A2007E, 0x80C: 0x8FC3087C,
            0x810: 0x00511023, 0x814: 0xA7C2084E, 0x818: 0x9462000C, 0x820: 0x10400027,
            0x8B4: 0x0062182A, 0x8B8: 0x1460FFDB, 0x8C8: 0x8C430010,
            0x8D4: 0x0043102B, 0x8D8: 0x14400008, 0x8F4: 0x24020001, 0x8F8: 0xAFC208AC,
            0x960: 0x28420003, 0x978: 0x2442FF80, 0x988: 0xAFC008BC,
            0x9A4: 0x14620005, 0x9AC: 0x24190004, 0x9B8: 0xAFC208AC,
            0x9C4: 0x24180001, 0x9D4: 0x24020008, 0x9E0: 0x28620040,
            0x9E4: 0x10400010, 0x9FC: 0x28420040, 0xA08: 0xAFC20878,
            0xA0C: 0x24020008, 0xA14: 0xAFC208AC, 0xA20: 0x24190002,
        }
        for module, data in self.legal_images():
            self.assertEqual(int(self.instances[module["name"]]["command_word"]), 568000)
            descriptor = data[0x215C:0x2170]
            self.assertEqual(hashlib.sha256(descriptor).hexdigest(),
                             "bb476a51ea96bff77d7f509074f461895e8172928e82b4fa068306c1b7b3bd3d")
            self.assertEqual(descriptor[4:7], bytes((8, 0, 0)))
            self.assertEqual(struct.unpack_from("<H", descriptor, 12)[0], 1)
            self.assertEqual(struct.unpack_from("<I", descriptor, 16)[0], 92)
            for pc, value in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], value)
