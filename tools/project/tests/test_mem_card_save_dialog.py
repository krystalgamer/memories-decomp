"""Save-dialog declaration controls and bounded, non-hardware state witnesses.

The MIPS controls use the real GCC 2.8.1 frontend. The behavior witnesses use
the complete promoted source with project headers, ILP32 layouts, and opaque
stubs in another translation unit; they do not emulate asynchronous card IO.
Each optimization runs 691 state-machine calls. Mutants prove sensitivity to
free-space accounting, the completion message, and undefined signed overflow.
"""

from __future__ import annotations

import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import unittest
import uuid


REPOSITORY = Path(__file__).resolve().parents[3]
SOURCE = REPOSITORY / "src/game/mem_card_dialog_update_save.c"
COMPILER = REPOSITORY / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
WIDE = "#define MEM_CARD_DIALOG_MESSAGE_WIDE\n"


class ScratchTest(unittest.TestCase):
    def setUp(self):
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        self.scratch = REPOSITORY / "tmp" / f"test-save-dialog-{uuid.uuid4().hex}"
        self.scratch.mkdir(parents=True)
        self.addCleanup(shutil.rmtree, self.scratch)
        self.environment = os.environ.copy()
        self.environment["TMPDIR"] = str(self.scratch)


@unittest.skipUnless(COMPILER.is_file(), "needs the GCC 2.8.1 toolchain")
class MemCardSaveDialogHeaderTests(ScratchTest):
    def compile(self, text=None):
        source = SOURCE
        if text is not None:
            source = self.scratch / "probe.c"
            source.write_text(text)
        return subprocess.run(
            [
                str(COMPILER), "-S", "-O2", "-G8", "-Werror",
                "-Wimplicit-function-declaration", "-I", str(REPOSITORY),
                str(source), "-o", str(self.scratch / "probe.s"),
            ],
            cwd=REPOSITORY, env=self.environment, capture_output=True,
            text=True, errors="replace", timeout=30,
        )

    def test_actual_source_compiles_with_owner_headers(self):
        result = self.compile()
        self.assertEqual(result.returncode, 0, result.stderr)
        for parameters, accepted in (("s32, s32", True), ("u8, u16", False)):
            with self.subTest(parameters=parameters):
                result = self.compile(
                    f'#include "{SOURCE}"\n'
                    f"void (*checked)({parameters}) = MemCardDialog_SetMessage;\n"
                )
                if accepted:
                    self.assertEqual(result.returncode, 0, result.stderr)
                else:
                    self.assertNotEqual(result.returncode, 0, result.stderr)
                    self.assertIn("incompatible pointer type", result.stderr)

    def test_default_and_wide_message_views_accept_correct_pointers(self):
        for prefix, parameters in (("", "u8, u16"), (WIDE, "s32, s32")):
            with self.subTest(wide=bool(prefix)):
                result = self.compile(
                    prefix + '#include "src/game/mem_card.h"\n'
                    f"void (*checked)({parameters}) = MemCardDialog_SetMessage;\n"
                )
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_opposite_message_views_fail_with_effective_werror(self):
        for prefix, parameters in (("", "s32, s32"), (WIDE, "u8, u16")):
            with self.subTest(wide=bool(prefix)):
                result = self.compile(
                    prefix + '#include "src/game/mem_card.h"\n'
                    f"void (*checked)({parameters}) = MemCardDialog_SetMessage;\n"
                )
                self.assertNotEqual(result.returncode, 0, result.stderr)
                self.assertIn("incompatible pointer type", result.stderr)

    def test_message_views_enforce_argument_count(self):
        for prefix in ("", WIDE):
            for arguments in ("", "1", "1, 2", "1, 2, 3"):
                with self.subTest(wide=bool(prefix), arguments=arguments):
                    result = self.compile(
                        prefix + '#include "src/game/mem_card.h"\n'
                        f"void checked(void) {{ MemCardDialog_SetMessage({arguments}); }}\n"
                    )
                    if arguments == "1, 2":
                        self.assertEqual(result.returncode, 0, result.stderr)
                    else:
                        self.assertNotEqual(result.returncode, 0, result.stderr)
                        self.assertRegex(result.stderr, r"too (?:few|many) arguments")

    def test_missing_owners_reject_implicit_calls(self):
        for owner, symbol, arguments in (
            ("mem_card.h", "MemCardDialog_SetMessage", "1, 2"),
            ("mem_card_dialog_load_save.h", "MemCardDialog_UpdateSave", ""),
        ):
            for prefix in ("", WIDE):
                with self.subTest(symbol=symbol, wide=bool(prefix)):
                    call = f"void checked(void) {{ {symbol}({arguments}); }}\n"
                    result = self.compile(prefix + f'#include "src/game/{owner}"\n' + call)
                    self.assertEqual(result.returncode, 0, result.stderr)
                    result = self.compile(prefix + '#include "src/types.h"\n' + call)
                    self.assertNotEqual(result.returncode, 0, result.stderr)
                    self.assertIn(f"implicit declaration of function `{symbol}'", result.stderr)

    def test_update_save_owner_has_void_prototype(self):
        prefix = '#include "src/game/mem_card_dialog_load_save.h"\n'
        result = self.compile(prefix + "void (*checked)(void) = MemCardDialog_UpdateSave;\n")
        self.assertEqual(result.returncode, 0, result.stderr)
        for body in (
            "s32 (*checked)(void) = MemCardDialog_UpdateSave;\n",
            "void checked(void) { MemCardDialog_UpdateSave(1); }\n",
        ):
            result = self.compile(prefix + body)
            self.assertNotEqual(result.returncode, 0, result.stderr)
            self.assertRegex(result.stderr, r"incompatible pointer type|too many arguments")


