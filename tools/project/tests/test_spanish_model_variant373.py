import csv
import hashlib
import importlib.util
import re
import struct

from tools.project.tests import test_french_model_variant373 as shared
from tools.project.tests import test_french_model_variant435 as family435
from tools.project.progress import load_spanish_overlay_inventories
from tools.project.overlay_sources import c_segments


class SpanishModelVariant373Tests(family435.FrenchModelVariant435Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    family = 373
    module_count = 2
    distinct_images = 2
    binding_count = 34
    tail_start = 0x3EF0
    spans = shared.SPANS
    helpers = ((0x176C, 1912, "grid", "func_8013C76C"),
               (0x1EE4, 856, "points", "func_8013CEE4"),
               (0x223C, 1352, "strip", "func_8013D23C"),
               (0x2784, 1764, "ribbons", "func_8013D784"),
               (0x2E68, 1412, "quads", "func_8013DE68"),
               (0x33EC, 2820, "rays", "func_8013E3EC"))
    reachable_helpers = {0x176C, 0x1EE4}
    local_call_targets = {0x10B0, 0x176C, 0x1EE4}
    models_by_stage = ((7, (707,)),)
    entry_anchors = {
        0x1C: 0x26B90D48, 0x7C: 0x26B82364, 0xA8: 0xAFB800AC,
        0x528: 0x8FA400AC, 0x52C: 0x0C020BAA,
        0x3C: 0x26B61DF8, 0x224: 0x0C020BBA, 0x228: 0x02C02021,
        0x6C: 0x26B11DB8, 0x1F0: 0x02202021,
        0x1F8: 0x0C020B92, 0x1FC: 0xA7A2004A,
        0x278: 0x26D60034, 0x27C: 0x0C020BBA, 0x280: 0x02C02021,
        0x94: 0xAFA50114, 0xCC: 0x00191880, 0xD0: 0x00791821,
        0xD4: 0x000318C0, 0xDC: 0xAEA32418,
        0x2190: 0x2462E800, 0x21E8: 0x28420007,
        0x26D0: 0x28420401, 0x26F4: 0x00021403,
        0x270C: 0xAE622484, 0x2750: 0xA6602476,
        0x2E08: 0x1462000B, 0x2E2C: 0x00031840, 0x2E34: 0xAFC22478,
        0x3204: 0x0043001B, 0x3210: 0x0007000D,
        0x3274: 0x00021403, 0x3304: 0x28420041, 0x339C: 0xAE002448,
        0x2658: 0x18400007, 0x265C: 0x28420800,
        0x2D9C: 0x18400008, 0x2DA0: 0x28420800,
        0x3180: 0x18C00007, 0x3188: 0x28C20800,
        0x4C: 0x26B821A0, 0x5C: 0x26B92224,
        0x4FC: 0x26520028, 0x500: 0x26730034, 0x508: 0x2AC20008,
        0xE7C: 0x0043102B, 0xE90: 0x28620008, 0xE98: 0x28620006,
        0xEB0: 0x26B821A0, 0xED4: 0x0C020BBA, 0xEF4: 0x0C020B6A,
        0xF00: 0x0C020B76, 0xF04: 0x00002821, 0xF10: 0x2AC20008,
        0x1968: 0xAE222440, 0x19EC: 0x2AC20009,
        0x1A84: 0x24090088, 0x1B9C: 0x27A200D0, 0x1BA4: 0x27A200D4,
        0x1BC8: 0x04C00009, 0x1BD8: 0x04400005, 0x1BE8: 0x30C6FFFF,
        0x1BFC: 0x2BC20010, 0x1C18: 0x26F70034, 0x1C38: 0x2AC20008,
        0x1C44: 0x28620081, 0x1CA0: 0x0043001B, 0x1CAC: 0x0007000D,
        0x1D24: 0x0043001B, 0x1D30: 0x0007000D,
    }

    assert_target_layout = shared.FrenchModelVariant373Tests.assert_target_layout
    test_target_compiled_grid_layout = shared.FrenchModelVariant373Tests.test_target_compiled_grid_layout
    test_grid_projection_timeline_and_phase_rules = shared.FrenchModelVariant373Tests.test_grid_projection_timeline_and_phase_rules
    test_target_compiled_measured_layout = shared.FrenchModelVariant373Tests.test_target_compiled_measured_layout
    test_target_compiled_strip_layout = shared.FrenchModelVariant373Tests.test_target_compiled_strip_layout
    test_target_compiled_ribbon_layout = shared.FrenchModelVariant373Tests.test_target_compiled_ribbon_layout
    test_target_compiled_quad_layout = shared.FrenchModelVariant373Tests.test_target_compiled_quad_layout
    test_sprite_colors_projection_and_wrap_rules = shared.FrenchModelVariant373Tests.test_sprite_colors_projection_and_wrap_rules
    test_strip_projection_signed_visibility_and_phase_rules = shared.FrenchModelVariant373Tests.test_strip_projection_signed_visibility_and_phase_rules
    test_ribbon_projection_colors_scale_and_ungated_angle_update = shared.FrenchModelVariant373Tests.test_ribbon_projection_colors_scale_and_ungated_angle_update
    test_quad_projection_colors_and_phase_rules = shared.FrenchModelVariant373Tests.test_quad_projection_colors_and_phase_rules
    test_target_compiled_ray_layout = shared.FrenchModelVariant373Tests.test_target_compiled_ray_layout
    test_ray_normalization_geometry_and_phase_rules = shared.FrenchModelVariant373Tests.test_ray_normalization_geometry_and_phase_rules

    def test_wrappers_only_rename_verified_functions(self):
        with (family435.ROOT / "notes/overlays/spanish-model-variant373-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 22)
        self.assertEqual([row["result"] for row in rows], ["text_exact", "matched"] * 10 + ["matched"] * 2)
        terminal = [row for row in rows if row["result"] == "matched"]
        self.assertEqual({(row["module"], int(row["function_offset"], 0)) for row in terminal},
                         {(module["name"], offset) for module in self.modules
                          for offset, _, _, _ in self.helpers})
        for slot in (0, 1):
            for offset, size, role, original in self.helpers:
                source = family435.ROOT / "src/overlays/french_model_variant" / (
                    f"variant373_{role}" + ("_slot1" if slot else "") + ".c")
                text = source.read_text()
                if slot:
                    self.assertEqual(text, '#include "../../types.h"\n\n'
                                     f"#define {original} func_{0x8017B000 + offset:X}\n"
                                     f'#include "variant373_{role}.c"\n')
                else:
                    self.assertIn(f"void {original}(u8 *context)", text)
                    self.assertNotRegex(text, r"\b(?:extern|asm|__asm__|register|volatile)\b")
                matching = [row for row in terminal if int(row["slot"]) == slot
                            and int(row["function_offset"], 0) == offset]
                self.assertEqual(len(matching), 1)
                row = matching[0]
                self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
                self.assertEqual((row["profile"], row["instruction_bytes"], row["different_words"]),
                                 ("gcc_2_8_1_g0_split", str(size), "0"))

    def test_ray_compiler_objects_calls_and_raw_owners_when_built(self):
        root = family435.ROOT
        for module in self.modules:
            if not (root / f"tmp/overlays/{module['name']}/build/{module['name']}.elf").is_file():
                self.skipTest("Build Spanish MODEL373 images before checking C owners")
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools is required for ELF ownership checks")
        from elftools.elf.elffile import ELFFile

        with (self.config / "functions.csv").open() as handle:
            resident = {int(row["address"], 0) for row in csv.DictReader(handle)}
        for module in self.modules:
            base = int(module["load_address"], 0)
            directory = root / f"tmp/overlays/{module['name']}"
            image = (root / module["output"]).read_bytes()
            self.assertEqual(hashlib.sha256(image).hexdigest(), module["sha256"])
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            selected, = [s for s in c_segments(root, root / module["layout"])
                         if "variant373_rays" in s["source"]]
            obj = directory / "build" / selected["object"]
            self.assertIn(f"{obj.relative_to(root)}(.text);",
                          (directory / f"{module['name']}.ld").read_text())
            with (directory / f"build/{module['name']}.elf").open("rb") as linked_handle, obj.open("rb") as handle:
                linked, compiled = ELFFile(linked_handle), ELFFile(handle)
                final_symbols = linked.get_section_by_name(".symtab")
                object_symbols = compiled.get_section_by_name(".symtab")
                name = f"func_{base+0x33EC:X}"
                own, = object_symbols.get_symbol_by_name(name)
                definition, = final_symbols.get_symbol_by_name(name)
                for symbol, elf, address in ((own, compiled, 0), (definition, linked, base + 0x33EC)):
                    self.assertIsInstance(symbol["st_shndx"], int)
                    self.assertEqual((symbol["st_value"], symbol["st_size"],
                                      symbol["st_info"]["type"]), (address, 2820, "STT_FUNC"))
                    self.assertTrue(elf.get_section(symbol["st_shndx"])["sh_flags"] & 4)
                section = linked.get_section(definition["st_shndx"])
                start = definition["st_value"] - section["sh_addr"]
                self.assertEqual(section.data()[start:start + 2820], image[0x33EC:0x3EF0])
                text = compiled.get_section(own["st_shndx"]).data()
                relocations = compiled.get_section_by_name(".rel.text")
                self.assertIsNotNone(relocations)
                targets = set()
                for relocation in relocations.iter_relocations():
                    if relocation["r_info_type"] != 4:
                        continue
                    target = object_symbols.get_symbol(relocation["r_info_sym"])
                    if target["st_shndx"] != "SHN_UNDEF":
                        continue
                    resolved, = final_symbols.get_symbol_by_name(target.name)
                    self.assertIn(resolved["st_value"], resident)
                    site = relocation["r_offset"]
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    word = struct.unpack_from("<I", image, 0x33EC + site)[0]
                    address = ((base + 0x33EC + site + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    self.assertEqual(address, resolved["st_value"] + addend, target.name)
                    targets.add(target.name)
                self.assertTrue({"RotTransPers4", "RotTransPers", "GsSortPoly"} <= targets)
                for offset, size in ((0, 4), (0x3EF0, 4368)):
                    raw, = final_symbols.get_symbol_by_name(f"D_{base+offset:X}")
                    self.assertIsInstance(raw["st_shndx"], int)
                    self.assertEqual(raw["st_value"], base + offset)
                    section = linked.get_section(raw["st_shndx"])
                    self.assertFalse(section["sh_flags"] & 4)
                    start = raw["st_value"] - section["sh_addr"]
                    self.assertEqual(section.data()[start:start + size], image[offset:offset + size])

    def test_spanish_resident_bindings_and_conditional_descriptor(self):
        from tools.project.overlay_function_inventory import walk_function

        archive_path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest("legal Spanish MODEL input required")
        bindings = {name: int(address, 16) for name, address in re.findall(
            r"^(\w+) = (0x[0-9A-F]+);$",
            (self.config / "overlays/model_variant373_linker_symbols.txt").read_text(), re.M)}
        self.assertEqual(len(bindings), 34)
        with (self.config / "functions.csv").open() as handle:
            resident = {int(row["address"], 0) for row in csv.DictReader(handle)}
        observed = set()
        with archive_path.open("rb") as archive:
            for module in self.modules:
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                for start, end in self.spans:
                    cfg = walk_function(data, base, start, end - start)
                    self.assertTrue(cfg["closed"])
                    self.assertFalse(cfg["indirect_calls"])
                    self.assertTrue(cfg["external"] <= resident)
                    observed.update(cfg["external"])
                archive.seek((607 * 276 + 275) * 2048 + 0x110)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, 539001)
                self.assertEqual(struct.unpack_from("<I", data, 0xB8)[0],
                                 0x3C020000 | ((base + 0x3F60 + 0x8000) >> 16))
                self.assertEqual(struct.unpack_from("<I", data, 0xBC)[0],
                                 0x24420000 | ((base + 0x3F60) & 65535))
                descriptor = 0x3F60 + (command % 1000) * 40
                self.assertEqual(descriptor, 0x3F88)
                self.assertGreaterEqual(descriptor, self.tail_start)
                self.assertLessEqual(descriptor + 40, len(data))
                self.assertEqual(struct.unpack_from("<H", data, descriptor + 12)[0], 1)
                self.assertEqual(struct.unpack_from("<I", data, descriptor + 20)[0], 60)
        self.assertEqual(observed, set(bindings.values()))
        self.assertEqual(bindings["RotTransPers3"], 0x80087898)
        self.assertEqual(bindings["GsGetActiveBuff"], 0x800852A8)

    def test_spanish_grid_packet_copy_contract(self):
        path = family435.ROOT / "game/spain/SLES_039.51"
        if not path.exists():
            self.skipTest("legal Spanish resident input required")
        data = path.read_bytes()
        self.assertEqual(hashlib.sha256(data).hexdigest(),
                         "b0fefd88b6510f49af4f01e6180e40371652b7ceaa5f31dcb938c942316fc790")
        with (self.config / "functions.csv").open() as handle:
            functions = {int(row["address"], 0): row for row in csv.DictReader(handle)}
        for address, size in ((0x800842A8, 452), (0x80084018, 152),
                              (0x80082EE8, 20)):
            self.assertEqual(int(functions[address]["size"], 0), size)
            self.assertEqual(functions[address]["status"], "sdk_asm")
        for address, word in {
            0x800842B4: 0x8D29F5C4, 0x800842D4: 0xAD220004,
            0x800842E0: 0x00431021, 0x800842E4: 0xA5220008,
            0x8008444C: 0x0C021006, 0x80084450: 0x30C6FFFF,
            0x80084458: 0xAC22F5C4, 0x80084048: 0x00C28823,
            0x80084074: 0x24420004, 0x80084080: 0xAE040000,
            0x80084084: 0xA2140003, 0x80084088: 0xAC700000,
            0x80082EE8: 0x2402000C, 0x80082EF0: 0x2402003C,
        }.items():
            self.assertEqual(struct.unpack_from("<I", data, address - 0x80010000 + 0x800)[0], word)

    def test_direct_stack_stores_preserve_projection_outputs(self):
        path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal Spanish MODEL input required")
        with path.open("rb") as archive:
            for module in self.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                words = struct.unpack("<5120I", data)
                for start, end, frame, outputs, gap in (
                    (0x176C, 0x1EE4, 288, (0xD0, 0xD4), None),
                    (0x1EE4, 0x223C, 272, (0xD8, 0xDC, 0xE0), (0xB8, 0xC8)),
                    (0x223C, 0x2784, 256, (0xC8, 0xCC), None),
                    (0x2784, 0x2E68, 296, (0xD0, 0xD4), None),
                    (0x2E68, 0x33EC, 288, (0xE0, 0xE4), (0x30, 0x40)),
                ):
                    self.assertEqual(words[start // 4], 0x27BD0000 | ((-frame) & 65535))
                    self.assertEqual(words[(end - 4) // 4], 0x27BD0000 | frame)
                    for word in words[start // 4:end // 4]:
                        op, rs = word >> 26, word >> 21 & 31
                        if rs != 29 or op not in (40, 41, 42, 43, 46):
                            continue
                        self.assertIn(op, (40, 41, 43))
                        size = {40: 1, 41: 2, 43: 4}[op]
                        offset = (word & 65535) - (65536 if word & 32768 else 0)
                        self.assertTrue(0 <= offset <= frame - size)
                        self.assertTrue(all(offset + size <= target or target + 4 <= offset
                                            for target in outputs))
                        if gap:
                            self.assertTrue(offset + size <= gap[0] or offset >= gap[1])
