"""Bounded ILP32 result-controller witnesses and semantic mutation controls.

The real C implementation and opaque helpers compile separately without LTO.
Only the Linux i386 startup is assembly; no multilib libc or project build is
needed. These helpers observe controller contracts, not rendering or save I/O.
"""

from __future__ import annotations

import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
SOURCE = REPOSITORY / "src/game/func_800218F0.c"
START = """
.text
.globl _start
_start:
    mov (%esp), %eax
    lea 4(%esp), %edx
    and $-16, %esp
    sub $8, %esp
    push %edx
    push %eax
    call main
    mov %eax, %ebx
    mov $1, %eax
    int $0x80
.section .note.GNU-stack,"",@progbits
"""

WITNESS = r"""
#include "src/game/save_data.h"
void DuelScene_UpdateResultRewards(void);
#define U16(p, n) (*(u16 *)((u8 *)(p) + (n)))
#define U32(p, n) (*(u32 *)((u8 *)(p) + (n)))
#define CHECK(c, n) do { if (!(c)) return (n); } while (0)

ViewState D_800F2848;
u16 gDuel_wSceneStateFlags, D_8009B16C, D_8009B32E;
s16 gGraphics_sViewportX, gGraphics_sViewportY;
DuelResultDisplayState *D_8009B1E8;
DuelResultDisplayState gDuel_awRitualData;
u8 D_8009B34E, D_8009B355, gDuel_bWinnerSide;
s8 D_8009B360[2], gDuel_bOpponentID;
u8 *D_8009B1D8[2];
s32 D_801D56A8[1];
u8 D_801AF000[2048], gText_abColorSlots[16];
u8 D_800E9EC8_arr[FADE_TRANSITION_STATE_SIZE];
s16 gDuel_wSelectedCardID;
u16 gInput_wPad1Pressed, gInput_wPad1Repeat;

static DisplayObject objects[16];
static DuelResultDisplayState alternate;
static SaveDataState save_records[2];
static u8 *saves[2] = {(u8 *)&save_records[0], (u8 *)&save_records[1]};
static s32 score_input[2], drop_result, object_count, allocation_count;
static s32 error, event_count, mutation, executions;
static u16 expected_angle;
typedef struct { s32 kind, object, a, b, c, d; } Event;
static Event events[96];

static void clear(void *pointer, u32 size)
{
    volatile u8 *p = pointer;
    while (size--) *p++ = 0;
}

static void event(s32 kind, s32 object, s32 a, s32 b, s32 c, s32 d)
{
    if (event_count >= 96) { error = 1; return; }
    events[event_count].kind = kind; events[event_count].object = object;
    events[event_count].a = a; events[event_count].b = b;
    events[event_count].c = c; events[event_count].d = d;
    event_count++;
}

static s32 object_id(void *p)
{
    s32 i;
    for (i = 0; i < 16; i++) if (p == &objects[i]) return i;
    error = 2;
    return -1;
}

void func_8001352C(void)
{
    event(1, 0, (u16)D_800F2848.angle, 0, 0, 0);
    if ((u16)D_800F2848.angle != expected_angle) error = 3;
    if (mutation == 1) gDuel_wSceneStateFlags = 0x8000;
}
void func_80015C84(void) { event(2, 0, 0, 0, 0, 0); }
void Fade_SetTargetLevel(s32 level, s32 speed)
{
    event(3, 0, level, speed, 0, 0);
}
void SD_BGMPlay(u32 id) { event(4, 0, id, 0, 0, 0); }
void Duel_CalcRankScore(void)
{
    event(5, 0, 0, 0, 0, 0);
    if (D_8009B1E8 != &gDuel_awRitualData ||
        D_8009B1E8->is_tec_rank) error = 4;
    if (mutation == 2) D_8009B1E8 = &alternate;
    D_8009B1E8->side_scores[0] = score_input[0];
    D_8009B1E8->side_scores[1] = score_input[1];
}
s32 DisplayObject_FindFreeGeneralSlot(void)
{
    event(6, allocation_count, 0, 0, 0, 0);
    return 100 + allocation_count++;
}
void *func_800400AC(s32 index, s32 key)
{
    s32 id = object_count++;
    event(7, id, index, key, 0, 0);
    if (id >= 16 || index != 100 + id || key != 2) {
        error = 5;
        return &objects[0];
    }
    return &objects[id];
}
void DisplayObject_ConfigureSpriteAtPosition(void *p, s32 x, s32 y, s32 a, s32 b, s32 c, s32 color, s32 texture)
{
    s32 id = object_id(p);
    event(8, id, x, y, b, c);
    if (a != 3 || color != 11 || texture != 524) error = 6;
}
void func_800428A8(void *p, s32 a, s32 b, s32 c, s32 d, s32 e,
                  s32 f, s32 g, void *resource)
{
    event(9, object_id(p), b, d, e, 0);
    if (a || c || f != 16 || g != 8 || resource != D_801AF000) error = 7;
}
void func_80042918(DisplayObject *p) { event(10, object_id(p), 0, 0, 0, 0); }
s32 func_800428EC(u8 *p, s8 order)
{
    event(11, object_id(p), order, 0, 0, 0);
    return 0;
}
s32 Duel_SelectCardDrop(s32 pool)
{
    event(12, 0, pool, 0, 0, 0);
    if (mutation == 3) D_8009B1E8->starchip_prize = 2;
    return drop_result;
}
void Duel_ShowResultPage(s32 page)
{
    event(13, 0, page, 0, 0, 0);
}
void SD_SEPlayFull(u32 id)
{
    event(14, 0, id, 0, 0, 0);
    if (mutation == 4) D_8009B1E8->page_index = 2;
}
void Fade_StartOut(void)
{
    event(15, 0, 0, 0, 0, 0);
    if (mutation == 5) gDuel_wSceneStateFlags |= 0x80;
}
void Fade_FillBandLevels(s32 level)
{
    event(16, 0, level, D_800E9EC8_arr[4], 0, 0);
}
void Duel_AwardCard(s32 card)
{
    event(17, 0, card, U32(saves[0], 0x5E0), 0, 0);
}

static void setup(u16 flags)
{
    s32 i;
    clear(&D_800F2848, sizeof(D_800F2848));
    clear(&alternate, sizeof(alternate));
    clear(&gDuel_awRitualData, sizeof(gDuel_awRitualData));
    clear(objects, sizeof(objects)); clear(events, sizeof(events));
    clear(save_records, sizeof(save_records)); clear(D_800E9EC8_arr, sizeof(D_800E9EC8_arr));
    for (i = 0; i < 16; i++) { objects[i].flags = 0x8101; gText_abColorSlots[i] = 0xCC; }
    gDuel_awRitualData.pad_3B = 0xAB;
    gDuel_awRitualData.pad_3E[0] = 0xCD;
    gDuel_awRitualData.pad_3E[1] = 0xEF;
    D_8009B1E8 = &gDuel_awRitualData;
    for (i = 0; i < 10; i++) D_8009B1E8->children[i] = (DisplayObject *)0x12345678;
    D_8009B1E8->rank_tier = D_8009B1E8->is_tec_rank = 0xCC;
    D_8009B1E8->starchip_prize = 0xA5;
    D_8009B1E8->dropped_card_id = -123;
    D_8009B1E8->page_index = 0;
    gDuel_wSceneStateFlags = flags; D_8009B16C = 0x12; D_8009B32E = 0x1234;
    gGraphics_sViewportX = 37; gGraphics_sViewportY = -43;
    D_8009B34E = 0xAA; D_8009B355 = 0xBB;
    gDuel_bWinnerSide = 0; gDuel_bOpponentID = 5;
    D_8009B360[0] = D_8009B360[1] = -1;
    D_8009B1D8[0] = (u8 *)saves[0]; D_8009B1D8[1] = (u8 *)saves[1];
    D_801D56A8[0] = 0x55555555; gDuel_wSelectedCardID = -1;
    gInput_wPad1Repeat = gInput_wPad1Pressed = 0;
    D_800F2848.angle = -1; expected_angle = 1;
    score_input[0] = score_input[1] = 50;
    drop_result = 0x8001;
    object_count = allocation_count = event_count = error = mutation = 0;
}

static void run(void) { executions++; DuelScene_UpdateResultRewards(); }

static s32 valid(void)
{
    if (error || !event_count || events[0].kind != 1) return 0;
    return gDuel_awRitualData.pad_3B == 0xAB &&
           gDuel_awRitualData.pad_3E[0] == 0xCD &&
           gDuel_awRitualData.pad_3E[1] == 0xEF;
}

static s32 check_object(s32 *cursor, s32 id, s32 kind, s32 a, s32 b, s32 c, s32 d)
{
    Event *e = &events[*cursor];
    if (e[0].kind != 6 || e[0].object != id ||
        e[1].kind != 7 || e[1].object != id || e[1].a != id + 100 || e[1].b != 2 ||
        e[2].kind != kind || e[2].object != id ||
        e[2].a != a || e[2].b != b || e[2].c != c || e[2].d != d ||
        e[3].kind != 10 || e[3].object != id) return 0;
    *cursor += 4;
    if (kind == 9) {
        e = &events[(*cursor)++];
        if (e->kind != 11 || e->object != id ||
            e->a != (b == 5 ? -2 : -1)) return 0;
    }
    return objects[id].flags == (id < 2 ? 0x8129 : 0x8121);
}

static s32 check_initial(s32 tier, s32 tec, s32 eligible, s32 label)
{
    s32 cursor = 5, id = 3, i, chips = eligible ? tier + 1 : 0;
    s32 pool = tier < 3 ? 1 : tec ? 2 : 0;
    DuelResultDisplayState *state = D_8009B1E8;
    CHECK(valid() && gDuel_wSceneStateFlags == 0x8012 && events[1].kind == 2 &&
          events[2].kind == 3 && events[2].a == 128 && events[2].b == 2 &&
          events[3].kind == 4 && events[3].a == (gDuel_bWinnerSide ? 0x72F1 : 0x72E1) &&
          events[4].kind == 5, 10);
    CHECK(!gGraphics_sViewportX && !gGraphics_sViewportY &&
          gText_abColorSlots[gDuel_bWinnerSide] == 0 &&
          gText_abColorSlots[gDuel_bWinnerSide ^ 1] == 4 && gText_abColorSlots[2] == 0xCC, 11);
    CHECK(state->rank_tier == tier && state->is_tec_rank == tec && !state->page_index, 12);
    CHECK(check_object(&cursor, 0, 8, 32, 16, 1, 2) &&
          check_object(&cursor, 1, 8, 288, 16, 1, 0) &&
          check_object(&cursor, 2, 9, 8, 4, 0, 0) && state->root == &objects[2], 13);
    if (eligible) {
        CHECK(events[cursor].kind == 12 && events[cursor++].a == pool &&
              state->starchip_prize == chips && (u16)gDuel_wSelectedCardID == (u16)drop_result &&
              state->dropped_card_id == (s16)drop_result &&
              D_801D56A8[0] == (s16)drop_result, 14);
        for (i = 0; i < chips; i++, id++)
            CHECK(check_object(&cursor, id, 8, 160 + 20 * i, 192, 4, 0) &&
                  state->children[i + 2] == &objects[id], 15);
    } else CHECK(!gDuel_wSelectedCardID && D_801D56A8[0] == 0x55555555 &&
                 state->starchip_prize == 0xA5 && state->dropped_card_id == -123, 16);
    if (label) {
        CHECK(check_object(&cursor, id, 9, 16, 5, tec, 0) &&
              state->children[0] == &objects[id], 17);
        id++;
        CHECK(check_object(&cursor, id, 9, 16, 6, tier, 0) &&
              state->children[1] == &objects[id], 18);
        id++;
    } else CHECK(!state->children[0] && !state->children[1], 19);
    for (i = chips + 2; i < 10; i++) CHECK(!state->children[i], 20);
    CHECK(object_count == id && allocation_count == id &&
          events[cursor].kind == 13 && events[cursor].a == 0 &&
          event_count == cursor + 1, 21);
    return 0;
}

static s32 ranks(void)
{
    static const s32 scores[] = {-2147483647-1, -1, 0, 9, 10, 19, 20, 29, 30, 39,
                                 40, 49, 50, 59, 60, 69, 70, 79, 80, 89, 90, 99, 100, 2147483647};
    static const u8 tiers[] = {4,4,4,4,3,3,2,2,1,1,0,0,0,0,1,1,2,2,3,3,4,4,4,4};
    s32 i, winner, result;
    for (winner = 0; winner < 2; winner++) for (i = 0; i < 24; i++) {
        setup(0x12); gDuel_bWinnerSide = winner;
        score_input[winner] = scores[i]; score_input[winner ^ 1] = 77;
        run();
        result = check_initial(tiers[i], i < 12, !winner, 1);
        if (result) return result;
    }
    return 0;
}

static s32 eligibility(void)
{
    static const s8 identities[] = {-128, 0};
    static const s8 opponents[] = {-128, -1, 0, 127};
    s32 winner, a, b, opponent, result, multi;
    for (winner = 0; winner < 2; winner++)
    for (a = 0; a < 2; a++) for (b = 0; b < 2; b++)
    for (opponent = 0; opponent < 4; opponent++) {
        setup(0x12); gDuel_bWinnerSide = winner;
        D_8009B360[0] = identities[a]; D_8009B360[1] = identities[b];
        gDuel_bOpponentID = opponents[opponent];
        run();
        result = check_initial(0, 0, !winner && !a && opponent >= 2,
                               winner ? !b : !a);
        if (result) return result;
        multi = !a && opponent < 2;
        CHECK(D_8009B34E == multi && D_8009B355 == winner + 2 * multi &&
              D_8009B32E == (winner ? (u16)(opponents[opponent] - 31960) : 0x1234), 22);
    }
    return 0;
}

static s32 drop_sign(void)
{
    static const s32 drops[] = {0, 1, 32767, 32768, 65535, 0x12348001};
    s32 i, result;
    for (i = 0; i < 6; i++) {
        setup(0x12); drop_result = drops[i]; run();
        result = check_initial(0, 0, 1, 1);
        if (result) return result;
    }
    return 0;
}

static s32 camera_idle(void)
{
    static const u16 angles[] = {0, 1, 32767, 32768, 65534, 65535};
    static const u16 next[] = {2, 3, 32769, 32770, 0, 1};
    s32 i;
    for (i = 0; i < 6; i++) {
        setup(0x8012); D_800F2848.angle = angles[i]; expected_angle = next[i];
        run();
        CHECK(valid() && (u16)D_800F2848.angle == next[i] && event_count == 1 &&
              gDuel_wSceneStateFlags == 0x8012 && !object_count &&
              gGraphics_sViewportX == 37 && gGraphics_sViewportY == -43, 30);
    }
    return 0;
}

static s32 pages(void)
{
    static const u8 initial[] = {0,1,2,3,127,128,254,255};
    static const s16 forward[] = {1,2,0,0,-128,-127,-1,0};
    static const s16 backward[] = {2,0,1,2,0,0,2,2};
    static const u16 keys[] = {0x2000,0x8000,0xA000};
    s32 i, key, expected;
    for (key = 0; key < 3; key++) for (i = 0; i < 8; i++) {
        setup(0x8012); D_8009B1E8->page_index = initial[i];
        gInput_wPad1Repeat = keys[key]; gInput_wPad1Pressed = 0x40;
        run();
        expected = key ? backward[i] : forward[i];
        CHECK(valid() && (s8)D_8009B1E8->page_index == expected &&
              event_count == 3 && events[1].kind == 14 && events[1].a == 6 &&
              events[2].kind == 13 && events[2].a == expected &&
              gDuel_wSceneStateFlags == 0x8012, 40);
    }
    return 0;
}

static s32 confirm(void)
{
    static const u16 keys[] = {0, 0x20, 0x40, 0x80, 0xC0, 0xFFFF};
    s32 i, pressed;
    for (i = 0; i < 6; i++) {
        setup(0x8012); gInput_wPad1Pressed = keys[i]; gInput_wPad1Repeat = 0x4000;
        run(); pressed = (keys[i] & 0x40) != 0;
        CHECK(valid() && event_count == (pressed ? 3 : 1) &&
              gDuel_wSceneStateFlags == (pressed ? 0xC012 : 0x8012), 50);
        if (pressed) CHECK(events[1].kind == 3 && events[1].a == 0 &&
                            events[1].b == 6 && events[2].kind == 14 && events[2].a == 48, 51);
    }
    return 0;
}

static s32 exit_handshake(void)
{
    s32 armed, fading, missing;
    for (armed = 0; armed < 2; armed++)
    for (fading = 0; fading < 2; fading++)
    for (missing = 0; missing < 2; missing++) {
        setup(armed ? 0xE012 : 0xC012);
        D_800E9EC8_arr[6] = fading ? 0xA5 : 0x25;
        D_800E9EC8_arr[4] = 7;
        if (missing) D_8009B1D8[0] = 0;
        D_8009B1E8->starchip_prize = 2;
        run();
        CHECK(valid() && gDuel_wSceneStateFlags == (armed || !fading ? 0xE012 : 0xC012), 60);
        if (fading) CHECK(event_count == 1 && D_8009B16C == 0x12 &&
                          D_800E9EC8_arr[4] == 7, 61);
        else if (!armed) CHECK(event_count == 3 && events[1].kind == 15 &&
                                events[2].kind == 16 && events[2].a == 255 &&
                                events[2].b == 255 && D_8009B16C == 0x12, 62);
        else CHECK(D_8009B16C == 0x2012 && event_count == (missing ? 1 : 2) &&
                   D_800E9EC8_arr[4] == 7, 63);
    }
    return 0;
}

static s32 chip_awards(void)
{
    static const u32 amounts[] = {0,999993,999999,1000000,0xFFFFFFFD,0xFFFFFFFF};
    static const u32 expected[] = {5,999998,999999,999999,2,4};
    s32 i, winner;
    for (winner = 0; winner < 2; winner++) for (i = 0; i < 6; i++) {
        setup(0xE012); gDuel_bWinnerSide = winner;
        D_8009B1E8->starchip_prize = 5; D_8009B1E8->dropped_card_id = -32767;
        U32(saves[0], 0x5E0) = amounts[i]; U32(saves[1], 0x5E0) = 123;
        run();
        CHECK(valid() && D_8009B16C == 0x2012 && event_count == 2 &&
              events[1].kind == 17 && events[1].a == -32767 &&
              (u32)events[1].b == expected[i] &&
              U32(saves[0], 0x5E0) == expected[i] && U32(saves[1], 0x5E0) == 123, 70);
    }
    return 0;
}

static s32 win_loss_awards(void)
{
    static const u16 counts[] = {0,9998,9999,10000,65534,65535};
    static const u16 expected[] = {1,9999,9999,9999,9999,0};
    s32 winner, mode, w, l;
    for (winner = 0; winner < 2; winner++)
    for (mode = 0; mode < 2; mode++)
    for (w = 0; w < 6; w++) for (l = 0; l < 6; l++) {
        setup(0xE012); gDuel_bWinnerSide = winner;
        if (mode) gDuel_bOpponentID = -1; else D_8009B360[0] = 0;
        U16(saves[winner], 0x518) = counts[w];
        U16(saves[winner ^ 1], 0x51A) = counts[l];
        U16(saves[winner], 0x51A) = 123;
        U16(saves[winner ^ 1], 0x518) = 456;
        run();
        CHECK(valid() && event_count == 1 && D_8009B16C == 0x2012 &&
              U16(saves[winner], 0x518) == expected[w] &&
              U16(saves[winner ^ 1], 0x51A) == expected[l] &&
              U16(saves[winner], 0x51A) == 123 &&
              U16(saves[winner ^ 1], 0x518) == 456, 80);
    }
    return 0;
}

static s32 helper_reloads(void)
{
    s32 result, i;
    setup(0x12); mutation = 1; run();
    CHECK(valid() && event_count == 1 && !object_count && gDuel_wSceneStateFlags == 0x8000, 90);
    setup(0x12); mutation = 2; run();
    CHECK(valid() && D_8009B1E8 == &alternate && alternate.root == &objects[2] &&
          alternate.rank_tier == 0 && alternate.starchip_prize == 1, 91);
    setup(0x12); score_input[0] = 99; mutation = 3; run();
    CHECK(valid() && D_8009B1E8->starchip_prize == 2 && object_count == 7 &&
          D_8009B1E8->children[2] == &objects[3] &&
          D_8009B1E8->children[3] == &objects[4], 92);
    for (i = 4; i < 10; i++) CHECK(!D_8009B1E8->children[i], 93);
    setup(0x8012); mutation = 4; gInput_wPad1Repeat = 0x2000; run();
    CHECK(valid() && event_count == 3 && events[2].kind == 13 && events[2].a == 2, 94);
    setup(0xC012); mutation = 5; run();
    CHECK(valid() && gDuel_wSceneStateFlags == 0xE092 && D_800E9EC8_arr[4] == 255 &&
          events[2].kind == 16 && events[2].a == 255, 95);
    return 0;
}

int main(int argc, char **argv)
{
    static const s32 cases[] = {48,32,6,6,24,6,8,12,144,5};
    s32 result, scenario;
    if (sizeof(void *) != 4 || sizeof(long) != 4 ||
        sizeof(DuelResultDisplayState) != 64 || argc != 2) return 110;
    scenario = argv[1][0] - '0';
    switch (scenario) {
    case 0: result = ranks(); break;
    case 1: result = eligibility(); break;
    case 2: result = drop_sign(); break;
    case 3: result = camera_idle(); break;
    case 4: result = pages(); break;
    case 5: result = confirm(); break;
    case 6: result = exit_handshake(); break;
    case 7: result = chip_awards(); break;
    case 8: result = win_loss_awards(); break;
    case 9: result = helper_reloads(); break;
    default: return 111;
    }
    if (result) return result;
    return executions == cases[scenario] ? 0 : 112;
}
"""