START = r"""
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

WITNESS = r"""
#define MEM_CARD_DIALOG_MESSAGE_WIDE
#define GDIALOG_CHOICE_IN_DATA
#include "src/types.h"
#include "src/psyq/libmcrd.h"
#include "src/game/mem_card.h"
#include "src/game/mem_card_directory.h"
#include "src/game/mem_card_dialog_load_save.h"
#include "src/game/file_transfer.h"
#include "src/game/save_data.h"
#include "src/game/text_staging.h"
#include "src/game/dialog_choice.h"
#include "src/unmatched.h"

typedef char ilp32_required[
    sizeof(void *) == 4 && sizeof(long) == 4 && sizeof(s32) == 4 ? 1 : -1
];

u8 D_8009B3EB, D_8009B3F9, D_8009B3EC, D_8009B3D4, D_8009B3DC, D_8009B3EF;
u16 D_8009B3C2, D_8009B3C4, gMemCard_wDialogFlags;
s32 D_8009B3F0, D_8009B3F4, D_801D5648[1], gSaveDataSequence;
s8 gDialog_bChoice;
u8 D_8009AF70[4], D_800EFE18[32];
u8 D_800EFBC0[MEM_CARD_DIRECTORY_ENTRY_SIZE * MEM_CARD_BLOCK_COUNT];
u8 gLibrary_aCardArtRecord[0x480];
TextStagingValues D_801D5608[1];
static u32 payload[0x1000 / sizeof(u32)];
u8 *gMemCard_pPrimaryTransferCursor;

enum { MESSAGE = 1, ACCEPT, DIRECTORY, FIND, FREE, CREATE, READ, WRITE, FORMAT, MATCH };
static struct {
    s32 kind, a, b;
    void *pointer;
} events[16];
static s32 event_count, stub_error, directory_result, file_count, find_result;
static s32 free_result, create_result, format_result, match_result, accept_waits;
static u32 cases;

static void event(s32 kind, s32 a, s32 b, void *pointer)
{
    if (event_count == 16) { stub_error = 1; return; }
    events[event_count].kind = kind;
    events[event_count].a = a;
    events[event_count].b = b;
    events[event_count].pointer = pointer;
    event_count++;
}

void MemCardDialog_SetMessage(s32 value, s32 bits)
{
    event(MESSAGE, value, bits, 0);
}

long MemCardAccept(long channel)
{
    if (channel != 0) stub_error = 1;
    event(ACCEPT, 0, 0, 0);
    return accept_waits-- > 0 ? 0 : 1;
}

long MemCardGetDirentry(long channel, char *pattern, struct DIRENTRY *directory,
                       long *files, long offset, long maximum)
{
    if (channel || pattern != (char *)D_8009AF70 ||
        directory != (struct DIRENTRY *)D_800EFBC0 || offset || maximum != 15)
        stub_error = 1;
    *files = file_count;
    event(DIRECTORY, 0, 0, directory);
    return directory_result;
}

