import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant163Tests(family435.FrenchModelVariant435Tests):
    family = 163
    slot_header_delta = 130
    module_count = 2
    distinct_images = 2
    binding_count = 26
    tail_start = 0xEF0
    spans = ((4, 0xEA8),)
    helpers = ((4, 3748, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (37,)),)
    entry_anchors = {
        4: 0x27BDFC40, 0x3C0: 0x96A20456, 0x59C: 0x02820018,
        0x6D0: 0x92E20000, 0x6D4: 0x92E30001, 0x6D8: 0x92E40002,
        0x6DC: 0xA3A2006C, 0x6E0: 0xA3A3006D, 0x6E4: 0xA3A4006E,
        0x9E4: 0x96A2045A, 0xEA0: 0x03E00008, 0xEA4: 0x27BD03C0,
        0xEA8: 4096, 0xEAC: 4096, 0xEB0: 4096, 0xEB4: 0,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant163_entry_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013B004 func_8017B004\n'
                         '#define D_8013BEA8 D_8017BEA8\n'
                         '#define D_8013BEB8 D_8017BEB8\n'
                         '#define D_8013BEF0 D_8017BEF0\n'
                         '#include "variant163_entry.c"\n')
        source = (directory / "variant163_entry.c").read_text()
        header = (directory / "variant163_entry.h").read_text()
        self.assertNotRegex(source, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        for declaration in (
            "Model163Config *G32 config;", "SVECTOR points[130];",
            "SVECTOR *G32 quads[16];", "u32 texture[2];",
            "u8 completed, unknown_45d[3];", "s32 elapsed;",
            "extern GsIMAGE D_8013BEB8[];", "extern Model163Config D_8013BEF0[];",
        ):
            self.assertIn(declaration, header)
        for expression in (
            "SVECTOR *G32 *vertices;",
            "&D_8013BEF0[command % 100]", "i < 96", "point->pad = 0;",
            "point = &work->points[54];", "point = &work->points[63];",
            "copyVector(point, point - 2);", "addVector(point, point - 1);",
            "applyVector(point, 2, 2, 2, /=);", "i < 48",
            "work->elapsed - config->delay - i * config->stagger",
            "u8 red, green, blue;", "red = config->red;",
            "green = config->green;", "blue = config->blue;",
            "RotTransPersN(&work->points[63], projected, depths, interpolation, flags, 66);",
            "DVECTOR projected[66];", "u16 depths[66], interpolation[66], flags[66];",
            "copyVector(&work->points[129], &work->points[0]);",
            "255 * (config->split_time - time) / config->split_time",
            "work->elapsed += step;", "work->completed++;",
        ):
            self.assertIn(expression, source)
        for expression, count in (
            ("Model_GetFrameStep()", 1), ("vertices = work->quads;", 2),
            ("j < 4; vertices += 4, j++", 2), ("RotAverage4(", 2),
            ("RotTransPersN(", 1), ("GsSortPoly(", 3),
        ):
            self.assertEqual(source.count(expression), count)
        self.assertLess(source.index("blue = config->blue;"),
                        source.index("setRGB0(&quad, red, green, blue);"))
        self.assertLess(source.index("addVector(point, point - 1);"),
                        source.index("applyVector(point, 2, 2, 2, /=);"))

    def test_attempt_ledger_preserves_failed_and_exact_experiments(self):
        path = family435.ROOT / "notes/overlays/french-model-variant163-attempts.csv"
        with path.open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 16)
        self.assertEqual(len({(r["attempt"], r["slot"]) for r in rows}), 16)
        experiments = [r for r in rows if r["result"] in ("mismatch", "text_exact")]
        self.assertEqual(len(experiments), 14)
        names = {r["attempt"] for r in experiments}
        self.assertEqual(len(names), 7)
        for name in names:
            pair = [r for r in experiments if r["attempt"] == name]
            self.assertEqual({r["slot"] for r in pair}, {"0", "1"})
            for field in ("profile", "result", "instruction_bytes", "different_words", "fingerprint"):
                self.assertEqual(pair[0][field], pair[1][field])
        exact = [r for r in experiments if r["result"] == "text_exact"]
        self.assertEqual({r["attempt"] for r in exact}, {"native-unsigned-packed-textures"})
        self.assertEqual(len(exact), 2)
        for row in rows:
            self.assertRegex(row["fingerprint"], r"^[0-9a-f]{64}$")
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
        terminal = [r for r in rows if r["result"] == "matched"]
        self.assertEqual({r["slot"] for r in terminal}, {"0", "1"})
        for row in terminal:
            suffix = "_slot1" if row["slot"] == "1" else ""
            path = family435.ROOT / f"src/overlays/french_model_variant/variant163_entry{suffix}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), ("3748", "0"))

    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, kind, size in ((0xEA8, "u32", 16), (0xEB8, "u8", 56)):
                self.assertIn(
                    f"D_{base + offset:X} = 0x{base + offset:X}; // type:{kind} size:0x{size:X} defined:true",
                    symbols)
            self.assertIn(f"[0xEB8, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        expected = (128,48,48,192,192,96,26,21,200,50,4,120,76,40,90,160)
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                self.assertEqual(int(row["command_word"]), 94000)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, 94000)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<8B8h", data, 0xEF0 + 24 * (command % 100)),
                                 expected)
                self.assertTrue(all(expected[index] > 0 for index in (11,12,13,14)))
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(v, 16) for v in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<937I", data[4:0xEA8])
                calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                self.assertEqual(set(calls), addresses)
                self.assertEqual(len(calls), 53)
                self.assertEqual(len(addresses), 26)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x464 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model163Config)": 24, "sizeof(Model163State)": 0x464,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(POLY_FT4)": 40, "sizeof(POLY_F4)": 24, "sizeof(GsIMAGE)": 28,
            "sizeof(((Model163State *)0)->points) / sizeof(SVECTOR)": 130,
            "sizeof(((Model163State *)0)->quads) / sizeof(SVECTOR *)": 16,
            "sizeof(((Model163State *)0)->texture) / sizeof(u32)": 2,
        }
        for typename, fields in (
            ("Model163Config", (
                ("first_part", 6), ("second_part", 7), ("radius", 8), ("thickness", 10),
                ("stagger", 12), ("particle_duration", 14), ("split_time", 16),
                ("flash_duration", 18), ("ring_duration", 20), ("delay", 22),
            )),
            ("Model163State", (
                ("config", 0), ("points", 4), ("points[48]", 0x184),
                ("points[54]", 0x1B4), ("points[63]", 0x1FC), ("points[96]", 0x304),
                ("points[129]", 0x40C), ("quads", 0x414), ("texture", 0x454),
                ("completed", 0x45C), ("elapsed", 0x460),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 32)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant163_entry.h")
