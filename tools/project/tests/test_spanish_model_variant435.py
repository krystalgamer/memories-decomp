import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant435Tests(family435.FrenchModelVariant435Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    reachable_helpers = {0x1084, 0x1AD4, 0x1E7C}
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    helpers = ((0x1084, 2640, "spiral", "func_8013C088"),
               (0x1AD4, 936, "sheet", "func_8013CAA4"),
               (0x1E7C, 988, "webs", "func_8013CE50"),
               (0x2258, 1612, "ribbons", "func_8013D238"),
               (0x28A4, 1804, "bands", "func_8013D86C"),
               (0x2FB0, 776, "spokes", "func_8013DF78"),
               (0x32B8, 892, "rings", "func_8013E284"),
               (0x3634, 868, "quad", "func_8013E604"))

    def test_bindings_cover_fallback_functions_as_well_as_c(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)

    def test_actual_descriptors_and_minimum_context_view(self):
        path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        resident = family435.ROOT / "game/spain/SLES_039.51"
        if not path.exists() or not resident.exists():
            self.skipTest("legal Spanish MODEL and resident inputs required")
        pointers = struct.unpack_from("<14I", resident.read_bytes(), 0x800)
        self.assertEqual((pointers[9], pointers[10]), (0x80136000, 0x80176000))
        widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
        requests = set()
        with path.open("rb") as archive:
            for module in self.modules:
                instance = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(instance["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                table = base + 0x3A94
                anchors = {
                    0xA0: 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF),
                    0xA4: 0x24420000 | (table & 0xFFFF),
                    0xB0: 0x00181900, 0xB4: 0x00781821, 0xB8: 0x00031880,
                    0xBC: 0x00621821, 0xC0: 0xAEC31B74,
                    0xF14: 0x0C000000 | (((base + 0x1E7C) >> 2) & 0x3FFFFFF),
                }
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                archive.seek((int(instance["record"]) * 276 + 275) * 2048 + 0x110 +
                             (int(instance["stage"]) - 7) // 2 * 4)
                request, = struct.unpack("<i", archive.read(4))
                self.assertEqual(request, int(instance["command_word"]))
                self.assertGreaterEqual(request, 0)
                requests.add(request)
                descriptor = 0x3A94 + request % 1000 * 68
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 68, len(data))
                accesses = [(w & 0xFFFF) + widths[w >> 26]
                            for w in struct.unpack("<1056I", data[4:0x1084])
                            if w >> 26 in widths and w >> 21 & 31 == 22 and w & 0x8000 == 0]
                self.assertEqual(max(accesses), 0x1BB4)
                context = pointers[9 + slot]
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048), (base, 10 * 2048)):
                    self.assertTrue(context + max(accesses) <= start or start + size <= context)
        self.assertEqual(requests, {601000, 601001, 601002, 601003, 601005,
                                   601006, 601007, 601009, 601010, 601011})

    def test_retained_ribbon_packet_stack_and_descriptor_gate(self):
        self.assertEqual(0xD50 + 8 * 116, 0x10F0)
        self.assertEqual(0x12DC + 136, 0x1364)
        self.assertEqual(0x19A4 + 28, 0x19C0)
        self.assertEqual(128 + 80, 208)
        self.assertEqual(208 + 4, 212)
        self.assertEqual(212 + 4, 216)
        self.assertEqual(216 + 4, 220)
        self.assertEqual(220 + 4, 224)
        self.assertLessEqual(248 + 4, 256)
        path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        anchors = {
            0x58: 0x26D119A4, 0x260: 0x0C020B92, 0x264: 0x02202021,
            0x268: 0x02202021, 0x26C: 0x0C020B6A, 0x270: 0x24050001,
            0x2298: 0xAFA200DC, 0x22AC: 0xAFA200E8, 0x22D0: 0xAFA200EC,
            0x22D8: 0xAFA800D8, 0x22F4: 0x00094823, 0x22F8: 0xAFA900E8,
            0x2314: 0xAFA200F0, 0x2338: 0x24110096, 0x233C: 0x24110028,
            0x2448: 0x246303FF, 0x2478: 0x246303FF, 0x24A8: 0x246303FF,
            0x24FC: 0x27A50030, 0x2500: 0x27A40080, 0x2568: 0xAFA900F4,
            0x259C: 0x27A200D0, 0x25A4: 0x27A200D4,
            0x260C: 0x27A200D0, 0x2614: 0x27A200D4,
            0x2674: 0x27A600D0, 0x267C: 0x27A700D4,
            0x2774: 0xA6C20008, 0x2788: 0xA6C2000A, 0x2794: 0xA6C20010,
            0x27A0: 0xA6C20012, 0x27B4: 0xA6C20018, 0x27E8: 0xA6C2001A,
            0x27C0: 0xA2D00004, 0x27C4: 0xA2D00005, 0x27C8: 0xA2D00006,
            0x27CC: 0xA2D1000C, 0x27D0: 0xA2C0000D, 0x27D4: 0xA2D1000E,
            0x27D8: 0xA2D00014, 0x27DC: 0xA2D00015, 0x27E0: 0xA2D00016,
            0x2804: 0x94C60064, 0x280C: 0x24070001, 0x281C: 0x1840FFCB,
            0x2854: 0x24630001, 0x2858: 0x14620006, 0x28A0: 0x27BD0128,
        }
        widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
        with path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                self.assertNotIn(0x0C000000 | ((base + 0x2258) >> 2 & 0x3FFFFFF),
                                 struct.unpack("<3685I", data[4:0x3998]))
                row = self.instances[module["name"]]
                descriptor = 0x3A94 + int(row["command_word"]) % 1000 * 68
                self.assertEqual(struct.unpack_from("<H", data, descriptor + 0x20)[0], 1)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                self.assertIn("RotTransPers = 0x80087868;", bindings)
                self.assertNotIn("func_french_80087868", bindings)
                for start, end, register, expected in (
                        (4, 0x268, 17, [0x58]), (4, 0x268, 22, [0x14]),
                        (0x2258, 0x28A4, 30, [0x2260, 0x2878]),
                        (0x2258, 0x28A4, 22, [0x22E8, 0x2880]),
                        (0x2258, 0x28A4, 19, [0x22BC, 0x23F4, 0x24D8, 0x2730, 0x2734, 0x2844, 0x288C]),
                        (0x2258, 0x28A4, 29, [0x2258, 0x28A0])):
                    writes = []
                    for offset in range(start, end, 4):
                        word, = struct.unpack_from("<I", data, offset)
                        op = word >> 26
                        destination = word >> 11 & 31 if op == 0 else word >> 16 & 31 if op in (
                            8, 9, 10, 11, 12, 13, 14, 15, 32, 33, 35, 36, 37) else None
                        if destination == register:
                            writes.append(offset)
                    self.assertEqual(writes, expected)
                words = struct.unpack("<403I", data[0x2258:0x28A4])
                for register, maximum in ((30, 0x1BB2), (29, 296), (22, 28)):
                    accesses = [(word & 0xFFFF) + widths[word >> 26] for word in words
                                if word >> 26 in widths and word >> 21 & 31 == register and not word & 0x8000]
                    self.assertEqual(max(accesses), maximum)
                self.assertFalse(any(word >> 26 in (32, 33, 35, 36, 37)
                                     and word >> 21 & 31 == 29 and word & 0xFFFF == 212 for word in words))
                self.assertEqual(
                    [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3],
                    [0x8005C018, *([0x80089928] * 4), 0x800866F8, 0x80086628, 0x800866F8,
                     0x80086628, 0x80087CB8, 0x800875F8, 0x80086258, 0x80085558,
                     0x80087958, 0x80087958, 0x80087868, 0x80089928, 0x800866F8, 0x80086628, 0x8004D5B8])


class SpanishModelSpiralDescriptorTests(family435.FrenchModelSpiralDescriptorTests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
