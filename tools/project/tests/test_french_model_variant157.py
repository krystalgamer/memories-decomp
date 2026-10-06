import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant157Tests(family435.FrenchModelVariant435Tests):
    family = 157
    slot_header_delta = 130
    module_count = 4
    distinct_images = 4
    binding_count = 27
    tail_start = 0x1084
    spans = ((4, 0x1058),)
    helpers = ((4, 4180, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (131, 145)),)
    entry_anchors = {
        4: 0x27BDFD38, 0x1050: 0x03E00008, 0x1054: 0x27BD02C8,
        0x1058: 4096, 0x105C: 4096, 0x1060: 4096, 0x1064: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant157_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013C058 D_8017C058\n'
                         '#define D_8013C068 D_8017C068\n'
                         '#define D_8013C084 D_8017C084\n'
                         '#include "variant157_entry.c"\n')
        source = (directory / "variant157_entry.c").read_text()
        header = (directory / "variant157_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        for declaration in (
            "Model157Config *G32 config;",
            "SVECTOR origin, ring[33], starts[32], trails[192];",
            "u8 trail_red, trail_green, trail_blue, unknown_09;",
            "u32 texture[1];", "extern GsIMAGE D_8013C068[];",
            "extern Model157Config D_8013C084[];",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "DVECTOR projected[34];", "u16 depths[34]",
            "angle = i * 4096 / 32;",
            "s32 ring_time = time - 12;",
            "point->pad = (point->pad + 1) % 6;",
            "history = &row[(point->pad + j - 1) % 6];",
            "phase_time = config->spacing * 32;",
            "terminal_duration = config->ring_duration + 12;",
            "if (time >= phase_time + terminal_duration)",
        ):
            self.assertIn(expression, source)
        for expression, count in (
            ("Model_GetFrameStep()", 1), ("rand()", 1), ("ccos(", 2),
            ("csin(", 2), ("RotTransPersN(", 1), ("MulMatrix2(", 1),
            ("RotAverage4(", 2), ("GsSortPoly(", 2), ("func_8005B260(", 5),
        ):
            self.assertEqual(source.count(expression), count)
        self.assertLess(source.index("history = &row[(point->pad + j - 1) % 6];"),
                        source.index("setRGB0(&quad, red, green, blue);"))

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        path = family435.ROOT / "notes/overlays/french-model-variant157-attempts.csv"
        with path.open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 44)
        self.assertEqual(len({(r["attempt"], r["slot"]) for r in rows}), 44)
        experiments = [r for r in rows if r["result"] in ("mismatch", "text_exact")]
        self.assertEqual(len(experiments), 42)
        names = {r["attempt"] for r in experiments}
        self.assertEqual(len(names), 21)
        for name in names:
            pair = [r for r in experiments if r["attempt"] == name]
            self.assertEqual({r["slot"] for r in pair}, {"0", "1"})
            for field in ("profile", "result", "instruction_bytes", "different_words", "fingerprint"):
                self.assertEqual(pair[0][field], pair[1][field])
        exact = [r for r in experiments if r["result"] == "text_exact"]
        self.assertEqual({r["attempt"] for r in exact}, {"native-staged-terminal-intervals"})
        self.assertEqual(len(exact), 2)
        for row in rows:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            expected = ("gcc_2_8_1_g0_split_no_cse_follow_jumps"
                        if row["attempt"] == "native-named-no-cse-follow-jumps"
                        else "gcc_2_8_1_g0_split")
            self.assertEqual(row["profile"], expected)
        terminal = [r for r in rows if r["result"] == "matched"]
        self.assertEqual({r["slot"] for r in terminal}, {"0", "1"})
        for row in terminal:
            suffix = "_slot1" if row["slot"] == "1" else ""
            path = family435.ROOT / f"src/overlays/french_model_variant/variant157_entry{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("4180", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, kind, size in ((0x1058, "u32", 16), (0x1068, "u8", 28)):
                self.assertIn(
                    f"D_{base + offset:X} = 0x{base + offset:X}; // type:{kind} size:0x{size:X} defined:true",
                    symbols)
            self.assertIn(f"[0x1068, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (192,192,192,128,128,128,96,0,128,10,500,150,1200,60,30,30,60,7,50),
            1: (192,128,192,128,128,128,96,0,128,10,600,150,1200,160,50,30,60,7,44),
        }
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = {131: 0, 145: 1}[int(row["model"])]
                self.assertEqual(int(row["command_word"]), 88000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 88000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                descriptor = struct.unpack_from("<10B9h", data, 0x1084 + 28 * argument)
                self.assertEqual(descriptor, descriptors[argument])
                self.assertGreater(descriptor[10], 2 * descriptor[11])
                self.assertTrue(all(descriptor[index] > 0 for index in (11,12,13,14,16,17)))
                observed.add(argument)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(v, 16) for v in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1045I", data[4:0x1058])
                calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                self.assertEqual(set(calls), addresses)
                self.assertEqual(len(calls), 50)
                self.assertEqual(len(addresses), 27)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x820 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model157Config)": 28, "sizeof(Model157State)": 0x820,
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(DVECTOR)": 4, "sizeof(POLY_G3)": 28, "sizeof(POLY_F4)": 24,
            "sizeof(POLY_FT4)": 40, "sizeof(GsIMAGE)": 28,
            "sizeof(DVECTOR[34])": 136, "sizeof(u16[34])": 68, "sizeof(SVECTOR[192])": 1536,
        }
        for typename, fields in (
            ("Model157Config", (
                ("red", 0), ("green", 1), ("blue", 2), ("sprite_red", 3),
                ("sprite_green", 4), ("sprite_blue", 5), ("trail_red", 6),
                ("trail_green", 7), ("trail_blue", 8), ("unknown_09", 9),
                ("radius", 10), ("half_size", 12), ("fall_distance", 14),
                ("ring_duration", 16), ("fall_duration", 18), ("unknown_14", 20),
                ("flash_duration", 22), ("spacing", 24), ("delay", 26),
            )),
            ("Model157State", (
                ("config", 0), ("origin", 4), ("ring", 12), ("starts", 276),
                ("trails", 532), ("texture", 2068), ("completed", 2072),
                ("toggle", 2073), ("elapsed", 2076),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 41)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant157_entry.h")
