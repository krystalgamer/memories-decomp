import json
import struct
import unittest

from tools.project.tests import test_french_model_variant418 as family418
from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant433Tests(family418.FrenchModelVariant418Tests):
    family = 433
    source_family = 416
    module_count = 4
    distinct_images = 4
    tail_start = 0x30B8
    spans = ((4, 0x1050), (0x1050, 0x1810), (0x1810, 0x1C64),
             (0x1C64, 0x21CC), (0x21CC, 0x26C8), (0x26C8, 0x29D8),
             (0x29D8, 0x2D54), (0x2D54, 0x30B8))
    standalone_helpers = frozenset({"strips", "sheet"})
    helpers = ((0x1050, 1984, "strips", "func_8013C050"),
               (0x1810, 1108, "sheet", "func_8013C810"),
               (0x1C64, 1384, "webs", "func_8013CC68"),
               (0x26C8, 784, "spokes", "func_8013D6D4"),
               (0x29D8, 892, "rings", "func_8013D9E8"),
               (0x2D54, 868, "quad", "func_8013DD68"))
    local_call_targets = {0x1050, 0x1810, 0x1C64, 0x21CC}
    reachable_helpers = {0x1050, 0x1810, 0x1C64}
    models_by_stage = ((7, (180, 440)),)
    entry_anchors = {
        offset + (0x24 if offset >= 0x498 else 0): word
        for offset, word in family418.FrenchModelVariant418Tests.entry_anchors.items()
    }
    entry_anchors.update({
        0x20: 0x26D810B0, 0x28: 0x26D81278, 0x30: 0x26D815D8,
        0x38: 0x26D81818, 0x7A4: 0x2AE20003,
    })
    entry_anchors.update({
        0x40: 0x26D80A80, 0x5C: 0xAFB80094,
        0xB0: 0x00181840, 0xB4: 0x00781821, 0xB8: 0x00031900, 0xC0: 0xAEC31A60,
        0x7B0: 0x8FB80094, 0x7BC: 0x2714019C,
        0x89C: 0x265000C0, 0x8D8: 0x26520008, 0x8F4: 0x2A620006,
        0x920: 0x27180030, 0x954: 0x2B020004,
        0x97C: 0xA282FFE4, 0x998: 0xA283FFE5, 0x9A4: 0x271801A0,
        0x9BC: 0xAE80FFFC, 0x9C4: 0xA283FFE6, 0x9D4: 0xAE82FFF8,
        0x9D8: 0x2AE20003, 0x9E0: 0x269401A0, 0xC8C: 0xAEC01A84,
        0x1C6C: 0x00809821, 0x1C70: 0x26680A80, 0x1C9C: 0xAFA800D8,
        0x1CB4: 0x267119E4, 0x1CF4: 0x26720C14, 0x1CF8: 0x8E621A84,
        0x1D10: 0x28621000, 0x1D2C: 0x28820801, 0x1D78: 0x28821801,
        0x1E0C: 0x8E621A0C, 0x1E18: 0x8E621A10, 0x1E24: 0x8E621A14,
        0x1E30: 0x86621A18, 0x1E3C: 0x86621A1A, 0x1E48: 0x86621A1C,
        0x1EF4: 0x0003B100, 0x1EFC: 0x26C200C0, 0x1F0C: 0x00052B43,
        0x1F18: 0x3C025000, 0x1F58: 0x0C021E56,
        0x1FA8: 0x04C0000A, 0x1FB8: 0x04400006,
        0x1FE0: 0x28420006, 0x1FFC: 0x28420004,
        0x2028: 0x8E641A60, 0x202C: 0x8E621A50, 0x2030: 0x8C85001C,
        0x2044: 0x8C820020, 0x2048: 0x00031B00, 0x2050: 0x0062001B,
        0x2074: 0x00021103, 0x2088: 0x2463F000, 0x2094: 0xAE420000,
        0x20B8: 0x000210C3, 0x20CC: 0x00021023,
        0x20EC: 0x8E621A58, 0x20F4: 0x00021200,
        0x2164: 0xAE621A84, 0x2170: 0x265201A0,
        0x2180: 0x252901A0, 0x2190: 0x28420003,
    })

    entry_anchors.update({
        0x18: 0x26D80F60, 0x1C: 0xAFB80080, 0x3B8: 0x00009821, 0x3BC: 0x8FB80080,
        0x3D4: 0x03001821, 0x474: 0xAC660068, 0x478: 0x24C6FF00, 0x480: 0x26730001,
        0x484: 0x2A620002, 0x48C: 0x24630004, 0x490: 0x24E7FE00, 0x494: 0x25080001,
        0x4A4: 0x24A500A8, 0x4AC: 0x29020002, 0x4B0: 0x271800A8, 0x4B8: 0xAFB80080,
        0x710: 0x28820004, 0x72C: 0xA062FFF0, 0x740: 0xA062FFF1, 0x754: 0xA062FFF2,
        0x768: 0xA062FFF4, 0x77C: 0xA062FFF5, 0x7A0: 0xA062FFF6, 0x7AC: 0x24630098,
        0xC7C: 0xAEC01A4C, 0xC88: 0xAEC01A58, 0xE7C: 0x8EC21A60, 0xE84: 0x8C430020,
        0xE88: 0x8EC21A50, 0xE90: 0x0043102B, 0xE94: 0x1440000F, 0xE9C: 0x8EC21A84,
        0xEA4: 0x28420005, 0xEA8: 0x10400003, 0xEB4: 0x02602021, 0xEF0: 0x8EC21A4C,
        0xEF8: 0x24420001, 0xF00: 0xAEC21A4C, 0xF30: 0xAEC21A58, 0x1810: 0x27BDFEF8,
        0x1818: 0x00809021, 0x1820: 0x265E0F60, 0x1828: 0x265510B0, 0x184C: 0x2651191C,
        0x1850: 0x0000B821, 0x1858: 0x26501138, 0x1860: 0x8E421A4C, 0x1868: 0x30420001,
        0x1874: 0x8E020000, 0x1880: 0x000218C3, 0x1884: 0x24420007, 0x1888: 0x000218C3,
        0x188C: 0x24020002, 0x1898: 0x16E20017, 0x18A0: 0x86421A18, 0x18AC: 0x86421A1A,
        0x18B8: 0x86421A1C, 0x18F8: 0x8FC30068, 0x1900: 0x18600006, 0x1904: 0x00002021,
        0x1908: 0x24040400, 0x190C: 0x0064102A, 0x1918: 0x00602021, 0x191C: 0x8E421A20,
        0x1924: 0x00440018, 0x1934: 0x246303FF, 0x1938: 0x8E421A0C, 0x193C: 0x00031A83,
        0x1948: 0x8E421A24, 0x1964: 0x8E421A10, 0x1974: 0x8E421A28, 0x1990: 0x8E421A14,
        0x19A0: 0x8E421A84, 0x19A8: 0x28420003, 0x19B0: 0x24020800, 0x19C4: 0xAFA00030,
        0x19C8: 0xAFA00034, 0x19CC: 0xAFA00038, 0x19D4: 0x0C021F2E, 0x1A28: 0x0C021896,
        0x1A30: 0x0C021556, 0x1A38: 0x0C021CAA, 0x1A44: 0x0C021F2E, 0x1A50: 0x0C021D7E,
        0x1A58: 0x0C021DCE, 0x1A6C: 0x001338C0, 0x1A70: 0x24E50020, 0x1A78: 0x24E60040,
        0x1A80: 0x26220008, 0x1A88: 0x26220014, 0x1A90: 0x26220020, 0x1A98: 0x2622002C,
        0x1AA0: 0x27A200D0, 0x1AA8: 0x27A200D4, 0x1AAC: 0x24E70060, 0x1AB4: 0x0C021E56,
        0x1ABC: 0x9203FFFC, 0x1AF0: 0x3C046666, 0x1AFC: 0x34846667, 0x1B08: 0x000210C0,
        0x1B28: 0x9203FFF8, 0x1B54: 0x04600008, 0x1B64: 0x04400004, 0x1B74: 0x3066FFFF,
        0x1B7C: 0x2A620004, 0x1B84: 0x26940008, 0x1B88: 0x24020002, 0x1B8C: 0x16E20023,
        0x1B94: 0x8E431A84, 0x1B9C: 0x14770010, 0x1BA8: 0x24047FFF, 0x1BB8: 0x8E421A58,
        0x1BC0: 0x00021300, 0x1BD4: 0x34028000, 0x1BE0: 0x1462000E, 0x1BF0: 0x1860000A,
        0x1BF8: 0x8E421A58, 0x1C00: 0x00021200, 0x1C10: 0x24020004, 0x1C14: 0xAE000000,
        0x1C18: 0xAE421A84, 0x1C1C: 0x26F70001, 0x1C20: 0x26100098, 0x1C24: 0x26B50098,
        0x1C28: 0x2AE20003, 0x1C30: 0x27DE00A8,
    })

    def test_sheet_companion_records_and_entry_gate(self):
        self.assertEqual(0xF60 + 2 * 0xA8, 0x10B0)
        self.assertEqual(0x10B0 + 3 * 0x98, 0x1278)
        archive_path = family435.ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        with archive_path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<II", data, 0xEB0),
                                 (0x0C000000 | ((base + 0x1810) >> 2 & 0x3FFFFFF),
                                  0x02602021))
                self.assertEqual(struct.unpack_from("<I", data, 0x1810)[0], 0x27BDFEF8)
                self.assertEqual(struct.unpack_from("<I", data, 0xE84)[0], 0x8C430020)
                self.assertEqual(struct.unpack_from("<I", data, 0xEA4)[0], 0x28420005)

    def test_webs_selected_timing_descriptors_and_binding(self):
        root = family435.ROOT
        archive_path = root / f"game/{self.region}/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        with archive_path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x31B4
                self.assertEqual(struct.unpack_from("<II", data, 0xA0),
                                 (0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF),
                                  0x24420000 | (table & 0xFFFF)))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, {180: 599001, 440: 599000}[int(row["model"])])
                self.assertEqual(command, int(row["command_word"]))
                descriptor = 0x31B4 + command % 1000 * 48
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 48, len(data))
                timing = struct.unpack_from("<II", data, descriptor + 0x1C)
                self.assertEqual(timing, (0, 76 if int(row["model"]) == 180 else 26))
                self.assertGreater(timing[1], timing[0])
                name = "ratan2" if self.region == "france" else "func_french_80089928"
                self.assertIn(f"{name} = 0x80089928;", (root / module["linker_symbols"]).read_text())


