import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant144Tests(family435.FrenchModelVariant435Tests):
    family = 144
    slot_header_delta = 130
    module_count = 2
    distinct_images = 2
    binding_count = 30
    tail_start = 0xEDC
    spans = ((4, 0xEB0),)
    helpers = ((4, 3756, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (492,)),)
    entry_anchors = {
        4: 0x27BDFC28, 0x94: 0x2454FFFF, 0x164: 0x00839821,
        0x260: 0x2A820040, 0x2CC: 0xA3C00224, 0x624: 0x3202FFFF,
        0x9F8: 0x24020040, 0xA4C: 0x87C2021E,
        0xBB4: 0x00042140, 0xBB8: 0x2482001F,
        0xBBC: 0xA3A40084, 0xBC4: 0xA3A40094,
        0xE68: 0x14400004, 0xE70: 0xA3C20224,
        0xEA8: 0x03E00008, 0xEAC: 0x27BD03D8,
        0xEB0: 4096, 0xEB4: 4096, 0xEB8: 4096, 0xEBC: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant144_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BEB0 D_8017BEB0\n'
                         '#define D_8013BEC0 D_8017BEC0\n'
                         '#define D_8013BEDC D_8017BEDC\n'
                         '#include "variant144_entry.c"\n')
        source = (directory / "variant144_entry.c").read_text()
        header = (directory / "variant144_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        self.assertNotRegex(source, r"tint_(?:start|end)\.b3\s*=")
        for declaration in (
            "Model144Config *G32 config;", "u8 unknown_004[24];", "SVECTOR points[64];",
            "s32 texture[1], elapsed;", "u8 started, frame_step_override;",
            "s16 parts[8];", "s16 tint_starts[8], tint_durations[8], tint_ramp_durations[8];",
            "extern GsIMAGE D_8013BEC0[];", "extern Model144Config D_8013BEDC[];",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "&D_8013BEDC[command % 100]", "if (command >= 100)",
            "value = config->radius + (rand() % config->radius) / 2;",
            "value * ccos(angle0) / 4096 * ccos(angle1) / 4096",
            "value * csin(angle0) / 4096 * ccos(angle1) / 4096",
            "ModelTintColor tint_start, tint_end;", "u16 red, green, blue;",
            "RotTransPers(&vertices[0], (PSXLONG *)&gradient.x3, &parameter, &flag);",
            "setXY3(&gradient, 319, 255, gradient.x3, 255, 319, gradient.y3);",
            "box.attribute = 0x50000000;", "box.w = 1;", "box.h = 1;",
            "SVECTOR vertices[4];", "DVECTOR projected[64];",
            "u16 depths[64], interpolation[64], flags[64];",
            "RotTransPersN(work->points, projected, depths, interpolation, flags, 64);",
            "depth = depths[i] >> 2;", "quad.tpage = work->texture[0] >> 16;",
            "u = (time / 2 % 3) * 32;",
            "setUV4(&quad, u, 0, u + 31, 0, u, 31, u + 31, 31);",
            "setVector(&rotation, 0, 0, time * 100);",
            "work->elapsed += step;", "if (work->started)",
        ):
            self.assertIn(expression, source)
        for expression, count in (
            ("Model_GetFrameStep()", 1), ("rand()", 3), ("ccos(", 3), ("csin(", 2),
            ("Model_GetSlotAnimationFrame(", 1), ("Model_SetFrameStepOverride(", 1),
            ("setXY3(&gradient,", 4), ("func_8005B260(", 5),
            ("RotAverage4(", 1), ("RotTransPersN(", 1), ("MulMatrix2(", 1),
            ("GsSortPoly(", 1), ("GsSortBoxFill(", 1),
        ):
            self.assertEqual(source.count(expression), count)

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        path = family435.ROOT / "notes/overlays/french-model-variant144-attempts.csv"
        with path.open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 11)
        self.assertEqual(len({(r["attempt"], r["slot"]) for r in rows}), 11)
        failed, = [r for r in rows if r["result"] == "compile_error"]
        self.assertEqual(failed["attempt"], "native-sphere-gradient-and-part-glow")
        self.assertIn("setXY2", failed["reason"])
        experiments = [r for r in rows if r["result"] in ("mismatch", "text_exact")]
        self.assertEqual(len(experiments), 8)
        names = {r["attempt"] for r in experiments}
        self.assertEqual(len(names), 4)
        for name in names:
            pair = [r for r in experiments if r["attempt"] == name]
            self.assertEqual({r["slot"] for r in pair}, {"0", "1"})
            for field in ("profile", "result", "instruction_bytes", "different_words", "fingerprint"):
                self.assertEqual(pair[0][field], pair[1][field])
        exact = [r for r in experiments if r["result"] == "text_exact"]
        self.assertEqual({r["attempt"] for r in exact}, {"native-local-atlas-coordinate"})
        self.assertEqual(len(exact), 2)
        for row in rows:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
        terminal = [r for r in rows if r["result"] == "matched"]
        self.assertEqual({r["slot"] for r in terminal}, {"0", "1"})
        for row in terminal:
            suffix = "_slot1" if row["slot"] == "1" else ""
            path = family435.ROOT / f"src/overlays/french_model_variant/variant144_entry{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("3756", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, kind, size in ((0xEB0, "u32", 16), (0xEC0, "u8", 28)):
                self.assertIn(
                    f"D_{base + offset:X} = 0x{base + offset:X}; // type:{kind} size:0x{size:X} defined:true",
                    symbols)
            self.assertIn(f"[0xEC0, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        expected = (
            255,255,255,255,64,64,128,128,128,31,32,-1,-1,-1,-1,-1,-1,300,50,60,90,80,124,
            70,-1,-1,-1,-1,-1,-1,-1,30,-1,-1,-1,-1,-1,-1,-1,
            7,-1,-1,-1,-1,-1,-1,-1,164,40)
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                self.assertEqual(int(row["command_word"]), 75100)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, 75100)
                argument = command % 1000
                self.assertEqual(argument, 100)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                descriptor = struct.unpack_from("<9Bx40h", data, 0xEDC + 90 * (argument % 100))
                self.assertEqual(descriptor, expected)
                self.assertEqual(descriptor[9:17], (31,32,-1,-1,-1,-1,-1,-1))
                self.assertEqual(descriptor[23:31], (70,-1,-1,-1,-1,-1,-1,-1))
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(v, 16) for v in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<939I", data[4:0xEB0])
                calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                self.assertEqual(set(calls), addresses)
                self.assertEqual(len(calls), 51)
                self.assertEqual(len(addresses), 30)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x228 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model144Config)": 90, "sizeof(Model144State)": 0x228,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(POLY_FT4)": 40, "sizeof(POLY_G4)": 36, "sizeof(GsBOXF)": 16,
            "sizeof(ModelTintColor)": 4, "sizeof(GsIMAGE)": 28,
            "sizeof(DVECTOR)": 4, "sizeof(PSXLONG)": 4,
            "sizeof(((Model144Config *)0)->parts) / sizeof(s16)": 8,
            "sizeof(((Model144Config *)0)->tint_starts) / sizeof(s16)": 8,
            "sizeof(((Model144Config *)0)->tint_durations) / sizeof(s16)": 8,
            "sizeof(((Model144Config *)0)->tint_ramp_durations) / sizeof(s16)": 8,
            "sizeof(((Model144State *)0)->points) / sizeof(SVECTOR)": 64,
            "sizeof(((Model144State *)0)->texture) / sizeof(s32)": 1,
        }
        for typename, fields in (
            ("Model144Config", (
                ("red", 0), ("particle_red", 3), ("glow_red", 6), ("unknown_09", 9),
                ("parts", 10), ("radius", 26), ("half_size", 28),
                ("gradient_duration", 30), ("particle_duration", 32),
                ("glow_fade_in", 34), ("glow_duration", 36), ("tint_starts", 38),
                ("tint_durations", 54), ("tint_ramp_durations", 70),
                ("main_delay", 86), ("glow_delay", 88),
            )),
            ("Model144State", (
                ("config", 0), ("unknown_004", 4), ("points", 28), ("texture", 540),
                ("elapsed", 544), ("started", 548), ("frame_step_override", 549),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 41)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant144_entry.h")
