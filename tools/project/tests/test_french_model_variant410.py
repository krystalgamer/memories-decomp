import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import tempfile
import unittest

import yaml

ROOT = Path(__file__).resolve().parents[3]
CONFIG = ROOT / "config/sles_03948"
SOURCE = ROOT / "src/overlays/french_model_variant/variant410_rings.c"
PROFILE = "gcc_2_8_1_g0_split"
IMAGES = (
    (7, 0, 15084, "222821e376a8958432667010b60d0bf3ec0c5e435a82edc8f591bf3b84f866d7"),
    (8, 1, 15094, "7062c4fd809234a8f7be3b4e4160ba8d304725bf1e3d18205f3ea257d92ac62e"),
)
SPANS = ((4, 0xAAC), (0xAAC, 0xF58), (0xF58, 0x18B8), (0x18B8, 0x1D04))


class FrenchModelVariant410Tests(unittest.TestCase):
    def setUp(self):
        self.modules = {row["name"]: row for row in json.loads(
            (CONFIG / "overlays.json").read_text())["modules"]}
        with (ROOT / "notes/overlays/french-model-variant410-instances.csv").open() as handle:
            self.instances = {row["module"]: row for row in csv.DictReader(handle)}

    def test_two_distinct_physical_registrations(self):
        self.assertEqual(set(self.instances), {
            f"french_model_variant_54_stage{stage}_slot{slot}" for stage, slot, *_ in IMAGES
        })
        for stage, slot, sector, digest in IMAGES:
            name = f"french_model_variant_54_stage{stage}_slot{slot}"
            module, instance = self.modules[name], self.instances[name]
            self.assertEqual(module["archive"], "game/france/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"],
                             "0c3f90cf4a3b4776d1188054a6cd5d89c5bc364215dac15c4730ef74edbc8da3")
            self.assertEqual((module["sector_offset"], module["sector_count"]), (sector, 10))
            self.assertEqual(module["load_address"], f"0x{0x8013B000 + slot * 0x40000:X}")
            self.assertEqual(module["sha256"], digest)
            self.assertEqual(module["linker_symbols"],
                             "config/sles_03948/overlays/model_variant402_linker_symbols.txt")
            self.assertEqual(module["output"], f"tmp/overlays/{name}/module.bin")
            self.assertEqual((int(instance["model"]), int(instance["stage"]),
                              int(instance["slot"]), int(instance["header"]),
                              int(instance["sector"]), instance["sha256"]),
                             (54, stage, slot, 410 + slot * 150, sector, digest))

    def test_c_owner_and_assembly_raw_fallbacks(self):
        from tools.project.overlay_sources import c_segments

        for stage, slot, *_ in IMAGES:
            name = f"french_model_variant_54_stage{stage}_slot{slot}"
            module = self.modules[name]
            base = int(module["load_address"], 0)
            layout = ROOT / module["layout"]
            source = "src/overlays/french_model_variant/variant410_rings" + (
                "_slot1" if slot else "") + ".c"
            quad_source = source.replace("variant410_rings", "variant410_quads")
            matching = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(matching, {"schema": 1, "functions": [{
                "address": f"0x{base + 0xAAC:X}", "size": "0x4AC",
                "profile": PROFILE, "source": quad_source,
            }, {
                "address": f"0x{base + 0x18B8:X}", "size": "0x44C",
                "profile": PROFILE, "source": source,
            }]})
            self.assertEqual([(row["source"], row["profile"]) for row in c_segments(ROOT, layout)],
                             [(quad_source, PROFILE), (source, PROFILE)])
            segments = yaml.safe_load(layout.read_text())["segments"]
            self.assertEqual(segments[-1], [0x5000])
            self.assertEqual(
                [(row["start"], row["vram"], row["subsegments"][0][:2]) for row in segments[:-1]],
                [(offset, base + offset, [offset, kind]) for offset, kind in (
                    (0, "data"), (4, "asm"), (0xAAC, "c"), (0xF58, "asm"),
                    (0x18B8, "c"), (0x1D04, "data"))])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            self.assertEqual(
                [(int(row["address"], 0), int(row["size"], 0), row["status"]) for row in inventory],
                [(base + start, end - start, "matching_c" if start in (0xAAC, 0x18B8) else "unmatched_asm")
                 for start, end in SPANS])
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x1D04, 0x32FC)):
                self.assertIn(f"D_{base + offset:X} = 0x{base + offset:X}; "
                              f"// type:u8 size:0x{size:X} defined:true", symbols)

    def test_shared_bindings_do_not_define_family_membership(self):
        from tools.project.tests import test_french_model_variant402 as family402

        case = family402.FrenchModelVariant402Tests("test_loader_slices_and_independent_hashes")
        case.setUp()
        self.assertEqual({row["name"] for row in case.modules}, {
            f"french_model_variant_{model}_stage{7 + slot}_slot{slot}"
            for model in (6, 551) for slot in (0, 1)
        })
        self.assertTrue(set(case.instances).isdisjoint(self.instances))
        for module in case.modules + [self.modules[name] for name in self.instances]:
            self.assertEqual(module["linker_symbols"],
                             "config/sles_03948/overlays/model_variant402_linker_symbols.txt")

    def test_terminal_fingerprints_and_symbol_only_wrapper(self):
        with (ROOT / "notes/overlays/french-model-variant410-attempts.csv").open() as handle:
            rows = [row for row in csv.DictReader(handle) if row["function_offset"] == "0x18B8"]
        self.assertGreaterEqual(len(rows), 4)
        self.assertGreaterEqual(sum(row["result"] == "text_exact" for row in rows), 2)
        terminals = {int(row["slot"]): row for row in rows if row["result"] == "matched"}
        self.assertEqual(set(terminals), {0, 1})
        for slot in (0, 1):
            source = SOURCE.with_name("variant410_rings" + ("_slot1" if slot else "") + ".c")
            row = terminals[slot]
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["function_offset"], row["profile"], row["instruction_bytes"],
                              row["different_words"]), ("0x18B8", PROFILE, "1100", "0"))
        self.assertEqual(SOURCE.with_name("variant410_rings_slot1.c").read_text(),
                         '#include "../../types.h"\n\n#define func_8013C8B8 func_8017C8B8\n'
                         '#include "variant410_rings.c"\n')
        self.assertNotIn("extern ", SOURCE.read_text())

    def test_paired_origins_colors_and_sequential_phase_checks(self):
        text = SOURCE.read_text()
        for expression in (
            "i < 2", "j < 4", "ring->scale / 8",
            "matrix.t[0] = work->origin[0]", "matrix.t[0] = work->target.vx",
            "ring->points[0][j]", "ring->points[3][j]",
            "setRGB0(quad, ring->outer.r, ring->outer.g, ring->outer.b)",
            "setRGB3(quad, ring->inner.r, ring->inner.g, ring->inner.b)",
            "depth > 0", "depth < 2048",
            "(work->elapsed << 12) / work->config->duration",
            "ring->scale -= work->step * 32", "ring->scale += work->step * 512",
            "ring->scale -= work->step * 96", "ring->scale = 16384", "work->phase = 4",
        ):
            self.assertIn(expression, text)
        self.assertEqual(text.count("RotMatrix(&rotation,"), 2)
        self.assertEqual(text.count("if (work->phase == 3 && ring->scale > 0)"), 2)
        self.assertNotIn("else if", text)
        self.assertNotIn("flag >=", text)
        self.assertIn("Family410Config *G32 config;", SOURCE.with_suffix(".h").read_text())

    def test_target_compiled_measured_layout(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("target layout requires pyelftools")
        from elftools.elf.elffile import ELFFile
        from tools.project.build_baseline import compile_c, load_compiler_profiles, tool

        profiles = load_compiler_profiles(ROOT)
        for path in (profiles[PROFILE]["compiler"], "tools/vendor/maspsx/maspsx.py"):
            if not (ROOT / path).is_file():
                self.skipTest(f"target layout requires {path}")
        checks = {
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(GsCOORDINATE2)": 80, "sizeof(PSXLONG)": 4, "sizeof(POLY_GT4)": 52,
            "sizeof(CVECTOR)": 4, "sizeof(Family410Ring)": 152,
            "sizeof(Family410Config)": 16, "sizeof(Family410RingView)": 0x21F4,
        }
        for typename, fields in (
            ("Family410Ring", (("points[1]", 32), ("points[2]", 64), ("points[3]", 96),
                               ("inner", 128), ("outer", 132), ("scale", 136))),
            ("Family410Config", (("duration", 12),)),
            ("Family410RingView", (("rings", 0x1EF8), ("quad", 0x20C4), ("origin", 0x2184),
                                   ("target", 0x2190), ("frame", 0x21C4), ("elapsed", 0x21C8),
                                   ("step", 0x21D0), ("config", 0x21D8), ("phase", 0x21F0))),
            ("POLY_GT4", (("x0", 8), ("x1", 20), ("x2", 32), ("x3", 44),
                          ("r0", 4), ("r1", 16), ("r2", 28), ("r3", 40))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 34)
        with tempfile.TemporaryDirectory(dir=ROOT / "tmp", prefix="french410-layout-") as name:
            directory = Path(name).relative_to(ROOT)
            source = directory / "layout.c"
            (ROOT / source).write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/overlays/french_model_variant/variant410_rings.h"\n'
                'const u32 layouts[] = {' + ", ".join(checks) + "};\n")
            obj = compile_c(ROOT, tool(ROOT, "as"),
                            {"source": str(source), "object": "layout.o", "profile": PROFILE},
                            profiles, object_directory=str(directory), asm_directory=str(directory / "asm"))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layouts")
                self.assertIsInstance(symbol["st_shndx"], int)
                self.assertEqual(struct.unpack("<34I", elf.get_section(symbol["st_shndx"]).data()),
                                 tuple(checks.values()))

    def test_retail_closed_spans_and_gt4_initializer(self):
        from tools.project.overlay_function_inventory import walk_function

        path = ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for stage, slot, sector, digest in IMAGES:
                base = 0x8013B000 + slot * 0x40000
                archive.seek(sector * 2048)
                payload = archive.read(20480)
                self.assertEqual(hashlib.sha256(payload).hexdigest(), digest)
                self.assertEqual(struct.unpack_from("<I", payload)[0], 410 + slot * 150)
                for start, end in SPANS:
                    result = walk_function(payload, base, start, end - start)
                    self.assertTrue(result["closed"])
                    self.assertEqual(result["visited"], set(range(start, end, 4)))
                    self.assertEqual(result["returns"], 1)
                    self.assertNotIn(base + 0x18B8, result["calls"])
                    if start == 4:
                        self.assertIn(base + 0xAAC, result["calls"])
                    if start == 0xAAC:
                        self.assertFalse(result["calls"])
                        self.assertEqual(result["external"], {
                            0x8005C018, 0x800842A8, 0x80085558, 0x80086258, 0x800872A8,
                            0x800875F8, 0x80087738, 0x80087958, 0x80087CB8,
                            0x80089928, 0x800866F8, 0x80086628,
                        })
                    if start == 0x18B8:
                        self.assertEqual(result["external"], {
                            0x8005C018, 0x800842A8, 0x80085558, 0x80086258, 0x800872A8,
                            0x800875F8, 0x80087738, 0x80087958, 0x80087CB8,
                        })
                self.assertEqual(struct.unpack_from("<I", payload, 0x18B8)[0], 0x27BDFEF0)
                self.assertEqual(struct.unpack_from("<I", payload, 0x1D00)[0], 0x27BD0110)
                self.assertEqual(struct.unpack_from("<I", payload, 0xAAC)[0], 0x27BDFEA8)
                self.assertEqual(struct.unpack_from("<I", payload, 0xF54)[0], 0x27BD0158)
                for offset, word in (
                    (0x48, 0x26D32028), (0x360, 0x02602021), (0x364, 0x0C020BAA),
                    (0x20, 0x26D41EF8), (0x28, 0x26D52090), (0x238, 0x26B50034),
                    (0x23C, 0x0C020BBA), (0x240, 0x02A02021),
                    (0x5F0, 0x26840090), (0x718, 0x240200FF), (0x720, 0x24030040),
                    (0x724, 0xA082FFF0), (0x728, 0xA082FFF1), (0x72C, 0xA082FFF2),
                    (0x730, 0xA080FFF4), (0x734, 0xA083FFF5), (0x738, 0xA082FFF6),
                    (0x748, 0x24840098), (0x754, 0x26940098),
                ):
                    self.assertEqual(struct.unpack_from("<I", payload, offset)[0], word)

    def test_quad_attempt_fingerprints_and_symbol_only_wrapper(self):
        with (ROOT / "notes/overlays/french-model-variant410-attempts.csv").open() as handle:
            rows = [row for row in csv.DictReader(handle) if row["function_offset"] == "0xAAC"]
        self.assertEqual([row["result"] for row in rows],
                         ["mismatch"] * 4 + ["text_exact"] * 2 + ["matched"] * 2)
        self.assertEqual([int(row["different_words"]) for row in rows], [86, 86, 85, 85, 0, 0, 0, 0])
        self.assertTrue(all(row["instruction_bytes"] == "1196" for row in rows))
        for slot, row in enumerate(rows[-2:]):
            source = SOURCE.with_name("variant410_quads" + ("_slot1" if slot else "") + ".c")
            self.assertEqual((row["slot"], row["profile"]), (str(slot), PROFILE))
            self.assertEqual(hashlib.sha256(source.read_bytes()).hexdigest(), row["fingerprint"])
        self.assertEqual(SOURCE.with_name("variant410_quads_slot1.c").read_text(),
                         '#include "../../types.h"\n\n#define func_8013BAAC func_8017BAAC\n'
                         '#include "variant410_quads.c"\n')
        self.assertNotRegex(SOURCE.with_name("variant410_quads.c").read_text(),
                            r"\b(?:extern|asm|__asm__|register|volatile)\b")

    def test_quad_projection_fade_and_phase_rules(self):
        text = SOURCE.with_name("variant410_quads.c").read_text()
        for expression in (
            "u8 unknown_stack[16];", "s16 i, j;", "s16 angle, angle2;",
            "turn = ratan2(work->direction[1], work->direction[2]);",
            "tilt = ratan2(work->screen_delta[1], work->screen_delta[0]);",
            "i < 1", "j < 10", "angle += 1300", "angle2 += 1700",
            "record->scale[j] >= 0", "record->scale[j] > 3072",
            "record->color.r * (4096 - record->scale[j]) / 1024",
            "rcos(angle + angle2 + record->angle[j]) * 128",
            "rsin(angle + record->angle[j]) * 128",
            "rsin(angle2 + record->angle[j]) * 128",
            "RotTransPers4(&record->points[0][j], &record->points[1][j]",
            "depth >= 0 && flag >= 0 && record->scale[j] < 4096",
            "work->phase < 3", "record->scale[j] += work->step << 8;",
            "record->scale[j] = 0;", "work->phase == 1", "work->phase = 2;",
            "record->scale[j] = 4096;", "record->done[j] = 1;",
            "done += record->done[j];",
            "j + 1 == 10 && done >= 10 && work->phase == 3", "work->phase = 4;",
        ):
            self.assertIn(expression, text)
        self.assertEqual(text.count("record->scale[j] += work->step << 8;"), 2)
        self.assertLess(text.index("ot = func_80058F10();"), text.index("done = 0;"))
        self.assertLess(text.index("ReadRotMatrix("), text.index("ScaleMatrix("))

    def test_target_compiled_quad_layout(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("target layout requires pyelftools")
        from elftools.elf.elffile import ELFFile
        from tools.project.build_baseline import compile_c, load_compiler_profiles, tool

        profiles = load_compiler_profiles(ROOT)
        for path in (profiles[PROFILE]["compiler"], "tools/vendor/maspsx/maspsx.py"):
            if not (ROOT / path).is_file():
                self.skipTest(f"target layout requires {path}")
        checks = {
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(GsCOORDINATE2)": 80, "sizeof(PSXLONG)": 4, "sizeof(POLY_FT4)": 40,
            "sizeof(CVECTOR)": 4, "sizeof(Family410QuadRecord)": 0x2FC,
            "sizeof(Family410QuadView)": 0x21F4,
        }
        for typename, fields in (
            ("Family410QuadRecord", (("points[1]", 0x50), ("points[2]", 0xA0),
                                     ("points[3]", 0xF0), ("color", 0x190), ("scale", 0x194),
                                     ("angle", 0x1BC), ("done", 0x1E4))),
            ("Family410QuadView", (("record", 0), ("quad", 0x2028), ("target", 0x2190),
                                   ("direction", 0x2198), ("screen_delta", 0x21A8),
                                   ("step", 0x21D0), ("phase", 0x21F0))),
            ("POLY_FT4", (("x0", 8), ("x1", 16), ("x2", 24), ("x3", 32), ("r0", 4))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 28)
        with tempfile.TemporaryDirectory(dir=ROOT / "tmp", prefix="french410-quad-layout-") as name:
            directory = Path(name).relative_to(ROOT)
            source = directory / "layout.c"
            (ROOT / source).write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/overlays/french_model_variant/variant410_quads.h"\n'
                'const u32 layouts[] = {' + ", ".join(checks) + "};\n")
            obj = compile_c(ROOT, tool(ROOT, "as"),
                            {"source": str(source), "object": "layout.o", "profile": PROFILE},
                            profiles, object_directory=str(directory), asm_directory=str(directory / "asm"))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layouts")
                self.assertEqual(symbol["st_value"], 0)
                self.assertEqual(struct.unpack("<28I", elf.get_section(symbol["st_shndx"]).data()),
                                 tuple(checks.values()))