s32 MemCard_FindEntry(u8 *name, struct DIRENTRY *directory, s32 count)
{
    if (name != D_800EFE18 || directory != (struct DIRENTRY *)D_800EFBC0 ||
        count != file_count) stub_error = 1;
    event(FIND, count, 0, directory);
    return find_result;
}

s32 MemCard_CalcFreeBlocks(struct DIRENTRY *directory, s32 count)
{
    if (directory != (struct DIRENTRY *)D_800EFBC0 || count != file_count)
        stub_error = 1;
    event(FREE, count, 0, directory);
    return free_result;
}

long MemCardCreateFile(long channel, char *name, long blocks)
{
    if (channel || name != (char *)D_800EFE18 || blocks != D_8009B3DC)
        stub_error = 1;
    event(CREATE, blocks, 0, name);
    return create_result;
}

long MemCardReadFile(long channel, char *name, unsigned long *buffer,
                     long offset, long bytes)
{
    if (channel || name != (char *)D_800EFE18 ||
        buffer != (unsigned long *)gLibrary_aCardArtRecord) stub_error = 1;
    event(READ, offset, bytes, buffer);
    return -37;
}

long MemCardWriteFile(long channel, char *name, unsigned long *buffer,
                      long offset, long bytes)
{
    if (channel || name != (char *)D_800EFE18) stub_error = 1;
    event(WRITE, offset, bytes, buffer);
    return -53;
}

long MemCardFormat(long channel)
{
    if (channel) stub_error = 1;
    event(FORMAT, 0, 0, 0);
    return format_result;
}

s32 SaveData_MatchesDuelistAndCurrentSequence(SaveDataState *left, SaveDataState *right)
{
    if (left != (SaveDataState *)gMemCard_pPrimaryTransferCursor ||
        right != (SaveDataState *)gLibrary_aCardArtRecord) stub_error = 1;
    event(MATCH, 0, 0, 0);
    return match_result;
}

static void reset(s32 state)
{
    D_8009B3EB = state;
    D_8009B3F9 = 0xB0;
    D_8009B3EC = 0;
    D_8009B3D4 = 1;
    D_8009B3DC = 3;
    D_8009B3EF = 0x55;
    D_8009B3C2 = 0x680;
    D_8009B3C4 = 0x200;
    D_8009B3F0 = 2;
    D_8009B3F4 = 0;
    D_801D5648[0] = -111;
    D_801D5608[0].blocks.used = -222;
    D_801D5608[0].blocks.needed = -333;
    gDialog_bChoice = 0;
    gSaveDataSequence = 41;
    gMemCard_wDialogFlags = 0xA042;
    gMemCard_pPrimaryTransferCursor = (u8 *)payload + 0x200;
    directory_result = 0;
    file_count = 7;
    find_result = -1;
    free_result = 3;
    create_result = format_result = match_result = accept_waits = 0;
    event_count = stub_error = 0;
}

static void step(void)
{
    event_count = 0;
    MemCardDialog_UpdateSave();
    cases++;
}

static s32 message(s32 index, s32 id, s32 bits)
{
    return events[index].kind == MESSAGE && events[index].a == id &&
        events[index].b == bits;
}

/* Report the failing C line without requiring a host C runtime. */
static void report(u32 value)
{
    char buffer[16];
    u32 size = 1;
    buffer[15] = '\n';
    do {
        buffer[15 - size++] = '0' + value % 10;
        value /= 10;
    } while (value);
    __asm__ volatile("int $0x80" : : "a"(4), "b"(1),
                     "c"(buffer + 16 - size), "d"(size) : "memory");
}

#define CHECK(condition) do { if (!(condition)) return __LINE__; } while (0)

