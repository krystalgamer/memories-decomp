"""Callers must see their canonical function declarations.

GCC 2.8.1 accepts a call with no visible prototype and treats the callee as
`int f()`. The build stays byte-exact, and the ownership checks
(unmatched_contracts.py, the candidate contract fingerprints) only ask
whether a declaration exists somewhere in the tree, so a caller that loses
its include goes unnoticed. That happened when #3878 moved these prototypes
to unmatched.h: five callers kept including the old headers and silently fell
back to implicit declarations.

Each pair below is compiled with its recorded profile's front-end flags and
-Wimplicit-function-declaration, and must report no implicit declaration of
the callee. The negative control removes a caller's declaration-providing
include while the prototype stays in its owning header, and requires the
compiler to report the implicit call, so this test fails for exactly the
regression it guards.
The shared CD position-conversion declarations are covered at every caller too.
"""

from __future__ import annotations

import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))
import candidate_builds

CONFIG = REPOSITORY / "config/slus_01411"
COMPILER = "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
IMPLICIT = re.compile(r"implicit declaration of function `([^']+)'")
FRONT_END_FLAG = re.compile(r"^-(?:D|U|I|G|m|O|f(?!no-builtin$))")

# (caller, callee, include that provides the callee's declaration)
PAIRS = [
    ("src/game/sound_effect_request.c", "SD_AllocateSoundEffectVoice", '#include "sound_voice_allocator.h"'),
    ("src/game/sound_effect_request.c", "SD_EnqueueCdSePlay", '#include "sound_output_state.h"'),
    ("src/game/sd_allocate_sound_effect_voice.c", "SD_KeyOffVoiceGroup", '#include "sound_voice_selection.h"'),
    ("src/game/sd_allocate_sound_effect_voice.c", "SD_StartSoundEffectVoice", '#include "sound_voice_selection.h"'),
    ("src/game/sd_allocate_sound_effect_voice.c", "SD_EnqueueCdSePlay", '#include "sound_output_state.h"'),
    ("src/game/display_object_runtime.c", "DisplayObject_ProjectPackedRotationFacing", '#include "display_object_projection.h"'),
    ("src/game/duel_draw_field_cards.c", "Duel_DrawProjectedFieldCardFrame", '#include "display_projection.h"'),
    ("src/game/duel_effect_states.c", "CardPreview_RequestResourceTransfer", '#include "duel_effect_resource_setup.h"'),
    ("src/game/debug_effect_screen.c", "CardPreview_RequestResourceTransfer", '#include "duel_effect_resource_setup.h"'),
    ("src/game/duel_scene_update_card_use.c", "CardPreview_RequestResourceTransfer", '#include "duel_effect_resource_setup.h"'),
    ("src/game/duel_field_effects.c", "CardPreview_RequestResourceTransfer", '#include "duel_effect_resource_setup.h"'),
    ("src/game/duel_scene_card_placement.c", "CardPreview_RequestResourceTransfer", '#include "duel_effect_resource_setup.h"'),
    ("src/game/duel_scene_battle.c", "CardPreview_RequestResourceTransfer", '#include "duel_effect_resource_setup.h"'),
    ("src/game/library_runtime.c", "CardPreview_RequestResourceTransfer", '#include "duel_effect_resource_setup.h"'),
    ("src/game/duel_effect_states.c", "CardPreview_CreateResourceObjects", '#include "card_preview_objects.h"'),
    ("src/game/debug_effect_screen.c", "CardPreview_CreateResourceObjects", '#include "card_preview_objects.h"'),
    ("src/game/duel_scene_update_card_use.c", "CardPreview_CreateResourceObjects", '#include "card_preview_objects.h"'),
    ("src/game/duel_field_effects.c", "CardPreview_CreateResourceObjects", '#include "card_preview_objects.h"'),
    ("src/game/duel_scene_card_placement.c", "CardPreview_CreateResourceObjects", '#include "card_preview_objects.h"'),
    ("src/game/duel_scene_battle.c", "CardPreview_CreateResourceObjects", '#include "card_preview_objects.h"'),
    ("src/game/library_runtime.c", "CardPreview_CreateResourceObjects", '#include "card_preview_objects.h"'),
    ("src/overlays/password/shop.c", "CardPreview_CreateResourceObjects", '#include "../../game/card_preview_objects.h"'),
    ("src/game/sound_spatialization.c", "SD_SetVoiceVolume", '#include "sound.h"'),
    ("src/game/sound_secondary_commands.c", "SD_UpdateSecondaryObjectVoices", '#include "sound.h"'),
    ("src/overlays/password/shop.c", "CardPreview_RequestResourceTransfer", '#include "../../game/duel_effect_resource_setup.h"'),
    ("src/game/sound_secondary_playback.c", "SD_ResetSecondaryPlayback", '#include "sound.h"'),
    ("src/game/fade_runtime.c", "Fade_StepBands", '#include "fade.h"'),
    ("src/game/sound_voice_setup.c", "SD_ResetVoiceEnvelope", '#include "sound.h"'),
    ("src/game/sd_update_secondary_object_voices.c", "SD_UpdateSecondaryObjectPitch", '#include "sound.h"'),
    ("src/game/duel_field_selection.c", "DisplayObject_ReleaseIfPresent", '#include "display_object_core.h"'),
    ("src/game/debug_menu_editor_entries.c", "DisplayObject_ReleaseIfPresent", '#include "display_object_core.h"'),
    ("src/game/duel_result_runtime.c", "DisplayObject_AcquireSlot", '#include "display_object_core.h"'),
    ("src/game/duel_result_runtime.c", "DisplayObject_ConfigureSpriteAtPosition", '#include "display_object_config.h"'),
    ("src/game/duel_field_effects.c", "DisplayObject_ReleaseIfPresent", '#include "display_object_core.h"'),
    ("src/game/duel_scene_field_actions.c", "Duel_GetOrCreateActionReadyMarker", '#include "duel_action_ready_marker.h"'),
    ("src/game/duel_effect_apply_terrain.c", "SD_SEPlayFull", '#include "sound.h"'),
    ("src/game/duel_scene_field_actions.c", "Duel_UpdateCardUseConfirmation", '#include "duel_update_card_use_confirmation.h"'),
    ("src/game/duel_scene_field_actions.c", "DuelCursor_StepTowardTarget", '#include "duel_cursor_step_toward_target.h"'),
    (
        "src/game/duel_effect_apply_terrain.c",
        "DisplayObject_SetResourceVariant",
        '#include "display_object_config.h"',
    ),
    ("src/game/file_stream.c", "CdPosToInt_8007E710", '#include "file_cd_helpers.h"'),
    ("src/game/file_transfer_runtime.c", "CdIntToPos_8007E600", '#include "file_cd_helpers.h"'),
    ("src/game/file_transfer_runtime.c", "CdPosToInt_8007E710", '#include "file_cd_helpers.h"'),
    ("src/game/movie_stream_requests.c", "CdIntToPos_8007E600", '#include "file_cd_helpers.h"'),
    ("src/game/movie_stream_requests.c", "CdPosToInt_8007E710", '#include "file_cd_helpers.h"'),
    ("src/game/card_preview_draw_detail_panel.c", "DisplayObject_SubmitPacket", '#include "display_object_packet_submit.h"'),
    ("src/game/display_object_runtime.c", "DisplayObject_SubmitPacket", '#include "display_object_packet_submit.h"'),
    ("src/game/model_slot_control.c", "Model_InitSlot", '#include "model_slot_control.h"'),
    ("src/game/model_scene_controllers.c", "Model_AdvanceSlotLoad", '#include "model_load_step.h"'),
    ("src/game/model_debug_controller.c", "Model_AdvanceSlotLoad", '#include "model_load_step.h"'),
    ("src/game/model_init_slot.c", "Model_FindBranchingChildCoord", '#include "../game/model_parent_search.h"'),
    ("src/game/duel_scene_update.c", "Duel_UpdateDisplayTransition", '#include "duel_field_display_objects.h"'),
    ("src/game/duel_field_display_objects.c", "Duel_CreateFieldCardPanel", '#include "duel_card_object_helpers.h"'),
    ("src/game/duel_scene_field_actions.c", "Duel_CreateFieldCursorObject", '#include "duel_field_display_objects.h"'),
    ("src/game/duel_scene_hand_actions.c", "DuelHand_PositionSelectionCursor", '#include "duel_hand_position_selection_cursor.h"'),
    ("src/game/duel_scene_hand_actions.c", "DuelHand_CreateSelectionOrderMarker", '#include "duel_hand_selection_cursor.h"'),
    ("src/game/build_deck_draw_card_list_page.c", "func_80031784", '#include "build_deck_draw_sort_icons.h"'),
    ("src/game/build_deck_draw_card_list_page.c", "CardList_DrawDecimalDigits", '#include "duel_card_stat_display.h"'),
    ("src/game/build_deck_draw_card_list_page.c", "Text_EncodeDecimalNoPadding", '#include "text_encode_decimal_no_padding.h"'),
    ("src/game/library_runtime.c", "Library_RefreshSelectedCardText", '#include "library_refresh_selected_card_text.h"'),
    ("src/game/main_run_selection_menus.c", "BuildDeck_InitScreen", '#include "build_deck_init_screen.h"'),
    ("src/game/main_run_duel_and_library.c", "BuildDeck_InitScreen", '#include "../game/build_deck_init_screen.h"'),
]

