import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import struct
import tempfile
import unittest

import yaml

ROOT = Path(__file__).resolve().parents[3]
CONFIG = ROOT / "config/sles_03948"
SOURCE = ROOT / "src/overlays/french_model_variant/variant373_points.c"
STRIP = SOURCE.with_name("variant373_strip.c")
RIBBONS = SOURCE.with_name("variant373_ribbons.c")
PROFILE = "gcc_2_8_1_g0_split"
IMAGES = (
    (7, 0, 167712, "015d6cc54e2cbdf108bc5ef6592eb67d65abdf34a30a25b8d5fc54432059e0d5"),
    (8, 1, 167722, "bcc28f9293acd66f0ea42c1f3e9ad51fd0591fa24eb03ebb44305cf7fdb8b055"),
)
SPANS = ((4, 0x10B0), (0x10B0, 0x176C), (0x176C, 0x1EE4), (0x1EE4, 0x223C),
         (0x223C, 0x2784), (0x2784, 0x2E68), (0x2E68, 0x33EC), (0x33EC, 0x3EF0))


class FrenchModelVariant373Tests(unittest.TestCase):
    def setUp(self):
        self.modules = {row["name"]: row for row in json.loads(
            (CONFIG / "overlays.json").read_text())["modules"]}
        with (ROOT / "notes/overlays/french-model-variant373-instances.csv").open() as handle:
            self.instances = {row["module"]: row for row in csv.DictReader(handle)}

    def test_two_distinct_physical_registrations(self):
        self.assertEqual(set(self.instances), {
            f"french_model_variant_707_stage{stage}_slot{slot}" for stage, slot, *_ in IMAGES
        })
        for stage, slot, sector, digest in IMAGES:
            name = f"french_model_variant_707_stage{stage}_slot{slot}"
            module, instance = self.modules[name], self.instances[name]
            self.assertEqual(module["archive"], "game/france/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"],
                             "0c3f90cf4a3b4776d1188054a6cd5d89c5bc364215dac15c4730ef74edbc8da3")
            self.assertEqual((module["sector_offset"], module["sector_count"]), (sector, 10))
            self.assertEqual(module["load_address"], f"0x{0x8013B000 + slot * 0x40000:X}")
            self.assertEqual(module["sha256"], digest)
            self.assertEqual(module["linker_symbols"],
                             "config/sles_03948/overlays/model_variant373_linker_symbols.txt")
            self.assertEqual(module["output"], f"tmp/overlays/{name}/module.bin")
            self.assertEqual((int(instance["model"]), int(instance["stage"]), int(instance["slot"]),
                              int(instance["header"]), int(instance["sector"]), instance["sha256"]),
                             (707, stage, slot, 373 + slot * 150, sector, digest))

    def test_c_ownership_and_exact_raw_assembly_spans(self):
        from tools.project.overlay_sources import c_segments

        for stage, slot, *_ in IMAGES:
            name = f"french_model_variant_707_stage{stage}_slot{slot}"
            module = self.modules[name]
            base, layout = int(module["load_address"], 0), ROOT / module["layout"]
            source = "src/overlays/french_model_variant/variant373_points" + (
                "_slot1" if slot else "") + ".c"
            strip_source = source.replace("points", "strip")
            ribbon_source = source.replace("points", "ribbons")
            matching = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(matching, {"schema": 1, "functions": [{
                "address": f"0x{base + 0x1EE4:X}", "size": "0x358",
                "profile": PROFILE, "source": source,
            }, {
                "address": f"0x{base + 0x223C:X}", "size": "0x548",
                "profile": PROFILE, "source": strip_source,
            }, {
                "address": f"0x{base + 0x2784:X}", "size": "0x6E4",
                "profile": PROFILE, "source": ribbon_source,
            }]})
            self.assertEqual([(s["source"], s["profile"]) for s in c_segments(ROOT, layout)],
                             [(source, PROFILE), (strip_source, PROFILE), (ribbon_source, PROFILE)])
            segments = yaml.safe_load(layout.read_text())["segments"]
            self.assertEqual(segments[-1], [0x5000])
            expected = [(0, "data")] + [(start, "c" if start in (0x1EE4, 0x223C, 0x2784) else "asm")
                                        for start, _ in SPANS] + [(0x3EF0, "data")]
            self.assertEqual([(s["start"], s["vram"], s["subsegments"][0][:2]) for s in segments[:-1]],
                             [(offset, base + offset, [offset, kind]) for offset, kind in expected])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            self.assertEqual(
                [(int(r["address"], 0), int(r["size"], 0), r["status"]) for r in inventory],
                [(base + start, end - start,
                  "matching_c" if start in (0x1EE4, 0x223C, 0x2784) else "unmatched_asm")
                 for start, end in SPANS])
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x3EF0, 0x1110)):
                self.assertIn(f"D_{base + offset:X} = 0x{base + offset:X}; "
                              f"// type:u8 size:0x{size:X} defined:true", symbols)

    def test_terminal_fingerprints_and_symbol_only_wrapper(self):
        with (ROOT / "notes/overlays/french-model-variant373-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 14)
        self.assertEqual({row["function_offset"] for row in rows}, {"0x1EE4", "0x223C", "0x2784"})
        ribbon_rows = [row for row in rows if row["function_offset"] == "0x2784"]
        strip_rows = [row for row in rows if row["function_offset"] == "0x223C"]
        rows = [row for row in rows if row["function_offset"] == "0x1EE4"]
        self.assertEqual([row["result"] for row in rows], ["text_exact"] * 2 + ["matched"] * 2)
        for slot, row in enumerate(rows[-2:]):
            source = SOURCE.with_name("variant373_points" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(hashlib.sha256(source.read_bytes()).hexdigest(), row["fingerprint"])
            self.assertEqual((row["function_offset"], row["slot"], row["profile"],
                              row["instruction_bytes"], row["different_words"]),
                             ("0x1EE4", str(slot), PROFILE, "856", "0"))
        self.assertEqual(SOURCE.with_name("variant373_points_slot1.c").read_text(),
                         '#include "../../types.h"\n\n#define func_8013CEE4 func_8017CEE4\n'
                         '#include "variant373_points.c"\n')
        self.assertNotRegex(SOURCE.read_text(), r"\b(?:extern|asm|__asm__|register|volatile)\b")
        self.assertEqual([row["result"] for row in strip_rows],
                         ["mismatch"] * 2 + ["text_exact"] * 2 + ["matched"] * 2)
        self.assertEqual([row["different_words"] for row in strip_rows], ["2"] * 2 + ["0"] * 4)
        for slot, row in enumerate(strip_rows[-2:]):
            source = STRIP.with_name("variant373_strip" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(hashlib.sha256(source.read_bytes()).hexdigest(), row["fingerprint"])
            self.assertEqual((row["slot"], row["profile"], row["instruction_bytes"]),
                             (str(slot), PROFILE, "1352"))
        self.assertEqual(STRIP.with_name("variant373_strip_slot1.c").read_text(),
                         '#include "../../types.h"\n\n#define func_8013D23C func_8017D23C\n'
                         '#include "variant373_strip.c"\n')
        self.assertNotRegex(STRIP.read_text(), r"\b(?:extern|asm|__asm__|register|volatile)\b")
        self.assertEqual([row["result"] for row in ribbon_rows], ["text_exact"] * 2 + ["matched"] * 2)
        self.assertEqual([row["different_words"] for row in ribbon_rows], ["0"] * 4)
        for slot, row in enumerate(ribbon_rows[-2:]):
            source = RIBBONS.with_name("variant373_ribbons" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(hashlib.sha256(source.read_bytes()).hexdigest(), row["fingerprint"])
            self.assertEqual((row["slot"], row["profile"], row["instruction_bytes"]),
                             (str(slot), PROFILE, "1764"))
        self.assertEqual(RIBBONS.with_name("variant373_ribbons_slot1.c").read_text(),
                         '#include "../../types.h"\n\n#define func_8013D784 func_8017D784\n'
                         '#include "variant373_ribbons.c"\n')
        self.assertNotRegex(RIBBONS.read_text(), r"\b(?:extern|asm|__asm__|register|volatile)\b")

    def test_ribbon_projection_colors_scale_and_ungated_angle_update(self):
        text = RIBBONS.read_text()
        for expression in (
            "ratan2(work->view_direction[2], work->view_direction[0]) + 3072",
            "ratan2(work->view_direction[1], work->view_direction[0]);",
            "ratan2(work->delta[2], work->delta[1]) + 1024",
            "ratan2(work->delta[2], work->delta[0]);",
            "work->mode == 1", "turn = -turn;", "work->phase > 0", "length = 16;",
            "i < 8", "j < 2", "j < 1", "angle = work->angle + i * 512",
            "radius = 128;", "radius = 40;", "j * ((flags & 1) * 32 + 192)",
            "work->delta[0] * work->factor / 1024", "work->scale < 4096",
            "scale.vx = work->scale;", "scale.vx = 4096;",
            "ribbon->depth[0] = RotTransPers4(", "ribbon->depth[j] = RotTransPers4(",
            "RotTransPers(&ribbon->edges[j], &ribbon->edge_projected[j], &interpolation, &flag)",
            "ribbon->angle[j] = ratan2(dy, dx) + 3072",
            "ribbon->width[j] = (s16)ribbon->edge_projected[j] - (s16)ribbon->projected[j]",
            "ribbon->x_offset[j] = rcos(ribbon->angle[j]) * ribbon->width[j] >> 12",
            "ribbon->y_offset[j] = rsin(ribbon->angle[j]) * ribbon->width[j] >> 12",
            "if (ribbon->depth[j] > 0) {\n                    if (ribbon->depth[j] < 2048)",
            "func_8005B260((u32 *)triangle, ot, (u16)ribbon->depth[j], 1)",
            "    }\n    if (work->index_242C + 1 == work->config->count_0C)",
            "work->angle += work->step * 50;",
        ):
            self.assertIn(expression, text)
        for channel in "rgb":
            self.assertIn(f"triangle->{channel}0 = colors->outer;", text)
            self.assertIn(f"triangle->{channel}1 = colors->inner;", text)
            self.assertIn(f"triangle->{channel}2 = colors->outer;", text)
        self.assertEqual(text.count("ratan2("), 5)
        self.assertNotIn("flag >=", text)
        self.assertLess(text.index("ratan2(work->delta[2], work->delta[0])"),
                        text.index("if (work->phase > 0)"))

    def test_strip_projection_signed_visibility_and_phase_rules(self):
        text = STRIP.read_text()
        for expression in (
            "ratan2(work->angle_delta[1], work->angle_delta[0])", "angle += 2048;",
            "work->phase > 0", "width = work->width;", "width += 31;", "size = width >> 5;",
            "i < 1", "j < 2", "j < 1", "rcos(1024) * size >> 12",
            "rsin(3072) * size >> 12", "work->delta[0] * work->factor / 1024",
            "scale.vz = scale.vy = scale.vx = 4096",
            "ReadRotMatrix(&light)", "SetRotMatrix(&light)",
            "strip->depth[j] = RotTransPers3(",
            "if (strip->depth[j] > 0) {\n                    if (strip->depth[j] < 2048)",
            "GsSortPoly(quad, ot, strip->depth[j] & 0xFFFF)",
            "work->index_242C + 1 == work->config->count_0C",
            "work->factor <= 1024", "work->factor += work->step * 32;",
            "work->factor >= 1024", "work->factor = 1024;", "work->phase = 2;",
            "work->phase == 5 && work->width > 0", "work->width -= work->step * 8;",
            "work->width <= 0", "work->width = 0;",
        ):
            self.assertIn(expression, text)
        self.assertEqual(text.count("setXY4(quad,"), 2)
        self.assertEqual(text.count("setRGB0(quad, 128, 0, 128)"), 2)
        self.assertEqual(text.count("setRGB3(quad, 192, 192, 192)"), 2)
        self.assertEqual(text.count("GsSortPoly("), 1)
        self.assertNotIn("flag >=", text)
        self.assertLess(text.index("ratan2("), text.index("if (work->phase > 0)"))
        self.assertLess(text.index("ReadRotMatrix("), text.index("ScaleMatrix("))

    def test_sprite_colors_projection_and_wrap_rules(self):
        text = SOURCE.read_text()
        for expression in (
            "group->scale > 0", "rsin(group->scale);", "group->scale < 5120",
            "ratan2(work->view_direction[2], work->view_direction[0]);",
            "light.r = 192 - fade * 192 / 1024;",
            "dark.r = 192 - fade * 192 / 1024;", "dark.g = 64 - fade / 16;",
            "dark.b = 0;", "u8 unknown_stack[16];", "j < 16", "while (i < 3)",
            "RotTransPers(&group->points[j], &screen, &interpolation, &flag)",
            "setRGB0(quad, light.r, light.g, light.b)",
            "quad->x0 = screen - 8;", "quad->y3 = (screen >> 16) + 8;",
            "depth >= 0 && flag >= 0", "group->scale < 6144",
            "group->scale += work->step * 192;", "group->scale -= 6144;",
            "group->rotation.vx += work->step * 192;",
            "group->rotation.vz += work->step * 400;",
            "work->phase >= 7", "group->scale = 6144;",
        ):
            self.assertIn(expression, text)
        self.assertNotIn("depth <", text)
        self.assertLess(text.index("RotMatrix("), text.index("ScaleMatrix("))
        self.assertLess(text.index("ScaleMatrix("), text.index("GsGetLs("))

    def test_target_compiled_measured_layout(self):
        checks = {
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(GsCOORDINATE2)": 80, "sizeof(PSXLONG)": 4, "sizeof(POLY_FT4)": 40,
            "sizeof(CVECTOR)": 4, "sizeof(Family373PointGroup)": 180,
            "sizeof(Family373PointView)": 0x2488,
        }
        for typename, fields in (
            ("Family373PointGroup", (("rotation", 0xA0), ("scale", 0xA8))),
            ("Family373PointView", (("groups", 0xD48), ("quad", 0x2364), ("target", 0x23D0),
                                    ("view_direction", 0x23EC), ("step", 0x2410), ("phase", 0x2484))),
            ("POLY_FT4", (("x0", 8), ("x1", 16), ("x2", 24), ("x3", 32), ("r0", 4))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 22)
        self.assert_target_layout(checks, "variant373_points.h")

    def test_target_compiled_strip_layout(self):
        checks = {
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(GsCOORDINATE2)": 80, "sizeof(PSXLONG)": 4, "sizeof(POLY_GT4)": 52,
            "sizeof(CVECTOR)": 4, "sizeof(Family373Strip)": 108,
            "sizeof(Family373StripConfig)": 14, "sizeof(Family373StripView)": 0x2488,
        }
        for typename, fields in (
            ("Family373Strip", (("points[1]", 16), ("points[2]", 32),
                                ("projected", 0x30), ("depth", 0x64))),
            ("Family373StripConfig", (("count_0C", 12),)),
            ("Family373StripView", (
                ("strip", 0xF64), ("quad", 0x1DF8), ("origin", 0x23C4),
                ("delta", 0x23D8), ("angle_delta", 0x23E8), ("step", 0x2410),
                ("config", 0x2418), ("index_242C", 0x242C), ("factor", 0x2474),
                ("width", 0x2476), ("phase", 0x2484))),
            ("POLY_GT4", (("x0", 8), ("x1", 20), ("x2", 32), ("x3", 44),
                          ("r0", 4), ("r1", 16), ("r2", 28), ("r3", 40))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 34)
        self.assert_target_layout(checks, "variant373_strip.h")

    def assert_target_layout(self, checks, header):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("target layout requires pyelftools")
        from elftools.elf.elffile import ELFFile
        from tools.project.build_baseline import compile_c, load_compiler_profiles, tool

        profiles = load_compiler_profiles(ROOT)
        for path in (profiles[PROFILE]["compiler"], "tools/vendor/maspsx/maspsx.py"):
            if not (ROOT / path).is_file():
                self.skipTest(f"target layout requires {path}")
        with tempfile.TemporaryDirectory(dir=ROOT / "tmp", prefix="french373-layout-") as name:
            directory = Path(name).relative_to(ROOT)
            source = directory / "layout.c"
            (ROOT / source).write_text(
                '#include "../../src/types.h"\n'
                f'#include "../../src/overlays/french_model_variant/{header}"\n'
                'const u32 layouts[] = {' + ", ".join(checks) + "};\n")
            obj = compile_c(ROOT, tool(ROOT, "as"),
                            {"source": str(source), "object": "layout.o", "profile": PROFILE},
                            profiles, object_directory=str(directory), asm_directory=str(directory / "asm"))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layouts")
                self.assertEqual(symbol["st_value"], 0)
                actual = struct.unpack(f"<{len(checks)}I", elf.get_section(symbol["st_shndx"]).data())
            self.assertEqual(actual, tuple(checks.values()))

    def test_target_compiled_ribbon_layout(self):
        checks = {
            "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
            "sizeof(GsCOORDINATE2)": 80, "sizeof(PSXLONG)": 4, "sizeof(POLY_G3)": 28,
            "sizeof(CVECTOR)": 4, "sizeof(Family373Ribbon)": 108,
            "sizeof(Family373RibbonColors)": 0x88, "sizeof(Family373RibbonConfig)": 14,
            "sizeof(Family373RibbonView)": 0x24A0,
        }
        for typename, fields in (
            ("Family373Ribbon", (
                ("projected", 0x10), ("angle", 0x18), ("edges", 0x20),
                ("edge_projected", 0x30), ("width", 0x38), ("depth", 0x5C),
                ("x_offset", 0x64), ("y_offset", 0x68))),
            ("Family373RibbonColors", (("outer", 0x80), ("inner", 0x84))),
            ("Family373RibbonConfig", (("count_0C", 12),)),
            ("Family373RibbonView", (
                ("colors", 0xFD0), ("ribbons", 0x1058), ("triangle", 0x1DB8),
                ("origin", 0x23C4), ("delta", 0x23D8), ("view_direction", 0x23EC),
                ("flags", 0x2404), ("step", 0x2410), ("config", 0x2418),
                ("index_242C", 0x242C), ("scale", 0x2444), ("factor", 0x2474),
                ("angle", 0x2478), ("phase", 0x2484), ("mode", 0x249C))),
            ("POLY_G3", (("x0", 8), ("x1", 16), ("x2", 24),
                         ("r0", 4), ("r1", 12), ("r2", 20))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 43)
        self.assert_target_layout(checks, "variant373_ribbons.h")

    def test_retail_cfgs_initializers_and_complete_resident_bindings(self):
        from tools.project.overlay_function_inventory import walk_function

        archive_path = ROOT / "game/france/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest("legal French MODEL input required")
        bindings = {name: int(address, 16) for name, address in re.findall(
            r"^(\w+) = (0x[0-9A-F]+);$", (CONFIG / "overlays/model_variant373_linker_symbols.txt").read_text(),
            re.M)}
        self.assertEqual(len(bindings), 34)
        self.assertEqual(len(set(bindings.values())), 34)
        with (CONFIG / "functions.csv").open() as handle:
            resident = {int(row["address"], 0) for row in csv.DictReader(handle)}
        observed = set()
        with archive_path.open("rb") as archive:
            for stage, slot, sector, digest in IMAGES:
                base = 0x8013B000 + slot * 0x40000
                archive.seek(sector * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), digest)
                entries = {base + start for start, _ in SPANS}
                for start, end in SPANS:
                    cfg = walk_function(data, base, start, end - start)
                    self.assertTrue(cfg["closed"])
                    self.assertEqual((cfg["extent"], len(cfg["visited"]) * 4), (end - start, end - start))
                    self.assertFalse(cfg["indirect_calls"])
                    self.assertTrue(cfg["calls"] <= entries)
                    self.assertTrue(cfg["external"] <= resident)
                    observed.update(cfg["external"])
                    self.assertEqual(cfg["calls"], {base + x for x in (0x10B0, 0x176C, 0x1EE4)}
                                     if start == 4 else set())
                for offset, word in {0x1C: 0x26B90D48, 0x7C: 0x26B82364,
                                     0xA8: 0xAFB800AC, 0x528: 0x8FA400AC, 0x52C: 0x0C020BAA,
                                     0x1EE4: 0x27BDFEF0, 0x2238: 0x27BD0110,
                                     0x3C: 0x26B61DF8, 0x224: 0x0C020BBA, 0x228: 0x02C02021,
                                     0x223C: 0x27BDFF00, 0x2780: 0x27BD0100,
                                     0x2658: 0x18400007, 0x265C: 0x28420800,
                                     0x6C: 0x26B11DB8, 0x1F0: 0x02202021,
                                     0x1F8: 0x0C020B92, 0x1FC: 0xA7A2004A,
                                     0x2784: 0x27BDFED8, 0x2E64: 0x27BD0128,
                                     0x2D9C: 0x18400008, 0x2DA0: 0x28420800}.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word)
        self.assertEqual(set(bindings.values()), observed)
        self.assertEqual(bindings["GsGetActiveBuff"], 0x800852A8)
        self.assertEqual(bindings["RotTransPers3"], 0x80087898)
        self.assertEqual(bindings["func_8005B260"], 0x8004D5B8)


if __name__ == "__main__":
    unittest.main()
