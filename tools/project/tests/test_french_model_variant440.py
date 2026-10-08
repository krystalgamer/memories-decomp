import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant440Tests(family435.FrenchModelVariant435Tests):
    family = 440
    source_family = 423
    module_count = 4
    distinct_images = 4
    binding_count = 49
    tail_start = 0x2B20
    spans = (
        (0x4, 0xF14),
        (0xF14, 0x16E0),
        (0x16E0, 0x1BD4),
        (0x1BD4, 0x2130),
        (0x2130, 0x2440),
        (0x2440, 0x27BC),
        (0x27BC, 0x2B20),
    )
    helpers = (
        (0x4, 0xF10, "entry", "func_8013B004"),
        (0x16E0, 1268, "sheets", "func_8013C6E4"),
        (0x1BD4, 1372, "webs", "func_8013CBDC"),
        (0x2130, 784, "spokes", "func_8013D13C"),
        (0x2440, 892, "rings", "func_8013D450"),
        (0x27BC, 868, "quad", "func_8013D7D0"),
    )
    reachable_helpers = {0x4, 0x16E0, 0x1BD4}
    local_call_targets = {0xF14, 0x16E0, 0x1BD4}
    models_by_stage = ((9, (262, 631)),)
    entry_anchors = {
        0xC: 0x809821,
        0x14: 0x260B021,
        0x20: 0x26D805E8,
        0x24: 0xAFB80084,
        0x28: 0x26D80718,
        0x2C: 0xAFB80088,
        0x30: 0x26D80A78,
        0x34: 0xAFB8008C,
        0x38: 0x26D80CB8,
        0x54: 0xAFB80090,
        0x464: 0xB821,
        0x46C: 0x8FB80090,
        0x474: 0x27120088,
        0x504: 0x26F70001,
        0x55C: 0xAE40FFF8,
        0x568: 0x8FB80090,
        0x570: 0x27180090,
        0x574: 0x1AE0FFC0,
        0x578: 0xAFB80090,
        0x57C: 0xB821,
        0x584: 0x8FB80084,
        0x58C: 0x27030090,
        0x590: 0x8FB80084,
        0x720: 0x26F70001,
        0x72C: 0x8FB80084,
        0x734: 0x27180098,
        0x738: 0xAFB80084,
        0x73C: 0xAC60FFF8,
        0x74C: 0x2AE20002,
        0x750: 0x1440FF8F,
        0x754: 0x24630098,
        0x98C: 0x8FB80088,
        0x994: 0xAFA000A8,
        0x998: 0x2712008C,
        0x9A4: 0x8FB00088,
        0x9BC: 0xA6020000,
        0x9E8: 0xA6030040,
        0x9F4: 0x26100008,
        0xA08: 0x2AE20008,
        0xA2C: 0x27180001,
        0xA38: 0xAFB800A8,
        0xA3C: 0x8FB80088,
        0xA44: 0x27180090,
        0xA48: 0xAFB80088,
        0xA4C: 0xA242FFF4,
        0xA6C: 0xAE400000,
        0xA78: 0xAE43FFFC,
        0xA7C: 0x8FB800A8,
        0xA84: 0x2B020006,
        0xA88: 0x1440FFC4,
        0xA90: 0x8FB8008C,
        0xA98: 0xAFA000A8,
        0xA9C: 0x2712008C,
        0xAA8: 0x8FB0008C,
        0xAC8: 0xA6030000,
        0xAF4: 0xA6020040,
        0xB00: 0x26100008,
        0xB10: 0x2AE20008,
        0xB30: 0x27180001,
        0xB34: 0xAFB800A8,
        0xB38: 0xA242FFF8,
        0xB5C: 0xAE43FFFC,
        0xB60: 0xAE400000,
        0xB64: 0x8FB8008C,
        0xB6C: 0x27180090,
        0xB70: 0xAFB8008C,
        0xB74: 0x8FB800A8,
        0xB7C: 0x2B020004,
        0xB80: 0x1440FFC7,
        0xD90: 0x02602021,
        0xD98: 0x02602021,
        0x16E0: 0x27BDFF00,
        0x16F0: 0x263505E8,
        0x171C: 0x26320DBC,
        0x1728: 0x26300670,
        0x1920: 0x24E50020,
        0x1928: 0x24E60040,
        0x195C: 0x24E70060,
        0x196C: 0x9203FFFC,
        0x19D8: 0x9203FFF8,
        0x1A30: 0x2A620004,
        0x1A84: 0x0043001B,
        0x1B94: 0x26100098,
        0x1B98: 0x2AE20002,
        0x1BA0: 0x26B50098,
        0x1BD4: 0x27BDFEE8,
        0x1BE0: 0x0080A021,
        0x1C34: 0x26910E84,
        0x1C58: 0x26920194,
        0x1F3C: 0x28420006,
        0x1F58: 0x28420004,
        0x20D4: 0x265201A0,
        0x20F4: 0x28420003,
    }

    def test_selected_descriptors_and_minimum_context_extent(self):
        archive_path = family435.ROOT / f"game/{self.region}/DATA/MODEL.MRG"
        resident_path = family435.ROOT / "game/france/SLES_039.48"
        if not archive_path.exists() or not resident_path.exists():
            self.skipTest("legal French MODEL and resident inputs required")
        pointers = struct.unpack_from("<14I", resident_path.read_bytes(), 0x800)
        widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
        with archive_path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, base = int(row["slot"]), int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                table = base + 0x2C1C
                self.assertEqual(struct.unpack_from("<II", data, 0x98),
                                 (0x3C020000 | ((table + 0x8000) >> 16 & 0xFFFF),
                                  0x24420000 | (table & 0xFFFF)))
                for call, target in ((0xD8C, 0x16E0), (0xD94, 0x1BD4)):
                    self.assertEqual(struct.unpack_from("<I", data, call)[0],
                                     0x0C000000 | ((base + target) >> 2 & 0x3FFFFFF))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110 +
                             (int(row["stage"]) - 7) // 2 * 4)
                request, = struct.unpack("<i", archive.read(4))
                self.assertEqual(request, 606000)
                self.assertEqual(request, int(row["command_word"]))
                descriptor = 0x2C1C + request % 1000 * 48
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 48, len(data))
                self.assertEqual(struct.unpack_from("<II", data, descriptor + 0x1C), (0, 36))
                accesses = [(word & 0xFFFF) + widths[word >> 26]
                            for word in struct.unpack("<964I", data[4:0xF14])
                            if word >> 26 in widths and word >> 21 & 31 == 22
                            and word & 0x8000 == 0]
                self.assertEqual(max(accesses), 0xF38)
                context = pointers[9 + slot]
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048), (base, 10 * 2048)):
                    self.assertTrue(context + max(accesses) <= start or start + size <= context)

    def test_entry_wrappers_select_pal_palette_and_local_names(self):
        if self.region != "france":
            self.skipTest("French entry wrappers only")
        body = (family435.ROOT / "src/overlays/model_variant/variant423_entry.c").read_text()
        self.assertIn('#include "variant423_entry.h"', body)
        self.assertIn("#define MODEL_VARIANT423_CLUT_X 512", body)
        self.assertIn("GetClut(MODEL_VARIANT423_CLUT_X, 244)", body)
        self.assertNotIn("func_8013DBF0", body)
        header = (family435.ROOT / "src/overlays/model_variant/variant423_entry.h").read_text()
        self.assertIn("u8 unknown_20[0x10];\n} Variant423EntryConfig;", header)
        self.assertNotIn("arms", header)
        directory = family435.ROOT / "src/overlays/french_model_variant"
        for slot, high in ((0, "8013"), (1, "8017")):
            name = "variant440_entry" + ("_slot1" if slot else "") + ".c"
            self.assertEqual((directory / name).read_text(),
                             '#include "../../types.h"\n'
                             "#define MODEL_VARIANT423_CLUT_X 640\n"
                             f"#define D_8013DB38 D_{high}DB20\n" +
                             ("#define func_8013B004 func_8017B004\n" if slot else "") +
                             f"#define func_8013BF18 func_{high}BF14\n"
                             f"#define func_8013C6E4 func_{high}C6E0\n"
                             f"#define func_8013CBDC func_{high}CBD4\n"
                             '#include "../model_variant/variant423_entry.c"\n')
