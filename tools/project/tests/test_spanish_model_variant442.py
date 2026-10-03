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
    binding_count = 37
    source_directories = {"ribbons": "spanish_model_variant"}
    standalone_helpers = frozenset({"ribbons"})
    helpers = ((0x1174, 1004, "sheet", "func_8013C178"),
               (0x1560, 2508, "spiral", "func_8013C568"),
               (0x1F2C, 2552, "rays", "func_8013CF14"),
               (0x2924, 964, "webs", "func_8013D8F8"),
               (0x2CE8, 1612, "ribbons", "func_8013DCE8"),
               (0x3334, 1804, "bands", "func_8013E2F4"),
               (0x3A40, 784, "spokes", "func_8013EA00"),
               (0x3D50, 892, "rings", "func_8013ED14"),
               (0x40CC, 868, "quad", "func_8013F094"))

    def test_rays_growth_and_sdk_output_alias_order(self):
        archive_path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        resident_path = family435.ROOT / "game/spain/SLES_039.51"
        if not archive_path.exists() or not resident_path.exists():
            self.skipTest("legal Spanish MODEL and resident inputs required")
        anchors = {
            0xCDC: 0xAEC0273C,
            0x28A8: 0x8EE3273C, 0x28AC: 0, 0x28B0: 0x28621000,
            0x28B4: 0x1040000F, 0x28B8: 0, 0x28BC: 0x8EE22700,
            0x28C0: 0, 0x28C4: 0x00021200, 0x28C8: 0x00621021,
            0x28CC: 0xAEE2273C, 0x28D0: 0x28421000, 0x28D4: 0x14400007,
            0x28D8: 0x24021000, 0x28DC: 0x8EE32748, 0x28E0: 0xAEE2273C,
            0x28E4: 0x24020002, 0x28E8: 0x14620002, 0x28EC: 0x24020003,
            0x28F0: 0xAEE22748,
        }
        with archive_path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                     (module["name"], hex(offset)))
        resident = resident_path.read_bytes()
        text_base = struct.unpack_from("<I", resident, 0x18)[0]
        function_offset = 0x800 + 0x80087958 - text_base
        for offset, word in {
            0x50: 0x8FA90020, 0x54: 0x8FAA0024,
            0x5C: 0xE9280000, 0x6C: 0xAD480000,
        }.items():
            self.assertEqual(struct.unpack_from("<I", resident, function_offset + offset)[0], word)
        self.assertEqual(0xD0 + 15 * 8 + 2 * 4, 0x150)

    def test_spiral_status_frame_and_original_context_caller(self):
        path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        anchors = {
            0xC: 0x00809821, 0x14: 0x0260B021,
            0x20: 0x26D81E68, 0x24: 0xAFB80084,
            0x50: 0x26D72570, 0x23C: 0x0C020BBA, 0x240: 0x02E02021,
            0x5A4: 0x8FB80084, 0x5B0: 0x27030090, 0x76C: 0xAC60FFF8,
            0xCD8: 0xAEC02738, 0xCE0: 0xA6C02708, 0xCF0: 0xAEC02710,
            0xD10: 0xAEC026F4, 0xD1C: 0xAEC02700, 0xD20: 0xAEC02748,
            0xFBC: 0x8EC42714, 0xFC8: 0x8C83001C, 0xFCC: 0x8EC226F8,
            0xFD4: 0x0043102B, 0xFD8: 0x1440000E, 0xFEC: 0x02602021,
            0x1038: 0x0C016FC9, 0x104C: 0x0C016FC9, 0x1054: 0xAEC22700,
            0x1560: 0x27BDFE48, 0x1860: 0x27A40028, 0x1864: 0x27B00040,
            0x186C: 0x27B70150, 0x1874: 0x27A900D0, 0x1890: 0xAFA90180,
            0x18CC: 0x27A50030, 0x18D0: 0x27A40080, 0x18D4: 0x27B00060,
            0x194C: 0x00021343, 0x1954: 0x24490004, 0x1958: 0xAFA20184,
            0x195C: 0xAFA90188, 0x199C: 0xAFB70020, 0x19A8: 0xAFAA0024,
            0x19B8: 0x27A70154, 0x1A7C: 0xAFB70020, 0x1A88: 0xAFA20024,
            0x1AA0: 0x27A70154, 0x1F28: 0x27BD01B8,
        }
        with path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                     (module["name"], hex(offset)))
                for offset, target in ((0xFE0, base + 0x1174), (0xFE8, base + 0x1560),
                                       (0x19A4, 0x80087958), (0x1A84, 0x80087958),
                                       (0x19BC, 0x80087868), (0x1AA8, 0x80087868)):
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0],
                                     0x0C000000 | (target >> 2 & 0x3FFFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x452C + 28)[0], 44)
        self.assertEqual(0xD0 + 16 * 2 * 4, 0x150)
        self.assertEqual(0x150 + 4, 0x154)
        self.assertLessEqual(0x154 + 4, 440)

    def test_spiral_binding_keeps_the_retained_assembly_alias(self):
        for module in self.modules:
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            self.assertIn("RotTransPers = 0x80087868;", bindings)
            self.assertIn("func_french_80087868 = 0x80087868;", bindings)

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
