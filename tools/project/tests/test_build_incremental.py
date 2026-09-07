from __future__ import annotations

from collections import Counter
from contextlib import ExitStack, redirect_stdout
import hashlib
import io
import os
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import build_baseline
import build_incremental
from workspace import WorkspaceError


class WorkspaceTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="incremental-build-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name) / "workspace"
        self.root.mkdir()
        self.patches = ExitStack()
        self.addCleanup(self.patches.close)
        self.compiler = self.write("tools/gcc", b"compiler fixture 1\n")
        self.filter = self.write("tools/filter.py", "# filter fixture 1\n")
        self.assembler = self.write(
            f"{build_baseline.TOOLCHAIN}/mipsel-none-elf-as", b"assembler"
        )
        self.write(
            f"{build_baseline.TOOLCHAIN}/mipsel-none-elf-objcopy", b"objcopy"
        )
        self.write("tools/vendor/maspsx/maspsx.py", "# maspsx fixture\n")
        self.write("tmp/splat/include/macro.inc", ".macro fixture\n.endm\n")
        for module in (build_baseline, build_incremental):
            path = self.write(f"tools/project/{module.__name__}.py", "# fixture\n")
            self.patches.enter_context(patch.object(module, "__file__", str(path)))
        self.profiles = {
            "gcc281": {
                "compiler": "tools/gcc",
                "compiler_flags": ["-O2", "-G0"],
                "maspsx_flags": ["--expand-div"],
                "aspsx_version": "2.81",
                "data_limit": 0,
                "assembly_filter": "tools/filter.py",
            }
        }

    def write(self, name: str, contents: str | bytes) -> Path:
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(contents.encode("utf-8") if isinstance(contents, str) else contents)
        return path

    def edit_preserving_metadata(self, path: Path, contents: bytes) -> None:
        before = path.stat()
        self.assertEqual(len(contents), before.st_size)
        path.write_bytes(contents)
        os.utime(path, ns=(before.st_atime_ns, before.st_mtime_ns))
        self.assertEqual(path.stat().st_mtime_ns, before.st_mtime_ns)
        self.assertEqual(path.stat().st_size, before.st_size)

    def context(self, file_cache=None):
        return build_incremental.dependency_context(
            self.root, self.profiles, file_cache=file_cache
        )

    def expected_include_digest(self, *names: str) -> str:
        return build_incremental.digest_value([
            (name, build_incremental.sha256(self.root / name))
            for name in names
        ])