static s32 directory_and_completion(void)
{
    static const u32 sequences[] = {0, 41, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu};
    s32 free, flags, needed, sequence;
    for (flags = 0; flags < 2; flags++) {
        for (needed = 1; needed <= 15; needed += 7) {
            for (free = 0; free <= 15; free++) {
                reset(3);
                gMemCard_wDialogFlags |= flags * 0x100;
                D_8009B3DC = needed;
                free_result = free;
                step();
                CHECK(!stub_error);
                CHECK(gSaveDataSequence == 41 && D_8009B3EF == 0x55);
                CHECK(events[0].kind == DIRECTORY && events[1].kind == FIND &&
                      events[2].kind == FREE);
                if (free < needed) {
                    CHECK(event_count == 4 && message(3, 0xDB, 0x18));
                    CHECK(D_8009B3EB == 3 && D_801D5608[0].blocks.used == 15 - free &&
                          D_801D5608[0].blocks.needed == needed);
                    CHECK(gMemCard_wDialogFlags == (0xA042 | flags * 0x100));
                    CHECK(gMemCard_pPrimaryTransferCursor == (u8 *)payload + 0x200);
                    CHECK(D_8009B3C4 == 0x200 && D_8009B3C2 == 0x680);
                } else {
                    CHECK(event_count == 6 && events[3].kind == CREATE &&
                          message(4, 0xD6, 0) && events[5].kind == WRITE);
                    CHECK(D_8009B3EB == 0x88);
                    CHECK(gMemCard_wDialogFlags == (0xB042 | flags * 0x100));
                    CHECK(events[5].pointer == (u8 *)payload + (flags ? 0x200 : 0));
                    CHECK(events[5].a == (flags ? 0x200 : 0));
                    CHECK(events[5].b == (flags ? 0x680 : 0x880));
                    CHECK(D_801D5608[0].blocks.used == -222 &&
                          D_801D5608[0].blocks.needed == -333);
                    step();
                    CHECK(D_8009B3EB == 10 && event_count == 0);
                }
                /* Alternate states 3 and 10 without resetting their globals.
                   The state-10 message must not inherit a free-block result. */
                for (sequence = 0; sequence < 5; sequence++) {
                    D_8009B3EB = 10;
                    D_8009B3D4 = 7;
                    gSaveDataSequence = (s32)sequences[sequence];
                    step();
                    CHECK(event_count == 1 && message(0, flags ? 0xCC : 0xD1, 0x18));
                    CHECK(D_8009B3EF == 1 && D_8009B3EB == 10);
                    CHECK((u32)gSaveDataSequence == sequences[sequence] + (flags ? 0u : 1u));
                    CHECK(D_8009B3D4 == (flags ? 7 : 0));
                    CHECK(gMemCard_wDialogFlags ==
                          ((free >= needed ? 0xB042 : 0xA042) | flags * 0x100));
                    CHECK(D_801D5608[0].blocks.used == (free < needed ? 15 - free : -222));
                    CHECK(D_801D5608[0].blocks.needed == (free < needed ? needed : -333));
                    CHECK(!stub_error);
                }
            }
        }
    }
    return 0;
}

static s32 directory_errors(void)
{
    reset(3);
    directory_result = 1;
    step();
    CHECK(D_8009B3EB == 13 && event_count == 1);
    reset(3);
    find_result = 0;
    step();
    CHECK(D_8009B3EB == 7 && event_count == 2);
    reset(3);
    D_8009B3D4 = 0;
    step();
    CHECK(D_8009B3EB == 14 && event_count == 2);
    reset(3);
    create_result = 1;
    step();
    CHECK(D_8009B3EB == 11 && event_count == 4 && events[3].kind == CREATE);
    CHECK(gMemCard_pPrimaryTransferCursor == (u8 *)payload + 0x200 &&
          D_8009B3C4 == 0x200 && D_8009B3C2 == 0x680);
    CHECK(gMemCard_wDialogFlags == 0xA042 && !stub_error);
    return 0;
}

