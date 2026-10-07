import hashlib
import struct
import unittest

from tools.project.tests import test_french_model_variant421 as french
from tools.project.tests import test_spanish_model_variant460 as instructions
from tools.project.tests.test_french_model_variant435 import ROOT
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant421Tests(french.FrenchModelVariant421Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    register_writes = staticmethod(instructions.SpanishModelVariant460Tests.register_writes)
    direct_stores = staticmethod(instructions.SpanishModelVariant460Tests.direct_stores)

    def test_selected_sources_and_assembly_inventory(self):
        # The Spanish palette spiral has its own attempt ledger and test.
        self.helpers = tuple(sorted(french.FrenchModelVariant421Tests.helpers +
                                    ((0x1860, 0xC1C, "spiral", "func_8013C860"),)))
        self.source_directories = {**self.source_directories, "spiral": "spanish_model_variant"}
        self.spanish_helpers = ()
        self.reachable_helpers = self.reachable_helpers | {0x1860}
        super().test_selected_sources_and_assembly_inventory()

    def legal_images(self):
        path = ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                yield module, data

    def test_original_context_reaches_all_five_delay_slot_dispatches(self):
        calls = {0x1024: 0x2C28, 0x102C: 0x310C, 0x1044: 0x11D0, 0x104C: 0x247C, 0x1068: 0x1860}
        for module, data in self.legal_images():
            base = int(module["load_address"], 0)
            word = lambda pc: struct.unpack_from("<I", data, pc)[0]
            self.assertEqual(word(0xC), 0x00809021)
            self.assertEqual(word(0x14), 0x0240B021)
            for pc, target in calls.items():
                self.assertEqual(word(pc), 0x0C000000 | ((base + target) >> 2 & 0x3FFFFFF))
                self.assertEqual(word(pc + 4), 0x02402021)
            definitions = {r: set(self.register_writes(data, 4, 0x11D0, r)) for r in (4, 18)}
            pending, visited, reaching = [(4, None, None)], set(), {}
            while pending:
                pc, root, arg = pending.pop()
                self.assertTrue(4 <= pc < 0x11D0 and pc % 4 == 0)
                if (pc, root, arg) in visited:
                    continue
                visited.add((pc, root, arg))
                if pc in definitions[18]:
                    root = pc
                if pc in definitions[4]:
                    arg = pc
                instruction = word(pc)
                op = instruction >> 26
                if op in (1, 2, 3, 4, 5, 6, 7) or instruction == 0x03E00008:
                    if pc + 4 in definitions[18]:
                        root = pc + 4
                    if pc + 4 in definitions[4]:
                        arg = pc + 4
                    if pc in calls:
                        reaching.setdefault(pc, set()).add((root, arg))
                    if instruction == 0x03E00008:
                        continue
                    if op == 2:
                        targets = [(((base + pc + 4) & 0xF0000000) | ((instruction & 0x3FFFFFF) << 2)) - base]
                    elif op == 3:
                        targets, arg = [pc + 8], None
                    else:
                        displacement = (instruction & 65535) - (65536 if instruction & 32768 else 0)
                        targets = [pc + 8, pc + 4 + displacement * 4]
                    pending.extend((target, root, arg) for target in targets)
                else:
                    pending.append((pc + 4, root, arg))
            self.assertEqual(reaching, {pc: {(0xC, pc + 4)} for pc in calls})
            self.assertEqual(sum(word(pc) >> 26 == 3 for pc in range(4, 0x11D0, 4)), 77)

    def test_ribbon_status_grid_packet_and_original_root(self):
        anchors = {
            0x1260: 0x27D71738, 0x1530: 0x27A20110, 0x153C: 0xAFA20020,
            0x1544: 0x0C021E56, 0x1548: 0xAFA90024, 0x159C: 0x27A20110,
            0x15A8: 0xAFA20020, 0x15B0: 0x0C021E56, 0x15B4: 0xAFA20024,
            0x1604: 0x27A60110, 0x1608: 0x0C021E1A, 0x160C: 0x27A70114,
            0x16B8: 0x28420008, 0x16C0: 0x2694006C, 0x1700: 0x94A30064,
            0x1714: 0x94A20068, 0x1798: 0x0440000D, 0x17A0: 0x00F11021,
            0x17A4: 0x03A21021, 0x17A8: 0x8C4200D0, 0x17B0: 0x04400007,
            0x17C0: 0x94C6005C, 0x17C4: 0x0C01356E, 0x17C8: 0x24070001,
            0x17D8: 0x1840FFC2, 0x17F8: 0x28420008, 0x1800: 0x2694006C,
            0x1808: 0x87C3194C, 0x180C: 0x94420018, 0x1824: 0x00021140,
            0x182C: 0xAFC3195C,
        }
        fields = {(offset, 1) for offset in (4, 5, 6, 12, 13, 14, 20, 21, 22)}
        fields |= {(offset, 2) for offset in (8, 10, 16, 18, 24, 26)}
        for _, data in self.legal_images():
            for pc, expected in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], expected)
            self.assertEqual(self.register_writes(data, 0x11D0, 0x1860, 30), [0x11D8, 0x1834])
            self.assertEqual(self.register_writes(data, 0x11D0, 0x1860, 23), [0x1260, 0x1838])
            self.assertEqual({(offset, size) for _, offset, size in self.direct_stores(data, 0x11D0, 0x1860, 23)}, fields)
            self.assertEqual(self.direct_stores(data, 0x11D0, 0x1860, 30), [(0x182C, 0x195C, 4)])
        self.assertEqual(208 + 8 * 2 * 4, 272)
        self.assertEqual(272 + 4, 276)

    def test_complete_register_write_sets_for_all_nine_functions(self):
        rows = (
            (0x4, 0x11D0, (
                [0x248, 0x24C, 0x2E4, 0x388, 0x550, 0x648, 0xA08, 0xB5C, 0xBB4, 0xCDC, 0xD3C, 0xFA0, 0xFC4, 0x1088, 0x11C4],
                [0x58, 0x2C0, 0x524, 0x588, 0x9A4, 0xA68, 0xB60, 0xBAC, 0xCE0, 0xD34, 0xF8C, 0xFC8, 0x11C0],
                [0xC, 0x264, 0x518, 0x630, 0x9A8, 0xA44, 0xB58, 0xC44, 0xCD8, 0xD9C, 0x11BC],
                [0x21C, 0x450, 0x4D0, 0x51C, 0x584, 0x93C, 0x9A0, 0xA00, 0xAA8, 0xB54, 0xBF0, 0xC5C, 0xCA4, 0xCD4, 0xDA0, 0x11B8],
                [0x234, 0x520, 0x580, 0x928, 0xB48, 0x11B4], [0x270, 0x510, 0x994, 0x11B0],
                [0x14, 0x11AC], [0x48, 0x320, 0x508, 0x960, 0x11A8], [0x4, 0x11CC],
                [0x50, 0x3C4, 0x92C, 0xAA0, 0xB4C, 0xBEC, 0xCCC, 0xD64, 0x11A4])),
            (0x11D0, 0x1860, (
                [0x12C8, 0x12CC, 0x1428, 0x1484, 0x1510, 0x15B8, 0x15F8, 0x161C, 0x16CC, 0x1854],
                [0x12B0, 0x12B4, 0x132C, 0x1508, 0x15E4, 0x15E8, 0x164C, 0x1650, 0x16DC, 0x1850],
                [0x12A8, 0x15E0, 0x184C], [0x12A0, 0x1564, 0x15DC, 0x1848],
                [0x1234, 0x136C, 0x1448, 0x16C0, 0x16C4, 0x1800, 0x1844],
                [0x1290, 0x1340, 0x14E0, 0x167C, 0x16D4, 0x17D0, 0x1840],
                [0x1298, 0x1468, 0x169C, 0x183C], [0x1260, 0x1838], [0x11D0, 0x185C], [0x11D8, 0x1834])),
            (0x1860, 0x247C, (
                [0x1A5C, 0x1A78, 0x1A7C, 0x1A90, 0x1AEC, 0x1AF0, 0x1AF4, 0x1BA0, 0x1C0C, 0x1D8C, 0x1DDC, 0x2054, 0x2470],
                [0x1A0C, 0x1AB4, 0x1AE0, 0x1AE4, 0x1CA8, 0x1DC4, 0x1E3C, 0x1E40, 0x1EBC, 0x246C],
                [0x1A48, 0x1B14, 0x1BD8, 0x1E90, 0x2468], [0x18B8, 0x2464],
                [0x19E8, 0x19F4, 0x19FC, 0x1A04, 0x1C9C, 0x2460],
                [0x1968, 0x1B50, 0x1BC8, 0x1EB4, 0x1EC4, 0x23C0, 0x245C],
                [0x1990, 0x1B28, 0x1C68, 0x1E78, 0x204C, 0x2390, 0x2458],
                [0x19D4, 0x1BA8, 0x2454], [0x1860, 0x2478], [0x1938, 0x1944, 0x1950, 0x1C78, 0x2450])),
            (0x247C, 0x2C28, (
                [0x2584, 0x2588, 0x25E4, 0x27A8, 0x27AC, 0x27C8, 0x2824, 0x2888, 0x2C1C],
                [0x2580, 0x2870, 0x2C18], [0x24C0, 0x2C14], [0x2484, 0x2C10],
                [0x254C, 0x284C, 0x2850, 0x2B0C, 0x2C0C], [0x255C, 0x298C, 0x2C08],
                [0x2568, 0x2820, 0x2858, 0x2AE4, 0x2C04], [0x2558, 0x2860, 0x2C00],
                [0x247C, 0x2C24], [0x2550, 0x2840, 0x2854, 0x2B00, 0x2BFC])),
            (0x2C28, 0x310C, (
                [0x2C70, 0x30CC, 0x3100], [0x2C30, 0x30FC], [0x2C64, 0x30F8],
                [0x2DB0, 0x2F64, 0x30F4], [0x2DC0, 0x2F70, 0x30F0], [0x2C38, 0x30D8, 0x30EC],
                [0x2C6C, 0x30E8], [0x2C68, 0x30C8, 0x30E4], [0x2C28, 0x3108], [0x2C60, 0x30E0])),
            (0x310C, 0x367C, (
                [0x3304, 0x332C, 0x338C, 0x3480, 0x3670], [0x31A0, 0x3620, 0x366C],
                [0x3160, 0x3668], [0x3114, 0x3664], [0x3200, 0x3288, 0x3294, 0x3660],
                [0x31F8, 0x327C, 0x3290, 0x365C], [0x33A0, 0x3658], [0x31F4, 0x3260, 0x328C, 0x3654],
                [0x310C, 0x3678], [0x3388, 0x349C, 0x3650])),
            (0x367C, 0x398C, (
                [0x36CC, 0x3940, 0x3980], [0x3698, 0x397C], [0x3730, 0x38F0, 0x3978],
                [0x3724, 0x38E4, 0x3974], [0x3684, 0x3970], [0x36C0, 0x396C],
                [0x3690, 0x3948, 0x3968], [0x36C8, 0x3964], [0x367C, 0x3988], [0x36C4, 0x3960])),
            (0x398C, 0x3D08, (
                [0x39D8, 0x3CC8, 0x3CFC], [0x39A4, 0x3CF8], [0x3994, 0x3CF4],
                [0x3A00, 0x3BFC, 0x3CF0], [0x39F4, 0x3BF0, 0x3CEC], [0x39CC, 0x3CE8],
                [0x39C8, 0x3CC4, 0x3CE4], [0x399C, 0x3CD4, 0x3CE0], [0x398C, 0x3D04], [0x39D4, 0x3CDC])),
            (0x3D08, 0x406C, (
                [0x3D48, 0x4038, 0x4060], [0x3D20, 0x405C], [0x3D10, 0x4058],
                [0x3D44, 0x4054], [0x3D18, 0x4040, 0x4050], [0x3D40, 0x4034, 0x404C],
                [0x3D3C, 0x4048], [], [0x3D08, 0x4068], [])),
        )
        for _, data in self.legal_images():
            for start, end, sets in rows:
                for register, expected in zip((*range(16, 24), 29, 30), sets, strict=True):
                    self.assertEqual(self.register_writes(data, start, end, register), expected)

    def test_all_frames_and_stage_specific_descriptors(self):
        frames = ((4, 232), (0x11D0, 360), (0x1860, 416), (0x247C, 296),
                  (0x2C28, 256), (0x310C, 296), (0x367C, 272), (0x398C, 264), (0x3D08, 248))
        expected = {
            587000: ((126, 130, 200, 238), 64), 587001: ((56, 68, 200, 250), 64),
            587002: ((136, 148, 280, 320), 64), 587003: ((56, 64, 136, 164), 64),
            587004: ((56, 64, 160, 200), 32), 587005: ((224, 236, 320, 360), 64),
        }
        seen = set()
        for module, data in self.legal_images():
            for pc, frame in frames:
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], 0x27BD0000 | ((-frame) & 65535))
            row = self.instances[module["name"]]
            alternate = (int(row["stage"]) - 7) // 2
            self.assertEqual(alternate, 0 if int(row["model"]) in (84, 162) else 1)
            command = int(row["command_word"])
            descriptor = 0x4168 + command % 1000 * 60
            self.assertEqual(struct.unpack_from("<H", data, descriptor + 0x18)[0], 1)
            self.assertEqual((struct.unpack_from("<4i", data, descriptor + 0x24),
                              struct.unpack_from("<i", data, descriptor + 0x38)[0]), expected[command])
            seen.add(command)
        self.assertEqual(seen, set(expected))

    def test_shared_gt4_and_distinct_line_quad_packet_footprints(self):
        rgb = {(base + channel, 1) for base in (4, 16, 28, 40) for channel in range(3)}
        xy = {(offset, 2) for offset in (8, 10, 20, 22, 32, 34, 44, 46)}
        line = {(0, 4)} | {(offset, 1) for offset in range(12, 18)}
        quad = {(base + channel, 1) for base in (4, 12, 20, 28) for channel in range(3)}
        rows = (
            (0x1860, 0x247C, 19, 0x18B8, 0x25331778, rgb | xy),
            (0x247C, 0x2C28, 18, 0x24C0, 0x26721778, rgb | xy),
            (0x2C28, 0x310C, 18, 0x2C64, 0x263217AC, rgb),
            (0x310C, 0x367C, 18, 0x3160, 0x26721874, line),
            (0x367C, 0x398C, 17, 0x3698, 0x26911874, line),
            (0x398C, 0x3D08, 17, 0x39A4, 0x26511874, line),
            (0x3D08, 0x406C, 17, 0x3D20, 0x26511754, quad),
        )
        for _, data in self.legal_images():
            for start, end, register, pc, setup, fields in rows:
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], setup)
                self.assertEqual({(off, size) for _, off, size in self.direct_stores(data, start, end, register)}, fields)
            stores = self.direct_stores(data, 4, 0x11D0, 29)
            self.assertEqual([(pc, off, size) for pc, off, size in stores if off + size > 232],
                             [(0x80, 236, 4)])

    def test_band_flag_clears_unsigned_division_and_projection_homes(self):
        anchors = {
            0x2560: 0x27A800C8, 0x2564: 0xAFA800FC, 0x27E4: 0x27A200F0,
            0x27F0: 0xAFA20018, 0x2804: 0x8FA800FC, 0x2814: 0x0C021E26,
            0x2818: 0xAFA2001C, 0x2830: 0x28630009, 0x2838: 0xAE0201A4,
            0x2970: 0x04410002, 0x2978: 0xAE2001A4, 0x2990: 0xAEA00000,
            0x299C: 0x04400006, 0x29A4: 0x962601A4, 0x29AC: 0x0C0210AA,
            0x2AB0: 0x04410002, 0x2AB8: 0xAC6001A4, 0x2ABC: 0xAEA00000,
            0x2AC8: 0x04400006, 0x2AD0: 0x962601A4, 0x2AD8: 0x0C0210AA,
            0x2AF0: 0x28420008, 0x2B50: 0x0043001B, 0x2BAC: 0x0064102B,
            0x2BC4: 0x0062001B,
            0x2E8C: 0x27A200D0, 0x2E90: 0xAFA20020, 0x2E94: 0x27A200D4,
            0x2EA0: 0x0C021E56, 0x2EA4: 0xAFA20024,
            0x33D4: 0x27A200D0, 0x33D8: 0xAFA20020, 0x33DC: 0x27A200D4,
            0x33E0: 0xAFA20024, 0x3404: 0x0C021E56,
        }
        for _, data in self.legal_images():
            for pc, expected in anchors.items():
                self.assertEqual(struct.unpack_from("<I", data, pc)[0], expected)
        self.assertEqual(200 + 9 * 4, 236)
        self.assertLess(236, 240)


if __name__ == "__main__":
    unittest.main()
