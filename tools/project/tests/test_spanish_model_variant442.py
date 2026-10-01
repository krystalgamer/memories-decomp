import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant442 as family442
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant442Tests(family442.FrenchModelVariant442Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    source_directories = {"ribbons": "spanish_model_variant"}
    standalone_helpers = frozenset({"ribbons"})
    helpers = ((0x1174, 1004, "sheet", "func_8013C178"),
               (0x2924, 964, "webs", "func_8013D8F8"),
               (0x2CE8, 1612, "ribbons", "func_8013DCE8"),
               (0x3334, 1804, "bands", "func_8013E2F4"),
               (0x3A40, 784, "spokes", "func_8013EA00"),
               (0x3D50, 892, "rings", "func_8013ED14"),
               (0x40CC, 868, "quad", "func_8013F094"))

    def test_ribbon_indexed_first_edge_and_packet_boundaries(self):
        source = (family435.ROOT / "src/overlays/spanish_model_variant/variant442_ribbons.c").read_text()
        self.assertIn("dx = (s16)ribbon->sa[k + 1] - (s16)ribbon->sa[0];", source)
        self.assertIn("dy = (ribbon->sa[k + 1] >> 16) - (ribbon->sa[0] >> 16);", source)
        self.assertIn("#define RotTransPers func_french_80087868", source)
        self.assertEqual(0x1940 + 8 * 108, 0x1CA0)
        self.assertEqual(0x2530 + 28, 0x254C)

    def test_bindings_cover_fallback_functions_as_well_as_c(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)

    def test_actual_descriptor_arithmetic_and_minimum_context(self):
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
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                self.assertEqual(struct.unpack_from("<5I", data, 0xB8),
                                 (0x001818C0, 0x00781823, 0x000318C0, 0x00621821, 0xAEC32714))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110)
                request, = struct.unpack("<i", archive.read(4))
                self.assertEqual(request, 608000)
                self.assertEqual(request, int(row["command_word"]))
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack("<1116I", data[4:0x1174])
                            if word >> 26 in widths and word >> 21 & 31 == 22
                            and word & 0x8000 == 0]
                self.assertEqual(max(accesses), 0x2760)
                context = pointers[9 + slot]
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048), (base, 10 * 2048)):
                    self.assertTrue(context + max(accesses) <= start or start + size <= context)

    def test_sheet_record_packet_and_frame_boundaries(self):
        archive_path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with archive_path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])

                def immediate(offset):
                    return struct.unpack_from("<I", data, offset)[0] & 0xFFFF

                sheet, stride = immediate(0x1184), immediate(0x1524)
                self.assertEqual((sheet, stride), (0x1E68, 152))
                self.assertEqual(sheet + stride, immediate(0x28))
                self.assertEqual(immediate(0x11BC) - sheet, 136)
                corners, vertex_stride = immediate(0x13E8), immediate(0x13F0)
                self.assertEqual((corners, vertex_stride), (4, 8))
                self.assertEqual(corners * vertex_stride, immediate(0x12DC))
                self.assertEqual(immediate(0x12E4), 2 * corners * vertex_stride)
                self.assertEqual(immediate(0x1318), 3 * corners * vertex_stride)
                packet, packet_bytes = immediate(0x11B0), immediate(0x28C)
                self.assertEqual((packet, packet_bytes), (0x25A4, 52))
                self.assertEqual(immediate(0x50) + packet_bytes, packet)
                self.assertEqual(packet + packet_bytes, immediate(0x70))
                frame = 0x10000 - immediate(0x1174)
                self.assertEqual(frame, 256)
                self.assertEqual(immediate(0x155C), frame)
                p, flag = immediate(0x130C), immediate(0x1314)
                self.assertEqual((p, flag), (208, 212))
                self.assertEqual(flag + 4 + 10 * 4, frame)

    def test_actual_sheet_caller_timing_and_sort_gates(self):
        archive_path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest("legal Spanish MODEL input required")
        anchors = {
            0x94: 0x04A00329, 0x28C: 0x26F70034,
            0x290: 0x0C020BBA, 0x294: 0x02E02021,
            0xFC8: 0x8C83001C, 0xFD4: 0x0043102B,
            0xFD8: 0x1440000E, 0xFE4: 0x02602021,
            0x1174: 0x27BDFF00, 0x117C: 0x00809021,
            0x1184: 0x26551E68, 0x11B0: 0x265125A4,
            0x11F8: 0x8E4226A8, 0x1204: 0x8E4226AC, 0x1210: 0x8E4226B0,
            0x130C: 0x27A200D0, 0x1314: 0x27A200D4,
            0x135C: 0x3C046666, 0x1368: 0x34846667,
            0x1374: 0x000210C0, 0x1380: 0x00440018,
            0x13A4: 0x000217C3, 0x13B8: 0x00081883, 0x13BC: 0x00621823,
            0x13C0: 0x04600008, 0x13D0: 0x04400004, 0x13E0: 0x3066FFFF,
            0x13E8: 0x2A620004, 0x1434: 0x0043001B, 0x1440: 0x0007000D,
            0x1478: 0x0064102B, 0x1490: 0x0062001B, 0x149C: 0x0007000D,
            0x14B4: 0xAE000000, 0x14C8: 0x24020004, 0x14CC: 0xAE422748,
            0x14E0: 0x8E432734, 0x14FC: 0x00021100,
            0x1510: 0x24020600, 0x1518: 0x24020002, 0x151C: 0xAE422748,
            0x1524: 0x26100098, 0x1528: 0x1AE0FF25, 0x155C: 0x27BD0100,
        }
        calls = [0x8005C018, 0x80087CB8, 0x80086258, 0x80085558, 0x800872A8,
                 0x80087CB8, 0x800875F8, 0x80087738, 0x80087958, 0x800842A8]
        with archive_path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                     (module["name"], hex(offset)))
                self.assertEqual(struct.unpack_from("<I", data, 0xFE0)[0],
                                 0x0C000000 | ((base + 0x1174) >> 2 & 0x3FFFFFF))
                words = struct.unpack("<251I", data[0x1174:0x1560])
                self.assertEqual([0x80000000 | ((word & 0x3FFFFFF) << 2)
                                  for word in words if word >> 26 == 3], calls)
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110)
                request, = struct.unpack("<i", archive.read(4))
                self.assertEqual(request, 608000)
                descriptor = 0x452C + request % 1000 * 56
                self.assertEqual(struct.unpack_from("<7I", data, descriptor + 28),
                                 (44, 80, 160, 280, 320, 440, 800))
