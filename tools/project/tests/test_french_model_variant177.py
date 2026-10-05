import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant177Tests(family435.FrenchModelVariant435Tests):
    family = 177
    slot_header_delta = 130
    module_count = 10
    distinct_images = 10
    binding_count = 24
    tail_start = 0x1200
    spans = ((4, 0x119C),)
    helpers = ((4, 4504, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (164, 191, 494, 521, 599)),)
    entry_anchors = {
        4: 0x27BDFD70, 0x1194: 0x03E00008, 0x1198: 0x27BD0290,
        0x119C: 4096, 0x11A0: 4096, 0x11A4: 4096, 0x11A8: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant177_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013C19C D_8017C19C\n'
                         '#define D_8013C1AC D_8017C1AC\n'
                         '#define D_8013C200 D_8017C200\n'
                         '#include "variant177_entry.c"\n')
        source = (directory / "variant177_entry.c").read_text()
        header = (directory / "variant177_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        for declaration in (
            "Model177Config *G32 config;", "SVECTOR ribbon[34];",
            "SVECTOR positions[64];", "SVECTOR origin;",
            "u8 unknown_31C[0x1F8];", "SVECTOR vertices[9];",
            "SVECTOR *G32 faces[16];", "u32 texture[2];",
            "extern GsIMAGE D_8013C1AC[];", "extern Model177Config D_8013C200[];",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "const VECTOR D_8013C19C = {4096, 4096, 4096, 0};",
            "&D_8013C200[command % 100]", "config->half_width * (i << 1)",
            "DVECTOR screen[2][17];", "u16 depths[2][17], projection[2][17], flags[2][17];",
            "origin = &work->origin;", "point->pad = 0;",
            "RotTransPersN(work->ribbon, screen[0], depths[0], projection[0], flags[0], 34);",
            "AverageZ4(depth0[0], depth0[1], depth1[0], depth1[1])",
            "(flags0[0] | flags0[1] | flags1[0] | flags1[1]) & 0x20",
            "func_8005B260((u32 *)&flash, ot, 1, 1);",
            "work->elapsed += step;", "work->animation++;", "work->completed++;",
            "if (time > config->fade_duration)",
        ):
            self.assertIn(expression, source)
        self.assertEqual(source.count("csin(i * 128)"), 2)
        self.assertEqual(source.count("origin++"), 1)
        self.assertEqual(source.count("if (depth >= 0 && flag >= 0)"), 2)
        self.assertEqual(source.count("MulMatrix2(&base, &matrix);"), 1)
        self.assertLess(source.index("step = Model_GetFrameStep();"), source.index("if (command >= 0)"))
        capture = source.index("value = work->animation % 2 * 256 + 4096;")
        self.assertLess(capture, source.index("setVector(&rotation, 0, 0, time * 32);"))
        self.assertLess(source.index("setVector(&position, matrix.t[0], matrix.t[1], matrix.t[2]);"),
                        capture)

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        with (family435.ROOT / "notes/overlays/french-model-variant177-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 35)
        self.assertEqual(len({(r["attempt"], r["slot"]) for r in rows}), 35)
        expected = {
            "typed-light-source-matrix": (4536, 1091),
            "shared-flash-color-lifetimes": (4520, 1079),
            "single-grid-scale-value": (4524, 1036),
            "paired-edge-projection-arrays": (4396, 1038),
            "explicit-edge-projection-cursors": (4516, 237),
            "native-local-declaration-order": (4516, 194),
            "positive-terminal-phase-scope": (4508, 192),
            "particle-cursor-before-index": (4504, 51),
            "ribbon-cursors-before-indices": (4504, 18),
            "doubled-index-before-width": (4504, 18),
            "shifted-ribbon-sample-index": (4504, 14),
            "face-table-before-grid-base": (4504, 12),
            "z-first-grid-translation": (4504, 12),
            "grid-rotation-before-translation": (4504, 12),
            "capture-animation-before-rotation": (4504, 0),
            "single-observed-origin-view": (4504, 0),
        }
        experiments = [r for r in rows if r["result"] in ("mismatch", "text_exact")]
        self.assertEqual({(r["attempt"], r["slot"]) for r in experiments},
                         {(name, slot) for name in expected for slot in ("0", "1")})
        for row in rows:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
        for row in experiments:
            size, differences = expected[row["attempt"]]
            self.assertEqual((int(row["instruction_bytes"]), int(row["different_words"])),
                             (size, differences))
            self.assertEqual(row["result"], "text_exact" if differences == 0 else "mismatch")
        failure, = [r for r in rows if r["result"] == "compile_error"]
        self.assertEqual((failure["attempt"], failure["slot"]),
                         ("native-ribbon-grid-flash", "0"))
        self.assertEqual((failure["instruction_bytes"], failure["different_words"]), ("", ""))
        self.assertIn("No entry or slot1 comparison occurred.", failure["reason"])
        terminal = [r for r in rows if r["result"] == "matched"]
        self.assertEqual({r["slot"] for r in terminal}, {"0", "1"})
        for row in terminal:
            suffix = "_slot1" if row["slot"] == "1" else ""
            path = family435.ROOT / f"src/overlays/french_model_variant/variant177_entry{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("4504", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size, name in ((0x11AC, 56, "image_view"), (0x11E4, 28, "unclassified_gap")):
                self.assertIn(f"D_{base + offset:X} = 0x{base + offset:X}; // type:u8 size:0x{size:X} defined:true",
                              symbols)
                self.assertIn(f"[0x{offset:X}, data, overlays/{module['name']}/{name}]", layout.read_text())
            self.assertIn(f"D_{base + 0x119C:X} = 0x{base + 0x119C:X}; // type:u32 size:0x10 defined:true", symbols)
            suffix = "_slot1" if module["name"].endswith("1") else ""
            self.assertIn(f"[0x119C, .rodata, overlays/french_model_variant/variant177_entry{suffix}]",
                          layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (128, 128, 128, 128, 128, 128, 255, 255, 255, 24, 36, 0, 400, 450, 350, 100, 60, 40, 90, 8, 90),
            1: (64, 64, 16, 32, 32, 32, 255, 255, 255, 20, 64, 0, 700, 450, 350, 100, 60, 60, 90, 4, 70),
            2: (96, 96, 96, 0, 0, 0, 0, 0, 0, 1, 4, 0, 800, 450, 350, 100, 60, 60, 90, 60, 70),
            3: (128, 0, 32, 128, 0, 32, 255, 224, 224, 22, 12, 0, 800, 450, 350, 100, 60, 60, 90, 10, 80),
        }
        arguments = {164: 3, 191: 0, 494: 1, 521: 2, 599: 0}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot = int(row["slot"])
                argument = arguments[int(row["model"])]
                self.assertEqual(int(row["command_word"]), 108000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048 + 0x114)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 108000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                descriptor = struct.unpack_from("<12B9h", data, 0x1200 + 30 * argument)
                self.assertEqual(descriptor, descriptors[argument])
                self.assertTrue(0 < descriptor[10] <= 64)
                self.assertTrue(all(descriptor[index] > 0 for index in (16, 17, 18, 19, 20)))
                observed.add(argument)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                for name, address in (("GsGetLwUnit", 0x8008A428), ("RotMatrix", 0x80087CB8),
                                      ("ScaleMatrix", 0x800875F8), ("RotTransPersN", 0x80087C48)):
                    self.assertIn(f"{name} = 0x{address:X};", bindings)
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1126I", data[4:0x119C])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 41)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x5B0 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model177Config)": 30, "sizeof(Model177State)": 0x5B0,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(GsIMAGE)": 28, "sizeof(POLY_FT4)": 40, "sizeof(POLY_F4)": 24,
            "sizeof(DVECTOR)": 4,
        }
        for typename, fields in (
            ("Model177Config", (("part", 9), ("count", 10), ("half_width", 12), ("height", 14),
                                ("depth", 16), ("grid_half_size", 18), ("travel_duration", 20),
                                ("fade_duration", 22), ("flash_duration", 24), ("spacing", 26),
                                ("delay", 28))),
            ("Model177State", (("ribbon", 4), ("positions", 0x114), ("origin", 0x314),
                               ("vertices", 0x514), ("faces", 0x55C), ("texture", 0x59C),
                               ("completed", 0x5A4), ("elapsed", 0x5A8), ("animation", 0x5AC))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 29)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant177_entry.h")
