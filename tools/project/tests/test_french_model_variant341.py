import hashlib
import json
import struct
import unittest

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant341Tests(family435.FrenchModelVariant435Tests):
    family = 341
    source_family = 324
    module_count = 4
    distinct_images = 4
    binding_count = 36
    tail_start = 0x2D58
    spans = ((4, 0xF38), (0xF38, 0x1354), (0x1354, 0x17C0),
             (0x17C0, 0x22F8), (0x22F8, 0x26C4), (0x26C4, 0x29D8), (0x29D8, 0x2D58))
    helpers = ((0xF38, 1052, "webs", "func_8013BF3C"),
               (0x1354, 1132, "fan", "func_8013C364"),
               (0x22F8, 972, "draw", "func_8013D2F8"),
               (0x26C4, 788, "spokes", "func_8013D6C8"),
               (0x29D8, 896, "rings", "func_8013D9E0"))
    reachable_helpers = {0xF38, 0x1354, 0x22F8}
    local_call_targets = {0xF38, 0x1354, 0x17C0, 0x22F8}
    models_by_stage = ((7, (7, 552)),)
    entry_anchors = {
        0x0C: 0x00809021, 0x14: 0x0240B021, 0x20: 0x26D806CC, 0x24: 0xAFB80084,
        0x28: 0x26D80764, 0x2C: 0xAFB80088, 0x30: 0x26D80AC4, 0x34: 0xAFB8008C,
        0x38: 0x26D80D04, 0xA8: 0x00181880, 0xAC: 0x00781821,
        0xB0: 0x00031880, 0xB8: 0xAEC30F2C, 0x888: 0xAC60FFF8,
        0x898: 0x24630098, 0x89C: 0x27180098, 0x8A0: 0x1AE0FFA4,
        0x924: 0x2AE20008, 0x960: 0x27180090, 0x9A0: 0x2B020006,
        0x9A8: 0x26520090, 0xA84: 0x27180090, 0xA94: 0x2B020004,
        0xA9C: 0x26520090, 0xAD4: 0xAEC00F50, 0xD88: 0x02402021,
        0x430: 0x2714019C, 0x510: 0x265000C0, 0x54C: 0x26520008,
        0x568: 0x2A620006, 0x594: 0x27180030, 0x5C8: 0x2B020004,
        0x5EC: 0x271801A0, 0x604: 0xA282FFE4, 0x610: 0x2AE20003,
        0x620: 0xAE80FFFC, 0x634: 0xAE83FFF8, 0x63C: 0x269401A0,
        0xDD4: 0x02402021, 0xF38: 0x27BDFED8, 0xF7C: 0x26B10EB0,
        0xFBC: 0x26B20194, 0xFE0: 0x28821801, 0x106C: 0x8E820F50,
        0x1080: 0x8E820ED8, 0x10A4: 0x86820EE4, 0x116C: 0x266200C0,
        0x11A0: 0x27A200D0, 0x11A8: 0x27A200D4, 0x11FC: 0xA228000F,
        0x121C: 0x18C0000A, 0x122C: 0x18400006, 0x1240: 0x30C6FFFF,
        0x1254: 0x28420006, 0x1270: 0x28420004, 0x1290: 0x8E820F24,
        0x1298: 0x00021200, 0x12C8: 0xAE440004, 0x12E8: 0x14840004,
        0x12F4: 0xAE820F50, 0x12FC: 0x265201A0, 0x1318: 0x28420003,
    }

    def test_selected_descriptor_and_context_load_separation(self):
        path = family435.ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest(f"legal {self.region} MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(row["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<I", data)[0], int(row["header"]))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(row["command_word"]))
                table = base + 0x2E54
                self.assertEqual(struct.unpack_from("<I", data, 0x98)[0],
                                 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x9C)[0],
                                 0x24420000 | (table & 0xFFFF))
                descriptor = 0x2E54 + command % 1000 * 20
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 20, len(data))
                self.assertEqual(3 * 416, 0x4E0)
                self.assertEqual(0x6CC + 152, 0x764)
                self.assertEqual(0x764 + 6 * 144, 0xAC4)
                self.assertEqual(0xAC4 + 4 * 144, 0xD04)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (base, 10 * 2048)):
                    self.assertTrue(context + 0xF64 <= start or start + size <= context)


