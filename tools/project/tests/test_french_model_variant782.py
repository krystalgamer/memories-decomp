import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant782Tests(family435.FrenchModelVariant435Tests):
    family = 782
    slot_header_delta = 97
    module_count = 12
    distinct_images = 12
    binding_count = 26
    tail_start = 0x154C
    spans = ((4, 0x14B0),)
    helpers = ((4, 5292, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (58, 217, 453, 563, 612)), (9, (78,)))
    entry_anchors = {
        4: 0x27BDFDB8, 0x14A8: 0x03E00008, 0x14AC: 0x27BD0248,
        0x14B0: 4096, 0x14B4: 4096, 0x14B8: 4096, 0x14BC: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant782_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013C4B0 D_8017C4B0\n'
                         '#define D_8013C4C0 D_8017C4C0\n'
                         '#define D_8013C54C D_8017C54C\n'
                         '#include "variant782_entry.c"\n')
        source = (directory / "variant782_entry.c").read_text()
        header = (directory / "variant782_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        for declaration in (
            "Model782Config *G32 config;", "SVECTOR origin;",
            "SVECTOR debris_positions[16];", "u8 unknown_08C[0x180];",
            "SVECTOR debris_velocities[16];", "u8 unknown_28C[0x180];",
            "SVECTOR dot_positions[64];", "SVECTOR dot_velocities[64];",
            "DVECTOR screen[64];", "s32 texture[5];", "u16 depths[64];",
            "s16 animation, elapsed, phase, growth_age;",
            "extern GsIMAGE D_8013C4C0[];", "extern Model782Config D_8013C54C[];",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "const VECTOR D_8013C4B0 = {4096, 4096, 4096, 0};",
            "&D_8013C54C[command]", "Model_CopySlotU16Values(slot ^ 1, (u16 *)&work->origin);",
            "for (i = 0; i < 5; i++)", "else if (work->phase > 0)",
            "point[i].vx += other[i].vx;", "RotTrans(&point[i], (VECTOR *)matrix.t, &flag);",
            "other->vy += (350 - other->vy) / work->config->dot_speed / 2;",
            "other->vz = work->config->dot_speed * csin(k) / 4096 * ccos(j) / 4096;",
            "if (work->elapsed < work->config->burst_end)",
            "func_8005F7B0(32, 8);", "work->shade -= step * 6;",
            "if (work->elapsed < work->config->duration)",
        ):
            self.assertIn(expression, source)
        for expression, count in (
            ("Model_SetFrameStepOverride(", 1), ("Model_GetFrameStep()", 1),
            ("SetPolyFT4(&quad)", 1), ("RotAverage4(", 5),
            ("MulMatrix2(", 2), ("GsSortBoxFill(&box, ot, work->depths[i] / 4)", 2),
            ("func_80059A50(", 1), ("func_8005F7B0(", 1),
        ):
            self.assertEqual(source.count(expression), count)
        third_panel = source[source.index("position.vx -= 2 * work->config->x_offset;"):]
        self.assertLess(third_panel.index("setVector(&vertices[0]"),
                        third_panel.index("GsSetLsMatrix(&base);"))

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        with (family435.ROOT / "notes/overlays/french-model-variant782-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        expected = {
            "native-two-dot-passes-four-panels": (5264, 576),
            "native-third-panel-and-cursor-order": (5284, 863),
            "native-halfword-dot-priorities": (5284, 863),
            "dot-priority-division-prior-art": (5292, 47),
            "shared-constructor-random-carriers": (5292, 40),
            "constructor-carrier-declaration-order": (5292, 40),
            "corresponding-random-angle-carriers": (5292, 0),
        }
        self.assertEqual(len(rows), 16)
        self.assertEqual(len({(r["attempt"], r["slot"]) for r in rows}), 16)
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
        terminal = [r for r in rows if r["result"] == "matched"]
        self.assertEqual({r["slot"] for r in terminal}, {"0", "1"})
        for row in terminal:
            suffix = "_slot1" if row["slot"] == "1" else ""
            path = family435.ROOT / f"src/overlays/french_model_variant/variant782_entry{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("5292", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(
                f"D_{base + 0x14C0:X} = 0x{base + 0x14C0:X}; // type:u8 size:0x8C defined:true",
                symbols)
            self.assertIn(f"[0x14C0, data, overlays/{module['name']}/image_view]", layout.read_text())
            self.assertIn(
                f"D_{base + 0x14B0:X} = 0x{base + 0x14B0:X}; // type:u32 size:0x10 defined:true",
                symbols)
            suffix = "_slot1" if module["name"].endswith("1") else ""
            self.assertIn(f"[0x14B0, .rodata, overlays/french_model_variant/variant782_entry{suffix}]",
                          layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (60, 0, 512, 0, 80, 64, 128, 0, 1024, 16, 64, 200, 24, 64, 64, 194, 196, 404),
            2: (60, 0, 512, 0, 60, 64, 128, 0, 1024, 16, 64, 200, 24, 64, 64, 72, 78, 150),
            3: (60, 0, 512, 0, 60, 64, 128, 0, 1024, 16, 64, 200, 24, 64, 64, 72, 76, 200),
            4: (60, 0, 512, 0, 60, 64, 128, 0, 1024, 16, 64, 200, 24, 64, 64, 136, 140, 300),
        }
        arguments = {58: 0, 217: 2, 453: 4, 563: 0, 612: 2, 78: 3}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = arguments[int(row["model"])]
                self.assertEqual(int(row["command_word"]), 314000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 314000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<8hi8hi", data, 0x154C + 40 * argument),
                                 descriptors[argument])
                observed.add(argument)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1323I", data[4:0x14B0])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 74)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x9B0 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model782Config)": 40, "sizeof(Model782State)": 0x9B0,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(GsIMAGE)": 28, "sizeof(POLY_FT4)": 40,
            "sizeof(GsBOXF)": 16, "sizeof(DVECTOR)": 4,
        }
        for typename, fields in (
            ("Model782Config", (("growth_rate", 16), ("debris_count", 20), ("dot_count", 28),
                                ("initial_delay", 32), ("burst_end", 34), ("duration", 36))),
            ("Model782State", (("origin", 4), ("debris_positions", 12),
                               ("debris_velocities", 0x20C), ("dot_positions", 0x40C),
                               ("dot_velocities", 0x60C), ("screen", 0x80C),
                               ("texture", 0x90C), ("level", 0x920), ("depths", 0x924),
                               ("animation", 0x9A4), ("elapsed", 0x9A6), ("phase", 0x9A8),
                               ("growth_age", 0x9AA), ("shade", 0x9AC))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 29)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant782_entry.h")