# func_80028B08 was the last candidate pinned to this contract; it has since
# matched and lives in src/game/card_preview_draw_detail_panel.c, outside candidates.json.
PACKET_SUBMIT_CANDIDATES: tuple[str, ...] = ()


def profiles_by_source() -> dict[str, str]:
    result: dict[str, str] = {}
    manifests = [CONFIG / "matching_c.json"]
    manifests += sorted((CONFIG / "overlays").glob("*_matching_c.json"))
    for manifest in manifests:
        for entry in json.loads(manifest.read_text(encoding="utf-8"))["functions"]:
            result[entry["source"]] = entry["profile"]
    for entry in json.loads((CONFIG / "candidates.json").read_text(encoding="utf-8"))["candidates"]:
        result[entry["source"]] = entry["profile"]
    return result


def compiler_diagnostics(
    path: Path,
    profile: dict[str, object],
    include_dir: Path,
    *,
    warnings_as_errors: bool = False,
) -> str:
    flags = [str(f) for f in profile["compiler_flags"] if FRONT_END_FLAG.match(str(f))]  # type: ignore[index]
    completed = subprocess.run(
        [
            str(REPOSITORY / str(profile["compiler"])),
            # -S stops before the assembler, which rejects -G on its own
            # command line; the output is discarded.
            "-S",
            "-o",
            os.devnull,
            "-Wimplicit-function-declaration",
            *(["-Werror"] if warnings_as_errors else []),
            f"-I{include_dir}",
            f"-I{REPOSITORY / 'src'}",
            *flags,
            str(path),
        ],
        cwd=REPOSITORY,
        capture_output=True,
        text=True,
        errors="replace",
    )
    if completed.returncode != 0:
        raise AssertionError(f"{path} does not compile:\n{completed.stderr}")
    return completed.stderr