class DependencyFingerprintTests(WorkspaceTests):
    def test_public_digest_helpers_keep_their_content_semantics(self) -> None:
        self.assertEqual(
            build_incremental.sha256(self.compiler),
            hashlib.sha256(self.compiler.read_bytes()).hexdigest(),
        )
        value = {"z": ["second", "first"], "a": 1}
        self.assertEqual(
            build_incremental.digest_value(value),
            hashlib.sha256(b'{"a":1,"z":["second","first"]}').hexdigest(),
        )

    def test_many_profiles_hash_each_resolved_compiler_and_filter_once(self) -> None:
        second_compiler = self.write("tools/gcc272", b"second compiler")
        (self.root / "tools/gcc-alias").symlink_to("gcc")
        profiles = {
            f"profile-{index}": {
                **self.profiles["gcc281"],
                "compiler": ("tools/gcc", "tools/gcc-alias", "tools/gcc272")[index % 3],
                "compiler_flags": ["-O2", f"-DPROFILE={index}"],
            }
            for index in range(31)
        }
        with patch.object(
            build_incremental, "sha256", wraps=build_incremental.sha256
        ) as hash_file:
            context = build_incremental.dependency_context(self.root, profiles)
        calls = Counter(call.args[0] for call in hash_file.call_args_list)
        self.assertEqual(calls[self.compiler], 1)
        self.assertEqual(calls[second_compiler], 1)
        self.assertEqual(calls[self.filter], 1)
        self.assertEqual(set(context["profiles"]), set(profiles))
        for name, profile in profiles.items():
            self.assertEqual(context["profiles"][name], {
                "configuration": profile,
                "compiler_sha256": build_incremental.sha256(
                    self.root / profile["compiler"]
                ),
                "assembly_filter_sha256": build_incremental.sha256(self.filter),
            })

    def test_shared_nested_headers_and_sources_are_read_hashed_and_parsed_once(self) -> None:
        files = {
            "src/game/first.c": (
                b'#include "../include/shared.h"\n'
                b'#include "../include/nested.h"\nint First(void) { return 1; }\n'
            ),
            "src/game/second.c": (
                b'#include "../include/shared-alias.h"\nint Second(void) { return 2; }\n'
            ),
            "src/include/shared.h": b'#include "nested.h"\n#define SHARED 1\n',
            "src/include/nested.h": b"#define NESTED 2\n",
        }
        for name, contents in files.items():
            self.write(name, contents)
        (self.root / "src/include/shared-alias.h").symlink_to("shared.h")
        file_cache = build_incremental._FileCache()
        context = self.context(file_cache)
        with (
            patch.object(Path, "open", autospec=True, side_effect=Path.open) as reads,
            patch.object(build_incremental.hashlib, "sha256", wraps=hashlib.sha256) as hashes,
            patch.object(
                build_incremental, "INCLUDE_PATTERN", wraps=build_incremental.INCLUDE_PATTERN
            ) as pattern,
        ):
            for name in ("first", "second"):
                component = build_incremental.Component(
                    "c", f"src/game/{name}.c", f"{name}.o", "gcc281"
                )
                build_incremental.component_signature(
                    self.root, component, context, file_cache=file_cache
                )
            actual = build_incremental.include_digest(
                self.root, self.root / "src/game/first.c", file_cache=file_cache
            )
        read_counts = Counter(call.args[0] for call in reads.call_args_list)
        self.assertEqual(read_counts, Counter({self.root / name: 1 for name in files}))
        for contents in files.values():
            self.assertEqual(
                sum(call.args == (contents,) for call in hashes.call_args_list), 1
            )
            self.assertEqual(
                sum(
                    call.args == (contents.decode("utf-8"),)
                    for call in pattern.findall.call_args_list
                ),
                1,
            )
        self.assertEqual(actual, self.expected_include_digest(
            "src/game/first.c", "src/include/shared.h", "src/include/nested.h"
        ))

    def test_context_and_component_signatures_share_the_same_file_cache(self) -> None:
        header = self.root / "tmp/splat/include/macro.inc"
        source = self.write(
            "src/game/example.c", '#include "../../tmp/splat/include/macro.inc"\n'
        )
        file_cache = build_incremental._FileCache()
        with patch.object(
            build_incremental, "sha256", wraps=build_incremental.sha256
        ) as hash_file:
            context = self.context(file_cache)
            for component in (
                build_incremental.Component(
                    "asm", str(header.relative_to(self.root)), "macro.o"
                ),
                build_incremental.Component(
                    "c", str(source.relative_to(self.root)), "example.o", "gcc281"
                ),
            ):
                build_incremental.component_signature(
                    self.root, component, context, file_cache=file_cache
                )
        self.assertEqual(
            sum(call.args == (header,) for call in hash_file.call_args_list), 1
        )

    def test_include_cycles_keep_per_source_depth_first_order(self) -> None:
        self.write("src/first.c", '#include "a.h"\n#include "b.h"\n')
        self.write("src/second.c", '#include "b.h"\n#include "a.h"\n')
        self.write("src/a.h", '#include "b.h"\n')
        self.write("src/b.h", '#include "a.h"\n')
        file_cache = build_incremental._FileCache()
        for source, headers in (
            ("src/first.c", ("src/a.h", "src/b.h")),
            ("src/second.c", ("src/b.h", "src/a.h")),
        ):
            with self.subTest(source=source):
                self.assertEqual(
                    build_incremental.include_digest(
                        self.root, self.root / source, file_cache=file_cache
                    ),
                    self.expected_include_digest(source, *headers),
                )

    def test_permissive_decoding_and_universal_newlines_are_preserved(self) -> None:
        source = self.write(
            "src/example.c",
            b'// Shift-JIS \x81\x82\r#include "first.h"\r\n#include <ignored.h>\n',
        )
        self.write("src/first.h", b'// header\r#include "second.h"\r')
        self.write("src/second.h", b"#define VALUE 1\r\n")
        self.assertEqual(
            build_incremental.include_digest(self.root, source),
            self.expected_include_digest("src/example.c", "src/first.h", "src/second.h"),
        )

    def test_signature_fields_and_dependency_order_match_the_previous_format(self) -> None:
        self.write("src/example.c", '#include "header.h"\nint Example;\n')
        self.write("src/header.h", "#define VALUE 1\n")
        self.write("tmp/splat/asm/example.s", "nop\n")
        self.write("tmp/splat/assets/example.bin", b"binary")
        context = self.context()
        for component in (
            build_incremental.Component("c", "src/example.c", "example.o", "gcc281"),
            build_incremental.Component("asm", "tmp/splat/asm/example.s", "example.o"),
            build_incremental.Component("binary", "tmp/splat/assets/example.bin", "example.o"),
        ):
            with self.subTest(kind=component.kind):
                value = {
                    "kind": component.kind,
                    "source": component.source,
                    "source_sha256": build_incremental.sha256(self.root / component.source),
                    "object": component.object_name,
                    "build_baseline_sha256": context["build_baseline_sha256"],
                    "build_incremental_sha256": context["build_incremental_sha256"],
                }
                if component.kind == "c":
                    value.update({
                        "source_and_includes_sha256": self.expected_include_digest(
                            "src/example.c", "src/header.h"
                        ),
                        "assembler_sha256": context["assembler_sha256"],
                        "maspsx_sha256": context["maspsx_sha256"],
                        "profile": "gcc281",
                        "profile_context": context["profiles"]["gcc281"],
                    })
                elif component.kind == "asm":
                    value.update({
                        "assembler_sha256": context["assembler_sha256"],
                        "splat_include_sha256": context["splat_include_sha256"],
                    })
                else:
                    value["objcopy_sha256"] = context["objcopy_sha256"]
                self.assertEqual(
                    build_incremental.component_signature(self.root, component, context),
                    build_incremental.digest_value(value),
                )
        self.assertEqual(
            context["splat_include_sha256"],
            self.expected_tree_digest(),
        )

    def expected_tree_digest(self) -> str:
        return build_incremental.digest_value([
            ("macro.inc", build_incremental.sha256(self.root / "tmp/splat/include/macro.inc"))
        ])

    def test_missing_source_or_nested_include_is_reported(self) -> None:
        source = self.write("src/example.c", '#include "first.h"\n')
        self.write("src/first.h", '#include "missing.h"\n')
        for path, name in ((source, "src/missing.h"), (self.root / "absent.c", "absent.c")):
            with self.subTest(path=path):
                with self.assertRaisesRegex(
                    build_incremental.IncrementalBuildError,
                    f"missing source/include: {name}",
                ):
                    build_incremental.include_digest(self.root, path)

    def test_escaping_includes_and_sources_are_rejected(self) -> None:
        outside = self.root.parent / "outside.h"
        outside.write_bytes(b"outside fixture")
        source = self.write("src/example.c", '#include "../../outside.h"\n')
        (self.root / "src/escape.h").symlink_to(outside)
        for include in ("../../outside.h", "escape.h"):
            with self.subTest(include=include):
                source.write_text(f'#include "{include}"\n', encoding="utf-8")
                with self.assertRaisesRegex(
                    build_incremental.IncrementalBuildError, "include escapes repository"
                ):
                    build_incremental.include_digest(self.root, source)
        component = build_incremental.Component("c", "../outside.h", "escape.o", "gcc281")
        with self.assertRaisesRegex(WorkspaceError, "path leaves the workspace"):
            build_incremental.component_signature(self.root, component, self.context())

    def test_missing_and_escaping_compiler_paths_are_rejected(self) -> None:
        (self.root.parent / "outside-gcc").write_bytes(b"outside fixture")
        for compiler in ("tools/missing-gcc", "../outside-gcc"):
            with self.subTest(compiler=compiler):
                profile = {**self.profiles["gcc281"], "compiler": compiler}
                with self.assertRaises(WorkspaceError):
                    build_incremental.dependency_context(self.root, {"test": profile})


