import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant422Tests(family435.FrenchModelVariant435Tests):
    family = 422
    source_family = 405
    module_count = 4
    distinct_images = 4
    binding_count = 36
    tail_start = 0x3834
    spans = ((4, 0x1128), (0x1128, 0x1620), (0x1620, 0x1CFC),
             (0x1CFC, 0x2408), (0x2408, 0x28F0), (0x28F0, 0x2E44),
             (0x2E44, 0x3154), (0x3154, 0x34D0), (0x34D0, 0x3834))
    helpers = ((0x4, 4388, "entry", "func_8013B004"),
               (0x1128, 1272, "halo", "func_8013C12C"),
               (0x1620, 1756, "veils", "func_8013C620"),
               (0x1CFC, 1804, "bands", "func_8013CD04"),
               (0x2408, 1256, "sheets", "func_8013D410"),
               (0x28F0, 1364, "webs", "func_8013D8FC"),
               (0x2E44, 784, "spokes", "func_8013DE54"),
               (0x3154, 892, "rings", "func_8013E168"),
               (0x34D0, 868, "quad", "func_8013E4E8"))
    reachable_helpers = {0x4, 0x1128, 0x1620}
    local_call_targets = {0x1128, 0x1620}
    models_by_stage = ((7, (1, 550)),)
    entry_anchors = {
        0x0C: 0x00809821, 0x14: 0x0260B021,
        0x18: 0x26D812B4, 0x1C: 0xAFB80088,
        0x20: 0x26D8147C, 0x24: 0xAFB8008C,
        0x28: 0x26D815AC, 0x2C: 0xAFB80090,
        0x30: 0x26D8190C, 0x34: 0xAFB80094,
        0x38: 0x26D81B4C, 0x3C: 0xAFB80098,
        0x57C: 0x00002821, 0x590: 0x0000A021,
        0x5B0: 0xA0620144, 0x5EC: 0xA0620168,
        0x618: 0x2A820009, 0x620: 0x24630004,
        0x624: 0x24A50001, 0x634: 0x8FB80088,
        0x63C: 0x271801C8, 0x640: 0x18A0FFD2, 0x644: 0xAFB80088,
        0x75C: 0x27180090, 0x76C: 0x1B00FFBB,
        0x938: 0x27180098, 0x958: 0x2B020002,
        0xC4C: 0x27180090, 0xC68: 0x2A620006,
        0xD70: 0x2A620004, 0xD74: 0x27180090, 0xDB0: 0xAEC01E18,
        0x64C: 0x8FB80098, 0x654: 0xAFA000B8,
        0x6E8: 0x8FB800B8, 0x6F0: 0x27180001, 0x6F4: 0xAFB800B8,
        0x754: 0x8FB80098, 0x760: 0xAFB80098, 0x764: 0x8FB800B8,
        0xB94: 0x00009821, 0xB98: 0x8FB80090,
        0xC90: 0x00009821, 0xC94: 0x8FB80094,
    }

    entry_anchors.update({
        0x40: 0x26D80DD4, 0x54: 0xAFB8009C, 0x964: 0x8FB8009C,
        0x970: 0x2715019C, 0x980: 0x8FB8009C, 0x988: 0xAFB800C8,
        0x9F0: 0x8FB100C8, 0xA50: 0x263000C0, 0xA8C: 0x26310008,
        0xAA8: 0x2A820006, 0xAD4: 0x27180030, 0xAF4: 0x2A620004,
        0xB1C: 0xA2A2FFE4, 0xB40: 0xA2A3FFE5, 0xB4C: 0x271801A0,
        0xB64: 0xAEA0FFFC, 0xB6C: 0xA2A3FFE6, 0xB78: 0x24421000,
        0xB7C: 0xAEA2FFF8, 0xB88: 0x2B020003, 0xB90: 0x26B501A0,
        0x28F0: 0x27BDFEE0, 0x28F8: 0x00809821, 0x28FC: 0x26680DD4,
        0x292C: 0x24090001, 0x2930: 0xAFA900E8, 0x2938: 0x8E641DA4,
        0x293C: 0x8E651D9C, 0x2940: 0x26711D4C, 0x2944: 0x0C02264A,
        0x294C: 0x8E641DA0, 0x2950: 0x8E651D9C, 0x2954: 0x266A1D50,
        0x2958: 0x0C02264A, 0x2960: 0x8E641D90, 0x2964: 0x8E651D8C,
        0x2968: 0x266B1D54, 0x296C: 0x0C02264A, 0x2974: 0x86641D9A,
        0x2978: 0x86651D98, 0x297C: 0x0C02264A, 0x2980: 0x26720F68,
        0x2984: 0x8E621E1C, 0x299C: 0x28621000, 0x29B8: 0x28820801,
        0x29C0: 0x24021000, 0x29D0: 0x24061000, 0x29D4: 0x0000B821,
        0x29EC: 0x18400003, 0x2A04: 0x28821801, 0x2A0C: 0x24022000,
        0x2A10: 0x9243FFEC, 0x2A2C: 0x9242FFED, 0x2A40: 0x0003BAC2,
        0x2A48: 0x9242FFEE, 0x2A84: 0x8E621E1C, 0x2A8C: 0x28420002,
        0x2A98: 0x8E621D74, 0x2AA4: 0x8E621D78, 0x2AB0: 0x8E621D7C,
        0x2ABC: 0x86621D80, 0x2AC8: 0x86621D82, 0x2AD4: 0x86621D84,
        0x2B80: 0x0003B100, 0x2B88: 0x26C200C0, 0x2B98: 0x00052B43,
        0x2BA4: 0x3C025000, 0x2BA8: 0xAE220000, 0x2BB4: 0x27A200D0,
        0x2BBC: 0x27A200D4, 0x2BD0: 0x00803021, 0x2BD4: 0x00A03821,
        0x2BE4: 0x0C021E56, 0x2C00: 0xA220000C, 0x2C0C: 0xA237000F,
        0x2C1C: 0xA237000C, 0x2C28: 0xA220000F, 0x2C34: 0x18400004,
        0x2C44: 0x3046FFFF, 0x2C58: 0x28420006, 0x2C74: 0x28420004,
        0x2C80: 0x8E621E1C, 0x2C98: 0x18400053, 0x2CA0: 0x8E641DC8,
        0x2CA4: 0x8E621DB8, 0x2CA8: 0x8C85001C, 0x2CB0: 0x00451023,
        0x2CBC: 0x8C820020, 0x2CC0: 0x00031B00, 0x2CC4: 0x00451023,
        0x2CC8: 0x0062001B, 0x2D00: 0x2463F000, 0x2D10: 0x24421000,
        0x2D44: 0x00021023, 0x2D58: 0x28622000, 0x2D64: 0x8E621DC0,
        0x2D6C: 0x00021200, 0x2D88: 0x28420004, 0x2D94: 0x24022000,
        0x2D98: 0xAE420000, 0x2DA0: 0x24030003, 0x2DB8: 0x8FAA00E8,
        0x2DC0: 0x154B0009, 0x2DC4: 0x24020004, 0x2DD4: 0x24020005,
        0x2DDC: 0xAE621E1C, 0x2DE0: 0xAE420000, 0x2DE4: 0xAFA000E8,
        0x2DE8: 0x265201A0, 0x2DF8: 0x252901A0, 0x2E08: 0x28420003,
    })

    entry_anchors.update({
        0x000c: 0x00809821,
        0x0014: 0x0260B021,
        0x0058: 0x26D80820,
        0x005c: 0xAFB80080,
        0x0078: 0x26D81CB8,
        0x0098: 0xAFB60084,
        0x00c4: 0x00181840,
        0x00c8: 0x00781821,
        0x00cc: 0x00031900,
        0x00d4: 0xAEC31DC8,
        0x0150: 0x2718019C,
        0x0154: 0xAFB800CC,
        0x03f0: 0x0000A021,
        0x03f4: 0x02809021,
        0x03f8: 0x8FB10084,
        0x0414: 0xA6230000,
        0x041c: 0xA6200002,
        0x0434: 0xA6230004,
        0x044c: 0xA6230088,
        0x0458: 0xA6180002,
        0x0470: 0xA6030004,
        0x0490: 0xA6230110,
        0x049c: 0xA6180002,
        0x04c0: 0x2A820011,
        0x04c8: 0xA6030004,
        0x04cc: 0x3C026666,
        0x04d4: 0x34426667,
        0x04dc: 0x03020018,
        0x04e0: 0x2718F800,
        0x0500: 0x271801A0,
        0x0510: 0xAF000000,
        0x0524: 0xAF02FFFC,
        0x0528: 0x271801A0,
        0x0538: 0x2B020005,
        0x0fa8: 0x02602021,
        0x0f8c: 0x8C83001C,
        0x0f90: 0x8EC21DB8,
        0x0f98: 0x0043102B,
        0x0f9c: 0x14400003,
        0x1620: 0x27BDFEC8,
        0x1628: 0x0080B021,
        0x1630: 0x02C0F021,
        0x165c: 0xAFAA0108,
        0x1660: 0x0C0214AA,
        0x1674: 0x26D21CB8,
        0x167c: 0xAFA2010C,
        0x168c: 0x26D5019C,
        0x1690: 0x8EA2FFFC,
        0x16d8: 0xA6600002,
        0x16e8: 0xA6620000,
        0x171c: 0xA6620004,
        0x1748: 0xA6620088,
        0x1754: 0x26710088,
        0x1764: 0x00031A02,
        0x176c: 0xA6230002,
        0x17d4: 0xA6620110,
        0x17e0: 0x26710110,
        0x17f0: 0x00031A02,
        0x17f8: 0xA6230002,
        0x1838: 0x2AE20011,
        0x1840: 0x0017A200,
        0x1854: 0xA3A300F0,
        0x1858: 0xA3A300F1,
        0x185c: 0xA3A300F2,
        0x1860: 0xA3A000E8,
        0x1864: 0xA3A200E9,
        0x1868: 0xA3A300EA,
        0x1878: 0x8FC21D74,
        0x18a0: 0x8FC21D78,
        0x18b0: 0x8FC21D7C,
        0x18c0: 0x27A40080,
        0x190c: 0xAFA000C8,
        0x1914: 0xAFA00080,
        0x1928: 0x26260110,
        0x1930: 0x26270118,
        0x1958: 0x27A200F8,
        0x1960: 0x27A200FC,
        0x1974: 0x284200A0,
        0x1980: 0x8FAB010C,
        0x199c: 0x24060140,
        0x19b0: 0x3050FFFF,
        0x1a04: 0x8FAC010C,
        0x1a10: 0x24060080,
        0x1a20: 0x240601C0,
        0x1a34: 0x3050FFFF,
        0x1a48: 0x2442FF80,
        0x1a88: 0x2442FF80,
        0x1a9c: 0x26260088,
        0x1aa4: 0x26270090,
        0x1ae4: 0x24050001,
        0x1af8: 0x00002821,
        0x1afc: 0x93A200F0,
        0x1b44: 0x93A200E8,
        0x1b84: 0x06000008,
        0x1b8c: 0x8FA200FC,
        0x1ba4: 0x3206FFFF,
        0x1bb4: 0x2AE20010,
        0x1bd4: 0x8FC21DC0,
        0x1bfc: 0x00000000,
        0x1c00: 0x8C430024,
        0x1c04: 0x8FC21DB8,
        0x1c0c: 0x0043102B,
        0x1c1c: 0xAEA2FFFC,
        0x1c28: 0xAEA20000,
        0x1c2c: 0xAEA2FFFC,
        0x1c30: 0x8FC21E1C,
        0x1c48: 0x8C430020,
        0x1c60: 0xAFC21E1C,
        0x1c64: 0x8EA20000,
        0x1c70: 0x01420018,
        0x1cac: 0xAFC41E1C,
        0x1cb0: 0x26B501A0,
        0x1cb8: 0x26D601A0,
        0x1cc0: 0x29820005,
    })

    entry_anchors.update({
        0x4: 0x27BDFF00, 0x1BC: 0xA7A2004E, 0x1D4: 0xA7A2004C,
        0x260: 0x97A2004E, 0x2B0: 0x97A2004E, 0x300: 0x97A2004C,
        0x358: 0x97A2004C, 0x540: 0x00009821, 0xD68: 0x24031000,
        0xD80: 0x24020800, 0xDA0: 0xA6C01E0C, 0xDA4: 0xA6C31E10,
        0xDA8: 0xA6C41E12, 0xF0C: 0x97B00070, 0xF3C: 0x97A2007C,
    })

    def test_entry_views_and_complete_slot_wrapper(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        source = (directory / "variant422_entry.c").read_text()
        header = (directory / "variant422_entry.h").read_text()
        self.assertIn("s32 func_8013B004(SVECTOR *point, s32 command)", source)
        self.assertIn('#include "variant422_entry.h"', source)
        self.assertNotIn("extern ", source)
        for declaration in ("Variant405Veil veils[5];", "Variant405Halo halo;",
                            "ModelVariantWebNarrow webs[3];", "u16 texture_cluts[2];",
                            "Variant422EntryConfig *G32 config;", "GsCOORDUNIT *G32 parts[3];"):
            self.assertIn(declaration, header)
        self.assertIn("halo->rise[row] = -(row * 256);", source)
        self.assertIn("halo->fall[row] = -((row + 1) * 256);", source)
        self.assertIn("screen_x = (u16)projection.projected.vx;", source)
        self.assertIn("dx = (u16)projection.target.vx - screen_x;", source)
        self.assertEqual(source.count("outer++, ring_angle = base_angle + outer * 512"), 2)
        self.assertEqual((directory / "variant422_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define func_8013C128 func_8017C128\n'
                         '#define func_8013C620 func_8017C620\n'
                         '#define D_8013E834 D_8017E834\n'
                         '#include "variant422_entry.c"\n')

    def test_entry_clut_stack_accesses_and_sdk_aliases(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            for path in (family435.ROOT / module["linker_symbols"],
                         layout.with_name(layout.stem + "_symbols.txt")):
                text = path.read_text()
                for name, address in (("SetPolyG3", 0x80082E48), ("SetPolyFT4", 0x80082EA8),
                                      ("SetPolyG4", 0x80082EC8), ("SquareRoot0", 0x80086DD8),
                                      ("RotTransPers", 0x80087868), ("Square0", 0x80089BC8),
                                      ("GsGetLwUnit", 0x8008A428)):
                    self.assertIn(f"{name} = 0x{address:X};", text)
                    self.assertNotIn(f"func_french_{address:X}", text)
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048 + 4)
                words = struct.unpack("<1097I", archive.read(4388))
                accesses = [word & 65535 for word in words
                            if word >> 26 in (32, 33, 35, 36, 37, 40, 41, 43)
                            and word >> 21 & 31 == 29]
                self.assertFalse(any(0x10 <= offset < 0x4C for offset in accesses))
                self.assertTrue({0x4C, 0x4E, 0x70, 0x7C} <= set(accesses))

    def test_entry_called_veil_deadlines_and_stack_view(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<I", data, 0xFA4)[0],
                                 0x0C000000 | ((base + 0x1620) >> 2 & 0x3FFFFFF))
                self.assertEqual(struct.unpack_from("<3I", data, 0x386C + 28), (60, 120, 360))
                words = struct.unpack("<439I", data[0x1620:0x1CFC])
                stack_accesses = [word & 65535 for word in words
                                  if word >> 26 in (32, 33, 35, 36, 37, 40, 41, 43)
                                  and word >> 21 & 31 == 29]
                self.assertFalse(any(0xD0 <= offset < 0xE8 for offset in stack_accesses))
                self.assertTrue({0xE8, 0xE9, 0xEA, 0xF0, 0xF1, 0xF2, 0xFC} <= set(stack_accesses))
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                for name, address in (("GsGetActiveBuff", 0x800852A8), ("GetTPage", 0x80082CE8),
                                      ("SetPolyGT4", 0x80082EE8), ("SetSemiTrans", 0x80082DA8),
                                      ("SetShadeTex", 0x80082DD8)):
                    self.assertIn(f"{name} = 0x{address:X};", bindings)

    entry_anchors.update({
        0x0058: 0x26D80820,
        0x005c: 0xAFB80080,
        0x0060: 0x26D81C10,
        0x0070: 0x26D81BDC,
        0x0074: 0xAFB800A8,
        0x00d4: 0xAEC31DC8,
        0x0544: 0x00132080,
        0x0548: 0x00131A00,
        0x054c: 0x26620001,
        0x0554: 0x00031823,
        0x0558: 0x00131200,
        0x0560: 0x00021023,
        0x0564: 0x03042021,
        0x0568: 0xAC82058C,
        0x056c: 0x2A620005,
        0x0570: 0xAC830578,
        0x0578: 0xAC8005A0,
        0x0fac: 0x8EC21E1C,
        0x0fb4: 0x28420002,
        0x0fc4: 0x02602021,
        0x1128: 0x27BDFEB8,
        0x1138: 0x27D21BDC,
        0x1160: 0x27C80820,
        0x1168: 0x8FC41D90,
        0x116c: 0x8FC51D88,
        0x117c: 0x8FC41D8C,
        0x1180: 0x8FC51D90,
        0x1194: 0x8E620578,
        0x11ac: 0x8E63058C,
        0x11d0: 0x00161040,
        0x11d4: 0x00561021,
        0x11dc: 0x00171040,
        0x11e4: 0x00571021,
        0x11f8: 0x00021200,
        0x1204: 0x00021200,
        0x1214: 0x000A1A00,
        0x1220: 0x246303FF,
        0x1224: 0x00031283,
        0x1228: 0x00021023,
        0x1230: 0x260302A8,
        0x1244: 0xA61102A8,
        0x1254: 0x00148A00,
        0x1264: 0x2A820011,
        0x1278: 0x28420301,
        0x1284: 0x2AC20400,
        0x12ac: 0xA06200D0,
        0x12b0: 0xA06200D8,
        0x12b4: 0xA06200E0,
        0x12d0: 0xA08200E8,
        0x12d4: 0xA06200F0,
        0x12dc: 0xA06200F8,
        0x1308: 0x8E62058C,
        0x1310: 0x28420400,
        0x131c: 0x8FC21DC0,
        0x1324: 0x00021140,
        0x132c: 0xAE630578,
        0x1338: 0x00021140,
        0x1348: 0xAE65058C,
        0x134c: 0x8FC21DC8,
        0x1354: 0x8C440028,
        0x1358: 0x8FC21DB8,
        0x1360: 0x0044102B,
        0x1390: 0x8E6205A0,
        0x139c: 0xAE6205A0,
        0x13a0: 0x26730004,
        0x13ac: 0x2AA20005,
        0x13b0: 0x254A0088,
        0x13c4: 0x240B02A8,
        0x13d8: 0x87C21D80,
        0x13e4: 0xAFA00058,
        0x13ec: 0x87C31D84,
        0x13f4: 0xAFA20030,
        0x13f8: 0xAFA20034,
        0x13fc: 0xAFA20038,
        0x1454: 0xAFA000C8,
        0x145c: 0xAFA00080,
        0x1498: 0x24110008,
        0x14f8: 0x24100010,
        0x1524: 0x00141100,
        0x1528: 0x240A007F,
        0x1530: 0x24080060,
        0x1538: 0xA242000C,
        0x1544: 0xA24A000D,
        0x1548: 0xA2500018,
        0x154c: 0xA24B0019,
        0x1550: 0xA2480025,
        0x1554: 0xA2500030,
        0x1558: 0xA2490031,
        0x1594: 0x04C00008,
        0x15a4: 0x04400004,
        0x15b4: 0x30C6FFFF,
        0x15c8: 0x2A820010,
        0x15d4: 0x26F70088,
        0x15e0: 0x2AA20005,
        0x15e4: 0x254A0088,
        0x000c: 0x00809821,
        0x0014: 0x0260B021,
        0x1130: 0x0080F021,
        0x12b8: 0x01371023,
        0x12bc: 0x00602021,
        0x12c0: 0x00021FC2,
        0x12c4: 0x00431021,
        0x12c8: 0x00021043,
        0x12cc: 0x00801821,
        0x1374: 0xAE680578,
        0x137c: 0xAE69058C,
        0x1380: 0x24A2FC00,
        0x1384: 0xAE620578,
        0x1388: 0x00431023,
        0x138c: 0xAE62058C,
    })

    def test_entry_called_halo_deadline_and_view(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<I", data, 0xFC0)[0],
                                 0x0C000000 | ((base + 0x1128) >> 2 & 0x3FFFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x386C + 0x28)[0], 380)
                self.assertEqual(0x820 + 5 * 17 * 8, 0xAC8)
                self.assertEqual(0x820 + 0x578, 0xD98)
                self.assertEqual(0x820 + 0x58C, 0xDAC)
                self.assertEqual(0x820 + 0x5A0, 0xDC0)
                self.assertEqual(0x820 + 0x5B4, 0xDD4)
                self.assertEqual(0x1BDC + 52, 0x1C10)
                words = struct.unpack("<318I", data[0x1128:0x1620])
                callees = {0x80000000 | ((word & 0x3FFFFFF) << 2)
                           for word in words if word >> 26 == 3}
                self.assertEqual(callees, {0x8005C018, 0x80089928, 0x800866F8, 0x80086628,
                                          0x80087CB8, 0x80086258, 0x80085558, 0x80087958,
                                          0x800842A8})

    def test_retained_sheet_web_descriptor_and_context(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                instance = self.instances[module["name"]]
                base, slot = int(module["load_address"], 0), int(instance["slot"])
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x386C
                self.assertEqual(struct.unpack_from("<I", data, 0xB4)[0],
                                 0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0xB8)[0],
                                 0x24420000 | (table & 0xFFFF))
                archive.seek((int(instance["record"]) * 276 + 275) * 2048 + 0x110)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(instance["command_word"]))
                self.assertEqual(command, 588000)
                descriptor = 0x386C + command % 1000 * 48
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 48, len(data))
                self.assertEqual(struct.unpack_from("<2i", data, descriptor + 0x1C), (60, 120))
                for offset, word in {
                    0xC4: 0x00181840, 0xC8: 0x00781821, 0xCC: 0x00031900,
                    0xD4: 0xAEC31DC8, 0x2408: 0x27BDFEF0, 0x2410: 0x00808821,
                    0x2418: 0x263E147C, 0x2440: 0x26321C84, 0x2448: 0x26301504,
                    0x2684: 0x0C021E56, 0x2714: 0x18400005, 0x2728: 0x3046FFFF,
                    0x273C: 0x2AE20004, 0x277C: 0x8E231DC8, 0x2784: 0x8C64001C,
                    0x2788: 0x8C630020, 0x2798: 0x0043001B, 0x28A4: 0x26100098,
                    0x28AC: 0x27DE0098, 0x28B4: 0x29020002,
                }.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                self.assertEqual(0x147C + 2 * 152, 0x15AC)
                self.assertEqual(0x147C + 136, 0x1504)
                self.assertEqual(0xDD4 + 3 * 416, 0x12B4)
                self.assertEqual(0xDD4 + 404, 0xF68)
                self.assertNotIn(0x2408, self.local_call_targets | self.reachable_helpers)
                self.assertNotIn(0x28F0, self.local_call_targets | self.reachable_helpers)
                widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack("<1097I", data[4:0x1128])
                            if word >> 26 in widths and word >> 21 & 31 == 22 and not word & 0x8000]
                self.assertEqual(max(accesses), 0x1E38)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 4096), (base, 20480)):
                    self.assertTrue(context + 0x1E38 <= start or start + size <= context)
