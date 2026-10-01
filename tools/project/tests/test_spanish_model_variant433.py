import re
import struct

from tools.project.tests import test_french_model_variant433 as family433
from tools.project.tests import test_french_model_variant435 as family435
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant433Tests(family433.FrenchModelVariant433Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    helpers = ((0x1050, 1984, "strips", "func_8013C050"),
               (0x1810, 1108, "sheet", "func_8013C810"),
               (0x1C64, 1384, "webs", "func_8013CC68"),
               (0x26C8, 784, "spokes", "func_8013D6D4"),
               (0x29D8, 892, "rings", "func_8013D9E8"),
               (0x2D54, 868, "quad", "func_8013DD68"))
    reachable_helpers = {0x1050, 0x1810, 0x1C64}
    standalone_helpers = frozenset({"strips", "sheet"})

    def test_bindings_cover_fallback_functions_as_well_as_c(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)

    def test_selected_descriptors_and_minimum_context_extent(self):
        archive_path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        resident_path = family435.ROOT / "game/spain/SLES_039.51"
        if not archive_path.exists() or not resident_path.exists():
            self.skipTest("legal Spanish MODEL and resident inputs required")
        pointers = struct.unpack_from("<14I", resident_path.read_bytes(), 0x800)
        self.assertEqual(pointers[9:11], (0x80136000, 0x80176000))
        widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
        with archive_path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, base = int(row["slot"]), int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x31B4
                self.assertEqual(struct.unpack_from("<II", data, 0xA0),
                                 (0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF),
                                  0x24420000 | (table & 0xFFFF)))
                self.assertEqual(struct.unpack_from("<5I", data, 0xB0),
                                 (0x00181840, 0x00781821, 0x00031900, 0x00621821, 0xAEC31A60))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110)
                request, = struct.unpack("<i", archive.read(4))
                self.assertEqual(request, {180: 599001, 440: 599000}[int(row["model"])])
                self.assertEqual(request, int(row["command_word"]))
                descriptor = 0x31B4 + request % 1000 * 48
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 48, len(data))
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack("<1043I", data[4:0x1050])
                            if word >> 26 in widths and word >> 21 & 31 == 22
                            and word & 0x8000 == 0]
                self.assertEqual(max(accesses), 0x1A98)
                context = pointers[9 + slot]
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048), (base, 10 * 2048)):
                    self.assertTrue(context + max(accesses) <= start or start + size <= context)

    def test_sheet_packet_context_and_stack_ownership(self):
        path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        self.assertEqual(0xF60 + 2 * 168, 0x10B0)
        self.assertEqual(0x10B0 + 3 * 152, 0x1278)
        self.assertEqual(0x191C + 52, 0x1950)
        self.assertEqual(128 + 80, 208)
        self.assertEqual(208 + 4, 212)
        self.assertEqual(212 + 4, 216)
        self.assertLessEqual(216 + 4, 224)
        anchors = {
            0x1810: 0x27BDFEF8, 0x1818: 0x00809021, 0x1820: 0x265E0F60,
            0x1828: 0x265510B0, 0x184C: 0x2651191C, 0x185C: 0xAFA200D8,
            0x188C: 0x24020002, 0x1898: 0x16E20017, 0x18F8: 0x8FC30068,
            0x1A24: 0xAFA000C8, 0x1A2C: 0xAFA00080,
            0x1A80: 0x26220008, 0x1A88: 0x26220014,
            0x1A90: 0x26220020, 0x1A98: 0x2622002C,
            0x1AA0: 0x27A200D0, 0x1AA4: 0xAFA20020,
            0x1AA8: 0x27A200D4, 0x1AB4: 0x0C021E56, 0x1AB8: 0xAFA20024,
            0x1B5C: 0x8FA200D4, 0x1B6C: 0x8FA500D8, 0x1B70: 0x0C0210AA,
            0x1C20: 0x26100098, 0x1C24: 0x26B50098, 0x1C28: 0x2AE20003,
            0x1C30: 0x27DE00A8, 0x1C38: 0x8FBE0100,
            0x1C50: 0x8FB200E8, 0x1C54: 0x8FB100E4,
            0x1C58: 0x8FB000E0, 0x1C60: 0x27BD0108,
        }
        widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
        with path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                self.assertEqual(struct.unpack_from("<I", data, 0x18F0)[0],
                                 0x08000000 | ((base + 0x19D0) >> 2 & 0x3FFFFFF))
                for start, end, register, expected_writes, extent in (
                        (4, 0x1050, 22, [0x14, 0x102C], 0x1A98),
                        (0x1810, 0x1C64, 18, [0x1818, 0x1C50], 0x1A88),
                        (0x1810, 0x1C64, 17, [0x184C, 0x1C54], None),
                        (0x1810, 0x1C64, 30, [0x1820, 0x1C30, 0x1C38], None)):
                    writes, accesses = [], []
                    for offset in range(start, end, 4):
                        word, = struct.unpack_from("<I", data, offset)
                        op = word >> 26
                        destination = word >> 11 & 31 if op == 0 else word >> 16 & 31 if op in (
                            8, 9, 10, 11, 12, 13, 14, 15, 32, 33, 35, 36, 37) else None
                        if destination == register:
                            writes.append(offset)
                        if op in widths and word >> 21 & 31 == register and not word & 0x8000:
                            accesses.append((word & 0xFFFF) + widths[op])
                    self.assertEqual(writes, expected_writes)
                    if extent is not None:
                        self.assertEqual(max(accesses), extent)