class IncrementalBuildTests(WorkspaceTests):
    def setUp(self) -> None:
        super().setUp()
        self.component = build_incremental.Component(
            "c", "src/game/grouped.c", "grouped.o", "gcc281"
        )
        self.source = self.write(
            self.component.source,
            '#include "../types.h"\ns32 First(void) { return 1; }\n'
            "s32 Second(void) { return 2; }\n",
        )
        self.header = self.write("src/types.h", "typedef int s32;\n#define VALUE 1\n")
        self.object_bytes = b"object-one"
        self.patches.enter_context(patch.object(
            build_baseline, "load_compiler_profiles", return_value=self.profiles
        ))
        self.patches.enter_context(patch.object(
            build_incremental, "load_components", return_value=[self.component]
        ))
        self.compile = self.patches.enter_context(patch.object(
            build_baseline, "compile_c", side_effect=self.fake_compile
        ))
        self.link = self.patches.enter_context(patch.object(
            build_incremental, "link", side_effect=self.fake_link
        ))
        self.stdout = io.StringIO()
        self.patches.enter_context(redirect_stdout(self.stdout))

    def fake_compile(self, root, assembler, segment, profiles, *, use_splat_object_paths):
        self.assertEqual(root, self.root)
        self.assertEqual(assembler, self.assembler)
        self.assertEqual(segment, {
            "source": self.component.source,
            "object": self.component.object_name,
            "profile": self.component.profile,
        })
        self.assertIs(profiles, self.profiles)
        self.assertTrue(use_splat_object_paths)
        output = self.installed()
        self.assertFalse(output.exists())
        output.write_bytes(self.object_bytes)
        return output

    def fake_link(self, root, objects):
        self.assertEqual(root, self.root)
        self.assertEqual(objects, [self.installed()])
        self.assertTrue(all(path.is_file() for path in objects))
        return self.write(build_incremental.OUTPUT_EXE, b"linked fixture")

    def installed(self) -> Path:
        return build_incremental.object_path(self.root, self.component)

    def cached(self) -> Path:
        return build_incremental.cached_object_path(self.root, self.component)

    def build(self):
        return build_incremental.build_incrementally(self.root, seed=False)

    def reset_observations(self) -> None:
        self.compile.reset_mock()
        self.link.reset_mock()
        self.stdout.seek(0)
        self.stdout.truncate()

    def warm(self) -> None:
        self.build()
        self.reset_observations()

    def test_grouped_translation_unit_is_compiled_once_with_original_profile(self) -> None:
        self.assertEqual(self.build(), self.root / build_incremental.OUTPUT_EXE)
        self.compile.assert_called_once()
        self.link.assert_called_once()
        self.assertEqual(self.installed().read_bytes(), self.object_bytes)
        self.assertEqual(self.cached().read_bytes(), self.object_bytes)
        self.assertEqual(build_incremental.load_cache(self.root), {
            self.component.object_name: build_incremental.component_signature(
                self.root, self.component, self.context()
            )
        })
        self.assertIn(
            "rebuilt=1 reused=0 retained=0 materialized=0", self.stdout.getvalue()
        )

    def test_unchanged_installed_object_is_not_replaced_and_is_always_relinked(self) -> None:
        self.warm()
        output = self.installed()
        os.utime(output, ns=(123456789000, 123456789000))
        before = output.stat()
        with patch.object(
            build_incremental, "copy_object", wraps=build_incremental.copy_object
        ) as copy:
            self.build()
        copy.assert_not_called()
        self.compile.assert_not_called()
        self.link.assert_called_once()
        self.assertEqual(output.stat().st_ino, before.st_ino)
        self.assertEqual(output.stat().st_mtime_ns, before.st_mtime_ns)
        self.assertIn(
            "rebuilt=0 reused=1 retained=1 materialized=0", self.stdout.getvalue()
        )

    def test_changed_or_missing_installed_outputs_are_atomically_restored(self) -> None:
        self.warm()
        output = self.installed()
        for damage in ("same-size-and-mtime", "different-size", "missing"):
            with self.subTest(damage=damage):
                self.reset_observations()
                if damage == "same-size-and-mtime":
                    self.edit_preserving_metadata(output, b"object-two")
                elif damage == "different-size":
                    output.write_bytes(b"short")
                else:
                    output.unlink()
                with patch.object(
                    build_incremental, "copy_object", wraps=build_incremental.copy_object
                ) as copy:
                    self.build()
                copy.assert_called_once_with(self.cached(), output)
                self.compile.assert_not_called()
                self.link.assert_called_once()
                self.assertEqual(output.read_bytes(), self.object_bytes)
                self.assertNotEqual(output.stat().st_ino, self.cached().stat().st_ino)
                self.assertFalse(output.with_name(output.name + ".incremental.tmp").exists())
                self.assertIn(
                    "rebuilt=0 reused=1 retained=0 materialized=1", self.stdout.getvalue()
                )

    def test_invalid_signature_or_missing_or_empty_cached_object_forces_rebuild(self) -> None:
        self.warm()
        for damage in ("signature", "missing", "empty"):
            with self.subTest(damage=damage):
                self.reset_observations()
                if damage == "signature":
                    build_incremental.write_cache(self.root, {self.component.object_name: "old"})
                elif damage == "missing":
                    self.cached().unlink()
                else:
                    self.cached().write_bytes(b"")
                self.build()
                self.compile.assert_called_once()
                self.assertEqual(self.cached().read_bytes(), self.object_bytes)
                self.assertIn("rebuilt=1 reused=0", self.stdout.getvalue())

    def test_fresh_build_detects_same_size_same_mtime_dependency_edits(self) -> None:
        self.warm()
        for path in (self.source, self.header, self.compiler, self.filter):
            with self.subTest(path=path.relative_to(self.root)):
                previous = build_incremental.load_cache(self.root)
                self.edit_preserving_metadata(
                    path, path.read_bytes().replace(b"1", b"2", 1)
                )
                self.reset_observations()
                self.build()
                self.compile.assert_called_once()
                self.assertNotEqual(build_incremental.load_cache(self.root), previous)
                self.assertIn("rebuilt=1 reused=0", self.stdout.getvalue())

    def test_each_build_and_seed_uses_one_new_dependency_cache(self) -> None:
        with patch.object(
            build_incremental, "_FileCache", wraps=build_incremental._FileCache
        ) as cache:
            self.build()
            self.build()
            self.write(build_incremental.TARGET_PATH, b"linked fixture")
            self.assertIsNone(build_incremental.build_incrementally(self.root, seed=True))
        self.assertEqual(cache.call_count, 3)
        self.assertEqual(self.compile.call_count, 1)
        self.assertEqual(self.link.call_count, 2)
        self.assertIn("incremental cache seeded: 1 objects", self.stdout.getvalue())

    def test_failed_first_compile_does_not_create_a_cache_entry(self) -> None:
        self.compile.side_effect = build_baseline.BuildError("fixture compile failed")
        with self.assertRaisesRegex(build_baseline.BuildError, "fixture compile failed"):
            self.build()
        self.assertFalse((self.root / build_incremental.CACHE_PATH).exists())
        self.assertFalse(self.cached().exists())
        self.link.assert_not_called()
        self.assertEqual(self.stdout.getvalue(), "")

    def test_failed_recompile_does_not_record_the_new_signature(self) -> None:
        self.warm()
        previous = (self.root / build_incremental.CACHE_PATH).read_bytes()
        self.edit_preserving_metadata(
            self.source, self.source.read_bytes().replace(b"1", b"2", 1)
        )
        self.compile.side_effect = build_baseline.BuildError("fixture compile failed")
        with self.assertRaisesRegex(build_baseline.BuildError, "fixture compile failed"):
            self.build()
        self.assertEqual((self.root / build_incremental.CACHE_PATH).read_bytes(), previous)
        self.assertEqual(self.cached().read_bytes(), self.object_bytes)
        self.assertFalse(self.installed().exists())
        self.link.assert_not_called()
        self.assertEqual(self.stdout.getvalue(), "")
        self.compile.side_effect = self.fake_compile
        self.reset_observations()
        self.build()
        self.compile.assert_called_once()
        self.assertNotEqual((self.root / build_incremental.CACHE_PATH).read_bytes(), previous)

    def test_failed_cache_copy_preserves_previous_cache_and_signature(self) -> None:
        self.warm()
        previous = (self.root / build_incremental.CACHE_PATH).read_bytes()
        previous_object = self.cached().read_bytes()
        self.object_bytes = b"object-two"
        self.source.write_bytes(self.source.read_bytes().replace(b"1", b"2", 1))

        def fail_copy(source, destination):
            destination.write_bytes(b"partial")
            raise OSError("fixture copy failed")

        with patch.object(build_incremental.shutil, "copyfile", side_effect=fail_copy):
            with self.assertRaisesRegex(OSError, "fixture copy failed"):
                self.build()
        self.assertEqual((self.root / build_incremental.CACHE_PATH).read_bytes(), previous)
        self.assertEqual(self.cached().read_bytes(), previous_object)
        self.assertFalse(self.cached().with_name(self.cached().name + ".incremental.tmp").exists())
        self.link.assert_not_called()
        self.assertEqual(self.stdout.getvalue(), "")

    def test_failed_materialization_preserves_existing_output(self) -> None:
        self.warm()
        self.installed().write_bytes(b"object-two")

        def fail_copy(source, destination):
            destination.write_bytes(b"partial")
            raise OSError("fixture copy failed")

        with patch.object(build_incremental.shutil, "copyfile", side_effect=fail_copy):
            with self.assertRaisesRegex(OSError, "fixture copy failed"):
                self.build()
        self.assertEqual(self.installed().read_bytes(), b"object-two")
        self.assertEqual(self.cached().read_bytes(), self.object_bytes)
        temporary = self.installed().with_name(self.installed().name + ".incremental.tmp")
        self.assertFalse(temporary.exists())
        self.compile.assert_not_called()
        self.link.assert_not_called()
        self.assertEqual(self.stdout.getvalue(), "")

    def test_comparison_errors_are_not_silently_accepted(self) -> None:
        self.warm()
        with patch.object(
            build_incremental, "same_contents", side_effect=OSError("fixture read failed")
        ):
            with self.assertRaisesRegex(OSError, "fixture read failed"):
                self.build()
        self.compile.assert_not_called()
        self.link.assert_not_called()
        self.assertEqual(self.stdout.getvalue(), "")

    def test_failed_link_does_not_publish_success_or_finalize_cache(self) -> None:
        self.warm()
        self.link.side_effect = build_baseline.BuildError("fixture link failed")
        with patch.object(
            build_incremental, "write_cache", wraps=build_incremental.write_cache
        ) as write_cache:
            with self.assertRaisesRegex(build_baseline.BuildError, "fixture link failed"):
                self.build()
        write_cache.assert_not_called()
        self.assertEqual(self.stdout.getvalue(), "")
        self.link.side_effect = self.fake_link
        self.reset_observations()
        self.build()
        self.link.assert_called_once()
        self.compile.assert_not_called()

    def test_nonmatching_seed_does_not_replace_cache(self) -> None:
        self.warm()
        previous = (self.root / build_incremental.CACHE_PATH).read_bytes()
        self.write(build_incremental.TARGET_PATH, b"nonmatching fixture")
        with patch.object(
            build_incremental, "copy_object", wraps=build_incremental.copy_object
        ) as copy:
            with self.assertRaisesRegex(
                build_incremental.IncrementalBuildError, "nonmatching executable"
            ):
                build_incremental.build_incrementally(self.root, seed=True)
        copy.assert_not_called()
        self.compile.assert_not_called()
        self.link.assert_not_called()
        self.assertEqual((self.root / build_incremental.CACHE_PATH).read_bytes(), previous)
        self.assertEqual(self.stdout.getvalue(), "")


class ObjectComparisonTests(WorkspaceTests):
    def test_comparison_checks_beyond_the_first_block(self) -> None:
        contents = b"a" * (1024 * 1024) + b"tail1"
        source = self.write("tmp/cached.o", contents)
        destination = self.write("tmp/installed.o", contents)
        self.assertTrue(build_incremental.same_contents(source, destination))
        self.edit_preserving_metadata(destination, contents[:-1] + b"2")
        self.assertFalse(build_incremental.same_contents(source, destination))
        self.assertEqual(source.read_bytes(), contents)
        destination.unlink()
        self.assertFalse(build_incremental.same_contents(source, destination))


if __name__ == "__main__":
    unittest.main()
