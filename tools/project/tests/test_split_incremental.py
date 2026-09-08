from __future__ import annotations

import contextlib
import importlib.metadata
import io
import json
import os
from pathlib import Path
import sys
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

import yaml

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import build_baseline
import split_incremental
from workspace import WorkspaceError


class IncrementalSplitTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        self.temporary = tempfile.TemporaryDirectory(
            prefix="split-cache-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name) / "workspace"
        self.root.mkdir()
        for name in split_incremental.INPUT_FILES:
            self.write(name, "fixture\n")
        self.write("game/SLUS_014.11", b"retail-fixture")
        self.write("src/game/example.c", "void Example(void) { return; }\n")
        self.write("src/game/shared.h", "#define VALUE 1\n")
        self.write("config/slus_01411/symbols.txt", "D_80010000 = 0x80010000;\n")
        self.write("config/slus_01411/relocations.txt", "")
        self.write(
            "config/slus_01411/functions.csv",
            "address,size,name,status,module,notes\n"
            "0x80010000,0x10,Example,matching_c,game,\n",
        )
        self.write_json(
            "config/slus_01411/matching_c.json",
            {"schema": 1, "functions": [{
                "address": "0x80010000", "size": "0x10",
                "source": "src/game/example.c", "profile": "test",
            }]},
        )
        self.write_json(
            "config/slus_01411/compiler_profiles.json",
            {"schema": 1, "profiles": {"test": {}}},
        )
        self.write_json(
            "config/slus_01411/image_map.json",
            {"schema": 1, "regions": []},
        )
        self.options = {
            "basename": "slus_01411",
            "base_path": "../..",
            "target_path": "game/SLUS_014.11",
            "platform": "psx",
            "compiler": "PSYQ",
            "endianness": "little",
            "asm_path": "tmp/splat/asm",
            "src_path": "src",
            "build_path": "tmp/splat/build",
            "asset_path": "tmp/splat/assets",
            "cache_path": "tmp/splat/cache",
            "data_path": "data",
            "nonmatchings_path": "nonmatchings",
            "matchings_path": "matchings",
            "ld_script_path": "tmp/splat/slus_01411.ld",
            "generated_asm_macros_directory": "tmp/splat/include",
            "lib_path": "tmp/splat/lib",
            "o_path": "tmp/splat/build",
            "undefined_funcs_auto_path": "tmp/splat/undefined_funcs_auto.txt",
            "undefined_syms_auto_path": "tmp/splat/undefined_syms_auto.txt",
            "symbol_addrs_path": ["config/slus_01411/symbols.txt"],
            "reloc_addrs_path": ["config/slus_01411/relocations.txt"],
            "use_legacy_include_asm": False,
        }
        self.write_config()
        tools_patch = patch.object(
            split_incremental, "splitter_tools", return_value={"tools": "first"}
        )
        self.tools = tools_patch.start()
        self.addCleanup(tools_patch.stop)
        generated_patch = patch.object(
            split_incremental, "regenerate", side_effect=self.fake_generate
        )
        self.generated = generated_patch.start()
        self.addCleanup(generated_patch.stop)
        self.output = io.StringIO()
        self.redirect = contextlib.redirect_stdout(self.output)
        self.redirect.__enter__()
        self.addCleanup(self.redirect.__exit__, None, None, None)

    def write(self, name: str, value: str | bytes) -> Path:
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        if isinstance(value, str):
            path.write_text(value, encoding="utf-8")
        else:
            path.write_bytes(value)
        return path

    def write_json(self, name: str, value: object) -> Path:
        return self.write(name, json.dumps(value))

    def write_config(self) -> None:
        self.write(
            split_incremental.CONFIG_PATH,
            yaml.safe_dump({"options": self.options, "segments": []}),
        )

    def fake_generate(self, root: Path) -> None:
        split_incremental.clean.remove_target(root, "generated")
        split_incremental.clean.remove_target(root, "splat")
        for name in split_incremental.REQUIRED_OUTPUTS:
            self.write(name, "generated " + name + "\n")
        self.write("tmp/splat/asm/generated/example.s", "nop\n")
        self.write("tmp/splat/include/macro.inc", ".macro example\n.endm\n")

    def prepare(self) -> bool:
        return split_incremental.prepare(self.root)

    def test_cold_then_unchanged_split_is_reused(self) -> None:
        self.assertFalse(self.prepare())
        path = self.root / split_incremental.GENERATED_CONFIG
        stat = path.stat()
        self.assertTrue(self.prepare())
        self.assertEqual(self.generated.call_count, 1)
        self.assertEqual(path.stat().st_mtime_ns, stat.st_mtime_ns)
        self.assertIn("incremental split: reused", self.output.getvalue())

    def test_body_and_header_changes_do_not_require_splitting(self) -> None:
        self.assertFalse(self.prepare())
        self.write("src/game/example.c", "void Example(void) { int changed = 5; }\n")
        self.write("src/game/shared.h", "#define VALUE 2\n")
        self.assertTrue(self.prepare())
        self.assertEqual(self.generated.call_count, 1)

    def test_function_and_inline_assembly_shapes_invalidate(self) -> None:
        self.assertFalse(self.prepare())
        variants = (
            "void Renamed(void) { return; }\n",
            'INCLUDE_ASM("asm", Example);\n',
            'INCLUDE_RODATA("asm", rodata_name);\n',
        )
        for source in variants:
            with self.subTest(source=source):
                self.write("src/game/example.c", source)
                self.assertFalse(self.prepare())
        self.assertEqual(self.generated.call_count, 4)

    def test_unterminated_include_macro_reports_its_source(self) -> None:
        self.write("src/game/example.c", 'INCLUDE_ASM("asm", Example;\n')
        with self.assertRaisesRegex(
            split_incremental.IncrementalSplitError, "unterminated.*example.c"
        ):
            self.prepare()
        self.generated.assert_not_called()

    def test_relevant_input_contents_invalidate_with_same_timestamp(self) -> None:
        self.assertFalse(self.prepare())
        path = self.root / "config/slus_01411/symbols.txt"
        before = path.stat()
        text = path.read_text()
        path.write_text(text.replace("80010000", "80010004"))
        os.utime(path, ns=(before.st_atime_ns, before.st_mtime_ns))
        self.assertFalse(self.prepare())
        self.assertEqual(self.generated.call_count, 2)
        for name in (
            "config/slus_01411/relocations.txt",
            "config/slus_01411/image_map.json",
            "tools/project/generate_build_config.py",
            "game/SLUS_014.11",
        ):
            with self.subTest(name=name):
                path = self.root / name
                path.write_bytes(path.read_bytes() + b"\n")
                self.assertFalse(self.prepare())

    def test_profile_and_configuration_changes_invalidate(self) -> None:
        self.assertFalse(self.prepare())
        self.write_json(
            "config/slus_01411/compiler_profiles.json",
            {"schema": 1, "profiles": {"test": {"compiler_flags": ["-G0"]}}},
        )
        self.assertFalse(self.prepare())
        self.options["gp_value"] = 0x8009AF08
        self.write_config()
        self.assertFalse(self.prepare())

    def test_tool_implementation_identity_invalidates(self) -> None:
        self.assertFalse(self.prepare())
        self.tools.return_value = {"tools": "second"}
        self.assertFalse(self.prepare())

    def test_source_presence_and_ownership_are_revalidated(self) -> None:
        self.assertFalse(self.prepare())
        path = self.root / "src/game/example.c"
        path.unlink()
        with self.assertRaises(WorkspaceError):
            self.prepare()
        self.write("src/game/example.c", "void Example(void) {}\n")
        self.write(
            "config/slus_01411/functions.csv",
            "address,size,name,status,module,notes\n"
            "0x80010000,0x10,Example,unmatched_asm,game,\n",
        )
        with self.assertRaises(split_incremental.generate_build_config.GenerationError):
            self.prepare()

    def test_matching_source_paths_are_cached_per_source(self) -> None:
        self.write(
            "src/game/example.c",
            "void Example(void) { return; }\n"
            "void ExampleTwo(void) { return; }\n",
        )
        self.write(
            "config/slus_01411/functions.csv",
            "address,size,name,status,module,notes\n"
            "0x80010000,0x10,Example,matching_c,game,\n"
            "0x80010010,0x10,ExampleTwo,matching_c,game,\n",
        )
        self.write_json(
            "config/slus_01411/matching_c.json",
            {
                "schema": 1,
                "functions": [
                    {
                        "address": "0x80010000",
                        "size": "0x10",
                        "source": "src/game/example.c",
                        "profile": "test",
                    },
                    {
                        "address": "0x80010010",
                        "size": "0x10",
                        "source": "src/game/example.c",
                        "profile": "test",
                    },
                ],
            },
        )
        original = split_incremental.generate_build_config.resolve_within
        source_resolutions = 0

        def tracked(root: Path, relative_path: str, *, must_exist: bool = False):
            nonlocal source_resolutions
            if relative_path == "src/game/example.c":
                source_resolutions += 1
            return original(root, relative_path, must_exist=must_exist)

        with patch.object(
            split_incremental.generate_build_config,
            "resolve_within",
            side_effect=tracked,
        ):
            functions = split_incremental.generate_build_config.load_matching_functions(
                self.root
            )
        self.assertEqual(len(functions), 2)
        self.assertEqual(source_resolutions, 1)

    def test_cached_matching_source_path_must_match_its_key(self) -> None:
        other = self.write("src/game/other.c", "void Other(void) {}\n").resolve()
        with self.assertRaisesRegex(
            split_incremental.generate_build_config.GenerationError,
            "cached source path",
        ):
            split_incremental.generate_build_config.load_matching_functions(
                self.root,
                resolved_sources={"src/game/example.c": other},
            )

    def test_source_tree_paths_are_reused_for_shape_scanning(self) -> None:
        self.assertFalse(self.prepare())
        original = split_incremental.resolve_within
        generator_original = split_incremental.generate_build_config.resolve_within
        source_resolutions = 0
        generator_source_resolutions = 0

        def tracked(root: Path, relative_path: str, *, must_exist: bool = False):
            nonlocal source_resolutions
            if relative_path == "src/game/example.c":
                source_resolutions += 1
            return original(root, relative_path, must_exist=must_exist)

        def generator_tracked(
            root: Path,
            relative_path: str,
            *,
            must_exist: bool = False,
        ):
            nonlocal generator_source_resolutions
            if relative_path == "src/game/example.c":
                generator_source_resolutions += 1
            return generator_original(root, relative_path, must_exist=must_exist)

        with (
            patch.object(split_incremental, "resolve_within", side_effect=tracked),
            patch.object(
                split_incremental.generate_build_config,
                "resolve_within",
                side_effect=generator_tracked,
            ),
        ):
            self.assertTrue(self.prepare())
        self.assertEqual(source_resolutions, 0)
        self.assertEqual(generator_source_resolutions, 0)

    def test_source_tree_symlinks_cannot_escape_workspace(self) -> None:
        outside = self.root.parent / "outside.c"
        outside.write_text("void Outside(void) {}\n", encoding="utf-8")
        (self.root / "src/game/escape.c").symlink_to(outside)
        with self.assertRaisesRegex(
            split_incremental.IncrementalSplitError,
            "not inside the workspace",
        ):
            self.prepare()
        self.generated.assert_not_called()

    def test_generated_corruption_and_missing_files_force_regeneration(self) -> None:
        self.assertFalse(self.prepare())
        path = self.root / "tmp/splat/asm/generated/example.s"
        before = path.stat()
        path.write_text("bad\n")
        os.utime(path, ns=(before.st_atime_ns, before.st_mtime_ns))
        self.assertFalse(self.prepare())
        (self.root / "tmp/splat/slus_01411.ld").unlink()
        self.assertFalse(self.prepare())
        (self.root / "tmp/splat/assets/bss_image.bin").unlink()
        self.assertFalse(self.prepare())
        self.assertEqual(self.generated.call_count, 4)

    def test_unexpected_generated_files_are_not_accepted(self) -> None:
        self.assertFalse(self.prepare())
        self.write("tmp/splat/asm/generated/unexpected.s", "stale\n")
        self.assertFalse(self.prepare())
        self.assertFalse((self.root / "tmp/splat/asm/generated/unexpected.s").exists())

    def test_compiler_objects_and_splat_internal_cache_are_not_split_outputs(self) -> None:
        self.assertFalse(self.prepare())
        obj = self.write("tmp/splat/build/src/game/example.o", b"object")
        self.write("tmp/splat/cache/state", "cache")
        before = obj.stat()
        self.assertTrue(self.prepare())
        self.assertEqual(obj.stat().st_mtime_ns, before.st_mtime_ns)
        obj.write_bytes(b"different object")
        self.assertTrue(self.prepare())
        self.assertEqual(self.generated.call_count, 1)

    def test_excluded_directories_are_not_traversed(self) -> None:
        self.assertFalse(self.prepare())
        self.write("tmp/splat/build/src/game/example.o", b"object")
        original = Path.iterdir

        def guarded(path: Path):
            if any(path.is_relative_to(self.root / excluded)
                   for excluded in split_incremental.EXCLUDED_OUTPUTS):
                self.fail(f"traversed excluded directory: {path}")
            return original(path)

        with patch.object(Path, "iterdir", guarded):
            self.assertTrue(self.prepare())

    def test_invalid_state_is_a_reported_cache_miss(self) -> None:
        self.assertFalse(self.prepare())
        self.write(split_incremental.CACHE_PATH, "{broken")
        self.assertFalse(self.prepare())
        self.assertIn("invalid split cache", self.output.getvalue())
        self.write_json(
            split_incremental.CACHE_PATH,
            {"schema": 1, "input_signature": "0" * 64, "outputs": {"../outside": "0" * 64}},
        )
        self.assertFalse(self.prepare())

    def test_failed_regeneration_does_not_leave_a_valid_stamp(self) -> None:
        self.assertFalse(self.prepare())
        self.write("config/slus_01411/relocations.txt", "changed")
        self.generated.side_effect = build_baseline.BuildError("forced failure")
        with self.assertRaisesRegex(build_baseline.BuildError, "forced failure"):
            self.prepare()
        self.assertFalse((self.root / split_incremental.CACHE_PATH).exists())

    def test_incomplete_regeneration_does_not_write_a_stamp(self) -> None:
        def incomplete(root: Path) -> None:
            self.write(split_incremental.GENERATED_CONFIG, "only one file")

        self.generated.side_effect = incomplete
        with self.assertRaisesRegex(split_incremental.IncrementalSplitError, "required"):
            self.prepare()
        self.assertFalse((self.root / split_incremental.CACHE_PATH).exists())

    def test_changed_inputs_during_regeneration_are_rejected(self) -> None:
        def changes_input(root: Path) -> None:
            self.fake_generate(root)
            self.write("config/slus_01411/relocations.txt", "changed while splitting")

        self.generated.side_effect = changes_input
        with self.assertRaisesRegex(split_incremental.IncrementalSplitError, "during generation"):
            self.prepare()
        self.assertFalse((self.root / split_incremental.CACHE_PATH).exists())

    def test_configuration_cannot_escape_or_add_untracked_extension_inputs(self) -> None:
        self.options["target_path"] = "../outside"
        self.write_config()
        with self.assertRaises(split_incremental.IncrementalSplitError):
            self.prepare()
        self.options["target_path"] = "game/SLUS_014.11"
        self.options["extensions_path"] = "tools/extensions"
        self.write_config()
        with self.assertRaisesRegex(split_incremental.IncrementalSplitError, "extension"):
            self.prepare()

    def test_optional_outputs_must_be_inside_the_tracked_split_tree(self) -> None:
        for name, value in (
            ("ld_symbol_header_path", "src/generated.h"),
            ("elf_section_list_path", "src/sections.txt"),
            ("ld_partial_scripts_path", "tmp/other-linkers"),
            ("generated_asm_macros_directory", "tmp/splat/build/include"),
            ("asset_path", "tmp/splat/cache/assets"),
            ("cache_path", "tmp/other-cache"),
        ):
            with self.subTest(name=name):
                original = self.options.get(name)
                self.options[name] = value
                self.write_config()
                with self.assertRaisesRegex(split_incremental.IncrementalSplitError, name):
                    self.prepare()
                if original is None:
                    del self.options[name]
                else:
                    self.options[name] = original
        self.generated.assert_not_called()

    def test_optional_generated_header_is_content_validated(self) -> None:
        name = "tmp/splat/include/segments.h"
        sections = "tmp/splat/elf_sections.txt"
        self.options["ld_symbol_header_path"] = name
        self.options["elf_section_list_path"] = sections
        self.write_config()

        def with_header(root: Path) -> None:
            self.fake_generate(root)
            self.write(name, "extern unsigned char header_ROM_START[];\n")
            self.write(sections, ".header\n.text\n")

        self.generated.side_effect = with_header
        self.assertFalse(self.prepare())
        self.assertTrue(self.prepare())
        (self.root / name).unlink()
        self.assertFalse(self.prepare())
        self.assertIn(name, split_incremental.output_snapshot(self.root))
        (self.root / sections).unlink()
        self.assertFalse(self.prepare())
        self.assertIn(sections, split_incremental.output_snapshot(self.root))

    def test_cache_symlinks_cannot_overwrite_inputs(self) -> None:
        source = self.root / "src/game/example.c"
        original = source.read_bytes()
        for name in (split_incremental.CACHE_PATH, f"{split_incremental.CACHE_PATH}.tmp"):
            with self.subTest(name=name):
                path = self.root / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.symlink_to(source)
                with self.assertRaisesRegex(split_incremental.IncrementalSplitError, "symlinks"):
                    self.prepare()
                self.assertEqual(source.read_bytes(), original)
                path.unlink()

    def test_generated_symlinks_are_not_reused(self) -> None:
        self.assertFalse(self.prepare())
        path = self.root / "tmp/splat/asm/generated/example.s"
        path.unlink()
        path.symlink_to(self.root / "game/SLUS_014.11")
        self.assertEqual(split_incremental.output_snapshot(self.root), {})


class SplitterToolsTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary = tempfile.TemporaryDirectory(
            prefix="split-tools-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.distributions = {}
        for name in ("packaging", "pyyaml", "splat64", "spimdisasm", "rabbitizer"):
            path = self.root / name
            path.mkdir()
            (path / "module.py").write_text(f"# {name}\n", encoding="utf-8")
            (path / "module.pyc").write_bytes(b"ignored bytecode")
            self.distributions[name] = SimpleNamespace(
                version="1.0",
                files=[importlib.metadata.PackagePath("module.py"),
                       importlib.metadata.PackagePath("module.pyc")],
                requires=[],
                locate_file=lambda item, path=path: path / item,
            )
        self.distributions["splat64"].requires = [
            "spimdisasm; extra == 'mips'",
            "rabbitizer; extra == 'mips'",
            "not-installed; extra == 'dev'",
            "not-installed; python_version < '2'",
        ]
        self.distributions["spimdisasm"].requires = ["rabbitizer", "splat64[mips]"]
        distribution_patch = patch.object(
            importlib.metadata, "distribution", side_effect=self.distributions.__getitem__
        )
        distribution_patch.start()
        self.addCleanup(distribution_patch.stop)

    def test_mips_extras_transitive_dependencies_and_cycles(self) -> None:
        with patch.object(split_incremental, "sha256", wraps=split_incremental.sha256) as hashed:
            tools = split_incremental.splitter_tools()
        self.assertEqual(set(tools["distributions"]), set(self.distributions))
        self.assertEqual(len(tools["files"]), 5)
        self.assertTrue(all(name.endswith("module.py") for name in tools["files"]))
        self.assertEqual(hashed.call_count, 6)
        before = split_incremental.digest_value(tools)
        (self.root / "rabbitizer/module.py").write_text("# changed\n", encoding="utf-8")
        self.assertNotEqual(before, split_incremental.digest_value(split_incremental.splitter_tools()))

    def test_transitive_requested_extras_are_followed(self) -> None:
        self.distributions["extra-dependency"] = self.distributions["pyyaml"]
        self.distributions["splat64"].requires.append("spimdisasm[special]")
        self.distributions["spimdisasm"].requires.append("extra-dependency; extra == 'special'")
        tools = split_incremental.splitter_tools()
        self.assertIn("extra-dependency", tools["distributions"])

    def test_disassembler_environment_is_fingerprinted(self) -> None:
        before = split_incremental.digest_value(split_incremental.splitter_tools())
        with patch.dict(os.environ, {"SPIMDISASM_ASM_COMMENTS": "FALSE"}):
            tools = split_incremental.splitter_tools()
            self.assertEqual(tools["spimdisasm_environment"]["SPIMDISASM_ASM_COMMENTS"], "FALSE")
            self.assertNotEqual(before, split_incremental.digest_value(tools))

    def test_missing_inventory_or_distribution_is_an_error(self) -> None:
        self.distributions["splat64"].files = None
        with self.assertRaisesRegex(split_incremental.IncrementalSplitError, "inventory"):
            split_incremental.splitter_tools()
        with patch.object(
            importlib.metadata, "distribution",
            side_effect=importlib.metadata.PackageNotFoundError("missing"),
        ):
            with self.assertRaisesRegex(split_incremental.IncrementalSplitError, "missing"):
                split_incremental.splitter_tools()


if __name__ == "__main__":
    unittest.main()
