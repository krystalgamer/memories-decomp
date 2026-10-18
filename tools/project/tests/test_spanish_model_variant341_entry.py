import hashlib
import re
import struct
import unittest

from tools.project.tests import test_french_model_variant341 as french
from tools.project.tests import test_spanish_model_variant341 as family341
from tools.project.tests import test_spanish_model_variant460 as instructions
from tools.project.tests.test_french_model_variant435 import ROOT


class SpanishModelVariant341EntryTests(unittest.TestCase):
    family = 341
    config_name = "sles_03951"
    module_prefix = "spanish"
    setUp = family341.SpanishModelVariant341Tests.setUp
    register_writes = staticmethod(instructions.SpanishModelVariant460Tests.register_writes)
    direct_stores = staticmethod(instructions.SpanishModelVariant460Tests.direct_stores)
    test_entry_views_and_complete_slot_wrapper = french.FrenchModelVariant341EntryTests.test_entry_views_and_complete_slot_wrapper

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

    def test_original_context_reaches_all_four_helpers(self):
        expected_calls = {0xD54: 0x1354, 0xD84: 0x22F8, 0xD8C: 0x17C0, 0xDD0: 0xF38}
        for module, data in self.legal_images():
            base = int(module["load_address"], 0)
            word = lambda pc: struct.unpack_from("<I", data, pc)[0]
            self.assertEqual(word(0xADC), 0x08000000 | ((base + 0xE20) >> 2 & 0x3FFFFFF))
            for pc in (0xD14, 0xD24, 0xD88, 0xD90, 0xDD4):
                self.assertEqual(word(pc), 0x02402021)
            self.assertEqual(word(0xC), 0x00809021)
            definitions = {r: set(self.register_writes(data, 4, 0xF38, r)) for r in (4, 18)}
            pending, visited, reaching = [(4, None, None)], set(), {}
            while pending:
                pc, root, arg = pending.pop()
                self.assertTrue(4 <= pc < 0xF38 and pc % 4 == 0)
                if (pc, root, arg) in visited:
                    continue
                visited.add((pc, root, arg))
                if pc in definitions[18]:
                    root = pc
                if pc in definitions[4]:
                    arg = pc
                instruction = word(pc)
                op = instruction >> 26
                if op in (1, 2, 3, 4, 5, 6, 7) or instruction == 0x03E00008:
                    if pc + 4 in definitions[18]:
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
            self.assertEqual(reaching, {0xD54: {(0xC, 0xD14), (0xC, 0xD24)}, 0xD84: {(0xC, 0xD88)},
                                       0xD8C: {(0xC, 0xD90)}, 0xDD0: {(0xC, 0xDD4)}})
            calls = [(pc, 0x80000000 | ((word(pc) & 0x3FFFFFF) << 2))
                     for pc in range(4, 0xF38, 4) if word(pc) >> 26 == 3]
            addresses = {int(value, 0) for value in re.findall(
                r"= (0x[0-9A-F]+);", (ROOT / module["linker_symbols"]).read_text())}
            self.assertEqual(len(addresses), 36)
            self.assertEqual(len(calls), 75)
            self.assertEqual(len({target for _, target in calls if target in addresses}), 25)
            for _, target in calls:
                self.assertIn(target, addresses | {base + offset for offset in expected_calls.values()})
            self.assertEqual({pc: target - base for pc, target in calls if base <= target < base + 20480},
                             expected_calls)

    def test_complete_register_lifetimes_and_advancing_stack_homes(self):
        lifetimes = (
            (16, [0x1B4, 0x1B8, 0x250, 0x2F4, 0x510, 0x680, 0x73C, 0x8C0, 0x910, 0x9C4, 0xA1C, 0xBE0, 0xC04, 0xDF0, 0xF2C]),
            (17, [0x50, 0x22C, 0x4AC, 0x570, 0x654, 0x6B8, 0x8BC, 0x908, 0x9C0, 0xA14, 0xBCC, 0xC08, 0xF28]),
            (18, [0xC, 0x1D0, 0x4B0, 0x54C, 0x648, 0x710, 0x8B4, 0x9A8, 0x9B8, 0xA9C, 0xF24]),
            (19, [0x188, 0x3C8, 0x3F0, 0x440, 0x4A8, 0x508, 0x5C4, 0x64C, 0x6B4, 0x8AC, 0x940, 0x9B0, 0xA80, 0xF20]),
            (20, [0x1A0, 0x430, 0x63C, 0x650, 0x6B0, 0xF1C]), (21, [0x1DC, 0x49C, 0xF18]),
            (22, [0x14, 0xF14]),
            (23, [0x40, 0x28C, 0x428, 0x5DC, 0x644, 0x6D8, 0x720, 0x868, 0x8B8, 0x914, 0x9BC, 0xA20, 0xF10]),
            (30, [0x48, 0x330, 0x468, 0xF0C]), (29, [4, 0xF34]),
        )
        for _, data in self.legal_images():
            for register, sites in lifetimes:
                self.assertEqual(self.register_writes(data, 4, 0xF38, register), sites)
            for pc, value in ((4, 0x27BDFF18), (0xF34, 0x27BD00E8), (0x78, 0xAFA500EC), (0x14, 0x0240B021)):
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], value)
            stores = self.direct_stores(data, 4, 0xF38, 29)
            self.assertEqual([(pc, off, size) for pc, off, size in stores if off + size > 232], [(0x78, 236, 4)])
            self.assertTrue(all(0 <= off and (off + size <= 232 or (pc, off, size) == (0x78, 236, 4))
                                for pc, off, size in stores))
            for low, high in ((16, 80), (112, 128)):
                self.assertFalse(any(off < high and low < off + size for _, off, size in stores))
            for begin, sites in ((128, [0x1C, 0x420]), (132, [0x24, 0x8A4]), (136, [0x2C, 0x964]),
                                 (140, [0x34, 0xA88]), (144, [0x54, 0x71C]), (148, [0x7C, 0x5F0]),
                                 (152, [0x5C]), (156, [0x88]), (236, [0x78])):
                self.assertEqual([pc for pc, off, size in stores if off < begin + 4 and begin < off + size], sites)

    def test_four_packet_footprints_and_selected_descriptor(self):
        for module, data in self.legal_images():
            for pc, value in ((0x40, 0x26D70DB4), (0x48, 0x26DE0E50), (0x28C, 0x26F70034),
                              (0x330, 0x27DE0028), (0x60, 0x26D80E1C), (0x88, 0xAFB8009C),
                              (0x384, 0x8FA4009C), (0x390, 0x8FA4009C), (0x39C, 0x8FA4009C)):
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], value)
            for register, regions, offsets, halves in (
                    (23, ((0x23C, 0x28C), (0x28C, 0x2DC)),
                     (12, 13, 24, 25, 36, 37, 48, 49), (14, 26)),
                    (30, ((0x2DC, 0x330), (0x330, 0x384)),
                     (12, 13, 20, 21, 28, 29, 36, 37), (14, 22))):
                for begin, end in regions:
                    self.assertEqual({(off, width) for _, off, width in self.direct_stores(data, begin, end, register)},
                                     {(off, 1) for off in offsets} | {(off, 2) for off in halves})
            self.assertEqual(int(self.instances[module["name"]]["command_word"]), 507000)
            descriptor = data[0x2E54:0x2E68]
            self.assertEqual(hashlib.sha256(descriptor).hexdigest(),
                             "c79e90c4b3bb4220a86cc7d8b84ad0d41a764249e9822c7039b434022f21cd41")
            self.assertEqual(descriptor[4:7], bytes((6, 10, 4)))
            self.assertEqual(struct.unpack_from("<H", descriptor, 12)[0], 2)
            self.assertEqual(struct.unpack_from("<I", descriptor, 16)[0], 50)

    def test_signed_substep_gates_and_unsigned_fade_reset(self):
        anchors = {
            0xC2C: 0x97A2007C, 0xC30: 0xA6C00F40, 0xC34: 0x00501023, 0xC38: 0xA6C20EFC,
            0xC3C: 0x87A2007E, 0xC40: 0x8EC30F2C, 0xC44: 0x00511023, 0xC48: 0xA6C20EFE,
            0xC4C: 0x9462000C, 0xC54: 0x10400059, 0xC5C: 0x86C30F40,
            0xC6C: 0x8EC40F34, 0xCB4: 0x8EC40F34, 0xD18: 0x8EC40F3C,
            0xC90: 0x24420020, 0xC98: 0x2463FFA6, 0xCA8: 0x2442FFE0,
            0xCD8: 0x2442FFE0, 0xCE0: 0x2463FFA6, 0xCEC: 0x24420020, 0xCF0: 0x2463005A,
            0xD04: 0x2442FFFA, 0xD64: 0x1840000B, 0xD68: 0x28420002,
            0xD74: 0x86C20F40, 0xD7C: 0x14400003, 0xD94: 0x96C20F40,
            0xD9C: 0x24420001, 0xDA0: 0xA6C20F40, 0xDA4: 0x00021400,
            0xDA8: 0x9463000C, 0xDAC: 0x00021403, 0xDB0: 0x0043102A, 0xDB4: 0x1440FFA9,
            0xDC4: 0x28420002, 0xDC8: 0x14400003, 0xDFC: 0x0C016FC9, 0xE10: 0x0C016FC9,
            0xE3C: 0x28420007, 0xE54: 0x2442FF80, 0xE64: 0xAEC00F5C,
            0xE80: 0x2462FFFE, 0xE84: 0x2C420003, 0xE8C: 0x24180004,
            0xE90: 0x24020006, 0xE98: 0x24020007, 0xE9C: 0x24180001,
            0xEA8: 0xAEC20F50, 0xEBC: 0x2C620040, 0xEC0: 0x10400010,
            0xED0: 0x00021042, 0xEDC: 0x2C420040, 0xEE8: 0xAEC00F28,
            0xEF0: 0xAEC20F50, 0xEFC: 0x24180002,
        }
        for _, data in self.legal_images():
            for pc, value in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], value)
