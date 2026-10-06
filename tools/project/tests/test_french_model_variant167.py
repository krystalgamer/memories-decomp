import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant167Tests(family435.FrenchModelVariant435Tests):
    family = 167
    slot_header_delta = 130
    module_count = 2
    distinct_images = 2
    binding_count = 22
    tail_start = 0xF20
    spans = ((4, 0xED8),)
    helpers = ((4, 3796, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (71,)),)
    entry_anchors = {
        4: 0x27BDFEF0, 0xA8: 0x27D00204, 0xAC: 0x27D10404,
        0xB8: 0xA7C00606, 0x140: 0x2A620100, 0x168: 0xA7A200B2,
        0x310: 0x24050002, 0x33C: 0x24050001, 0x574: 0x97C2060E,
        0x688: 0x00C0A021, 0x7E8: 0x87C20606, 0x98C: 0x0C0135B1,
        0xA98: 0x04610002, 0xA9C: 0xA6020000, 0xB60: 0xA7C20606,
        0xB9C: 0x97C2060A, 0xE84: 0x24020004, 0xE90: 0x10400003,
        0xEA0: 0xA3C20610, 0xED0: 0x03E00008, 0xED4: 0x27BD0110,
        0xED8: 4096, 0xEDC: 4096, 0xEE0: 4096, 0xEE4: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant167_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BED8 D_8017BED8\n'
                         '#define D_8013BEE8 D_8017BEE8\n'
                         '#define D_8013BF20 D_8017BF20\n'
                         '#include "variant167_entry.c"\n')
        source = (directory / "variant167_entry.c").read_text()
        header = (directory / "variant167_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        for declaration in (
            "Model167Config *G32 config;", "SVECTOR points[64];",
            "s16 offsets[256], increments[256];", "s16 phase, scroll;",
            "u32 texture[2];", "u8 started;", "s32 elapsed;", "u8 unknown_06[2];",
            "s16 amplitude, half_size, radius, fade_duration, unknown_10;",
            "extern GsIMAGE D_8013BEE8[];", "extern Model167Config D_8013BF20[];",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "&D_8013BF20[command % 100]", "offset++, increment++, i++",
            "*offset = 0;", "config->amplitude * csin(i * 32) / 4096",
            "Model_CopySlotU16Values(Model_GetActiveSlotIndex() ^ 1, (u16 *)&vertices[0]);",
            "vertices[0].vy = -350;", "value = rand() % config->radius;",
            "angle0 = rand() % 4096;", "angle1 = rand() % 2048;",
            "value * ccos(angle0) / 4096 * csin(angle1) / 4096",
            "value * csin(angle0) / 4096 * csin(angle1) / 4096",
            "func_80059A50(Model_GetActiveSlotIndex(), 2, &D_8013BEE8[0])",
            "func_80059A50(Model_GetActiveSlotIndex(), 1, &D_8013BEE8[i])",
            "u8 red, green, blue;", "func_800595C8(2, -4096, -4096, -4096);",
            "func_800595C8(2, 2048, 2048, 2048);",
            "quad.tpage = work->texture[1] >> 16;",
            "quad.tpage = work->texture[0] >> 16;",
            "(work->scroll / 256 + i) % 64", "*offset / 256 - 64",
            "*offset / 256 + 320", "increment = &work->increments[work->phase];",
            "work->scroll = (work->scroll + (step << 8)) % 16384;",
            "work->phase = (work->phase + step) % 256;",
            "128 + value % 4 * 32", "work->elapsed += step;",
            "return 4;", "if (!work->started)", "work->started++;",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("*offset += *increment;"), 2)
        self.assertEqual(source.count("*offset %= 16384;"), 2)
        self.assertEqual(source.count(
            "Graphics_SubmitTextureWindowPacket((u32 *)&quad, ot, 4095, 0, 0, 64, 64);"), 2)
        for expression, count in (
            ("Model_GetFrameStep()", 1), ("Model_GetActiveSlotIndex()", 4),
            ("rand()", 3), ("ccos(", 2), ("csin(", 4),
            ("func_800595C8(", 4), ("RotAverage4(", 1), ("SetSemiTrans(", 4),
            ("MulMatrix2(", 0), ("GsSortPoly(", 1),
        ):
            self.assertEqual(source.count(expression), count)

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        with (family435.ROOT / "notes/overlays/french-model-variant167-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 10)
        self.assertEqual(len({(r["attempt"], r["slot"]) for r in rows}), 10)
        experiments = [r for r in rows if r["result"] in ("mismatch", "text_exact")]
        self.assertEqual(len(experiments), 8)
        expected = {
            "native-screen-strips-and-particles": (3760, 870),
            "native-paired-wave-cursors": (3772, 554),
            "native-byte-color-components": (3796, 5),
            "native-zero-started-terminal": (3796, 0),
        }
        self.assertEqual({r["attempt"] for r in experiments}, set(expected))
        for name, (size, differences) in expected.items():
            pair = [r for r in experiments if r["attempt"] == name]
            self.assertEqual({r["slot"] for r in pair}, {"0", "1"})
            for row in pair:
                self.assertEqual((int(row["instruction_bytes"]), int(row["different_words"])),
                                 (size, differences))
            for field in ("profile", "result", "instruction_bytes", "different_words", "fingerprint"):
                self.assertEqual(pair[0][field], pair[1][field])
        self.assertEqual({r["attempt"] for r in experiments if r["result"] == "text_exact"},
                         {"native-zero-started-terminal"})
        for row in rows:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
        terminal = [r for r in rows if r["result"] == "matched"]
        self.assertEqual({r["slot"] for r in terminal}, {"0", "1"})
        for row in terminal:
            suffix = "_slot1" if row["slot"] == "1" else ""
            path = family435.ROOT / f"src/overlays/french_model_variant/variant167_entry{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("3796", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, kind, size in ((0xED8, "u32", 16), (0xEE8, "u8", 56)):
                self.assertIn(
                    f"D_{base + offset:X} = 0x{base + offset:X}; // type:{kind} size:0x{size:X} defined:true",
                    symbols)
            self.assertIn(f"[0xEE8, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        expected = (32,32,32,48,48,48,1300,200,350,90,40,64,90,4,64,20)
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                self.assertEqual(int(row["command_word"]), 98000)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, 98000)
                argument = command % 1000
                self.assertEqual(argument, 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<6B2x10h", data, 0xF20 + 28 * (argument % 100)),
                                 expected)
                self.assertEqual(data[0xF26:0xF28], b"\x19\x00")
                self.assertEqual([struct.unpack_from("<i4hI4hI", data, 0xEE8 + i * 28)
                                  for i in range(2)],
                                 [(8,864,256,32,64,0,640,8,16,1,0),
                                  (9,832,256,32,64,0,656,8,256,1,0)])
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(v, 16) for v in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<949I", data[4:0xED8])
                calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                self.assertEqual(set(calls), addresses)
                self.assertEqual(len(calls), 37)
                self.assertEqual(len(addresses), 22)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x618 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model167Config)": 28, "sizeof(Model167State)": 0x618,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(POLY_FT4)": 40, "sizeof(GsIMAGE)": 28, "sizeof(PSXLONG)": 4,
            "sizeof(((Model167State *)0)->points) / sizeof(SVECTOR)": 64,
            "sizeof(((Model167State *)0)->offsets) / sizeof(s16)": 256,
            "sizeof(((Model167State *)0)->increments) / sizeof(s16)": 256,
            "sizeof(((Model167State *)0)->texture) / sizeof(u32)": 2,
        }
        for typename, fields in (
            ("Model167Config", (
                ("red", 0), ("particle_red", 3), ("unknown_06", 6), ("amplitude", 8),
                ("half_size", 10), ("radius", 12), ("fade_duration", 14), ("unknown_10", 16),
                ("particle_duration", 18), ("particle_delay", 20), ("stagger", 22),
                ("count", 24), ("delay", 26),
            )),
            ("Model167State", (
                ("config", 0), ("points", 4), ("offsets", 516), ("increments", 1028),
                ("phase", 1540), ("scroll", 1542), ("texture", 1544),
                ("started", 1552), ("elapsed", 1556),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 34)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant167_entry.h")