class FrenchModelVariant433StripTests(unittest.TestCase):
    def test_strip_source_preserves_guest_pointer_and_counter_views(self):
        source = (family435.ROOT / "src/overlays/french_model_variant/variant433_strips.c").read_text()
        header = (family435.ROOT / "src/overlays/french_model_variant/variant433_strips.h").read_text()
        self.assertIn('#include "variant433_strips.h"', source)
        self.assertIn("*(u8 *G32 *)(work + 0x1A60)", source)
        self.assertIn("s32 done[2];", header)
        self.assertNotIn("strip->done[2]", source)
        self.assertIn("done *= MODEL_VARIANT_WORD(column, 0x70);", source)
        self.assertIn("MODEL_VARIANT_WORD(column, 0x68) -= decrement;", source)
        self.assertIn("PSXLONG flag[2][2];", source)
        self.assertIn("radius = raw_radius < 0 ? (u32)(raw_radius + 255) >> 8 : (u32)raw_radius >> 8;", source)
        for raw in range(-32768, 32768):
            shifted = (((raw + 255) if raw < 0 else raw) & 0xFFFFFFFF) >> 8
            narrowed = shifted & 0xFFFF
            if narrowed >= 32768:
                narrowed -= 65536
            expected = -((-raw) // 256) if raw < 0 else raw // 256
            self.assertEqual(narrowed, expected)

    def test_strip_retail_layout_rounding_reset_and_entry_gate(self):
        root = family435.ROOT
        modules = [m for m in json.loads((root / "config/sles_03948/overlays.json").read_text())["modules"]
                   if m["linker_symbols"].endswith("/model_variant433_linker_symbols.txt")]
        self.assertEqual(len(modules), 4)
        archive_path = root / "game/france/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest("legal French MODEL input required")
        anchors = {
            0x3A0: 0x00004021, 0x3B8: 0x00009821, 0x474: 0xAC660068,
            0x47C: 0xAC600070, 0x484: 0x2A620002, 0x48C: 0x24630004,
            0x4AC: 0x29020002, 0x4B0: 0x271800A8,
            0xE84: 0x8C430020, 0xE90: 0x0043102B, 0xEA4: 0x28420005,
            0xEB8: 0x8EC21A84, 0xEC0: 0x28420003, 0xEC4: 0x10400003, 0xED0: 0x02602021,
            0x1050: 0x27BDFED8, 0x10A0: 0xAFA200EC, 0x10A4: 0x86621A78,
            0x10AC: 0x04410002, 0x10B0: 0x267118E8, 0x10B4: 0x244200FF,
            0x10B8: 0x00021202, 0x10BC: 0x26740F60, 0x10C0: 0x00021400,
            0x10C4: 0x00021403, 0x10FC: 0x8C430068, 0x1360: 0xAC8200A0,
            0x13F0: 0x8E621A60, 0x13F8: 0x8C430028, 0x13FC: 0x8E621A50,
            0x1404: 0x0043102B, 0x1428: 0xAC430070, 0x142C: 0x0000A821,
            0x1450: 0x00042400, 0x145C: 0x28840002, 0x1468: 0xACA20068,
            0x146C: 0x00151400, 0x1470: 0x00021383, 0x1474: 0x02821021,
            0x1478: 0x8C420070, 0x14D8: 0xAE621A84,
            0x1548: 0x94A20030, 0x1578: 0x94A20038, 0x15A8: 0x90A20078,
            0x15F0: 0x90A20080, 0x1638: 0x8CA200A0, 0x1680: 0x94A20040,
            0x16B0: 0x94A20038, 0x1770: 0x8CA200A0,
            0x17D4: 0x28420002, 0x17DC: 0x269400A8, 0x180C: 0x27BD0128,
        }
        self.assertEqual(0xF60 + 2 * 168, 0x10B0)
        self.assertEqual(0x70 + 2 * 4, 0x78)
        with archive_path.open("rb") as archive:
            for module in modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                base = int(module["load_address"], 0)
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                     (module["name"], hex(offset)))
                for site, target in ((0xEB0, 0x1810), (0xECC, 0x1050)):
                    self.assertEqual(struct.unpack_from("<II", data, site),
                                     (0x0C000000 | ((base + target) >> 2 & 0x3FFFFFF), 0x02602021))
                model = int(module["name"].split("_")[3])
                descriptor, timing = {180: (0x31E4, (0, 76, 90, 216)),
                                      440: (0x31B4, (0, 26, 70, 250))}[model]
                self.assertEqual(struct.unpack_from("<4I", data, descriptor + 0x1C), timing)
                bindings = (root / module["linker_symbols"]).read_text()
                self.assertIn("rcos = 0x800866F8;", bindings)
                self.assertIn("RotTransPers3 = 0x80087898;", bindings)