def implicit_calls(
    path: Path,
    profile: dict[str, object],
    include_dir: Path,
    *,
    warnings_as_errors: bool = False,
) -> set[str]:
    return set(IMPLICIT.findall(compiler_diagnostics(
        path, profile, include_dir, warnings_as_errors=warnings_as_errors,
    )))


@unittest.skipUnless((REPOSITORY / COMPILER).is_file(), "needs the GCC 2.8.1 toolchain")
class CandidateCallerVisibilityTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.profiles = json.loads(
            (CONFIG / "compiler_profiles.json").read_text(encoding="utf-8")
        )["profiles"]
        cls.sources = profiles_by_source()

    def profile(self, source: str) -> dict[str, object]:
        self.assertIn(source, self.sources, f"{source} is not a built source")
        return self.profiles[self.sources[source]]

    def test_callers_see_their_callees(self) -> None:
        for source, callee, _ in PAIRS:
            with self.subTest(source=source, callee=callee):
                text = (REPOSITORY / source).read_text(encoding="utf-8")
                self.assertRegex(text, rf"\b{callee}\s*\(", "the call is gone")
                found = implicit_calls(
                    REPOSITORY / source,
                    self.profile(source),
                    (REPOSITORY / source).parent,
                )
                self.assertNotIn(callee, found)

    def test_lost_include_is_caught_while_the_prototype_still_exists(self) -> None:
        # Keep each selected prototype in its owner while hiding that owner
        # from the caller, reproducing the lost-include regression.
        unmatched = (REPOSITORY / "src/unmatched.h").read_text(encoding="utf-8")
        sound = (REPOSITORY / "src/game/sound.h").read_text(encoding="utf-8")
        for source, callee, include in PAIRS:
            if "unmatched.h" in include:
                declarations = unmatched
            elif callee == "SD_ResetSecondaryPlayback":
                declarations = sound
            else:
                continue
            with self.subTest(source=source, callee=callee):
                self.assertRegex(
                    declarations,
                    rf"(?m)^(?![ \t]*/?\*)[^\n]*\b{callee}\s*\(",
                )
                text = (REPOSITORY / source).read_text(encoding="utf-8")
                self.assertIn(include + "\n", text)
                (REPOSITORY / "tmp").mkdir(exist_ok=True)
                scratch = Path(tempfile.mkdtemp(dir=REPOSITORY / "tmp"))
                try:
                    # Nested so ordinary relative includes still resolve
                    # through -I, while this probe can supply a shadow owner
                    # with only the selected prototype removed.
                    probe = scratch / "a/b/c" / Path(source).name
                    probe.parent.mkdir(parents=True)
                    include_path = re.search(r'"([^"]+)"', include)
                    self.assertIsNotNone(include_path)
                    shadow = (probe.parent / include_path.group(1)).resolve()
                    shadow.parent.mkdir(parents=True, exist_ok=True)
                    without_callee, count = re.subn(
                        rf"(?m)^[^\n]*\b{callee}\s*\([^;]*;\s*$",
                        "",
                        declarations,
                        count=1,
                    )
                    self.assertEqual(count, 1)
                    shadow.write_text(without_callee, encoding="utf-8")
                    probe.write_text(text, encoding="utf-8")
                    found = implicit_calls(
                        probe, self.profile(source), (REPOSITORY / source).parent
                    )
                finally:
                    shutil.rmtree(scratch, ignore_errors=True)
                self.assertIn(callee, found)

    def test_lost_cd_header_is_caught_while_sdk_types_remain_visible(self) -> None:
        header = (
            REPOSITORY / "src/psyq/libcd_abi_variants.h"
        ).read_text(encoding="utf-8")
        for source, callee, include in PAIRS:
            if "file_cd_helpers.h" not in include:
                continue
            with self.subTest(source=source, callee=callee):
                self.assertRegex(header, rf"\b{callee}\s*\(")
                text = (REPOSITORY / source).read_text(encoding="utf-8")
                self.assertIn(include + "\n", text)
                with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as directory:
                    probe = Path(directory) / "a/b/c" / Path(source).name
                    probe.parent.mkdir(parents=True)
                    probe.write_text(
                        text.replace(include, '#include "../psyq/libcd.h"', 1),
                        encoding="utf-8",
                    )
                    found = implicit_calls(
                        probe, self.profile(source), (REPOSITORY / source).parent
                    )
                self.assertIn(callee, found)

    def test_lost_shared_owner_header_is_caught(self) -> None:
        for source, callee, include in PAIRS:
            if not any(
                owner in include
                for owner in (
                    "display_object_packet_submit.h",
                    "duel_effect_resource_setup.h",
                    "card_preview_objects.h",
                    "library_refresh_selected_card_text.h",
                    "model_slot_control.h",
                    "model_load_step.h",
                    "duel_field_display_objects.h",
                    "duel_hand_position_selection_cursor.h",
                    "duel_hand_selection_cursor.h",
                    "duel_card_object_helpers.h",
                    "build_deck_draw_sort_icons.h",
                    "duel_card_stat_display.h",
                    "text_encode_decimal_no_padding.h",
                )
            ):
                continue
            with self.subTest(source=source):
                text = (REPOSITORY / source).read_text(encoding="utf-8")
                self.assertIn(include + "\n", text)
                replacement = ""
                if source == "src/game/card_preview_draw_detail_panel.c" and "display_object_packet_submit.h" in include:
                    # Preserve the real layout, not the lost function declaration.
                    owner = (REPOSITORY / "src/game/display_object_packet_submit.h").read_text()
                    layout = re.search(r"typedef struct \{[^}]*\} DisplayObjectPacketOrigin;", owner)
                    self.assertIsNotNone(layout)
                    replacement = (
                        '#include "../psyq/libgte.h"\n#include "../psyq/libgpu.h"\n'
                        + layout.group() + "\n"
                    )
                if "duel_field_display_objects.h" in include:
                    # Retain actual data types, not the lost function declarations.
                    replacement = (
                        '#include "duel_selection_layout.h"\n'
                    )
                shadow = None
                if "model_slot_control.h" in include:
                    # The unit needs the rest of its own header; shadow it
                    # with a copy that loses only the callee's declaration.
                    owner = (REPOSITORY / "src/game/model_slot_control.h").read_text()
                    prototype = (
                        "void Model_InitSlot(s32 slot, u8 *hmd, s32 size, s32 flags);\n"
                    )
                    self.assertEqual(owner.count(prototype), 1)
                    shadow = owner.replace(prototype, "", 1)
                    replacement = include + "\n"
                with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as directory:
                    probe = Path(directory) / "a/b/c" / Path(source).name
                    probe.parent.mkdir(parents=True)
                    if shadow is not None:
                        (probe.parent / "model_slot_control.h").write_text(
                            shadow, encoding="utf-8"
                        )
                    probe.write_text(
                        text.replace(include + "\n", replacement, 1),
                        encoding="utf-8",
                    )
                    found = implicit_calls(
                        probe, self.profile(source), (REPOSITORY / source).parent
                    )
                self.assertIn(callee, found)

    def test_packet_submit_contract_tracks_declaration_changes(self) -> None:
        entries = {
            entry["source"]: entry
            for entry in json.loads(
                (CONFIG / "candidates.json").read_text(encoding="utf-8")
            )["candidates"]
        }
        index = candidate_builds.canonical_declaration_index({"DisplayObject_SubmitPacket"})
        declarations = index["DisplayObject_SubmitPacket"]
        expected = candidate_builds.canonical_symbol_contract_hash(
            "DisplayObject_SubmitPacket", declarations
        )

        for source in PACKET_SUBMIT_CANDIDATES:
            with self.subTest(source=source):
                self.assertEqual(
                    entries[source]["canonical_contracts"]["DisplayObject_SubmitPacket"],
                    expected,
                )

        changed = [
            (path, statement.replace("s32 mode", "u32 mode", 1))
            for path, statement in declarations
        ]
        changed_hash = candidate_builds.canonical_symbol_contract_hash(
            "DisplayObject_SubmitPacket", changed
        )
        self.assertNotEqual(expected, changed_hash)

    def test_model_dispatch_uses_shared_owner_view_after_promotion(self) -> None:
        candidates = json.loads(
            (CONFIG / "candidates.json").read_text(encoding="utf-8")
        )["candidates"]
        self.assertFalse(any(candidate["address"] == "0x80056828" for candidate in candidates))
        entry = next(
            candidate
            for candidate in json.loads(
                (CONFIG / "matching_c.json").read_text(encoding="utf-8")
            )["functions"]
            if candidate["address"] == "0x80056828"
        )
        source = "src/game/model_slot_control.c"
        self.assertEqual(entry["source"], source)
        self.assertEqual(entry["profile"], "gcc_2_8_1_g8_split")
        declarations = candidate_builds.canonical_declaration_index(
            {"Model_InitSlot"}
        )["Model_InitSlot"]
        self.assertEqual(
            {statement for _, statement in declarations},
            {"void Model_InitSlot(s32 slot, u8 *hmd, s32 size, s32 flags);"},
        )
        expected = candidate_builds.canonical_symbol_contract_hash(
            "Model_InitSlot", declarations
        )
        path = REPOSITORY / source
        text = path.read_text(encoding="utf-8")
        selector = "Model_InitSlot(arg0, arg1, arg2, arg3);"
        self.assertEqual(text.count(selector), 1)
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as directory:
            probe = Path(directory) / "a/b/c" / path.name
            probe.parent.mkdir(parents=True)
            probe.write_text(
                text.replace(selector, "Model_InitSlot();", 1), encoding="utf-8"
            )
            with self.assertRaisesRegex(AssertionError, "too few arguments to function"):
                compiler_diagnostics(probe, self.profile(source), path.parent)

        changed = [
            (path, statement.replace("s32 flags", "u32 flags", 1))
            for path, statement in declarations
        ]
        changed_hash = candidate_builds.canonical_symbol_contract_hash(
            "Model_InitSlot", changed
        )
        self.assertNotEqual(expected, changed_hash)

    def test_candidate_sprite_records_cannot_be_swapped(self) -> None:
        source = "src/game/card_preview_draw_detail_panel.c"
        path = REPOSITORY / source
        profile = self.profile(source)
        original = path.read_text(encoding="utf-8")
        call = "DisplayObject_SubmitPacket(PRM, packet, (GsOT *)arg1, arg, origin);"
        swapped = "DisplayObject_SubmitPacket(PRM, origin, (GsOT *)arg1, arg, packet);"
        self.assertIn(call, original)
        base_diagnostics = compiler_diagnostics(path, profile, path.parent)
        self.assertNotRegex(
            base_diagnostics,
            r"passing arg [25] of `DisplayObject_SubmitPacket' from incompatible pointer type",
        )

        (REPOSITORY / "tmp").mkdir(exist_ok=True)
        scratch = Path(tempfile.mkdtemp(dir=REPOSITORY / "tmp"))
        try:
            probe = scratch / "a/b/c" / path.name
            probe.parent.mkdir(parents=True)
            probe.write_text(original.replace(call, swapped, 1), encoding="utf-8")
            diagnostics = compiler_diagnostics(probe, profile, path.parent)
        finally:
            shutil.rmtree(scratch, ignore_errors=True)
        self.assertRegex(
            diagnostics,
            r"passing arg 2 of `DisplayObject_SubmitPacket' from incompatible pointer type",
        )
        self.assertRegex(
            diagnostics,
            r"passing arg 5 of `DisplayObject_SubmitPacket' from incompatible pointer type",
        )

    def test_parent_search_caller_needs_owning_header(self) -> None:
        source = "src/game/model_init_slot.c"
        include = '#include "../game/model_parent_search.h"\n'
        text = (REPOSITORY / source).read_text(encoding="utf-8")
        self.assertIn(include, text)
        self.assertNotRegex(text, r"extern[^\n]*\bModel_FindBranchingChildCoord\b")
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as temporary:
            path = Path(temporary) / "a/b/c/caller.c"
            path.parent.mkdir(parents=True)
            path.write_text(text.replace(include, "", 1), encoding="utf-8")
            found = implicit_calls(
                path, self.profile(source), (REPOSITORY / source).parent
            )
        self.assertIn("Model_FindBranchingChildCoord", found)

    def test_cursor_motion_object_and_direction_views(self) -> None:
        definition = "src/game/duel_cursor_grid_direction.c"
        ambient = "#define FUNC_80023D08_AMBIENT_DIRECTION_ARG\n"
        views = [
            ("", "DuelSelectionRecord *, s32", True),
            ("", "DuelSelectionRecord *", False),
            ("", "DisplayObject *, s32", False),
            (ambient, "DuelSelectionRecord *", True),
            (ambient, "DuelSelectionRecord *, s32", False),
            (ambient, "DisplayObject *", False),
        ]
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as temporary:
            path = Path(temporary) / "cursor.c"
            for prefix, parameters, accepted in views:
                with self.subTest(prefix=prefix, parameters=parameters):
                    path.write_text(
                        prefix + '#include "duel_cursor_grid_direction.h"\n'
                        + '#include "duel_cursor_held_grid_direction.h"\n'
                        + "DisplayObject *G32 *object_view(DuelSelectionRecord *cursor) "
                        + "{ return &cursor->cursor_object; }\n"
                        + "s8 *column_view(DuelSelectionRecord *cursor) "
                        + "{ return &cursor->col; }\n"
                        + "s8 *row_view(DuelSelectionRecord *cursor) "
                        + "{ return &cursor->row; }\n"
                        + "u8 *status_view(DuelSelectionRecord *cursor) "
                        + "{ return &cursor->status; }\n"
                        + "void (*held_input)(DuelSelectionRecord *) = DuelCursor_ProcessHeldGridDirection;\n"
                        + f"void (*step)({parameters}) = DuelCursor_ProcessGridDirection;\n",
                        encoding="utf-8",
                    )
                    if accepted:
                        self.assertEqual(
                            implicit_calls(
                                path, self.profile(definition),
                                REPOSITORY / "src/game",
                                warnings_as_errors=True,
                            ),
                            set(),
                        )
                    else:
                        with self.assertRaisesRegex(AssertionError, "incompatible"):
                            implicit_calls(
                                path, self.profile(definition),
                                REPOSITORY / "src/game",
                                warnings_as_errors=True,
                            )

    def test_active_selection_global_has_one_record_view(self) -> None:
        definition = "src/game/duel_phase_entry.c"
        views = [
            ("DuelSelectionRecord", True),
            ("DuelHandSlot", False),
        ]
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as temporary:
            path = Path(temporary) / "selection.c"
            for record, accepted in views:
                with self.subTest(record=record):
                    path.write_text(
                        '#include "duel_selection_layout.h"\n'
                        + "struct DisplayObject *G32 *display_parent_slot("
                        + "DuelSelectionRecord *record) "
                        + "{ return &record->field_00; }\n"
                        + f"{record} *G32 *active_record(void) "
                        + "{ return &gDuel_pActiveSelectionRecord; }\n"
                        + "DuelHandSlot *G32 *hand_slot(DuelSelectionRecord *record) "
                        + "{ return &record->hand; }\n",
                        encoding="utf-8",
                    )
                    if accepted:
                        self.assertEqual(
                            implicit_calls(
                                path, self.profile(definition),
                                REPOSITORY / "src/game",
                                warnings_as_errors=True,
                            ),
                            set(),
                        )
                    else:
                        with self.assertRaisesRegex(AssertionError, "incompatible"):
                            implicit_calls(
                                path, self.profile(definition),
                                REPOSITORY / "src/game",
                                warnings_as_errors=True,
                            )

    def test_hand_slot_has_canonical_display_object_pointers(self) -> None:
        definition = "src/game/duel_hand_selection_cursor.c"
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as temporary:
            path = Path(temporary) / "hand.c"
            for member in ("object", "order_marker"):
                for pointee, accepted in (("struct DisplayObject", True), ("u8", False)):
                    with self.subTest(member=member, pointee=pointee):
                        path.write_text(
                            '#include "duel_hand.h"\n'
                            + f"{pointee} *G32 *slot_view(DuelHandSlot *slot) "
                            + f"{{ return &slot->{member}; }}\n",
                            encoding="utf-8",
                        )
                        if accepted:
                            self.assertEqual(
                                implicit_calls(
                                    path, self.profile(definition),
                                    REPOSITORY / "src/game",
                                    warnings_as_errors=True,
                                ),
                                set(),
                            )
                        else:
                            with self.assertRaisesRegex(AssertionError, "incompatible"):
                                implicit_calls(
                                    path, self.profile(definition),
                                    REPOSITORY / "src/game",
                                    warnings_as_errors=True,
                                )

    def test_parent_search_has_one_pointer_view(self) -> None:
        definition = "src/game/model_find_branching_child.c"
        self.assertIn(
            '#include "model_parent_search.h"',
            (REPOSITORY / definition).read_text(encoding="utf-8"),
        )
        views = [
            ("ModelSlot *, GsCOORDUNIT *", True),
            ("ModelSlot *, void *", False),
            ("u8 *, GsCOORDUNIT *", False),
        ]
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as temporary:
            path = Path(temporary) / "view.c"
            for parameters, accepted in views:
                with self.subTest(parameters=parameters):
                    path.write_text(
                        '#include "model_parent_search.h"\n'
                        + f"s32 (*view)({parameters}) = Model_FindBranchingChildCoord;\n",
                        encoding="utf-8",
                    )
                    if accepted:
                        self.assertEqual(
                            implicit_calls(
                                path, self.profile(definition),
                                REPOSITORY / "src/game",
                                warnings_as_errors=True,
                            ),
                            set(),
                        )
                    else:
                        with self.assertRaisesRegex(AssertionError, "incompatible"):
                            implicit_calls(
                                path, self.profile(definition),
                                REPOSITORY / "src/game",
                                warnings_as_errors=True,
                            )


if __name__ == "__main__":
    unittest.main()
