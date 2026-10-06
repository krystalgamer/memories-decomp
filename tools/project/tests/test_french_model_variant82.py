import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant82Tests(family435.FrenchModelVariant435Tests):
    family = 82
    slot_header_delta = 130
    module_count = 2
    distinct_images = 2
    binding_count = 21
    tail_start = 0xE10
    spans = ((4, 0xDE4),)
    helpers = ((4, 3552, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (66,)),)
    entry_anchors = {
        4: 0x27BDFEE8, 0xDDC: 0x03E00008, 0xDE0: 0x27BD0118,
        0x37C: 0x2484015E, 0x380: 0x00031840, 0x394: 0x0062001A,
        0x7F0: 0x2484015E, 0x7F4: 0x00031840, 0x808: 0x0062001A,
        0xD00: 0x2442FFFF, 0xD04: 0xA2422004, 0xDA4: 0x1440FFF0,
        0xDE4: 4096, 0xDE8: 4096, 0xDEC: 4096, 0xDF0: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant82_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BDE4 D_8017BDE4\n'
                         '#define D_8013BDF4 D_8017BDF4\n'
                         '#define D_8013BE10 D_8017BE10\n'
                         '#include "variant82_entry.c"\n')
        source = (directory / "variant82_entry.c").read_text()
        header = (directory / "variant82_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        for declaration in (
            "Model82Config *G32 config;", "SVECTOR positions[201];",
            "SVECTOR velocities[201];", "u8 unknown_64c[0x9b8];",
            "u8 unknown_164c[0x9b8];", "u8 remaining[200], unknown_20cc[0x138];",
            "s32 texture, elapsed;", "extern GsIMAGE D_8013BDF4[];",
            "extern Model82Config D_8013BE10[];",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "SVECTOR vertices[3];", "i <= config->count",
            "setVector(&vertices[0], 0, 0, 0);",
            "RotTransPers4(&vertices[1], &vertices[1], &vertices[2], &vertices[2],",
            "direction * 350 + rand()", "rand() % config->target_spread + direction * 225",
            "work->remaining[i]--;", "goto return_zero;", "return_zero:",
            "if (work->elapsed > config->end_time)",
        ):
            self.assertIn(expression, source)
        for expression, count in (
            ("Model_GetFrameStep()", 5), ("rand()", 13),
            ("s32 sample = rand();", 2),
            ("s32 destination_y = config->target_spread + 350;", 2),
            ("RotTransPers4(", 1), ("GsSortBoxFill(", 1), ("GsSortPoly(", 1),
        ):
            self.assertEqual(source.count(expression), count)
        self.assertLess(source.index("work->elapsed = -config->delay;"),
                        source.index("work->completed = 0;"))
        self.assertIn("Native delay path passes this stack matrix without initializing it.", source)
        self.assertLess(source.index("GsSetLsMatrix(&base);"),
                        source.index("base = *(MATRIX *)Model_GetLightSourceMatrix();"))

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        path = family435.ROOT / "notes/overlays/french-model-variant82-attempts.csv"
        with path.open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 42)
        self.assertEqual(len({(r["attempt"], r["slot"]) for r in rows}), 42)
        experiments = [r for r in rows if r["result"] in ("mismatch", "text_exact")]
        self.assertEqual(len(experiments), 40)
        names = {r["attempt"] for r in experiments}
        self.assertEqual(len(names), 20)
        for name in names:
            pair = [r for r in experiments if r["attempt"] == name]
            self.assertEqual({r["slot"] for r in pair}, {"0", "1"})
            for field in ("profile", "result", "instruction_bytes", "different_words", "fingerprint"):
                self.assertEqual(pair[0][field], pair[1][field])
        exact = [r for r in experiments if r["result"] == "text_exact"]
        self.assertEqual({r["attempt"] for r in exact}, {"native-block-local-y-sampling"})
        self.assertEqual(len(exact), 2)
        for row in rows:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            expected = ("gcc_2_8_1_g0_split_no_cse_follow_jumps"
                        if row["attempt"] == "named-no-cse-follow-jumps"
                        else "gcc_2_8_1_g0_split")
            self.assertEqual(row["profile"], expected)
        terminal = [r for r in rows if r["result"] == "matched"]
        self.assertEqual({r["slot"] for r in terminal}, {"0", "1"})
        for row in terminal:
            suffix = "_slot1" if row["slot"] == "1" else ""
            path = family435.ROOT / f"src/overlays/french_model_variant/variant82_entry{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("3552", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, kind, size in ((0xDE4, "u32", 16), (0xDF4, "u8", 28)):
                self.assertIn(
                    f"D_{base + offset:X} = 0x{base + offset:X}; // type:{kind} size:0x{size:X} defined:true",
                    symbols)
            self.assertIn(f"[0xDF4, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        expected = (160,160,64,0,29,200,200,150,100,100,200,10,40,1,10,330)
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                self.assertEqual(int(row["command_word"]), 12008)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 12008)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                descriptor = struct.unpack_from("<4B12h", data, 0xE10 + 28 * 8)
                self.assertEqual(descriptor, expected)
                self.assertTrue(all(descriptor[index] > 0 for index in (5,6,9,11,13)))
                self.assertEqual(descriptor[10], 200)
                self.assertEqual(descriptor[10] + 1, 201)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(v, 16) for v in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<888I", data[4:0xDE4])
                calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                self.assertEqual(set(calls), addresses)
                self.assertEqual(len(calls), 47)
                self.assertEqual(len(addresses), 21)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x2210 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model82Config)": 28, "sizeof(Model82State)": 0x2210,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(GsBOXF)": 16, "sizeof(POLY_FT4)": 40, "sizeof(GsIMAGE)": 28,
            "sizeof(((Model82State *)0)->positions) / sizeof(SVECTOR)": 201,
            "sizeof(((Model82State *)0)->velocities) / sizeof(SVECTOR)": 201,
            "sizeof(((Model82State *)0)->remaining)": 200,
        }
        for typename, fields in (
            ("Model82Config", (
                ("part", 4), ("spread", 6), ("target_spread", 8), ("half_size", 10),
                ("y_bias", 12), ("travel_duration", 14), ("count", 16),
                ("spark_divisor", 18), ("delay", 20), ("rate", 22),
                ("unknown_18", 24), ("end_time", 26),
            )),
            ("Model82State", (
                ("positions", 4), ("velocities", 0x1004), ("remaining", 0x2004),
                ("texture", 0x2204), ("elapsed", 0x2208), ("completed", 0x220C),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 29)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant82_entry.h")