static s32 acceptance_and_format(void)
{
    static const s32 outcomes[] = {3, 9, 13, 3, 4};
    s32 result, permission, choice;
    reset(0);
    step();
    CHECK(D_8009B3EB == 0x80 && D_801D5648[0] == 12 &&
          event_count == 1 && message(0, 0xC9, 0x20));
    step();
    CHECK(D_8009B3EB == 0x81 && event_count == 2 &&
          message(0, 0xD4, 0) && events[1].kind == ACCEPT);
    CHECK(gMemCard_wDialogFlags == 0xB042);
    reset(0x80);
    gDialog_bChoice = -1;
    step();
    CHECK(D_8009B3EB == 12 && event_count == 0);
    reset(1);
    accept_waits = 2;
    step();
    CHECK(D_8009B3EB == 0x81 && event_count == 4 &&
          message(0, 0xD4, 0) && events[3].kind == ACCEPT && !stub_error);
    for (permission = 0; permission < 3; permission++) {
        for (result = 0; result < 5; result++) {
            reset(0x81);
            D_8009B3D4 = permission == 1;
            if (permission == 2) gMemCard_wDialogFlags |= 0x100;
            D_8009B3F4 = result;
            step();
            CHECK(D_8009B3EB == (result == 4 && !permission ? 14 : outcomes[result]));
            CHECK(event_count == 0);
        }
    }
    for (choice = 0; choice < 2; choice++) {
        reset(4);
        step();
        CHECK(D_8009B3EB == 0x84 && event_count == 1 && message(0, 0xDE, 0x10));
        step();
        CHECK(D_8009B3EB == 0xC4 && event_count == 1 && message(0, 0xDF, 0x20));
        gDialog_bChoice = choice;
        step();
        CHECK(D_8009B3EB == (choice ? 5 : 12));
        CHECK(choice ? event_count == 1 && message(0, 0xBE, 0) : event_count == 0);
    }
    reset(5);
    step();
    CHECK(D_8009B3EB == 0x85 && event_count == 1 && events[0].kind == ACCEPT);
    CHECK(gMemCard_wDialogFlags == 0xB042);
    D_8009B3F0 = 1;
    step();
    CHECK(D_8009B3EB == 0x85 && event_count == 0);
    for (result = 0; result < 5; result++) {
        reset(0x85);
        D_8009B3F4 = result;
        step();
        if (result == 0 || result == 3) {
            CHECK(D_8009B3EB == 0x85 && event_count == 1 && message(0, 0xC3, 0x18));
        } else if (result == 4) {
            CHECK(D_8009B3EB == 0xC5 && event_count == 2 &&
                  events[0].kind == FORMAT && message(1, 0xBF, 0x10));
            step();
            CHECK(D_8009B3EB == 1 && event_count == 0);
        } else {
            CHECK(D_8009B3EB == (result == 1 ? 9 : 13) && event_count == 0);
        }
    }
    reset(0x85);
    D_8009B3F4 = 4;
    format_result = 1;
    step();
    CHECK(D_8009B3EB == 0xC5 && event_count == 2 &&
          events[0].kind == FORMAT && message(1, 0xDD, 0x18) && !stub_error);
    return 0;
}

static s32 read_verify_and_write(void)
{
    s32 result, choice, retry, permission;
    reset(7);
    D_8009B3EC = 9;
    step();
    CHECK(D_8009B3EB == 0x87 && D_8009B3EC == 0 && event_count == 1);
    CHECK(events[0].kind == READ && events[0].a == 0x200 && events[0].b == 0x480);
    CHECK(gMemCard_wDialogFlags == 0xB042 && !stub_error);
    for (result = 1; result < 5; result++) {
        reset(0x87);
        D_8009B3F4 = result;
        step();
        CHECK(D_8009B3EB == (result == 1 ? 9 : 13) && event_count == 0);
    }
    for (permission = 0; permission < 2; permission++) {
        for (result = 0; result < 2; result++) {
            for (retry = 0; retry < 2; retry++) {
                reset(0x87);
                D_8009B3D4 = permission;
                D_8009B3EC = retry;
                match_result = result;
                step();
                CHECK(!stub_error);
                if (permission || result) {
                    CHECK(D_8009B3EB == 0xC7 && event_count == (permission ? 1 : 2));
                    CHECK(message(event_count - 1, 0xCF, 0x20));
                } else if (retry) {
                    CHECK(D_8009B3EB == 14 && event_count == 1 && events[0].kind == MATCH);
                } else {
                    CHECK(D_8009B3EB == 0x87 && D_8009B3EC == 1 && event_count == 2);
                    CHECK(events[0].kind == MATCH && events[1].kind == READ &&
                          events[1].a == 0x880 && events[1].b == 0x480);
                    CHECK(gMemCard_wDialogFlags == 0xB042);
                }
            }
        }
    }
    for (choice = 0; choice < 2; choice++) {
        reset(0xC7);
        gDialog_bChoice = choice;
        step();
        if (choice) {
            CHECK(D_8009B3EB == 12 && event_count == 0);
        } else {
            CHECK(D_8009B3EB == 0x88 && event_count == 2 && message(0, 0xD6, 0));
            CHECK(events[1].kind == WRITE && events[1].a == 0x200 &&
                  events[1].b == 0x680 &&
                  events[1].pointer == (u8 *)payload + 0x200);
            CHECK(gMemCard_wDialogFlags == 0xB042 && !stub_error);
        }
    }
    for (result = 0; result < 5; result++) {
        reset(0x88);
        D_8009B3F4 = result;
        step();
        CHECK(D_8009B3EB == (result == 0 ? 10 : result == 1 ? 9 : 11));
        CHECK(event_count == 0);
    }
    return 0;
}

