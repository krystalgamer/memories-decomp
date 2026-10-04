import struct

from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant338Tests(family435.FrenchModelVariant435Tests):
    family = 338
    source_family = 321
    module_count = 16
    distinct_images = 16
    binding_count = 35
    standalone_helpers = frozenset({"streamers"})
    tail_start = 0x270C
    spans = ((4, 0xBA0), (0xBA0, 0x16D8), (0x16D8, 0x1B90),
             (0x1B90, 0x1ED4), (0x1ED4, 0x270C))
    helpers = ((0x4, 2972, "entry", "func_8013B004"),
               (0xBA0, 2872, "ribbon", "func_8013BBA4"),
               (0x16D8, 1208, "rings", "func_8013C6A8"),
               (0x1B90, 836, "strand", "func_8013CB64"),
               (0x1ED4, 2104, "streamers", "func_8013CED4"))
    reachable_helpers = {0x4, 0xBA0, 0x16D8, 0x1ED4}
    local_call_targets = {0xBA0, 0x16D8, 0x1ED4}
    models_by_stage = ((7, (164, 165, 210, 424, 609)), (9, (34, 443, 459)))
    entry_anchors = {
        0x0C: 0x00809821, 0x14: 0x0260B021, 0x1C: 0x26D21234, 0x20: 0x26D8167C,
        0x8C: 0x00191040, 0x90: 0x00591021, 0x94: 0x00021080,
        0x98: 0x00591021, 0x9C: 0x00021080, 0xA4: 0xAEC21F3C,
        0x110: 0x26C71EE4, 0x138: 0x26C51EC4,
        0x5F0: 0x26500020, 0x5F4: 0x264F0040, 0x5F8: 0x264E0060,
        0x784: 0x26520098, 0x798: 0x2A220002, 0x7A0: 0x24630098,
        0x834: 0xAEC01F68, 0xA20: 0x02602021,
        0x1B98: 0x0080A021, 0x1BA0: 0x26971364, 0x1BD4: 0x26951EB4,
        0x1D0C: 0x2A42000D, 0x1D34: 0x2AC20006, 0x1D3C: 0x26F70084,
        0x1D54: 0x86921F4A, 0x1D58: 0x86821F4C,
        0x1E14: 0x2AC20006, 0x1E1C: 0x26F70084,
        0x1ED4: 0x27BDFEB8, 0x1F28: 0x8D231354, 0x1F2C: 0x253E1E64,
        0x1FC0: 0x2534167C, 0x217C: 0x26940378, 0x233C: 0x0C021E56,
        0x2324: 0x268202EC, 0x2358: 0xAE6202F0, 0x23E4: 0x260202AC,
        0x24FC: 0x26B50378, 0x2628: 0x0440000A, 0x2638: 0x04400006,
        0x26B8: 0x8D021F34, 0x26D4: 0xAD001F50, 0x26D8: 0xAD021F68,
    }

    ribbon_anchors = {
        0x000c: 0x00809821,
        0x0014: 0x0260B021,
        0x001c: 0x26D21234,
        0x0030: 0x26D51E14,
        0x0038: 0x26D41E64,
        0x0040: 0x02C08021,
        0x008c: 0x00191040,
        0x0090: 0x00591021,
        0x0094: 0x00021080,
        0x0098: 0x00591021,
        0x009c: 0x00021080,
        0x00a4: 0xAEC21F3C,
        0x014c: 0x00E09821,
        0x0158: 0x261702D0,
        0x04d0: 0x90420000,
        0x04d8: 0xA2E2FF50,
        0x04e4: 0x90420001,
        0x04ec: 0xA2E2FF51,
        0x04f8: 0x90430002,
        0x050c: 0x00131100,
        0x0510: 0x00021023,
        0x0520: 0xA6E2FF6A,
        0x0528: 0xA2E3FF52,
        0x05b0: 0x26730001,
        0x05d0: 0x2A620005,
        0x05d8: 0x26F703A4,
        0x0834: 0xAEC01F68,
        0x0848: 0xA6D81F7A,
        0x09dc: 0x02602021,
        0x09f8: 0xA6C31F0E,
        0x0ba0: 0x27BDFEC0,
        0x0ba8: 0x0080F021,
        0x0bd4: 0x8FC41F18,
        0x0bd8: 0x8FC51F10,
        0x0be4: 0x8FC41F14,
        0x0be8: 0x8FC51F10,
        0x0bec: 0x24420C00,
        0x0bf8: 0x8FC31F68,
        0x0c00: 0x18600298,
        0x0c08: 0x87C21F4E,
        0x0c14: 0x03C0A021,
        0x0c18: 0x2442001F,
        0x0c1c: 0x00021143,
        0x0c20: 0xA7A000E0,
        0x0c24: 0xAFA000EC,
        0x0c44: 0x24090400,
        0x0c88: 0x24420400,
        0x1020: 0x26920040,
        0x1024: 0x268B0020,
        0x1028: 0xAFA9010C,
        0x102c: 0xAFAA0110,
        0x1030: 0xAFAB0114,
        0x1050: 0x268200C8,
        0x105c: 0x2682035C,
        0x107c: 0x26840190,
        0x1080: 0x268501D8,
        0x1090: 0xAE4202D8,
        0x1094: 0x86430088,
        0x1098: 0x8642008A,
        0x10b4: 0xAE4200CC,
        0x10b8: 0x86420198,
        0x10cc: 0xAE4201DC,
        0x10e8: 0xA5220360,
        0x1114: 0xA5220382,
        0x1148: 0x2602031C,
        0x1164: 0x26310110,
        0x116c: 0x26050198,
        0x1184: 0xAE0202D8,
        0x11b4: 0xAE0200CC,
        0x11cc: 0xAE0201DC,
        0x11ec: 0xA6220360,
        0x1210: 0xA6220382,
        0x1224: 0x28420011,
        0x1230: 0x26B503A4,
        0x124c: 0x28420005,
        0x1254: 0x269403A4,
        0x1260: 0x27D1023A,
        0x1268: 0x86220000,
        0x1274: 0x27D21E14,
        0x1278: 0x26950004,
        0x127c: 0x26930002,
        0x1280: 0x27D01E1A,
        0x1284: 0x8FC21F28,
        0x128c: 0x30420001,
        0x12a4: 0x26100028,
        0x12ac: 0x26520028,
        0x12b0: 0x2610FFD8,
        0x12b4: 0x2652FFD8,
        0x14a8: 0x9222FFE7,
        0x14b4: 0x9222FFE8,
        0x14cc: 0x8C6202D8,
        0x14d4: 0x04400009,
        0x14dc: 0x8C62031C,
        0x14e4: 0x04400005,
        0x14ec: 0x946602D8,
        0x153c: 0x86230000,
        0x1554: 0x96240000,
        0x1558: 0x28420011,
        0x1564: 0x8FC31F34,
        0x1574: 0x00021042,
        0x157c: 0xA6220000,
        0x1588: 0x28420010,
        0x1590: 0x24020010,
        0x1594: 0xA6220000,
        0x15ac: 0x8FC31F68,
        0x15b8: 0x24020002,
        0x15bc: 0xAFC21F68,
        0x15c0: 0x263103A4,
        0x15dc: 0x28420005,
        0x15e4: 0x269403A4,
        0x15e8: 0x8FC31F68,
        0x15ec: 0x24020003,
        0x15f8: 0x87C21F4E,
        0x1608: 0x8FC51F3C,
        0x160c: 0x8FC31F2C,
        0x1610: 0x8CA4002C,
        0x1618: 0x0064102B,
        0x1624: 0x8CA20030,
        0x1628: 0x00031A80,
        0x1630: 0x0062001B,
        0x164c: 0xA7C21F4E,
        0x1660: 0xA7C01F4E,
        0x1664: 0x8FC31F34,
        0x1684: 0x8FC31F60,
        0x1694: 0xAFC31F60,
        0x1698: 0x8FC31F64,
        0x169c: 0x000211C0,
        0x16a4: 0xAFC31F64,
    }

    def test_entry_shared_views_and_slot_wrapper(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        source = (directory / "variant338_entry.c").read_text()
        header = (directory / "variant338_entry.h").read_text()
        self.assertIn('#include "variant338_entry.h"', source)
        self.assertIn("s32 func_8013B004(SVECTOR *point, s32 command)", source)
        self.assertNotRegex(source, r"\b(?:asm|__asm__|register|volatile|extern)\b")
        self.assertIn('#include "variant337_entry.h"', header)
        for declaration in ("Variant337EntryRecord records[5];",
                            "Variant337EntryRing rings[2];",
                            "Variant337EntryStreamer streamers[2];",
                            "Variant338EntryConfig *G32 config;",
                            "GsCOORDUNIT *G32 part;"):
            self.assertIn(declaration, header)
        self.assertEqual(source.count("if (work->config->mode == 1)"), 2)
        self.assertEqual(source.count("func_800593D0(work->slot, work->config->part"), 2)
        self.assertIn("record->field_23A = -(i * 16);", source)
        self.assertIn("s32 packed;\n    s32 flat_page, extra_page;", source)
        self.assertEqual((directory / "variant338_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define func_8013BBA0 func_8017BBA0\n'
                         '#define func_8013C6D8 func_8017C6D8\n'
                         '#define func_8013CED4 func_8017CED4\n'
                         '#define D_8013D70C D_8017D70C\n'
                         '#include "variant338_entry.c"\n')

    def test_streamer_native_endpoints_and_flags(self):
        source = (family435.ROOT / "src/overlays/french_model_variant/variant338_streamers.c").read_text()
        self.assertIn('#include "../model_variant/variant321_streamers.h"', source)
        self.assertIn("previous = &streamer->sa[15]", source)
        self.assertIn("previous = (PSXLONG *)((u8 *)previous + sizeof(Variant321Streamer))", source)
        self.assertIn("&streamer->a[15], &streamer->a[16]", source)
        self.assertIn("&p, &streamer->flag[16]", source)
        self.assertEqual(source.count("streamer->ox[k] = rcos("), 2)
        self.assertEqual(source.count("streamer->oy[k] = rsin("), 2)
        self.assertIn("k = 0, wave = 0, twist = base * 2", source)
        self.assertIn("reach = MODEL_VARIANT_WORD(work, 0x1354) / 32;", source)
        self.assertIn("streamer->flag[k] >= 0", source)
        self.assertNotIn("if (work)", source)
        self.assertNotIn("if (poly)", source)
        self.assertNotRegex(source, r"\b(?:asm|__asm__|register|volatile|extern)\b")

    def test_target_compiled_streamer_view(self):
        import importlib.util
        import tempfile
        from pathlib import Path

        if importlib.util.find_spec("elftools") is None:
            self.skipTest("pyelftools required for target layouts")
        from elftools.elf.elffile import ELFFile
        from build_baseline import TOOLCHAIN, compile_c, load_compiler_profiles, tool

        root = family435.ROOT
        profiles = load_compiler_profiles(root)
        for path in (profiles["gcc_2_8_1_g0_split"]["compiler"],
                     f"{TOOLCHAIN}/mipsel-none-elf-as", "tools/vendor/maspsx/maspsx.py"):
            if not (root / path).is_file():
                self.skipTest(f"target layout requires {path}")
        constants = {"sizeof(Variant321Streamer)": 888}
        for field, offset in (("a", 0), ("sa", 0x88), ("angle", 0xCC),
                              ("b", 0x110), ("sb", 0x198), ("width", 0x1DC),
                              ("color", 0x264), ("flag", 0x2AC), ("otz", 0x2F0),
                              ("ox", 0x334), ("oy", 0x356)):
            constants[f"(u32)&((Variant321Streamer *)0)->{field}"] = offset
        constants.update({
            "sizeof(SVECTOR)": 8, "sizeof(PSXLONG)": 4, "(PSXLONG)-1 < 0": 1,
            "sizeof(MATRIX)": 32, "sizeof(VECTOR)": 16, "sizeof(GsCOORDINATE2)": 80,
            "sizeof(POLY_G4)": 36,
            "sizeof(((Variant321Streamer *)0)->a) / sizeof(SVECTOR)": 17,
            "sizeof(((Variant321Streamer *)0)->ox) / sizeof(s16)": 17,
        })
        with tempfile.TemporaryDirectory(dir=root / "tmp", prefix="streamers338-layout-") as name:
            directory = Path(name).relative_to(root)
            source = directory / "layout.c"
            (root / source).write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/overlays/model_variant/variant321_streamers.h"\n'
                "const u32 layouts[] = {" + ", ".join(constants) + "};\n")
            obj = compile_c(root, tool(root, "as"), {
                "kind": "text", "source": str(source), "object": "layout.o", "profile": "gcc_2_8_1_g0_split"},
                profiles, object_directory=str(directory), asm_directory=str(directory / "asm"))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layouts")
                self.assertIsInstance(symbol["st_shndx"], int)
                self.assertEqual(symbol["st_value"], 0)
                data = elf.get_section(symbol["st_shndx"]).data()
                self.assertEqual(struct.unpack(f"<{len(constants)}I", data), tuple(constants.values()))

    def test_entry_vertex_dispatch_and_texture_stack_anchors(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        anchors = {
            0x4: 0x27BDFF30, 0xF0: 0x8CC3001C, 0x108: 0x94C60012,
            0x10C: 0x0C017136, 0x134: 0x0C02290A,
            0x1EC: 0xAFA20094, 0x23C: 0x8FB80094, 0x240: 0x97B90094,
            0x24C: 0xAFB80098, 0x258: 0xAFA20094, 0x348: 0x97B80098,
            0x410: 0xAFB8009C, 0x414: 0x97B9009C, 0xB9C: 0x27BD00D0,
        }
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                for name, address in (
                    ("GsGetLwUnit", 0x8008A428), ("func_800593D0", 0x8005C4D8),
                    ("GetTPage", 0x80082CE8), ("GetClut", 0x80082D28),
                    ("SetSemiTrans", 0x80082DA8), ("SetShadeTex", 0x80082DD8),
                    ("SetPolyG3", 0x80082E48), ("SetPolyFT4", 0x80082EA8),
                    ("SetPolyG4", 0x80082EC8), ("SetPolyGT4", 0x80082EE8),
                    ("SquareRoot0", 0x80086DD8), ("Square0", 0x80089BC8),
                ):
                    self.assertIn(f"{name} = 0x{address:X};", bindings)

    def test_ribbon_view_and_original_context_dispatch(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for offset, word in self.ribbon_anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
                self.assertEqual(struct.unpack_from("<I", data, 0x9F4)[0],
                                 0x0C000000 | ((base + 0xBA0) >> 2 & 0x3FFFFFF))
                self.assertEqual(5 * 0x3A4, 0x1234)
                self.assertEqual(0x1E14 + 2 * 40, 0x1E64)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                self.assertIn("ratan2 = 0x80089928;", bindings)
                self.assertIn("RotTransPers = 0x80087868;", bindings)

    def test_selected_timing_records_and_context_separation(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<I", data)[0], int(row["header"]))
                base, slot = int(module["load_address"], 0), int(row["slot"])
                config = base + 0x2808
                self.assertEqual(struct.unpack_from("<I", data, 0x78)[0],
                                 0x3C030000 | ((config + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", data, 0x7C)[0],
                                 0x24630000 | (config & 0xFFFF))
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x110 +
                             (int(row["stage"]) - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, int(row["command_word"]))
                offset = 0x2808 + command % 1000 * 52
                self.assertGreaterEqual(offset, self.tail_start)
                self.assertLessEqual(offset + 52, len(data))
                start, end, _, fade_start, fade_end = struct.unpack_from("<5I", data, offset + 32)
                self.assertLess(start, end)
                self.assertLess(fade_start, fade_end)
                self.assertEqual(0x1234 + 2 * 152, 0x1364)
                self.assertEqual(0x1364 + 6 * 132, 0x167C)
                self.assertEqual(struct.unpack_from("<I", data, 0x848)[0] & 0xFFFF, 0x1F7A)
                context = 0x80136000 + slot * 0x40000
                for load, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                   (0x8013A000 + slot * 0x40000, 2 * 2048),
                                   (base, 10 * 2048)):
                    self.assertTrue(context + 0x1F7C <= load or load + size <= context)
