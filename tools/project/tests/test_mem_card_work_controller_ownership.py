"""Real compiler contracts and shared native backing for the card work UI."""

from __future__ import annotations

import contextlib
import io
import json
import os
from pathlib import Path
import platform
import shutil
import subprocess
import sys
import unittest
import uuid


ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))
from build_baseline import compile_c, tool


GCC = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
CC = shutil.which("cc")
RESULT_VIEW = "#define MEM_CARD_FIND_LOADED_ENTRY_RESULT_VIEW\n"
ABSOLUTE = "#define MEM_CARD_DIRECTORY_IN_DATA\n"
START = """
.text
.globl _start
_start:
    andl $-16, %esp
    call main
    movl %eax, %ebx
    movl $1, %eax
    int $0x80
.section .note.GNU-stack,"",@progbits
"""
REQUESTS = (
    ("MemCard_ReqLoadDirectory", "s32", "s32", "0"),
    ("MemCard_ReqReadFile", "s32", "s32, s32, s32, s32, s32", "0, 0, 0, 0, 0"),
    ("MemCard_ReqWriteFile", "s32", "s32, s32, s32, s32, s32", "0, 0, 0, 0, 0"),
    ("MemCard_ReqReadSector", "s32", "s32, s32, s32", "0, 0, 0"),
    ("MemCard_ReqWriteSector", "s32", "s32, s32, s32", "0, 0, 0"),
    ("MemCard_ReqCreateFile", "s32", "s32, s32, s32", "0, 0, 0"),
    ("func_80044838", "s32", "s32, s32 *, s32 *", "0, 0, 0"),
)


class MemCardWorkControllerOwnershipTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        self.scratch = ROOT / "tmp" / f"test-card-work-ownership-{uuid.uuid4().hex}"
        self.scratch.mkdir(parents=True)
        self.addCleanup(shutil.rmtree, self.scratch)
        self.environment = os.environ.copy()
        self.environment["TMPDIR"] = str(self.scratch)

    def command(self, arguments: list[str]) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            arguments, cwd=ROOT, env=self.environment, capture_output=True,
            text=True, errors="replace", timeout=60,
        )

    def probe(self, body: str, *, accepted: bool, diagnostic: str = "") -> None:
        source = self.scratch / "probe.c"
        source.write_text(body)
        compilers = []
        if GCC.is_file():
            compilers.append([str(GCC), "-S", "-O2", "-G8", "-Werror"])
        if CC:
            compilers.append([
                CC, "-S", "-m32", "-std=gnu99", "-D_LANGUAGE_C",
                "-Werror=incompatible-pointer-types", "-Werror=int-conversion",
                "-Werror=implicit-function-declaration",
            ])
        if not compilers:
            self.skipTest("needs GCC 2.8.1 or native C compiler")
        for compiler in compilers:
            with self.subTest(compiler=compiler[0], body=body):
                result = self.command(compiler + [
                    "-Wimplicit-function-declaration", "-I", str(ROOT / "src/game"),
                    "-o", str(self.scratch / "probe.s"), str(source),
                ])
                if accepted:
                    self.assertEqual(result.returncode, 0, result.stderr)
                else:
                    self.assertNotEqual(result.returncode, 0, result.stderr)
                    self.assertRegex(result.stderr, diagnostic)

    def test_request_owners_preserve_word_and_pointer_arguments(self) -> None:
        for symbol, result, args, call in REQUESTS:
            with self.subTest(symbol=symbol):
                owner = '#include "mem_card.h"\n'
                self.probe(
                    owner + f"{result} (*checked)({args}) = {symbol};",
                    accepted=True,
                )
                self.probe(
                    owner + f"void (*checked)({args}) = {symbol};",
                    accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
                )
                self.probe(
                    owner + f"void run(void) {{ {symbol}({call}); }}",
                    accepted=True,
                )
                self.probe(
                    owner + f"void run(void) {{ {symbol}(); }}",
                    accepted=False, diagnostic="too few arguments",
                )
                self.probe(
                    '#include "../types.h"\n'
                    + f"{result} (*checked)({args}) = {symbol};",
                    accepted=False, diagnostic="undeclared",
                )
        self.probe(
            '#include "mem_card.h"\n'
            "s32 (*file)(s32, u8 *, u8 *, s32, s32) = MemCard_ReqReadFile;",
            accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
        )
        self.probe(
            '#include "mem_card.h"\n'
            "s32 (*poll)(s32, s16 *, s32 *) = func_80044838;",
            accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
        )

    def test_event_initializers_keep_long_and_void_prototypes(self) -> None:
        header = '#include "io_event_helpers.h"\n'
        self.probe(
            header + "void (*init)(long) = MemCard_Init;\n"
            "void (*events)(void) = MemCard_InitIOEvents;", accepted=True,
        )
        self.probe(
            header + "void (*init)(s32) = MemCard_Init;", accepted=False,
            diagnostic="incompatible pointer|incompatible-pointer",
        )
        self.probe(
            header + "void run(void) { MemCard_InitIOEvents(1); }",
            accepted=False, diagnostic="too many arguments",
        )

    def test_directory_views_keep_actual_sdk_record_and_absolute_types(self) -> None:
        for prefix in ("", ABSOLUTE):
            self.probe(
                prefix + '#include "mem_card_directory.h"\n'
                "struct DIRENTRY **entries = &gMemCard_pDirEntries;\n"
                "s32 *count = &gMemCard_nDirEntries;\n"
                "s32 *free_blocks = &gMemCard_nFreeBlocks;\n"
                "typedef char size[sizeof(struct DIRENTRY) == 40 ? 1 : -1];\n"
                "typedef char head[(u32)&((struct DIRENTRY *)0)->head == 32 ? 1 : -1];",
                accepted=True,
            )
        self.probe(
            ABSOLUTE + '#include "mem_card_directory.h"\n'
            "u8 *free_blocks = &gMemCard_nFreeBlocks;",
            accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
        )

    def test_inherited_loaded_entry_return_views_are_explicit(self) -> None:
        for prefix, result, wrong in (
            ("", "void", "s32"), (RESULT_VIEW, "s32", "void"),
        ):
            header = prefix + '#include "mem_card_directory.h"\n'
            self.probe(
                header + f"{result} (*find)(u8 *) = MemCard_FindLoadedEntry;",
                accepted=True,
            )
            self.probe(
                header + f"{wrong} (*find)(u8 *) = MemCard_FindLoadedEntry;",
                accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
            )
        self.probe(
            '#include "mem_card_directory.h"\n'
            "s32 run(u8 *name) { return MemCard_FindLoadedEntry(name); }",
            accepted=False, diagnostic="void value|void expression",
        )
        self.probe(
            RESULT_VIEW + '#include "mem_card_directory.h"\n'
            "s32 run(u8 *name) { return MemCard_FindLoadedEntry(name); }",
            accepted=True,
        )

    def test_work_area_and_frame_have_complete_bounded_views(self) -> None:
        header = '#include "mem_card_work.h"\n'
        self.probe(
            header + "MemCardWorkArea D_800EF6D0;\n"
            "typedef char size[sizeof(D_800EF6D0) == 0x4EC ? 1 : -1];\n"
            "typedef char frame[sizeof(MEM_CARD_WORK_FRAME_BYTES) == 128 ? 1 : -1];\n"
            "MemCardWorkSlot (*slots)[2] = &D_800EF6D0.slots;\n"
            "u8 (*bytes)[128] = &D_80210000;",
            accepted=True,
        )
        for body, diagnostic in (
            ("u8 D_800EF6D0[0x1C];", "conflicting types"),
            ("u8 D_80210000[1];", "conflicting types"),
            ("MemCardWorkSlot (*slots)[1] = &D_800EF6D0.slots;",
             "incompatible pointer|incompatible-pointer"),
        ):
            self.probe(header + body, accepted=False, diagnostic=diagnostic)

    def test_callback_and_controller_contracts(self) -> None:
        header = '#include "mem_card_work.h"\n'
        self.probe(
            header
            + "void (*(*callbacks)[5])(MemCardWorkRoot *, MemCardWorkSlot *) = &D_80090F88;\n"
            "void (*controller)(void) = func_8003DC1C;",
            accepted=True,
        )
        self.probe(
            header + "void (*(*callbacks)[5])(MemCardWorkRoot *) = &D_80090F88;",
            accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
        )
        self.probe(
            header + "void run(void) { func_8003DC1C(1); }",
            accepted=False, diagnostic="too many arguments",
        )

    def build_object(self, path: Path, name: str, profile: str) -> Path:
        profiles = json.loads(
            (ROOT / "config/slus_01411/compiler_profiles.json").read_text()
        )["profiles"]
        with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            return compile_c(
                ROOT, tool(ROOT, "as"),
                {"source": str(path.relative_to(ROOT)), "profile": profile,
                 "object": name + ".o"}, profiles,
                object_directory=str(self.scratch.relative_to(ROOT)),
                asm_directory=str(self.scratch.relative_to(ROOT)),
            )

    @unittest.skipUnless(GCC.is_file(), "needs GCC 2.8.1")
    def test_directory_absolute_arm_changes_relocations_not_width(self) -> None:
        for prefix, absolute in (("", False), (ABSOLUTE, True)):
            with self.subTest(absolute=absolute):
                path = self.scratch / "directory.c"
                path.write_text(
                    prefix + f'#include "{ROOT / "src/game/mem_card_directory.h"}"\n'
                    "s32 count(void) { return gMemCard_nDirEntries; }\n"
                    "u8 free_blocks(void) { return *(u8 *)&gMemCard_nFreeBlocks; }\n"
                    "struct DIRENTRY *entries(void) { return gMemCard_pDirEntries; }\n"
                )
                obj = self.build_object(path, "directory", "gcc_2_8_1_g8_split")
                result = self.command([str(tool(ROOT, "objdump")), "-dr", str(obj)])
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertIn("lbu", result.stdout)
                for symbol in ("gMemCard_nDirEntries", "gMemCard_nFreeBlocks",
                               "gMemCard_pDirEntries"):
                    kind = "R_MIPS_HI16" if absolute else "R_MIPS_GPREL16"
                    self.assertRegex(result.stdout, kind + r"\s+" + symbol)
                    if absolute:
                        self.assertRegex(result.stdout, r"R_MIPS_LO16\s+" + symbol)

    @unittest.skipUnless(GCC.is_file(), "needs GCC 2.8.1")
    def test_void_definition_still_leaves_the_mips_result_register(self) -> None:
        path = ROOT / "src/game/mem_card_find_loaded_entry.c"
        self.assertIn("void MemCard_FindLoadedEntry(u8 *name)", path.read_text())
        self.build_object(path, "loaded-entry", "gcc_2_8_1_g8")
        assembly = (self.scratch / "loaded-entry.o.compiler.s").read_text()
        self.assertIn("jal\tMemCard_FindEntry", assembly)
        after_call = assembly.split("jal\tMemCard_FindEntry", 1)[1]
        self.assertNotRegex(after_call, r"(?m)^\s*\w+\s+\$2\s*,")
        self.assertNotIn("MEM_CARD_FIND_LOADED_ENTRY_RESULT_VIEW", path.read_text())

    def test_existing_work_backing_and_callback_table_boundaries(self) -> None:
        import yaml

        segments = yaml.safe_load(
            (ROOT / "config/slus_01411/split.yaml").read_text()
        )["segments"]
        backing = next(s for s in segments if isinstance(s, dict)
                       and s.get("name") == "bss_image_after_viewport")
        self.assertLessEqual(backing["vram"], 0x800EF6D0)
        self.assertEqual(0x800EF6D0 + 0x1C + 2 * 0x268, 0x800EFBBC)
        self.assertLess(0x800EFBBC, 0x800EFBC0)
        self.assertEqual(0x80090F88 + 5 * 4, 0x80090F9C)
        initializer = (ROOT / "src/game/func_8003D300.c").read_text()
        self.assertIn("D_8009B3DD = 0;", initializer)
        self.assertIn('#include "mem_card_work.h"', initializer)

    def native_frame(self, wrong_checksum_alias: bool) -> int:
        if not (
            CC and platform.system() == "Linux"
            and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
        ):
            self.skipTest("needs native Linux x86 ILP32 execution")
        source = self.scratch / "frame.c"
        source.write_text(
            '#include "mem_card_work.h"\n'
            'u8 D_80210000[128] __attribute__((section(".card_frame"))) = {0};\n'
            'u8 guard[4] __attribute__((section(".card_guard"))) = {0xA5, 0x5A, 0xA5, 0x5A};\n'
            "MemCardWorkArea D_800EF6D0;\n"
            "int main(void) {\n"
            " s32 i; s32 sum = 0;\n"
            " if (sizeof(void *) != 4) return 1;\n"
            " if ((u32)D_80210000 != MEM_CARD_WORK_FRAME_ADDRESS) return 2;\n"
            " for (i = 0; i < 122; i++) MEM_CARD_WORK_FRAME_BYTES[i] = i;\n"
            " D_8021007A = 255; D_8021007B = 255;\n"
            " D_8021007C = 255; D_8021007D = 255; D_8021007E = 1;\n"
            " for (i = 0; i < 127; i++) sum ^= MEM_CARD_WORK_FRAME_BYTES[i];\n"
            " D_8021007F = sum;\n"
            " for (i = 122; i < 126; i++) if (D_80210000[i] != 255) return 3;\n"
            " if (D_80210000[126] != 1 || D_80210000[127] != (u8)sum) return 4;\n"
            " if (guard[0] != 0xA5 || guard[1] != 0x5A) return 7;\n"
            " D_800EF6D0.root.remaining = -1;\n"
            " D_800EF6D0.slots[0].free_blocks = 15;\n"
            " D_800EF6D0.slots[1].entry_index = 14;\n"
            " if ((u32)&D_800EF6D0.slots[1] - (u32)&D_800EF6D0 != 0x284) return 8;\n"
            " if (D_800EF6D0.slots[0].free_blocks != 15 ||\n"
            "     D_800EF6D0.root.remaining != -1) return 9;\n"
            " return 0;\n}\n"
        )
        startup = self.scratch / "start.S"
        startup.write_text(START)
        executable = self.scratch / "frame"
        aliases = []
        for offset in range(0x7A, 0x80):
            target = 0x80 if wrong_checksum_alias and offset == 0x7F else offset
            aliases.append(f"-Wl,--defsym,D_802100{offset:02X}=D_80210000+{target}")
        result = self.command([
            CC, "-m32", "-std=gnu99", "-O2", "-nostdlib", "-fno-pie", "-no-pie",
            "-ffreestanding", "-fno-builtin", "-fno-stack-protector",
            "-fno-strict-aliasing", "-I", str(ROOT / "src/game"),
            str(source), str(startup), *aliases,
            "-Wl,--section-start=.card_frame=0x80210000",
            "-Wl,--section-start=.card_guard=0x80210080",
            "-o", str(executable),
        ])
        self.assertEqual(result.returncode, 0, result.stderr)
        return self.command([str(executable)]).returncode

    def test_native_frame_scalars_and_checksum_share_literal_address_backing(self) -> None:
        self.assertEqual(self.native_frame(wrong_checksum_alias=False), 0)

    def test_native_checksum_alias_cannot_escape_128_byte_frame(self) -> None:
        self.assertEqual(self.native_frame(wrong_checksum_alias=True), 7)


if __name__ == "__main__":
    unittest.main()