static s32 other_states(void)
{
    static const s32 states[] = {9, 11, 12, 13, 14};
    static const s32 messages[] = {0xD2, 0xD9, 0xCD, 0xDA, 0xCE};
    s32 i;
    for (i = 0; i < 5; i++) {
        reset(states[i]);
        step();
        CHECK(event_count == 1 && message(0, messages[i], 0x18));
        CHECK(D_8009B3EF == (states[i] == 12 ? 3 : 0x55));
    }
    reset(2);
    step();
    CHECK(D_8009B3EB == 0x82 && D_8009B3EC == 3 && event_count == 0);
    step();
    CHECK(D_8009B3EB == 0x82 && event_count == 0);
    D_8009B3EB |= 0x40;
    step();
    CHECK(event_count == 1 && message(0, 0xB8, 0x18));
    reset(0xFF);
    step();
    CHECK(D_8009B3EB == 0xFF && event_count == 0 && !stub_error);
    return 0;
}

s32 main(void)
{
    s32 result;
    if (!(result = directory_and_completion()) &&
        !(result = directory_errors()) &&
        !(result = acceptance_and_format()) &&
        !(result = read_verify_and_write()))
        result = other_states();
    report(result ? (u32)result : cases);
    return result ? 1 : 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host C compiler and ILP32 execution",
)
class MemCardSaveDialogBehaviorTests(ScratchTest):
    def run_witness(self, optimization, mutation=None):
        import resource

        source = SOURCE
        if mutation is not None:
            text = SOURCE.read_text()
            if mutation == "completion-message":
                text, count = re.subn(r"=\s*0xD1;", "= 0xCC;", text)
            elif mutation == "free-block-result":
                text, count = re.subn(
                    r"MemCard_CalcFreeBlocks\(([^;\n]*)\)",
                    r"(MemCard_CalcFreeBlocks(\1) + 1)", text,
                )
            elif mutation == "signed-sequence-overflow":
                text, count = re.subn(
                    r"gSaveDataSequence\s*=\s*[^;]+;",
                    "gSaveDataSequence++;", text,
                )
            else:
                raise ValueError(mutation)
            self.assertEqual(count, 1)
            text = re.sub(
                r'^#include "([^"]+)"',
                lambda match: '#include "' + str((SOURCE.parent / match[1]).resolve()) + '"',
                text, flags=re.MULTILINE,
            )
            source = self.scratch / "mutant.c"
            source.write_text(text)
        implementation = self.scratch / "implementation.c"
        implementation.write_text(f'#include "{source}"\n')
        witness = self.scratch / "witness.c"
        witness.write_text(WITNESS)
        startup = self.scratch / "start.S"
        startup.write_text(START)
        binary = self.scratch / "witness"
        result = subprocess.run(
            [
                "cc", "-m32", optimization, "-std=gnu99", "-nostdlib",
                "-D_LANGUAGE_C",
                "-fno-pie", "-no-pie", "-fno-stack-protector", "-ffreestanding",
                "-fno-builtin", "-fno-strict-aliasing",
                "-fsanitize=signed-integer-overflow", "-fsanitize-undefined-trap-on-error",
                "-I", str(REPOSITORY), str(implementation), str(witness), str(startup),
                "-o", str(binary),
            ],
            cwd=REPOSITORY, env=self.environment, capture_output=True, text=True, timeout=60,
        )
        self.assertEqual(result.returncode, 0, result.stderr)
        return subprocess.run(
            [str(binary)], cwd=REPOSITORY, env=self.environment,
            capture_output=True, text=True, timeout=30,
            preexec_fn=lambda: resource.setrlimit(resource.RLIMIT_CORE, (0, 0)),
        )

    def test_state_witnesses_at_o0_and_o2(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization)
                self.assertEqual(result.returncode, 0, "failing C line: " + result.stdout + result.stderr)
                self.assertEqual(result.stdout, "691\n")

    def test_wrong_free_block_result_is_rejected(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization, "free-block-result")
                self.assertEqual(result.returncode, 1, result.stdout + result.stderr)

    def test_wrong_completion_message_is_rejected(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization, "completion-message")
                self.assertEqual(result.returncode, 1, result.stdout + result.stderr)

    def test_signed_sequence_overflow_is_rejected(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization, "signed-sequence-overflow")
                self.assertEqual(result.returncode, -4, result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
