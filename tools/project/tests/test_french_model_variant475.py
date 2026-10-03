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
    helpers = ((0xE98, 2784, "ribbons", "func_8013BE98"),
               (0x1978, 1212, "sheets", "func_8013C968"),
               (0x1E34, 940, "quads", "func_8013CE20"),
               (0x21E0, 832, "strand", "func_8013D1D0"))
    reachable_helpers = {0xE98, 0x1978, 0x1E34}
    entry_anchors = {
        **spanish475.SpanishModelVariant475Tests.entry_anchors,
        0xA4: 0x001910C0, 0xA8: 0x00591023, 0xAC: 0x000210C0,
        0xB4: 0xAEE22FCC, 0xCD8: 0x02602021, 0xD14: 0x02602021, 0xD34: 0x02602021,
    }

    def test_wrappers_only_rename_verified_functions(self):
        with (ROOT / "notes/overlays/french-model-variant475-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 20)
        self.assertEqual({result: sum(row["result"] == result for row in rows)
                          for result in ("text_exact", "mismatch", "matched")},
                         {"text_exact": 8, "mismatch": 4, "matched": 8})
        for row in rows:
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
            if row["result"] == "mismatch":
                self.assertEqual((row["instruction_bytes"], row["different_words"]),
                                 {0xE98: ("2768", "498"), 0x2520: ("2004", "514")}[int(row["function_offset"], 0)])
        terminals = [row for row in rows if row["result"] == "matched"]
        self.assertEqual({(int(row["function_offset"], 0), int(row["slot"])) for row in terminals},
                         {(offset, slot) for offset, *_ in self.helpers for slot in (0, 1)})
        for row in terminals:
            offset, slot = int(row["function_offset"], 0), int(row["slot"])
            _, size, role, original = next(helper for helper in self.helpers if helper[0] == offset)
            source = ROOT / ("src/overlays/spanish_model_variant/variant475_" + role +
                             ("_slot1" if slot else "") + ".c")
            if role == "ribbons":
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
