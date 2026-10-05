import csv
import hashlib
import importlib.util
import json
import re
import struct

from tools.project.tests import test_french_model_variant101 as shared
from tools.project.overlay_sources import c_segments
from tools.project.progress import load_spanish_overlay_inventories

ROOT = shared.family435.ROOT


class SpanishModelVariant101Tests(shared.FrenchModelVariant101Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    load_inventories = staticmethod(load_spanish_overlay_inventories)

    def test_entry_compiler_constant_calls_and_raw_owners_when_built(self):
        for module in self.modules:
            if not (ROOT / f"tmp/overlays/{module['name']}/build/{module['name']}.elf").is_file():
                self.skipTest("Build Spanish MODEL101 images before checking compiler owners")
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools is required for ELF ownership checks")
        from elftools.elf.elffile import ELFFile

        with (self.config / "functions.csv").open() as handle:
            resident = {int(row["address"], 0) for row in csv.DictReader(handle)}
        for module in self.modules:
            base = int(module["load_address"], 0)
            directory = ROOT / f"tmp/overlays/{module['name']}"
            image = (ROOT / module["output"]).read_bytes()
            self.assertEqual(hashlib.sha256(image).hexdigest(), module["sha256"])
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            selected, = c_segments(ROOT, ROOT / module["layout"])
            obj = directory / "build" / selected["object"]
            script = (directory / f"{module['name']}.ld").read_text()
            self.assertIn(f"{obj.relative_to(ROOT)}(.text);", script)
            self.assertIn(f"{obj.relative_to(ROOT)}(.rodata);", script)
            with (directory / f"build/{module['name']}.elf").open("rb") as final_handle, obj.open("rb") as source_handle:
                final, source = ELFFile(final_handle), ELFFile(source_handle)
                final_symbols = final.get_section_by_name(".symtab")
                source_symbols = source.get_section_by_name(".symtab")
                name = f"func_{base+4:X}"
                own, = source_symbols.get_symbol_by_name(name)
                definition, = final_symbols.get_symbol_by_name(name)
                for symbol, elf, address in ((own, source, 0), (definition, final, base + 4)):
                    self.assertIsInstance(symbol["st_shndx"], int)
                    self.assertEqual((symbol["st_value"], symbol["st_size"], symbol["st_info"]["type"]),
                                     (address, 2784, "STT_FUNC"))
                    self.assertTrue(elf.get_section(symbol["st_shndx"])["sh_flags"] & 4)
                section = final.get_section(definition["st_shndx"])
                start = definition["st_value"] - section["sh_addr"]
                self.assertEqual(section.data()[start:start + 2784], image[4:0xAE4])
                # GCC leaves this symbol unsized; Splat combines its input with entry text.
                for elf, address, section_size, flags in (
                        (source, 0, 16, 2), (final, base + 0xAE4, 2800, 7)):
                    constant, = elf.get_section_by_name(".symtab").get_symbol_by_name(f"D_{base+0xAE4:X}")
                    self.assertIsInstance(constant["st_shndx"], int)
                    self.assertEqual((constant["st_info"]["type"], constant["st_size"],
                                      constant["st_value"]), ("STT_NOTYPE", 0, address))
                    section = elf.get_section(constant["st_shndx"])
                    self.assertEqual((section["sh_size"], section["sh_flags"]), (section_size, flags))
                    start = constant["st_value"] - section["sh_addr"]
                    self.assertEqual(start + 16, section_size)
                    self.assertEqual(section.data()[start:start + 16], image[0xAE4:0xAF4])
                text = source.get_section(own["st_shndx"]).data()
                relocations = source.get_section_by_name(".rel.text")
                self.assertIsNotNone(relocations)
                targets, count = set(), 0
                for relocation in relocations.iter_relocations():
                    if relocation["r_info_type"] != 4:
                        continue
                    target = source_symbols.get_symbol(relocation["r_info_sym"])
                    site = relocation["r_offset"]
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    word = struct.unpack_from("<I", image, 4 + site)[0]
                    address = ((base + 8 + site) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    if target["st_shndx"] != "SHN_UNDEF":
                        self.assertEqual(target["st_shndx"], own["st_shndx"])
                        self.assertEqual(word >> 26, 2)
                        self.assertEqual(address, base + 4 + target["st_value"] + addend)
                        self.assertTrue(base + 4 <= address < base + 0xAE4)
                        continue
                    resolved, = final_symbols.get_symbol_by_name(target.name)
                    self.assertIn(resolved["st_value"], resident)
                    self.assertEqual(address, resolved["st_value"] + addend, target.name)
                    targets.add(address)
                    count += 1
                self.assertEqual((len(targets), count), (23, 43))
                for offset, size, stem in ((0, 4, "module_header"), (0xAF4, 28, "image_view"),
                                          (0xB10, 17648, "unclassified_tail")):
                    raw_obj = directory / f"build/tmp/overlays/{module['name']}/asm/data/overlays/{module['name']}/{stem}.data.o"
                    self.assertIn(raw_obj.relative_to(ROOT).as_posix(), script)
                    with raw_obj.open("rb") as raw_handle:
                        raw_elf = ELFFile(raw_handle)
                        for elf in (raw_elf, final):
                            raw, = elf.get_section_by_name(".symtab").get_symbol_by_name(f"D_{base+offset:X}")
                            self.assertIsInstance(raw["st_shndx"], int)
                            self.assertEqual((raw["st_info"]["type"], raw["st_size"]), ("STT_OBJECT", size))
                            section = elf.get_section(raw["st_shndx"])
                            self.assertFalse(section["sh_flags"] & 4)
                            start = raw["st_value"] - section["sh_addr"]
                            self.assertEqual(section.data()[start:start + size], image[offset:offset + size])

    def test_resident_callee_input_and_final_definitions_when_built(self):
        linked = ROOT / "tmp/project-build/SLES_039.51.elf"
        if not linked.is_file():
            self.skipTest("Build Spanish resident before checking real callee owners")
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools is required for ELF ownership checks")
        from elftools.elf.elffile import ELFFile

        retail = (ROOT / "game/spain/SLES_039.51").read_bytes()
        self.assertEqual(hashlib.sha256(retail).hexdigest(),
                         "b0fefd88b6510f49af4f01e6180e40371652b7ceaa5f31dcb938c942316fc790")
        self.assertEqual(linked.with_suffix("").read_bytes(), retail)
        directory = ROOT / "tmp/splat/sles_03951"
        script = (directory / "sles_03951.ld").read_text()
        bindings = (ROOT / self.modules[0]["linker_symbols"]).read_text()
        addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
        self.assertEqual(len(addresses), 23)
        with (self.config / "functions.csv").open() as handle:
            inventory = {int(row["address"], 0): row for row in csv.DictReader(handle)}
        matching = {int(row["address"], 0): row for row in json.loads(
            (self.config / "matching_c.json").read_text())["functions"]}
        with linked.open("rb") as handle:
            final = ELFFile(handle)
            for address in addresses:
                row = inventory[address]
                size = int(row["size"], 0)
                if address in matching:
                    obj = directory / "build" / (matching[address]["source"][:-2] + ".o")
                else:
                    self.assertTrue(address == 0x8005C018 or address >= 0x80073C4C)
                    start = 0x8005C018 if address == 0x8005C018 else 0x80073C4C
                    obj = directory / f"build/tmp/splat/sles_03951/asm/generated/spanish_{start:08x}.o"
                self.assertIn(obj.relative_to(ROOT).as_posix(), script)
                with obj.open("rb") as source_handle:
                    source = ELFFile(source_handle)
                    own, = source.get_section_by_name(".symtab").get_symbol_by_name(row["name"])
                    self.assertIsInstance(own["st_shndx"], int)
                    self.assertEqual((own["st_info"]["type"], own["st_size"]), ("STT_FUNC", size))
                    self.assertTrue(source.get_section(own["st_shndx"])["sh_flags"] & 4)
                definition, = final.get_section_by_name(".symtab").get_symbol_by_name(row["name"])
                self.assertIsInstance(definition["st_shndx"], int)
                self.assertEqual((definition["st_value"], definition["st_size"],
                                  definition["st_info"]["type"]), (address, size, "STT_FUNC"))
                section = final.get_section(definition["st_shndx"])
                self.assertTrue(section["sh_flags"] & 4)
                start = address - section["sh_addr"]
                offset = 0x800 + address - 0x80010000
                self.assertEqual(section.data()[start:start + size], retail[offset:offset + size])
