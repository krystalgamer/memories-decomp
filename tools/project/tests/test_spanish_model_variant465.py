import re
import struct

from tools.project.tests import test_french_model_variant465 as family465
from tools.project.tests import test_spanish_model_variant460 as lifetimes460
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant465Tests(family465.FrenchModelVariant465Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    resident_name = "SLES_039.51"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    register_writes = staticmethod(lifetimes460.SpanishModelVariant460Tests.register_writes)
    direct_stores = staticmethod(lifetimes460.SpanishModelVariant460Tests.direct_stores)
    entry_anchors = {
        **family465.FrenchModelVariant465Tests.entry_anchors,
        0x4: 0x27BDFF08, 0x1248: 0x27BD00F8, 0x1668: 0x27BD0110,
        0x2AB0: 0x27BD0130, 0x2AB4: 0x27BDFED8, 0x3010: 0x27BD0128,
        0x3014: 0x27BDFEF0, 0x301C: 0x0080A021, 0x3020: 0x268811F0,
        0x3028: 0x26961678, 0x3030: 0x26911A84, 0x3054: 0xAFA800D8,
        0x305C: 0x269E1A88, 0x3060: 0x26971A8C, 0x3064: 0x26901704,
        0x3088: 0x8D230088, 0x3178: 0x24A50040,
        0x3188: 0xAE220000, 0x318C: 0x27A200D0, 0x3194: 0x27A200D4,
        0x31AC: 0x0C021E56, 0x3270: 0x30C6FFFF, 0x3278: 0x2A620008,
        0x3280: 0x26520008, 0x32D8: 0x26D60090, 0x32E0: 0x29020004,
        0x3318: 0x27BD0110, 0x331C: 0x27BDFEF8, 0x3324: 0x00809021,
        0x332C: 0x26571318, 0x3334: 0x26511A84, 0x3360: 0x26481A88,
        0x3364: 0x265E1A8C, 0x3368: 0x265013A4,
        0x347C: 0xAE220000, 0x3480: 0x27A200D0, 0x3488: 0x27A200D4,
        0x34A4: 0x0C021E56, 0x3560: 0x18C00007, 0x3568: 0x28C20800,
        0x3578: 0x0C02102E, 0x357C: 0x30C6FFFF, 0x3584: 0x2A820008,
        0x358C: 0x26730008, 0x365C: 0x2AC20006, 0x3664: 0x26F70090,
        0x3694: 0x27BD0108, 0x3698: 0x27BDFF08, 0x36A0: 0x00809021,
        0x36A8: 0x265418B8, 0x36B0: 0x26511964,
        0x37F8: 0x26840058, 0x37FC: 0x26850060, 0x3800: 0x26860068, 0x3804: 0x26870070,
        0x3808: 0x26220008, 0x3810: 0x26220010, 0x3818: 0x26220018, 0x3820: 0x26220020,
        0x3828: 0x27A200D0, 0x3830: 0x27A200D4, 0x3834: 0x0C021E56,
        0x38D4: 0x3046FFFF, 0x38DC: 0x24070001, 0x390C: 0x8C630020,
        0x3914: 0x0043001B, 0x39CC: 0x1AA0FF43, 0x39D0: 0x26940090,
        0x39F8: 0x27BD00F8,
        0x2AEC: 0xAFA800D8, 0x2B20: 0x266B1A88, 0x2B28: 0xAFAB00F8,
        0x2B34: 0x26681A8C, 0x2B3C: 0xAFA800FC,
        0x2D80: 0x27A200D0, 0x2D88: 0x27A200D4, 0x2DB0: 0x0C021E56,
        0x2E0C: 0x0C02102E, 0x2E10: 0x3046FFFF,
    }

    def legal_images(self):
        path = family465.family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                yield module, archive.read(20480)

    def test_original_context_reaches_all_three_entry_calls(self):
        for module, data in self.legal_images():
            definitions = set(self.register_writes(data, 4, 0x124C, 18))
            base = int(module["load_address"], 0)
            pending, visited, reaching = [(4, None)], set(), {}
            while pending:
                pc, definition = pending.pop()
                self.assertTrue(4 <= pc < 0x124C and pc % 4 == 0)
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
                    if pc in (0x1088, 0x10A4, 0x10BC):
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
            self.assertEqual(reaching, {pc: {0xC} for pc in (0x1088, 0x10A4, 0x10BC)})

    def test_context_packet_and_record_register_lifetimes(self):
        lifetimes = (
            (4, 0x124C, 22, [0x14, 0x1228]), (4, 0x124C, 29, [4, 0x1248]),
            (0x124C, 0x166C, 23, [0x125C, 0x1644]),
            (0x124C, 0x166C, 20, [0x1254, 0x162C, 0x1650]),
            (0x124C, 0x166C, 19, [0x1264, 0x1654]),
            (0x124C, 0x166C, 18, [0x1298, 0x1658]),
            (0x124C, 0x166C, 29, [0x124C, 0x1668]),
            (0x256C, 0x2AB4, 21, [0x2574, 0x2A94]),
            (0x256C, 0x2AB4, 17, [0x2580, 0x2AA4]),
            (0x256C, 0x2AB4, 18, [0x25AC, 0x2AA0]),
            (0x256C, 0x2AB4, 29, [0x256C, 0x2AB0]),
            (0x2AB4, 0x3014, 19, [0x2ABC, 0x2FFC]),
            (0x2AB4, 0x3014, 18, [0x2B0C, 0x3000]),
            (0x2AB4, 0x3014, 17, [0x2B4C, 0x2FB8, 0x3004]),
            (0x2AB4, 0x3014, 29, [0x2AB4, 0x3010]),
            (0x3014, 0x331C, 20, [0x301C, 0x3300]),
            (0x3014, 0x331C, 22, [0x3028, 0x32D8, 0x32F8]),
            (0x3014, 0x331C, 17, [0x3030, 0x330C]),
            (0x3014, 0x331C, 29, [0x3014, 0x3318]),
            (0x331C, 0x3698, 18, [0x3324, 0x3684]),
            (0x331C, 0x3698, 23, [0x332C, 0x3664, 0x3670]),
            (0x331C, 0x3698, 17, [0x3334, 0x3688]),
            (0x331C, 0x3698, 29, [0x331C, 0x3694]),
            (0x3698, 0x39FC, 18, [0x36A0, 0x39E8]),
            (0x3698, 0x39FC, 20, [0x36A8, 0x39D0, 0x39E0]),
            (0x3698, 0x39FC, 17, [0x36B0, 0x39EC]),
            (0x3698, 0x39FC, 29, [0x3698, 0x39F8]),
        )
        for _, data in self.legal_images():
            for start, end, register, expected in lifetimes:
                self.assertEqual(self.register_writes(data, start, end, register), expected)
            for start, end, spill, expected in (
                    (0x2AB4, 0x3014, 216, [0x2AEC]), (0x3014, 0x331C, 216, [0x3054]),
                    (0x2AB4, 0x3014, 248, [0x2B28]), (0x2AB4, 0x3014, 252, [0x2B3C])):
                overlapping = [pc for pc, off, size in self.direct_stores(data, start, end, 29)
                               if off < spill + 4 and spill < off + size]
                self.assertEqual(overlapping, expected)

    def test_packet_fields_projection_slots_and_record_bounds(self):
        packets = (
            (0x124C, 0x166C, 19, 28, (4, 12, 20)),
            (0x124C, 0x166C, 18, 36, (4, 12, 20, 28)),
            (0x256C, 0x2AB4, 18, 52, (4, 16, 28, 40)),
            (0x2AB4, 0x3014, 18, 20, (12, 15)),
            (0x3014, 0x331C, 17, 20, (12, 15)),
            (0x331C, 0x3698, 17, 20, (12, 15)),
            (0x3698, 0x39FC, 17, 36, (4, 12, 20, 28)),
        )
        for _, data in self.legal_images():
            for start, end, register, size, colors in packets:
                fields = {(off, width) for _, off, width in self.direct_stores(data, start, end, register)}
                expected = {(off + channel, 1) for off in colors for channel in range(3)}
                if size == 20:
                    expected.add((0, 4))
                self.assertEqual(fields, expected)
                self.assertLessEqual(max(off + width for off, width in fields), size)
            for start, end, frame in ((0x124C, 0x166C, 272), (0x256C, 0x2AB4, 304),
                                      (0x2AB4, 0x3014, 296), (0x3014, 0x331C, 272),
                                      (0x331C, 0x3698, 264), (0x3698, 0x39FC, 248)):
                stores = self.direct_stores(data, start, end, 29)
                self.assertTrue(all(0 <= off and off + width <= frame for _, off, width in stores))
                self.assertFalse(any(off < 216 and 208 < off + width for _, off, width in stores))
        self.assertEqual(0x6F8 + 3 * 416, 0xBD8)
        self.assertEqual(0xBD8 + 3 * 520, 0x11F0)
        self.assertEqual(0x11F0 + 296, 0x1318)
        self.assertEqual(0x1318 + 6 * 144, 0x1678)
        self.assertEqual(0x1678 + 4 * 144, 0x18B8)
        self.assertEqual(0x18B8 + 144, 0x1948)
        self.assertEqual(0x1948 + 28, 0x1964)
        self.assertEqual(0x1964 + 36, 0x1988)

    def test_all_fallback_and_c_calls_have_resident_bindings(self):
        for module, data in self.legal_images():
            bindings = (family465.family435.ROOT / module["linker_symbols"]).read_text()
            addresses = {int(address, 0) for address in re.findall(r"= (0x[0-9A-F]+);", bindings)}
            base = int(module["load_address"], 0)
            local = {base + start for start, _ in self.spans}
            for word in struct.unpack("<4308I", data[4:0x4354]):
                if word >> 26 == 3:
                    target = 0x80000000 | ((word & 0x3FFFFFF) << 2)
                    self.assertIn(target, addresses | local)
