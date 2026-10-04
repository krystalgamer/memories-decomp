import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import struct
import tempfile

from tools.project.tests import test_spanish_model_variant475 as spanish475
from tools.project.overlay_function_inventory import instruction, walk_function
from tools.project.progress import load_french_overlay_inventories

ROOT = spanish475.family435.ROOT


class FrenchModelVariant475Tests(spanish475.SpanishModelVariant475Tests):
    region = "france"
    module_prefix = "french"
    config_name = "sles_03948"
    resident_name = "SLES_039.48"
    load_inventories = staticmethod(load_french_overlay_inventories)
    binding_count = 34
    helpers = ((4, 3732, "entry", "func_8013B004"),
               (0xE98, 2784, "ribbons", "func_8013BE98"),
               (0x1978, 1212, "sheets", "func_8013C968"),
               (0x1E34, 940, "quads", "func_8013CE20"),
               (0x21E0, 832, "strand", "func_8013D1D0"),
               (0x2520, 2072, "streamers", "func_8013D520"))
    reachable_helpers = {4, 0xE98, 0x1978, 0x1E34}
    entry_anchors = {
        **spanish475.SpanishModelVariant475Tests.entry_anchors,
        0xA4: 0x001910C0, 0xA8: 0x00591023, 0xAC: 0x000210C0,
        0xB4: 0xAEE22FCC, 0xCD8: 0x02602021, 0xD14: 0x02602021, 0xD34: 0x02602021,
        0x2520: 0x27BDFEB0, 0x25F0: 0x251425DC, 0x27AC: 0x26940334,
        0x2854: 0x27AA00D0, 0x285C: 0x27B600D4, 0x2888: 0x253526A0,
        0x2974: 0x0C021E56, 0x2990: 0xAE6202AC, 0x29E8: 0xAE6201DC,
        0x2AEC: 0xA60202F0, 0x2B10: 0xA6020312, 0x2B30: 0x26B50334,
        0x2C5C: 0x18400007, 0x2C60: 0x28420800, 0x2CE4: 0x8D022FC4,
        0x2D00: 0xAD003008, 0x2D04: 0xAD023020,
    }

    def setUp(self):
        super().setUp()
        # Header 400 reuses the SDK bindings, but not header 475's image layout.
        self.modules = [module for module in self.modules if module["name"] in self.instances]

    def test_wrappers_only_rename_verified_functions(self):
        with (ROOT / "notes/overlays/french-model-variant475-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 60)
        self.assertEqual({result: sum(row["result"] == result for row in rows)
                          for result in ("text_exact", "mismatch", "matched")},
                         {"text_exact": 12, "mismatch": 36, "matched": 12})
        for row in rows:
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
            if row["result"] == "mismatch":
                if int(row["function_offset"], 0) == 4:
                    self.assertIn((row["instruction_bytes"], row["different_words"]),
                                  {("3728", "798"), ("3732", "744"),
                                   ("3712", "365"), ("3720", "297")})
                elif int(row["function_offset"], 0) == 0x2520:
                    self.assertIn((row["instruction_bytes"], row["different_words"]), {
                        ("2004", "514"), ("2028", "503"), ("2020", "512"),
                        ("2072", "124"), ("2080", "230"), ("2076", "239"),
                        ("2072", "118"), ("2068", "187"), ("2068", "189"),
                        ("2080", "232"), ("2072", "39"),
                    })
                else:
                    self.assertEqual(int(row["function_offset"], 0), 0xE98)
                    self.assertEqual((row["instruction_bytes"], row["different_words"]), ("2768", "498"))
        terminals = [row for row in rows if row["result"] == "matched"]
        self.assertEqual({(int(row["function_offset"], 0), int(row["slot"])) for row in terminals},
                         {(offset, slot) for offset, *_ in self.helpers for slot in (0, 1)})
        for row in terminals:
            offset, slot = int(row["function_offset"], 0), int(row["slot"])
            _, size, role, original = next(helper for helper in self.helpers if helper[0] == offset)
            directory = "french_model_variant" if role in ("entry", "streamers") else "spanish_model_variant"
            source = ROOT / ("src/overlays/" + directory + "/variant475_" + role +
                             ("_slot1" if slot else "") + ".c")
            if role == "entry":
                if slot == 0:
                    self.assertIn('#include "variant475_entry.h"', source.read_text())
                    self.assertIn("s32 func_8013B004(u8 *context, s32 command)", source.read_text())
                else:
                    self.assertEqual(source.read_text(), '#include "../../types.h"\n'
                                     '#define func_8013B004 func_8017B004\n'
                                     '#define func_8013BE98 func_8017BE98\n'
                                     '#define func_8013C978 func_8017C978\n'
                                     '#define func_8013CE34 func_8017CE34\n'
                                     '#define D_8013DD38 D_8017DD38\n'
                                     '#include "variant475_entry.c"\n')
            elif role == "streamers":
                if slot == 0:
                    body = source.read_text()
                    self.assertIn('#include "../model_variant/variant458_streamers.h"', body)
                    self.assertIn("void func_8013D520(u8 *ctx)", body)
                    self.assertIn("previous = &streamer->sa[15]", body)
                    self.assertIn("sizeof(Variant458Streamer)", body)
                    self.assertIn("twist = base * 2;\n        for (k = 0, wave = 0;", body)
                    endpoint = body.split("if (k == 16) {", 1)[1].split("} else {", 1)[0]
                    self.assertIn("&streamer->a[15]", endpoint)
                    self.assertIn("&streamer->a[16]", endpoint)
                    self.assertIn("&streamer->b[16]", endpoint)
                    self.assertIn("streamer->otz[k]", endpoint)
                    self.assertEqual(body.count("streamer->ox[k] ="), 2)
                    self.assertEqual(body.count("streamer->oy[k] ="), 2)
                    self.assertIn("if (streamer->otz[k] > 0)", body)
                    self.assertIn("if (streamer->otz[k] < 0x800)", body)
                    for forbidden in ("if (work)", "if (poly)", "register ", "asm(", "extern "):
                        self.assertNotIn(forbidden, body)
                else:
                    self.assertEqual(source.read_text(), '#include "../../types.h"\n'
                                     '#define func_8013D520 func_8017D520\n'
                                     '#include "variant475_streamers.c"\n')
            elif role == "ribbons":
                if slot == 0:
                    self.assertIn('#include "../model_variant/variant458_ribbons.h"', source.read_text())
                    endpoint = source.read_text().split("if (k == 12) {", 1)[1].split("} else {", 1)[0]
                    self.assertIn("ribbon->otz[k]", endpoint)
                    self.assertIn("&ribbon->a[k - 1]", endpoint)
                    self.assertNotIn("[12]", endpoint)
                    self.assertNotIn("[11]", endpoint)
                else:
                    self.assertEqual(source.read_text(), '#include "../../types.h"\n'
                                     '#define func_8013BE98 func_8017BE98\n'
                                     '#include "variant475_ribbons.c"\n')
            else:
                self.assertEqual(source.read_text(), '#include "../../types.h"\n'
                                 f"#define {original} func_{0x8013B000 + slot * 0x40000 + offset:X}\n"
                                 f'#include "../model_variant/variant458_{role}.c"\n')
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["instruction_bytes"], row["different_words"]), (str(size), "0"))

    def test_strict_french_cfg_original_context_and_spanish_image_identity(self):
        path = ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        spanish = {module["name"]: module for module in json.loads(
            (ROOT / "config/sles_03951/overlays.json").read_text())["modules"]}
        with path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                payload = archive.read(20480)
                self.assertEqual(hashlib.sha256(payload).hexdigest(), module["sha256"])
                self.assertEqual(spanish[module["name"].replace("french_", "spanish_", 1)]["sha256"],
                                 module["sha256"])
                context_register = instruction(struct.unpack_from("<I", payload, 0xC)[0],
                                               base + 0xC).getDestinationGpr()
                for offset in range(0x968, 0xD38, 4):
                    destination = instruction(struct.unpack_from("<I", payload, offset)[0],
                                              base + offset).getDestinationGpr()
                    if destination is not None:
                        self.assertNotEqual(destination, context_register)
                address = base + 0x2E34
                self.assertEqual(struct.unpack_from("<I", payload, 0x94)[0],
                                 0x3C030000 | ((address + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", payload, 0xA0)[0], 0x24630000 | (address & 0xFFFF))
                for pc, destination in ((0xCD4, 0xE98), (0xD10, 0x1E34), (0xD30, 0x1978)):
                    self.assertEqual(struct.unpack_from("<I", payload, pc)[0],
                                     0x0C000000 | ((base + destination) >> 2 & 0x3FFFFFF))
                bindings = {int(address, 16) for address in re.findall(
                    r"= (0x[0-9A-Fa-f]+);", (ROOT / module["linker_symbols"]).read_text())}
                for start, end in self.spans:
                    result = walk_function(payload, base, start, end - start)
                    self.assertTrue(result["closed"])
                    self.assertEqual(result["visited"], set(range(start, end, 4)))
                    self.assertEqual(result["returns"], 1)
                    self.assertEqual(result["problems"], set())
                    self.assertEqual(result["indirect_calls"], 0)
                    self.assertLessEqual(result["external"], bindings)
                self.assertNotIn(0x21E0, self.reachable_helpers)
                self.assertNotIn(0x2520, self.reachable_helpers)

    def test_target_compiled_accessed_views(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("pyelftools required for target layouts")
        from elftools.elf.elffile import ELFFile
        from build_baseline import TOOLCHAIN, compile_c, load_compiler_profiles, tool

        profiles = load_compiler_profiles(ROOT)
        for path in (profiles["gcc_2_8_1_g0_split"]["compiler"],
                     f"{TOOLCHAIN}/mipsel-none-elf-as", "tools/vendor/maspsx/maspsx.py"):
            if not (ROOT / path).is_file():
                self.skipTest(f"target layout requires {path}")
        layouts = {
            "sizeof(ModelVariantSheet)": 0x98, "sizeof(ModelVariantSheetSet)": 0x9C,
            "sizeof(ModelVariantStrandWide)": 0x84, "sizeof(Variant458Quads)": 0x1E4,
            "sizeof(SVECTOR)": 8, "sizeof(CVECTOR)": 4, "sizeof(MATRIX)": 32,
            "sizeof(GsCOORDINATE2)": 80, "sizeof(PSXLONG)": 4, "sizeof(s16)": 2,
            "sizeof(POLY_GT4)": 52, "sizeof(POLY_FT4)": 40, "sizeof(GsLINE)": 16,
            "sizeof(Variant458Ribbon)": 0x2E4,
            "sizeof(Variant458Streamer)": 0x334,
            "sizeof(((Variant458Streamer *)0)->a) / sizeof(SVECTOR)": 17,
            "sizeof(((Variant458Streamer *)0)->ox) / sizeof(s16)": 17,
            "sizeof(POLY_G4)": 36,
            "sizeof(Variant475EntryConfig)": 56,
            "sizeof(Variant475EntryQuads)": 0x1E4,
            "sizeof(Variant475EntryRibbon)": 0x2E4,
            "sizeof(Variant475EntryState)": 0x3034,
            "sizeof(Variant475EntryProjection)": 16,
            "(PSXLONG)-1 < 0": 1,
        }
        for name, fields in (
            ("ModelVariantSheetSet", {"v0": 0, "v1": 0x20, "v2": 0x40, "v3": 0x60,
                                      "outer": 0x80, "inner": 0x84, "size": 0x88, "shown": 0x98}),
            ("ModelVariantStrandWide", {"point": 0, "point[12]": 0x60, "pad68": 0x68}),
            ("Variant458Quads", {"a": 0, "b": 0x40, "c": 0x80, "d": 0xC0,
                                 "color": 0x140, "level": 0x154, "hidden": 0x174}),
            ("GsLINE", {"attribute": 0, "x0": 4, "x1": 8, "r": 12, "g": 13, "b": 14}),
            ("Variant458Ribbon", {"a": 0, "a[12]": 0x60, "sa": 0x68, "angle": 0x9C,
                                  "b": 0xD0, "b[12]": 0x130, "sb": 0x138, "width": 0x16C,
                                  "color": 0x1A0, "count": 0x1C0, "state": 0x1C8, "len": 0x1CC,
                                  "otz": 0x248, "flag": 0x27C, "ox": 0x2B0, "oy": 0x2CA}),
            ("Variant458Streamer", {"a": 0, "sa": 0x88, "angle": 0xCC,
                                    "b": 0x110, "sb": 0x198, "width": 0x1DC,
                                    "color": 0x264, "otz": 0x2AC, "ox": 0x2F0, "oy": 0x312}),
            ("Variant475EntryConfig", {"parts": 4, "start": 0x20, "orbit_start": 0x28,
                                      "ribbon_start": 0x2C, "phase4": 0x30, "phase5": 0x34}),
            ("Variant475EntryQuads", {"a": 0, "b": 0x40, "c": 0x80, "d": 0xC0,
                                     "color": 0x140, "level": 0x154, "hidden": 0x174,
                                     "field_194": 0x194, "position": 0x1B4}),
            ("Variant475EntryRibbon", {"a": 0, "sa": 0x68, "angle": 0x9C, "b": 0xD0,
                                      "sb": 0x138, "width": 0x16C, "inner": 0x1A0,
                                      "outer": 0x1A4, "scale": 0x1A8, "field_1B8": 0x1B8,
                                      "field_1BC": 0x1BC, "count": 0x1C0, "field_1C4": 0x1C4,
                                      "state": 0x1C8, "len": 0x1CC, "velocity": 0x238,
                                      "otz": 0x248, "flag": 0x27C, "ox": 0x2B0, "oy": 0x2CA}),
            ("Variant475EntryState", {"quads": 0, "ribbons": 0x1E4, "sheets": 0x1904,
                                     "streamers": 0x25DC, "triangle": 0x2C44, "quad": 0x2C60,
                                     "textured": 0x2C84, "extra": 0x2CEC, "flat": 0x2D20,
                                     "extra_flat": 0x2D98, "matrix": 0x2DE4, "target": 0x2E04,
                                     "starts": 0x2E0C, "ends": 0x2E8C, "deltas": 0x2F0C,
                                     "direction": 0x2F8C, "screen_delta": 0x2F9C,
                                     "view_delta": 0x2FA0, "angles": 0x2FB0,
                                     "frame_count": 0x2FB8, "frame": 0x2FBC,
                                     "animation_frame": 0x2FC0, "step": 0x2FC4, "fade": 0x2FC8,
                                     "config": 0x2FCC, "parts": 0x2FD4, "field_2FF4": 0x2FF4,
                                     "field_2FF6": 0x2FF6, "field_2FF8": 0x2FF8,
                                     "field_2FFA": 0x2FFA, "field_2FFC": 0x2FFC,
                                     "orbit": 0x3000, "orbit_progress": 0x3004,
                                     "field_3008": 0x3008, "field_300C": 0x300C,
                                     "field_3010": 0x3010, "field_3014": 0x3014,
                                     "field_3018": 0x3018, "field_301C": 0x301C,
                                     "phase": 0x3020, "tint": 0x302C, "slot": 0x3030,
                                     "command": 0x3032}),
            ("Variant475EntryProjection", {"origin": 0, "p": 4, "flag": 8, "target": 12}),
        ):
            for field, offset in fields.items():
                layouts[f"(u32)&(({name} *)0)->{field}"] = offset
        with tempfile.TemporaryDirectory(dir=ROOT / "tmp", prefix="french475-layout-") as name:
            directory = Path(name).relative_to(ROOT)
            source = directory / "layout.c"
            (ROOT / source).write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/overlays/model_variant/variant458_quads.h"\n'
                '#include "../../src/overlays/model_variant/variant458_ribbons.h"\n'
                '#include "../../src/overlays/model_variant/variant458_streamers.h"\n'
                '#include "../../src/overlays/french_model_variant/variant475_entry.h"\n'
                "const u32 layouts[] = {" + ", ".join(layouts) + "};\n")
            obj = compile_c(ROOT, tool(ROOT, "as"), {
                "kind": "text", "source": str(source), "object": "layout.o", "profile": "gcc_2_8_1_g0_split"},
                profiles, object_directory=str(directory), asm_directory=str(directory / "asm"))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layouts")
                self.assertIsInstance(symbol["st_shndx"], int)
                self.assertEqual(symbol["st_value"], 0)
                data = elf.get_section(symbol["st_shndx"]).data()
                self.assertEqual(struct.unpack(f"<{len(layouts)}I", data), tuple(layouts.values()))