def normalized(text: str, directory: Path) -> str:
    return re.sub(
        r'^#include "([^"]+)"',
        lambda match: '#include "' + str((directory / match[1]).resolve()) + '"',
        text, flags=re.MULTILINE,
    )


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires Linux x86 and a host compiler with ILP32 execution",
)
class DuelResultControllerTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        (REPOSITORY / "tmp").mkdir(exist_ok=True)
        temporary = tempfile.TemporaryDirectory(prefix="duel-result-test-", dir=REPOSITORY / "tmp")
        cls.addClassCleanup(temporary.cleanup)
        cls.directory = Path(temporary.name)
        cls.witness = cls.directory / "witness.c"
        prefix = SOURCE.read_text().split("void DuelScene_UpdateResultRewards(void)", 1)[0]
        cls.witness.write_text(normalized(prefix, SOURCE.parent) + WITNESS)
        cls.startup = cls.directory / "start.S"
        cls.startup.write_text(START)
        # Modern GCC rejects graphics_frame.h's array of incomplete elements
        # before graphics_frame_buffer.h gets to its definition. Supply the
        # actual owner's complete layout first, not a fabricated placeholder.
        owner = (REPOSITORY / "src/game/graphics_frame_buffer.h").read_text()
        layouts = re.findall(r"struct GraphicsFrameBuffer \{.*?\n\};", owner, re.DOTALL)
        if len(layouts) != 1:
            raise AssertionError("expected one canonical graphics frame-buffer layout")
        cls.host_layout = cls.directory / "host-layout.h"
        cls.host_layout.write_text(
            '#ifndef __ASSEMBLER__\n#include "src/types.h"\n'
            '#include "src/psyq/libgte.h"\n#include "src/psyq/libgpu.h"\n'
            '#include "src/psyq/libgs.h"\n'
            + layouts[0] + "\n#endif\n"
        )
        cls.binaries = {
            optimization: cls.compile(SOURCE, optimization, "actual")
            for optimization in ("-O0", "-O2")
        }

    @classmethod
    def compile(cls, source: Path, optimization: str, label: str) -> Path:
        binary = cls.directory / (label + optimization)
        result = subprocess.run(
            ["cc", "-m32", optimization, "-std=gnu99", "-ffreestanding",
             "-fno-builtin", "-fno-strict-aliasing", "-fno-pie", "-no-pie",
             "-fno-stack-protector", "-nostdlib", f"-I{REPOSITORY}",
             "-include", str(cls.host_layout),
             str(source), str(cls.witness), str(cls.startup), "-o", str(binary)],
            cwd=REPOSITORY, env={**os.environ, "TMPDIR": str(cls.directory)},
            capture_output=True, text=True, timeout=60,
        )
        if result.returncode:
            raise AssertionError(result.stderr)
        return binary

    def witness_scenario(self, scenario: int):
        for optimization, binary in self.binaries.items():
            with self.subTest(optimization=optimization):
                result = subprocess.run(
                    [str(binary), str(scenario)], cwd=REPOSITORY,
                    capture_output=True, text=True, timeout=10,
                )
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_rank_boundaries_object_order_children_and_rewards(self):
        self.witness_scenario(0)

    def test_winner_opponent_flags_sound_and_reward_eligibility(self):
        self.witness_scenario(1)

    def test_selected_dropped_and_staged_card_sign_extension(self):
        self.witness_scenario(2)

    def test_camera_wrap_and_idle_frames(self):
        self.witness_scenario(3)

    def test_page_signed_wrap_direction_and_confirm_priority(self):
        self.witness_scenario(4)

    def test_confirmation_input_fade_and_sound(self):
        self.witness_scenario(5)

    def test_exit_fade_handshake_and_missing_save(self):
        self.witness_scenario(6)

    def test_chip_addition_wrap_cap_and_award(self):
        self.witness_scenario(7)

    def test_win_loss_halfword_wrap_cap_and_other_side(self):
        self.witness_scenario(8)

    def test_opaque_helpers_require_pointer_state_and_page_reloads(self):
        self.witness_scenario(9)

    def mutant(self, old: str, new: str, scenario: int, expected: range):
        text = SOURCE.read_text()
        self.assertEqual(text.count(old), 1, "semantic mutation must identify one operation")
        source = self.directory / f"mutant-{scenario}.c"
        source.write_text(normalized(text.replace(old, new), SOURCE.parent))
        binary = self.compile(source, "-O2", f"mutant-{scenario}")
        result = subprocess.run(
            [str(binary), str(scenario)], cwd=REPOSITORY,
            capture_output=True, text=True, timeout=10,
        )
        self.assertIn(result.returncode, expected, result.stderr)

    def test_wrong_rank_divisor_is_detected(self):
        self.mutant("score / 10", "score / 11", 0, range(10, 22))

    def test_wrong_chip_spacing_is_detected(self):
        self.mutant("x += 20", "x += 19", 0, range(10, 22))

    def test_wrong_page_direction_is_detected(self):
        self.mutant("page_index - 2", "page_index - 1", 4, range(40, 41))


if __name__ == "__main__":
    unittest.main()
