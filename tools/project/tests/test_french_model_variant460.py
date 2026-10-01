import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant460Tests(family435.FrenchModelVariant435Tests):
    family = 460
    source_family = 443
    module_count = 28
    distinct_images = 28
    binding_count = 34
    tail_start = 0x28CC
    spans = ((4, 0xCFC), (0xCFC, 0x17EC), (0x17EC, 0x1D74),
             (0x1D74, 0x20B4), (0x20B4, 0x28CC))
    helpers = ((0xCFC, 2800, "ribbons", "func_8013BD00"),
               (0x17EC, 1416, "sheets", "func_8013C808"),
               (0x1D74, 832, "strand", "func_8013CD84"))
    reachable_helpers = {0xCFC, 0x17EC}
    local_call_targets = {0xCFC, 0x17EC}
    models_by_stage = ((7, (70, 125, 168, 460, 469, 704)),
                       (9, (44, 98, 161, 370, 400, 458, 462, 558)))
    entry_anchors = {0x10: 0x0080B021, 0x14: 0x26D81740, 0x30: 0x02C0B821,
                     0x34: 0xAFB80080, 0x94: 0x00191980, 0x98: 0x00621821,
                     0x9C: 0xAEC32E7C, 0x590: 0x2A820008, 0x598: 0x26F702E8,
                     0x59C: 0x00008021, 0x740: 0x26100001, 0x74C: 0x8FB90080,
                     0x754: 0x2739009C, 0x758: 0xAFB90080, 0x770: 0x2A020010}

    entry_anchors.update({
        0x000c: 0xAFA400D0,
        0x0010: 0x0080B021,
        0x0014: 0x26D81740,
        0x0028: 0x26D52BB4,
        0x0030: 0x02C0B821,
        0x0094: 0x00191980,
        0x0098: 0x00621821,
        0x009c: 0xAEC32E7C,
        0x0410: 0x26F70258,
        0x0484: 0x90420004,
        0x048c: 0xA2E2FFCC,
        0x0498: 0x90420005,
        0x04a0: 0xA2E2FFCD,
        0x04ac: 0x90430006,
        0x04b0: 0x00141100,
        0x04b4: 0x00021023,
        0x04b8: 0x24180400,
        0x04c4: 0xAEE2FFE8,
        0x04cc: 0xAEE0FFF0,
        0x04d0: 0xAEF8FFF4,
        0x04d8: 0xA2E3FFCE,
        0x0590: 0x2A820008,
        0x0598: 0x26F702E8,
        0x0ad4: 0x8C620038,
        0x0ad8: 0x8EC32E6C,
        0x0ae0: 0x0043102B,
        0x0afc: 0xAEC22EC4,
        0x0b08: 0x8C42003C,
        0x0b2c: 0xAEC22EC4,
        0x0b30: 0x8EC22E7C,
        0x0b38: 0x8C430030,
        0x0b3c: 0x8EC22E6C,
        0x0b44: 0x0043102B,
        0x0b48: 0x14400004,
        0x0b50: 0x8FA400D0,
        0x0b58: 0x00000000,
        0x0cfc: 0x27BDFCA8,
        0x0d2c: 0xAFA402F8,
        0x0d38: 0x8EC42E58,
        0x0d3c: 0x8EC52E50,
        0x0d48: 0x8EC42E54,
        0x0d50: 0x24420C00,
        0x0d64: 0x8C420020,
        0x0d6c: 0x18400282,
        0x0d70: 0xAFA0030C,
        0x0d74: 0x27A902F0,
        0x0d7c: 0x26D70248,
        0x0de0: 0x0043001A,
        0x0dec: 0x0007000D,
        0x0e30: 0x0043001A,
        0x0e3c: 0x0007000D,
        0x10d4: 0x8D822EBC,
        0x110c: 0x8FAA032C,
        0x1130: 0x000A1100,
        0x1134: 0x004A1021,
        0x1138: 0x00021080,
        0x1140: 0x24420040,
        0x114c: 0xAFA20024,
        0x1150: 0x26C40190,
        0x1154: 0x26C501D8,
        0x115c: 0x27A702F4,
        0x1164: 0xAEE20058,
        0x1204: 0xAE020260,
        0x124c: 0xAE4200CC,
        0x1254: 0xAE4301DC,
        0x1294: 0xA60202A4,
        0x12b4: 0xA60202C6,
        0x1470: 0x26100028,
        0x147c: 0x2610FFD8,
        0x1480: 0x2631FFD8,
        0x1544: 0x92E2FFD8,
        0x1550: 0x92E2FFD9,
        0x155c: 0x92E2FFDA,
        0x1568: 0x8CE20260,
        0x1570: 0x0440000D,
        0x1588: 0x8C4200D0,
        0x1590: 0x04400005,
        0x1598: 0x94E60260,
        0x15a0: 0x0C0210AA,
        0x1600: 0x8EE3FFF8,
        0x1608: 0x28620010,
        0x1614: 0x8EE20000,
        0x162c: 0x8D622E74,
        0x1634: 0x00021040,
        0x163c: 0xAEE2FFF8,
        0x1650: 0xAEE2FFF8,
        0x1654: 0xAEEC0000,
        0x166c: 0xAD622EC4,
        0x1680: 0x8EE30004,
        0x16a0: 0x00021180,
        0x16ac: 0xAEE20004,
        0x16c8: 0xAEE00004,
        0x16cc: 0xAEE0FFF8,
        0x16d4: 0xAEE20000,
        0x16ec: 0x8C420020,
        0x16f4: 0x000210C0,
        0x16f8: 0x00021023,
        0x16fc: 0xAEE2FFF8,
        0x1734: 0xAD822EC4,
        0x1738: 0x26F702E8,
        0x173c: 0x26D602E8,
        0x1774: 0xAFA3030C,
        0x17a8: 0xAD832EBC,
        0x17b8: 0xAD832EC0,
    })

    def test_entry_called_ribbon_bounds_and_flags(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<I", data, 0xB54)[0],
                                 0x0C000000 | ((base + 0xCFC) >> 2 & 0x3FFFFFF))
                descriptor = 0x29C8 + int(row["command_word"]) % 1000 * 64
                count, = struct.unpack_from("<i", data, descriptor + 0x20)
                self.assertIn(count, (4, 5, 6, 8))
                self.assertLessEqual(count * 0x2E8, 0x1740)
                self.assertLessEqual(count * 17 * 4, 544)
                self.assertEqual(0xD0 + 544, 0x2F0)
                self.assertEqual(0x2BB4 + 2 * 40, 0x2C04)
                words = struct.unpack("<700I", data[0xCFC:0x17EC])
                callees = {0x80000000 | ((word & 0x3FFFFFF) << 2)
                           for word in words if word >> 26 == 3}
                self.assertEqual(callees, {0x8005C018, 0x80089928, 0x800866F8, 0x80086628,
                                          0x80087CB8, 0x800875F8, 0x80086258, 0x80085558,
                                          0x80087958, 0x80087868, 0x800842A8})
                widths = {32: 1, 33: 2, 35: 4, 36: 1, 37: 2, 40: 1, 41: 2, 43: 4}
                accesses = [(word & 65535) + widths[word >> 26]
                            for word in struct.unpack("<830I", data[4:0xCFC])
                            if word >> 26 in widths and word >> 21 & 31 == 22 and not word & 0x8000]
                self.assertEqual(max(accesses), 0x2ED8)
                slot = int(row["slot"])
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 4096), (base, 20480)):
                    self.assertTrue(context + 0x2ED8 <= start or start + size <= context)

    def test_command_selected_sheet_configuration_bounds(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                base = int(module["load_address"], 0)
                config = base + 0x29C8
                self.assertEqual(struct.unpack_from("<I", data, 0x88)[0],
                                 0x3C020000 | ((config + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x90)[0],
                                 0x24420000 | (config & 0xFFFF))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110 +
                             4 * ((int(row["stage"]) - 7) // 2))
                command = struct.unpack("<i", archive.read(4))[0]
                self.assertEqual(command, int(row["command_word"]))
                self.assertGreaterEqual(command, 0)
                start = 0x29C8 + command % 1000 * 64
                self.assertGreaterEqual(start, self.tail_start)
                self.assertLessEqual(start + 64, len(data))
                n, mode, kind = struct.unpack_from("<3i", data, start + 0x20)
                self.assertEqual(mode, 1)
                self.assertIn(kind, (0, 1, 2))
                count, first = ((n + 1, 1) if kind == 0 else
                                (n + 2, 2) if kind == 2 else (2 * n, n))
                self.assertGreater(first, 0)
                self.assertLessEqual(first, 8)
                self.assertLessEqual(first, count)
                self.assertLessEqual(count, 16)
                self.assertLessEqual(count - first, 8)