# Spanish341 inherits the family fixture, but not these French-only anchors.
class FrenchModelVariant341FanTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        manifest = family435.ROOT / "config/sles_03948/overlays.json"
        cls.modules = [m for m in json.loads(manifest.read_text())["modules"]
                       if m["linker_symbols"].endswith("/model_variant341_linker_symbols.txt")]

    def _images(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        self.assertEqual(len(self.modules), 4)
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                yield module, data

    def test_single_fan_extent_initializer_and_neighboring_packet(self):
        self.assertEqual(11 * 8, 0x58)
        self.assertEqual(0xD04 + 0x70, 0xD74)
        self.assertEqual(0xD90 + 36, 0xDB4)
        anchors = {
            0x38: 0x26D80D04, 0x40: 0x26D70DB4, 0x50: 0x26D10D74,
            0x58: 0x26D80D90, 0x648: 0x27120068, 0x658: 0xA6800000,
            0x678: 0xA6820008, 0x6A8: 0xA6830030, 0x6C8: 0x2A620005,
            0x6DC: 0x240200FF, 0x6E4: 0x240200C0, 0x6F0: 0x24020040,
            0x6FC: 0xA240FFF6, 0x700: 0xAE40FFF8, 0x704: 0xAE40FFFC,
            0x708: 0xAE400000, 0x710: 0x26520070, 0x718: 0x1AE0FFCC,
        }
        for module, data in self._images():
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                 (module["name"], hex(offset)))

    def test_caller_substep_loop_and_last_substep_scale_gate(self):
        for module, data in self._images():
            base = int(module["load_address"], 0)
            self.assertEqual(struct.unpack_from("<H", data, 0x2E54 + 12)[0], 2)
            self.assertEqual(struct.unpack_from("<I", data, 0x2E54 + 16)[0], 50)
            self.assertEqual(struct.unpack_from("<I", data, 0xD54)[0],
                             0x0C000000 | ((base + 0x1354) >> 2 & 0x3FFFFFF))
            for offset, word in {
                0xC30: 0xA6C00F40, 0xC4C: 0x9462000C, 0xC54: 0x10400059,
                0xD14: 0x02402021, 0xD24: 0x02402021, 0xD94: 0x96C20F40,
                0xD9C: 0x24420001, 0xDA0: 0xA6C20F40, 0xDAC: 0x00021403,
                0xDB0: 0x0043102A, 0xDB4: 0x1440FFA9,
                0x16C0: 0x8E840F2C, 0x16C4: 0x86820F40,
                0x16C8: 0x9483000C, 0x16CC: 0x24420001, 0x16D0: 0x14430026,
            }.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                 (module["name"], hex(offset)))

    def test_projection_visibility_and_phase_angle_updates(self):
        anchors = {
            0x1354: 0x27BDFF00, 0x139C: 0x26910D64, 0x13A8: 0x30420001,
            0x13B4: 0x8E230000, 0x13C0: 0x96820F4C,
            0x14A8: 0x26440008, 0x14B0: 0x02602821,
            0x14B4: 0x26460010, 0x14BC: 0x26470018,
            0x14E4: 0x27A200D0, 0x14EC: 0x27A200D4,
            0x1584: 0x04C00009, 0x1594: 0x04400005, 0x15A0: 0x30C6FFFF,
            0x15AC: 0x26440030, 0x15B8: 0x26460038, 0x15C0: 0x26470040,
            0x16B0: 0x26B50002, 0x16B4: 0x2AA20004,
            0x16D8: 0x8E820F50, 0x16FC: 0x8E820F1C, 0x1700: 0x8C830010,
            0x1704: 0x00021300, 0x1708: 0x0043001B, 0x173C: 0xAE820F50,
            0x1758: 0x00021140, 0x1768: 0xAE200000,
            0x1774: 0x1AE0FF0A, 0x1784: 0x00021140, 0x178C: 0xAE830F4C,
        }
        for module, data in self._images():
            for offset, word in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                 (module["name"], hex(offset)))
