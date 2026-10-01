import re
import struct

from tools.project.tests import test_french_model_variant418 as family418
from tools.project.tests import test_french_model_variant435 as family435
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant418Tests(family418.FrenchModelVariant418Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    helpers = ((0x2204, 1320, "rays", "func_8013D1CC"),
               (0x272C, 784, "spokes", "func_8013D728"),
               (0x2A3C, 892, "rings", "func_8013DA3C"),
               (0x2DB8, 868, "quad", "func_8013DDBC"))
    reachable_helpers = {0x2204}

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
                table = base + 0x3218
                self.assertEqual(struct.unpack_from("<II", data, 0xA0),
                                 (0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF),
                                  0x24420000 | (table & 0xFFFF)))
                self.assertEqual(struct.unpack_from("<5I", data, 0xB0),
                                 (0x00181840, 0x00781821, 0x00031900, 0x00621821, 0xAEC31AF4))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x114)
                request, = struct.unpack("<i", archive.read(4))
                self.assertEqual(request, 584002)
                self.assertEqual(request, int(row["command_word"]))
                descriptor = 0x3218 + request % 1000 * 48
                self.assertEqual(descriptor, 0x3278)
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 48, len(data))
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack("<1015I", data[4:0xFE0])
                            if word >> 26 in widths and word >> 21 & 31 == 22
                            and word & 0x8000 == 0]
                self.assertEqual(max(accesses), 0x1B2C)
                context = pointers[9 + slot]
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048), (base, 10 * 2048)):
                    self.assertTrue(context + max(accesses) <= start or start + size <= context)


class SpanishModelVariant418RayTests(family418.FrenchModelVariant418RayTests):
    region = "spain"
    config_name = "sles_03951"

    def test_ray_context_packet_and_flag_grid_bounds(self):
        self.assertEqual(128 + 80, 208)
        self.assertEqual(208 + 16 * 9 * 4, 784)
        flag_words = [208 + i * 36 + j * 4 for i in range(16) for j in range(8)]
        self.assertEqual(len(set(flag_words)), 128)
        self.assertEqual((min(flag_words), max(flag_words) + 4), (208, 780))
        self.assertTrue(all(208 <= address and address + 4 <= 784 for address in flag_words))
        self.assertLessEqual(784 + 9, 800)
        self.assertLessEqual(800 + 9, 816)
        self.assertLessEqual(816 + 9, 832)
        self.assertEqual(832 + 4, 836)
        self.assertEqual(836 + 4, 840)
        self.assertEqual(840 + 4, 844)
        self.assertLessEqual(864 + 4, 888)
        self.assertEqual(0x1A78 + 20, 0x1A8C)
        anchors = {
            0x8C: 0x04A002FC, 0x220C: 0x0080A021, 0x2244: 0xAFA20348,
            0x2254: 0x26961A78, 0x227C: 0xAFB40344, 0x2280: 0x86821B28,
            0x2298: 0x27AA0310, 0x229C: 0x27AB0320, 0x22A0: 0x27BE0330,
            0x22A4: 0x26881A7C, 0x22A8: 0x26891A80, 0x230C: 0x27A40080,
            0x2358: 0xAFA000C8, 0x2360: 0xAFA00080, 0x25DC: 0x27A20340,
            0x25E0: 0xAFA20020, 0x2608: 0xAFA20024, 0x260C: 0xAFAA0010,
            0x2610: 0xAFAB0014, 0x2614: 0xAFAA0018, 0x261C: 0xAFAB001C,
            0x2634: 0xA2C3000C, 0x264C: 0xA2C3000D, 0x265C: 0xA2C3000E,
            0x266C: 0xA2C3000F, 0x267C: 0xA2C30010, 0x268C: 0xA2C30011,
            0x2708: 0x8FB60390, 0x2710: 0x8FB40388, 0x2720: 0x8FB00378,
            0x2728: 0x27BD03A0,
        }
        widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
        for module, data in self.images():
            base = int(module["load_address"], 0)
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
            self.assertEqual(struct.unpack_from("<I", data, 0xC78)[0],
                             0x08000000 | ((base + 0xEC8) >> 2 & 0x3FFFFFF))
            for start, end, register, expected in (
                    (0xC80, 0xE80, 19, []),
                    (0x2204, 0x272C, 20, [0x220C, 0x26F8, 0x2710]),
                    (0x2204, 0x272C, 22, [0x2254, 0x2708]),
                    (0x2204, 0x272C, 29, [0x2204, 0x2728])):
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
                        if register == 29 and op < 40:
                            self.assertFalse((word & 0xFFFF) < 64 and accesses[-1] > 48)
                self.assertEqual(writes, expected)
                if register == 20:
                    self.assertEqual(max(accesses), 0x1B2A)
                elif register == 29:
                    self.assertEqual(max(accesses), 928)
            words = struct.unpack("<330I", data[0x2204:0x272C])
            self.assertEqual(
                [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3],
                [0x8005C018, *([0x80089928] * 4), 0x80087CB8, 0x80086258,
                 0x80085558, 0x800866F8, 0x80086628, 0x80086628, 0x80087958, 0x800840B8])
