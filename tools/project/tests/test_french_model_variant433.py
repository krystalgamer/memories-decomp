import struct

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
    helpers = ((0x1C64, 1384, "webs", "func_8013CC68"),
               (0x26C8, 784, "spokes", "func_8013D6D4"),
               (0x29D8, 892, "rings", "func_8013D9E8"),
               (0x2D54, 868, "quad", "func_8013DD68"))
    local_call_targets = {0x1050, 0x1810, 0x1C64, 0x21CC}
    reachable_helpers = {0x1C64}
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
                self.assertIn("ratan2 = 0x80089928;", (root / module["linker_symbols"]).read_text())
