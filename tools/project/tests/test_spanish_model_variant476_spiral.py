import hashlib
import struct
import unittest

from tools.project.tests import test_spanish_model_variant476 as family476
from tools.project.tests.test_french_model_variant435 import ROOT


class SpanishModelVariant476SpiralTests(unittest.TestCase):
    family = 476
    config_name = "sles_03951"
    module_prefix = "spanish"
    setUp = family476.SpanishModelVariant476Tests.setUp
    register_writes = staticmethod(family476.SpanishModelVariant476Tests.register_writes)
    direct_stores = staticmethod(family476.SpanishModelVariant476Tests.direct_stores)
    test_original_context_reaches_all_five_entry_calls = (
        family476.SpanishModelVariant476Tests.test_original_context_reaches_all_five_entry_calls)

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

    def test_complete_register_writes_and_spill_separation(self):
        lifetimes = (
            (16, [0x183C, 0x185C, 0x1860, 0x1874, 0x18D8, 0x18DC, 0x18E0, 0x1984,
                  0x19AC, 0x1A14, 0x1B90, 0x1BE4, 0x1CD0, 0x20B0]),
            (17, [0x189C, 0x18CC, 0x18D0, 0x1AA0, 0x1BCC, 0x1C44, 0x1C48, 0x1CC8, 0x20AC]),
            (18, [0x1804, 0x1900, 0x1A7C, 0x1CD4, 0x20A8]),
            (19, [0x1758, 0x20A4]),
            (20, [0x171C, 0x193C, 0x19B4, 0x1CBC, 0x1CC0, 0x1FB0, 0x20A0]),
            (21, [0x17D8, 0x17E4, 0x17F4, 0x17FC, 0x1A94, 0x1CCC, 0x209C]),
            (22, [0x17BC, 0x1914, 0x1A70, 0x1C80, 0x1CD8, 0x1F80, 0x2098]),
            (23, [0x17B4, 0x19DC, 0x1C98, 0x2094]),
            (29, [0x170C, 0x20B8]), (30, [0x1714, 0x2090]),
        )
        for _, data in self.legal_images():
            for register, expected in lifetimes:
                self.assertEqual(self.register_writes(data, 0x170C, 0x20BC, register), expected)
            stores = self.direct_stores(data, 0x170C, 0x20BC, 29)
            self.assertTrue(all(0 <= offset and offset + width <= 304 for _, offset, width in stores))
            self.assertFalse(any(offset < 216 and 208 < offset + width for _, offset, width in stores))
            for offset, width, sites in (
                    (216, 4, [0x1760]), (224, 2, [0x1748, 0x1950, 0x19B8, 0x1CA8, 0x1CC4, 0x1F9C]),
                    (232, 4, [0x17B8]), (236, 4, [0x1768]),
                    (240, 4, [0x1A84]), (244, 4, [0x1A88]), (248, 4, [0x19D0]),
                    (252, 4, [0x1A8C]), (256, 4, [0x174C, 0x1964])):
                self.assertEqual([(pc, off, size) for pc, off, size in stores
                                  if off < offset + width and offset < off + size],
                                 [(pc, offset, width) for pc in sites])

    def test_packet_footprints_and_projection_arguments(self):
        fields = {(offset, 2) for offset in (8, 10, 20, 22, 32, 34, 44, 46)}
        fields |= {(offset + channel, 1) for offset in (4, 16, 28, 40) for channel in range(3)}
        anchors = {
            0x171C: 0x27D40720, 0x1758: 0x27D322B4, 0x193C: 0x26940084,
            0x19B4: 0x27D40720, 0x19CC: 0x27AB00D0, 0x19DC: 0x27D70740,
            0x1A74: 0x26880010, 0x1A78: 0x26890018, 0x1A7C: 0x26920004,
            0x1A80: 0x268A0002, 0x1AB0: 0x26820024, 0x1ABC: 0x26820068,
            0x1ADC: 0x26840038, 0x1AE0: 0x26850044, 0x1AE8: 0x27A700D4,
            0x1AF0: 0xAE42006C, 0x1B14: 0xAE420028, 0x1B2C: 0xAE420048,
            0x1B48: 0xA5220074, 0x1B74: 0xA5220078, 0x1BAC: 0x26020064,
            0x1BD4: 0x26050040, 0x1BDC: 0x27A700D4, 0x1BEC: 0xAE02006C,
            0x1C1C: 0xAE020028, 0x1C34: 0xAE020048, 0x1C54: 0xA6220074,
            0x1C78: 0xA6220078, 0x1C98: 0x26F70084, 0x1CBC: 0x26940084,
            0x1CC0: 0x27D40720, 0x1CC8: 0x24110040, 0x1CCC: 0x0000A821,
            0x1CD0: 0x241000A0, 0x1CD4: 0x24120080,
            0x1E04: 0x0440000A, 0x1E14: 0x04400007, 0x1E1C: 0x9466006C,
            0x1E20: 0x8FA500D8, 0x1E28: 0x02602021,
            0x1F54: 0x0440000A, 0x1F64: 0x04400006, 0x1F6C: 0x9466006C,
            0x1F70: 0x8FA500D8, 0x1F78: 0x02602021, 0x1FB0: 0x26940084,
        }
        for _, data in self.legal_images():
            for start, end in ((0x1CDC, 0x1E2C), (0x1E2C, 0x1F7C)):
                self.assertEqual({(off, width) for _, off, width in self.direct_stores(data, start, end, 19)},
                                 fields)
            for pc, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], word, hex(pc))
        self.assertEqual(0x720 + 16 * 132, 0xF60)
        self.assertEqual(0x22B4 + 52, 0x22E8)

    def test_literal_control_flow_and_all_actual_calls(self):
        calls = {
            0x173C: 0x8005C018, 0x175C: 0x80089928, 0x182C: 0x800866F8,
            0x1838: 0x80086628, 0x1870: 0x800866F8, 0x188C: 0x80086628,
            0x1898: 0x80086628, 0x18C8: 0x800866F8, 0x1904: 0x80086628,
            0x19FC: 0x80087CB8, 0x1A08: 0x800875F8, 0x1A60: 0x80086258,
            0x1A68: 0x80085558, 0x1AD4: 0x80087958, 0x1AEC: 0x80087868,
            0x1B08: 0x80089928, 0x1B28: 0x800866F8, 0x1B50: 0x80086628,
            0x1BC4: 0x80087958, 0x1BE8: 0x80087868, 0x1C10: 0x80089928,
            0x1C30: 0x800866F8, 0x1C5C: 0x80086628, 0x1E24: 0x800842A8,
            0x1F74: 0x800842A8,
        }
        anchors = {pc: word for pc, word in family476.SpanishModelVariant476Tests.entry_anchors.items()
                   if 0x170C <= pc < 0x20BC}
        anchors.update({
            0x170C: 0x27BDFED0, 0x1714: 0x0080F021, 0x17DC: 0x2442003F,
            0x17F8: 0x2442000F, 0x1804: 0x00029403, 0x1920: 0x28420002,
            0x195C: 0x28420010, 0x1998: 0x24420007, 0x199C: 0x97C327E4,
            0x19A0: 0x000210C3, 0x19E4: 0x00031400, 0x19EC: 0x00021403,
            0x1C8C: 0x28420002, 0x1CB4: 0x28420010, 0x1F88: 0x1840FF55,
            0x1FA8: 0x28420010, 0x1FC4: 0x0064102B, 0x1FD8: 0x28421000,
            0x1FF0: 0x0043001B, 0x200C: 0x28421000, 0x2018: 0xAFC227E4,
            0x202C: 0x0064102B, 0x2040: 0x1840000E, 0x2054: 0x0062001B,
            0x2068: 0x24020400, 0x2070: 0x1C400002, 0x2078: 0xAFC027EC,
            0x2084: 0x24420030, 0x20B8: 0x27BD0130,
        })
        for module, data in self.legal_images():
            base = int(module["load_address"], 0)
            for pc, expected in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], expected, hex(pc))
            observed = {pc: 0x80000000 | (word & 0x3FFFFFF) << 2
                        for pc in range(0x170C, 0x20BC, 4)
                        if (word := struct.unpack_from("<I", data, pc)[0]) >> 26 == 3}
            self.assertEqual(observed, calls)
            for pc, target in ((0x1790, 0x17B4), (0x17E0, 0x1800), (0x1820, 0x18C4),
                               (0x1980, 0x19B0), (0x1B70, 0x1C7C), (0x1DB8, 0x1DF0),
                               (0x1F08, 0x1F40)):
                self.assertEqual(struct.unpack_from("<I", data, pc)[0],
                                 0x08000000 | ((base + target) >> 2 & 0x3FFFFFF))

    def test_actual_descriptor_and_complete_state_writes(self):
        for module, data in self.legal_images():
            self.assertEqual(struct.unpack_from("<3I", data, 0x3470 + 0x20), (144, 152, 260))
            record = 612
            with (ROOT / "game/spain/DATA/MODEL.MRG").open("rb") as archive:
                archive.seek((record * 276 + 275) * 2048 + 0x114)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 642000)
            self.assertEqual(self.direct_stores(data, 0x170C, 0x20BC, 30), [
                (0x2008, 0x27E4, 4), (0x2018, 0x27E4, 4),
                (0x2074, 0x27EC, 4), (0x2078, 0x27EC, 4), (0x2088, 0x27F0, 4),
            ])
            self.assertEqual(module["sector_offset"], record * 276 + 200 +
                             (int(module["load_address"], 0) - 0x8013B000) // 0x40000 * 10)
        self.assertEqual(152 - 144, 8)
        self.assertEqual(260 - 152, 108)