class SpanishModelVariant433StripTests(family433.FrenchModelVariant433StripTests):
    region = "spain"
    config_name = "sles_03951"

    def test_strip_context_fixed_packet_and_stack_ownership(self):
        path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        self.assertEqual(len(self.modules), 4)
        self.assertEqual(0xF60 + 2 * 168, 0x10B0)
        self.assertEqual(0x18E8 + 52, 0x191C)
        self.assertEqual(120 + 80, 200)
        self.assertEqual(200 + 2 * 2 * 4, 216)
        self.assertEqual(216 + 4, 220)
        self.assertEqual(220 + 4, 224)
        self.assertLess(252, 256)
        anchors = {
            0x1050: 0x27BDFED8, 0x1058: 0x00809821,
            0x1098: 0xAFA200DC, 0x10B0: 0x267118E8, 0x10BC: 0x26740F60,
            0x10C8: 0x27B70058, 0x10D0: 0xAFA200F0, 0x10E8: 0xAFA200F4,
            0x127C: 0x27A40078, 0x12C4: 0xAFA000C0, 0x12CC: 0xAFA00078,
            0x1314: 0x001E8080, 0x1318: 0x26020038, 0x1320: 0xAFA20010,
            0x1324: 0x26020040, 0x132C: 0xAFA20014, 0x1330: 0x27A200D8,
            0x1334: 0xAFA20018, 0x1338: 0x27A200C8, 0x133C: 0x26070030,
            0x1340: 0x8FA800F4, 0x1348: 0x000818C0, 0x134C: 0x00431021,
            0x1350: 0x00501021, 0x1354: 0x0C021E26, 0x1358: 0xAFA2001C,
            0x1360: 0xAC8200A0, 0x13F0: 0x8E621A60, 0x13F8: 0x8C430028,
            0x142C: 0x0000A821, 0x145C: 0x28840002, 0x1468: 0xACA20068,
            0x1478: 0x8C420070, 0x1518: 0x269400A8, 0x151C: 0x26740F60,
            0x1638: 0x8CA200A0, 0x164C: 0x8C4200C8, 0x165C: 0x94A600A0,
            0x1668: 0x02202021, 0x1788: 0x8C4200C8, 0x1798: 0x94A600A0,
            0x17A4: 0x02202021, 0x17DC: 0x269400A8,
            0x17F4: 0x8FB40110, 0x17F8: 0x8FB3010C,
            0x1800: 0x8FB10104, 0x1804: 0x8FB00100, 0x180C: 0x27BD0128,
        }
        widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                for register, expected_writes in (
                        (19, [0x1058, 0x17F8]), (17, [0x10B0, 0x1800]),
                        (20, [0x10BC, 0x1518, 0x151C, 0x17DC, 0x17F4])):
                    writes, accesses = [], []
                    for offset in range(0x1050, 0x1810, 4):
                        word, = struct.unpack_from("<I", data, offset)
                        op = word >> 26
                        destination = word >> 11 & 31 if op == 0 else word >> 16 & 31 if op in (
                            8, 9, 10, 11, 12, 13, 14, 15, 32, 33, 35, 36, 37) else None
                        if destination == register:
                            writes.append(offset)
                        if op in widths and word >> 21 & 31 == register and not word & 0x8000:
                            accesses.append((word & 0xFFFF) + widths[op])
                    self.assertEqual(writes, expected_writes)
                    if register == 19:
                        self.assertEqual(max(accesses), 0x1A88)
