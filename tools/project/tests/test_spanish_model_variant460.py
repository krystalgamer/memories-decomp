import re
import struct

from tools.project.tests import test_french_model_variant460 as family460
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant460Tests(family460.FrenchModelVariant460Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    helpers = ((0xCFC, 2800, "ribbons", "func_8013BD00"),
               (0x17EC, 1416, "sheets", "func_8013C808"),
               (0x1D74, 832, "strand", "func_8013CD84"))
    reachable_helpers = {0xCFC, 0x17EC}

    @staticmethod
    def register_writes(data, start, end, register):
        result = []
        for offset in range(start, end, 4):
            word, = struct.unpack_from("<I", data, offset)
            op = word >> 26
            destination = None
            if op == 0 and word & 63 not in (8, 12, 13, 17, 19, 24, 25, 26, 27):
                destination = word >> 11 & 31
            elif op in (8, 9, 10, 11, 12, 13, 14, 15, 32, 33, 34, 35, 36, 37, 38):
                destination = word >> 16 & 31
            if destination == register:
                result.append(offset)
        return result

    @staticmethod
    def direct_stores(data, start, end, register):
        result = []
        for offset in range(start, end, 4):
            word, = struct.unpack_from("<I", data, offset)
            op = word >> 26
            if op in (40, 41, 43) and word >> 21 & 31 == register:
                displacement = (word & 65535) - (65536 if word & 32768 else 0)
                result.append((offset, displacement, {40: 1, 41: 2, 43: 4}[op]))
        return result

    def legal_images(self):
        path = family460.family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                yield module, archive.read(20480)

    def test_original_contexts_and_advancing_sheet_cursor(self):
        lifetimes = (
            (4, 0xCFC, 22, [0x10, 0xCD8]), (4, 0xCFC, 29, [4, 0xCF8]),
            (0xB50, 0xB5C, 4, [0xB50]), (0xB90, 0xB9C, 4, [0xB90]),
            (0xCFC, 0x17EC, 22, [0xD30, 0x173C, 0x17C8]),
            (0xCFC, 0x17EC, 23, [0xD7C, 0x1738, 0x17C4]),
            (0xCFC, 0x17EC, 29, [0xCFC, 0x17E8]),
            (0x1430, 0x1600, 16, [0x1444, 0x1470, 0x147C, 0x15D0, 0x15DC]),
            (0x1430, 0x1600, 17, [0x1430, 0x1478, 0x1480, 0x15D8, 0x15E0]),
            (0x17EC, 0x1D74, 17, [0x17FC, 0x18F0, 0x1D64]),
            (0x17EC, 0x1D74, 20, [0x183C, 0x1D58]),
            (0x17EC, 0x1D74, 21, [0x17F4, 0x1D40, 0x1D54]),
            (0x17EC, 0x1D74, 29, [0x17EC, 0x1D70]),
            (0x1D74, 0x20B4, 20, [0x1D7C, 0x2098]),
            (0x1D74, 0x20B4, 21, [0x1DB8, 0x2094]),
            (0x1D74, 0x20B4, 23, [0x1D84, 0x1F20, 0x1F24, 0x1FFC, 0x208C]),
            (0x1D74, 0x20B4, 29, [0x1D74, 0x20B0]),
        )
        anchors = {
            4: 0x27BDFF30, 0x70: 0x04A001F5, 0xB90: 0x8FA400D0,
            0xB98: 0, 0xD30: 0x8FB602F8, 0xD44: 0xAFA202FC,
            0x17EC: 0x27BDFEF0, 0x17F4: 0x24951740, 0x17FC: 0x24912B18,
            0x1824: 0xAFA400D8, 0x183C: 0x0100A021, 0x1844: 0xAFA200DC,
            0x18F0: 0x26310034, 0x1BA4: 0x02C9102A, 0x1BA8: 0x1040003D,
            0x1CA0: 0x8FA800D8, 0x1CA8: 0x8D030248, 0x1CDC: 0x8D22024C,
            0x1D14: 0x8FAA00D8, 0x1D1C: 0x254A02E8, 0x1D20: 0xAFAA00D8,
            0x1D2C: 0x2610009C, 0x1D40: 0x26B5009C, 0x1D74: 0x27BDFEF8,
        }
        for module, data in self.legal_images():
            for offset, expected in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], expected)
            base = int(module["load_address"], 0)
            self.assertEqual(struct.unpack_from("<I", data, 0xB94)[0],
                             0x0C000000 | ((base + 0x17EC) >> 2 & 0x3FFFFFF))
            for start, end, register, expected in lifetimes:
                self.assertEqual(self.register_writes(data, start, end, register), expected)
            for start, end, spill, expected in ((4, 0xCFC, 208, [0xC]),
                                               (0xCFC, 0x17EC, 760, [0xD2C]),
                                               (0x17EC, 0x1D74, 216, [0x1824, 0x1D20])):
                overlaps = [pc for pc, off, size in self.direct_stores(data, start, end, 29)
                            if off < spill + 4 and spill < off + size]
                self.assertEqual(overlaps, expected)

    def test_packet_fields_frames_and_actual_timing_intervals(self):
        for module, data in self.legal_images():
            fields = {(off + 6, size) for _, off, size in self.direct_stores(data, 0x1430, 0x1600, 16)}
            self.assertEqual(fields, {(4, 1), (5, 1), (6, 1), (8, 2), (10, 2), (16, 2),
                                      (18, 2), (24, 2), (26, 2), (32, 2), (34, 2)})
            self.assertEqual({(off, size) for _, off, size in self.direct_stores(data, 0x17EC, 0x1D74, 17)},
                             {(off, 1) for off in (4, 5, 6, 16, 17, 18, 28, 29, 30, 40, 41, 42)})
            self.assertEqual({(off, size) for _, off, size in self.direct_stores(data, 0x1D74, 0x20B4, 21)},
                             {(0, 4), (12, 1), (13, 1), (14, 1)})
            for offset, expected in {
                0x1430: 0x25512BB4, 0x1444: 0x25502BBA,
                0x145C: 0x14490009, 0x1468: 0x14400004,
                0x15BC: 0x1440000A, 0x15C8: 0x14400004,
                0x17E8: 0x27BD0358, 0x1D70: 0x27BD0110, 0x20B0: 0x27BD0108,
                0x1A84: 0x26220008, 0x1A8C: 0x26220014,
                0x1A94: 0x26220020, 0x1A9C: 0x2622002C,
                0x1DB8: 0x26952C04, 0x1F2C: 0x26AA0004, 0x1F34: 0x26BE0008,
            }.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], expected)
            command = int(self.instances[module["name"]]["command_word"])
            descriptor = 0x29C8 + command % 1000 * 64
            growth_start, growth_end = struct.unpack_from("<2I", data, descriptor + 0x2C)
            fade_start, fade_end = struct.unpack_from("<2I", data, descriptor + 0x38)
            self.assertLess(growth_start, growth_end)
            self.assertLess(fade_start, fade_end)
        self.assertEqual(208 + 544, 752)
        self.assertEqual(752 + 8, 760)
        self.assertEqual(816 + 40, 856)
        self.assertEqual(0x1740 + 16 * 156, 0x2100)
        self.assertEqual(0x2100 + 6 * 124, 0x23E8)
        self.assertEqual(0x2BB4 + 2 * 40, 0x2C04)
        self.assertEqual(0x2B18 + 2 * 52, 0x2B80)
        for parity in (0, 1):
            packet = 0
            for segment in range(16):
                if parity:
                    packet += -1 if segment & 1 else 1
                self.assertIn(packet, (0, 1))
                if not parity:
                    packet += -1 if segment & 1 else 1
                self.assertIn(packet, (0, 1))

    def test_all_fallback_and_c_calls_have_resident_bindings(self):
        for module, data in self.legal_images():
            bindings = (family460.family435.ROOT / module["linker_symbols"]).read_text()
            addresses = {int(address, 0) for address in re.findall(r"= (0x[0-9A-F]+);", bindings)}
            base = int(module["load_address"], 0)
            local = {base + start for start, _ in self.spans}
            for word in struct.unpack("<2610I", data[4:0x28CC]):
                if word >> 26 == 3:
                    target = 0x80000000 | ((word & 0x3FFFFFF) << 2)
                    self.assertIn(target, addresses | local)
