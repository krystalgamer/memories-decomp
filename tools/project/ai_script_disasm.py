#!/usr/bin/env python3
"""Disassemble the opponent AI scripts of Yu-Gi-Oh! Forbidden Memories.

    python tools/project/ai_script_disasm.py [--mrg game/DATA/WA_MRG.MRG]
                                             [--out tmp/ai-scripts] [--cards CSV]

The duel package loader (src/game/duel_load_package_stage.c, phases 8 and 9)
copies two 3-sector chunks of the terrain record at WA_MRG.MRG sector
0x16C6 + terrain * 0xEB to 0x801A8000 (hand script, relative sectors
184-186) and 0x801A9800 (field script, relative sectors 187-189).
DuelScene_UpdateHandActions calls AiScript_Init(0x801A8000) and
DuelScene_UpdateFieldActions calls AiScript_Init(0x801A9800); execution
always starts at offset 0 of the chunk.  Each chunk is one program shared
by every duelist: it reads the duelist id (opcode 0x15) and branches on it.

The tool
  * checks that the seven terrain records carry identical chunks,
  * decodes each chunk by recursive descent from offset 0 (operand lengths
    and branch semantics come from the handlers in src/game/ai_script_*.c,
    ai_fusion.c and ai_script_end.c; see notes/ai-scripts.md),
  * accounts for every byte of the 0x1800-byte chunk (code, unreachable gaps),
  * runs a constant-propagation pass per duelist id (1-39) to find the part
    of each program that duelist can reach, resolving branches whose
    operands are known (Store constants, the duelist id, the duelist's row
    of gDuel_aOpponentData parsed from src/game/ai_opponent_data.c),
  * writes full listings, one listing per duelist, an index and a
    verification report to --out.

The output is a dump of game data: keep it out of the repository (tmp/ is
ignored).  Only Python 3 standard library is needed.
"""
import argparse
import collections
import csv
import hashlib
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))

SECTOR = 2048
TERRAIN_FIRST_SECTOR = 0x16C6       # DUEL_TERRAIN_PACKAGE_FIRST_SECTOR
TERRAIN_SECTORS = 0xEB              # DUEL_TERRAIN_PACKAGE_SECTOR_COUNT
TERRAIN_COUNT = 7
CHUNKS = (                          # name, relative sector, RAM base, end op
    ('hand', 184, 0x801A8000),
    ('field', 187, 0x801A9800),
)
CHUNK_SIZE = 3 * SECTOR
MEMORY_COUNT = 20                   # AI_SCRIPT_MEMORY_COUNT
RETURN_STACK = 8                    # AI_SCRIPT_RETURN_STACK_COUNT
OPPONENT_IDS = range(1, 40)

# Operand kinds: 'T' 16-bit script-relative target, 'I' 16-bit immediate,
# 'B' raw byte, 'D' destination register, anything else a source register
# with a semantic tag used to print a known value.
#   card  card id (1-722)          slot  gDuel_aActiveCards index
#   crng  Ai_GetCardRange kind     wrng  Ai_GetWinningCardRange kind
#   pct   percent (rand()%100 <)   stat  0 ATK, 1 DEF, else higher
#   sets  1 card set, 2 type set   type  card type
#   face  0 empty, 1 up, 2 down    side  duelist record selector
#   flag  0/1                      opp   duelist id
#   odf   gDuel_aOpponentData field  depth  fusion depth-1   val  plain value
OPS = {
    0x01: ('Jump', 'T', 'control_flow'),
    0x02: ('JumpGreaterEqual', 'val val T', 'control_flow'),
    0x03: ('JumpGreater', 'val val T', 'control_flow'),
    0x04: ('JumpEqual', 'val val T', 'control_flow'),
    0x05: ('JumpNotEqual', 'val val T', 'control_flow'),
    0x06: ('JumpBetween', 'val val val T', 'control_flow'),
    0x07: ('JumpRandom', 'pct T', 'control_flow'),
    0x08: ('Call', 'T', 'control_flow'),
    0x09: ('Return', '', 'control_flow'),
    0x0A: ('SetRandom', 'I I D', 'control_flow'),
    0x0B: ('Subtract', 'val val D', 'control_flow'),
    0x0C: ('PlayFieldCard', 'slot val val val', 'end'),
    0x0D: ('EndHand', '', 'end'),
    0x0E: ('EndField', '', 'end'),
    0x0F: ('CalcCardPower', 'card stat D', 'calc_card_power'),
    0x10: ('TestHighStat', 'card D', 'query_commands'),
    0x11: ('LoadCardType', 'card D', 'query_commands'),
    0x12: ('LoadCardID', 'slot D', 'query_commands'),
    0x13: ('SetFaceDown', 'slot D', 'query_commands'),
    0x14: ('LoadLifePoint', 'side D', 'query_commands'),
    0x15: ('LoadOpponentID', 'D', 'load_duel_globals'),
    0x16: ('LoadTerrain', 'D', 'load_duel_globals'),
    0x17: ('LoadDeckSize', 'D', 'combo'),
    0x18: ('LoadSelectionRandom', 'D', 'combo'),
    0x19: ('TestPinned', 'side D', 'combo'),
    0x1A: ('StartCombo', 'D', 'combo'),
    0x1B: ('LoadBestDifference', 'D', 'combo'),
    0x1C: ('LoadBestAttacker', 'D', 'combo'),
    0x1D: ('LoadBestTarget', 'D', 'combo'),
    0x1E: ('FindStrongest', 'stat wrng flag sets D', 'combo'),
    0x1F: ('FindWeakest', 'stat wrng flag sets D', 'combo'),
    0x20: ('FindKiller', 'slot stat D', 'find_killer'),
    0x21: ('FindDefenseStopper', 'flag D', 'find_card'),
    0x22: ('CountCards', 'crng D', 'find_card'),
    0x23: ('FindFirstCard', 'face crng D', 'find_card'),
    0x24: ('FindCard', 'card crng flag D', 'find_card'),
    0x25: ('MatchType', 'type crng flag sets D', 'match_type'),
    0x26: ('FindEquipTarget', 'card flag sets D', 'fusion'),
    0x27: ('CheckRitual', 'card D', 'fusion'),
    0x28: ('FindFirstMonster', 'D', 'fusion'),
    0x29: ('FindFirstType', 'type D', 'fusion'),
    0x2A: ('FindBestCombo', 'depth sets D', 'fusion'),
    0x2B: ('EvaluateFusion', 'card depth sets D', 'fusion'),
    0x2C: ('SkipHand', 'B B B B', 'skip'),
    0x2D: ('SkipField', 'B B B B', 'skip'),
    0x2E: ('PlayFaceUp', 'slot slot slot slot slot', 'actions'),
    0x2F: ('SetPosition', 'val', 'actions'),
    0x30: ('FindBestAttack', 'flag D', 'find_best_attack'),
    0x31: ('PushComboCard', '', 'state_ops'),
    0x32: ('PushComboEmpty', '', 'state_ops'),
    0x33: ('HandNop', '', 'state_ops'),
    0x34: ('FieldNop', '', 'state_ops'),
    0x35: ('AddCard', 'card', 'state_ops'),
    0x36: ('ClearCards', '', 'state_ops'),
    0x37: ('AddType', 'type', 'state_ops'),
    0x38: ('ClearTypes', '', 'state_ops'),
    0x39: ('SetSelectionRandom', '', 'state_ops'),
    0x3A: ('ClearSelectionRandom', '', 'state_ops'),
    0x3B: ('SetFlag97', '', 'state_ops'),      # func_80073464
    0x3C: ('ClearFlag97', '', 'state_ops'),    # func_80073474
    0x3D: ('MoveCard', 'slot', 'state_ops'),
    0x3E: ('LoadOpponentData', 'opp odf D', 'state_ops'),
    0x3F: ('Store', 'I D', 'state_ops'),
    0x40: ('SetRegister', 'val D', 'state_ops'),
    0x41: ('Add', 'val val D', 'state_ops'),
    0x42: ('LoadGuardianStarChoice', 'val', 'state_ops'),
    0x43: ('Print', 'B', 'state_ops'),
}
BRANCHES = {0x02, 0x03, 0x04, 0x05, 0x06, 0x07}
TERMINAL = {0x09, 0x0C, 0x0D, 0x0E, 0x01}   # no fall-through
RUN_ENDS = {0x0C: 2, 0x0D: 1, 0x0E: 3}      # AiScript_Run return values


def op_len(op):
    n = 1
    for k in OPS[op][1].split():
        n += 2 if k in ('T', 'I') else 1
    return n


TYPES = ['Dragon', 'Spellcaster', 'Zombie', 'Warrior', 'Beast-Warrior',
         'Beast', 'Winged Beast', 'Fiend', 'Fairy', 'Insect', 'Dinosaur',
         'Reptile', 'Fish', 'Sea Serpent', 'Machine', 'Thunder', 'Aqua',
         'Pyro', 'Rock', 'Plant', 'Magic', 'Trap', 'Ritual', 'Equip']
TERRAINS = ['Normal', 'Forest', 'Wasteland', 'Mountain', 'Meadow', 'Sea',
            'Dark']
CARD_RANGE = {0: 'AI monsters', 1: 'AI monsters not used this turn',
              2: 'AI magic/trap row', 3: 'AI magic/trap row not used',
              4: 'AI hand', 5: 'player monsters',
              6: 'player monsters not used', 7: 'player magic/trap row',
              8: 'player magic/trap row not used', 9: 'player hand'}
WIN_RANGE = {0: 'AI monsters', 1: 'AI monsters not used this turn',
             2: 'AI hand', 3: 'player monsters',
             4: 'player attack-position monsters not used',
             5: 'player defence-position monsters not used',
             6: 'player hand'}
DUELISTS = """Simon Muran|Teana|Jono|Villager 1|Villager 2|Villager 3|Seto|
Heishin|Rex Raptor|Weevil Underwood|Mai Valentine|Bandit Keith|Shadi|
Yami Bakura|Pegasus|Isis|Kaiba|Mage Soldier|Jono 2nd|Teana 2nd|Ocean Mage|
High Mage Secmeton|Forest Mage|High Mage Anubisius|Mountain Mage|
High Mage Atenza|Desert Mage|High Mage Martis|Meadow Mage|High Mage Kepura|
Labyrinth Mage|Seto 2nd|Guardian Sebek|Guardian Neku|Heishin 2nd|Seto 3rd|
DarkNite|Nitemare|Duel Master K""".replace('\n', '').split('|')


def duelist(i):
    return DUELISTS[i - 1] if 1 <= i <= len(DUELISTS) else 'duelist %d' % i


def load_cards(path):
    names = {}
    if path and os.path.exists(path):
        with open(path, newline='', encoding='utf-8') as f:
            for row in csv.DictReader(f):
                names[int(row['id'])] = row['name']
    return names


def load_opponent_data():
    """Parse gDuel_aOpponentData (0x800917F0) from its C definition."""
    src = os.path.join(ROOT, 'src', 'game', 'ai_opponent_data.c')
    text = open(src, encoding='utf-8').read()
    rows = re.findall(r'\{\{([-\d,\s]+)\}\}', text)
    return [[int(v) for v in r.split(',')] for r in rows]


class Insn:
    __slots__ = ('addr', 'op', 'raw', 'args', 'kinds', 'target', 'size')

    def __init__(self, data, addr):
        self.addr = addr
        self.op = data[addr]
        if self.op not in OPS:
            raise ValueError('unknown opcode 0x%02X at 0x%04X' % (self.op, addr))
        self.kinds = OPS[self.op][1].split()
        self.size = op_len(self.op)
        if addr + self.size > len(data):
            raise ValueError('instruction at 0x%04X runs off the chunk' % addr)
        self.raw = data[addr:addr + self.size]
        self.args = []
        self.target = None
        p = addr + 1
        for k in self.kinds:
            if k in ('T', 'I'):
                v = data[p] | (data[p + 1] << 8)
                p += 2
                if k == 'T':
                    self.target = v
            else:
                v = data[p]
                p += 1
            self.args.append(v)

    @property
    def name(self):
        return OPS[self.op][0]

    def successors(self):
        out = []
        if self.op not in TERMINAL:
            out.append(self.addr + self.size)
        if self.target is not None:
            out.append(self.target)
        return out

    def regs(self):
        """(register, kind) for every register operand."""
        return [(a, k) for a, k in zip(self.args, self.kinds)
                if k not in ('T', 'I', 'B')]


def descend(data):
    """Recursive descent from offset 0; returns ({addr: Insn}, problems)."""
    insns, problems, work = {}, [], [0]
    while work:
        a = work.pop()
        if a in insns:
            continue
        if not 0 <= a < len(data):
            problems.append('target 0x%04X outside the chunk' % a)
            continue
        try:
            i = Insn(data, a)
        except ValueError as e:
            problems.append(str(e))
            continue
        insns[a] = i
        work.extend(i.successors())
    # Overlap check: no instruction start may fall inside another.
    covered = {}
    for a, i in insns.items():
        for b in range(a, a + i.size):
            if b in covered:
                problems.append('bytes overlap at 0x%04X (0x%04X and 0x%04X)'
                                % (b, covered[b], a))
            covered[b] = a
    return insns, problems


# ---------------------------------------------------------------- analysis
UNK = None


def join(a, b):
    return tuple(x if x == y else (x[0] if x[0] == y[0] else UNK,
                                   x[1] if x[1] == y[1] else None)
                 for x, y in zip(a, b))


def interpret(insns, opp, odata):
    """Constant propagation over (pc, return stack).  Each register is a
    (value, tag) pair; value None = unknown.  opp None = any duelist.
    Returns ({pc: joined regs}, {pc: set(resolved outcomes)}, issues)."""
    start = tuple((0, None) for _ in range(MEMORY_COUNT))  # bzero'd by Init
    states = {(0, ()): start}
    work = [(0, ())]
    issues = set()
    outcomes = collections.defaultdict(set)
    while work:
        key = work.pop()
        pc, stack = key
        regs = list(states[key])
        i = insns.get(pc)
        if i is None:
            issues.add('reached undecoded 0x%04X' % pc)
            continue
        for r, _ in i.regs():
            if r >= MEMORY_COUNT:
                issues.add('register r%d >= %d at 0x%04X' % (r, MEMORY_COUNT, pc))
        a = i.args
        V = lambda r: regs[r][0] if r < MEMORY_COUNT else UNK
        T = lambda r: regs[r][1] if r < MEMORY_COUNT else None

        def put(r, v, tag=None):
            if r < MEMORY_COUNT:
                regs[r] = (v, tag)

        nxt = []
        fall = pc + i.size
        op = i.op
        if op == 0x01:
            nxt = [(i.target, stack)]
        elif op in BRANCHES:
            if op == 0x06:
                v, hi, lo = V(a[0]), V(a[1]), V(a[2])
                cond = None if UNK in (v, hi, lo) else (lo <= v <= hi)
            elif op == 0x07:
                p = V(a[0])
                cond = None if p is UNK else (True if p >= 100 else
                                              False if p <= 0 else None)
            else:
                x, y = V(a[0]), V(a[1])
                if UNK in (x, y):
                    cond = None
                else:
                    cond = {0x02: x >= y, 0x03: x > y, 0x04: x == y,
                            0x05: x != y}[op]
            outcomes[pc].add(cond)
            if cond is not False:
                nxt.append((i.target, stack))
            if cond is not True:
                nxt.append((fall, stack))
        elif op == 0x08:
            if len(stack) >= RETURN_STACK:
                issues.add('call stack overflow at 0x%04X' % pc)
            else:
                nxt = [(i.target, stack + (fall,))]
        elif op == 0x09:
            if not stack:
                issues.add('return with empty stack at 0x%04X' % pc)
            else:
                nxt = [(stack[-1], stack[:-1])]
        elif op in RUN_ENDS:
            nxt = []
        else:
            d = a[-1] if i.kinds and i.kinds[-1] == 'D' else None
            if op == 0x3F:
                put(d, a[0], None)
            elif op == 0x15:
                put(d, opp, 'opp')
            elif op == 0x40:
                put(a[1], V(a[0]), T(a[0]))
            elif op in (0x0B, 0x41):
                x, y = V(a[0]), V(a[1])
                v = UNK if UNK in (x, y) else (x - y if op == 0x0B else x + y)
                put(d, v)
            elif op == 0x3E:
                o, f = V(a[0]), V(a[1])
                v = UNK
                if o is not UNK and f is not UNK and 0 <= o < len(odata):
                    row = odata[o]
                    if f == 0:
                        v = row[1] * 100
                    elif 0 <= f + 1 < len(row):
                        v = row[f + 1]
                put(d, v, 'odf%s' % ('?' if f is UNK else f))
            elif d is not None:
                tag = {0x16: 'terrain', 0x12: 'card', 0x11: 'type',
                       0x13: 'face', 0x14: 'lp'}.get(op)
                put(d, UNK, tag)
            nxt = [(fall, stack)]
        for k in nxt:
            new = tuple(regs)
            old = states.get(k)
            if old is None:
                states[k] = new
                work.append(k)
            else:
                j = join(old, new)
                if j != old:
                    states[k] = j
                    work.append(k)
    per_pc = {}
    for (pc, _), regs in states.items():
        per_pc[pc] = regs if pc not in per_pc else join(per_pc[pc], regs)
    return per_pc, outcomes, issues


# -------------------------------------------------------------- formatting
def fmt_value(kind, v, cards, tag=None):
    if kind == 'val' and tag:
        kind = {'opp': 'opp', 'terrain': 'terrain', 'card': 'card',
                'type': 'type', 'face': 'face'}.get(tag, kind)
    if kind == 'card':
        n = cards.get(v)
        return '#%d %s' % (v, n) if n else '#%d' % v
    if kind == 'crng':
        return CARD_RANGE.get(v, 'range %d' % v)
    if kind == 'wrng':
        return WIN_RANGE.get(v, 'range %d' % v)
    if kind == 'pct':
        return '%d%%' % v
    if kind == 'stat':
        return {0: 'ATK', 1: 'DEF'}.get(v, 'higher of ATK/DEF')
    if kind == 'sets':
        return {1: 'exclude card set', 2: 'exclude type set'}.get(v, 'no exclusion')
    if kind == 'type':
        return TYPES[v] if 0 <= v < len(TYPES) else 'type %d' % v
    if kind == 'face':
        return {0: 'empty', 1: 'face-up', 2: 'face-down'}.get(v, str(v))
    if kind == 'opp':
        return '%d %s' % (v, duelist(v))
    if kind == 'terrain':
        return TERRAINS[v] if 0 <= v < len(TERRAINS) else 'terrain %d' % v
    if kind == 'slot':
        if v == 0:
            return 'slot 0 (none)'
        for base, what in ((66, 'player hand'), (61, 'player M/T'),
                           (56, 'player monster'), (11, 'AI hand'),
                           (6, 'AI M/T'), (1, 'AI monster')):
            if v >= base:
                return 'slot %d (%s %d)' % (v, what, v - base + 1)
    return str(v)


def fmt_insn(i, regs, cards, labels):
    parts = []
    tags = [regs[r][1] if regs and r < MEMORY_COUNT else None
            for r in i.args] if regs else [None] * len(i.args)
    # A compare against a tagged register prints the constant with that tag.
    cmp_tag = None
    if i.op in (0x02, 0x03, 0x04, 0x05, 0x06) and regs:
        for r in i.args[:-1]:
            if r < MEMORY_COUNT and regs[r][1]:
                cmp_tag = regs[r][1]
    for n, (v, k) in enumerate(zip(i.args, i.kinds)):
        if k == 'T':
            parts.append(labels.get(v, 'L_%04X' % v))
        elif k == 'I':
            parts.append('%d' % v)
        elif k == 'B':
            parts.append('0x%02X' % v)
        elif k == 'D':
            parts.append('->r%d' % v)
        else:
            s = 'r%d' % v
            if regs and v < MEMORY_COUNT:
                val, tag = regs[v]
                kk = k
                if val is not UNK:
                    if k == 'val' and not tag and cmp_tag:
                        tag = cmp_tag
                    s += '=' + fmt_value(kk, val, cards, tag)
                elif tag:
                    s += '(%s)' % tag
            parts.append(s)
    return '%-20s %s' % (i.name, ', '.join(parts))


def comment(i, regs, cards):
    """Store with a value that a later use gives meaning to is left plain;
    a few opcodes get a fixed explanatory comment."""
    c = {0x0C: 'ends the run (AiScript_Run returns 2)',
         0x0D: 'ends the run (returns 1)', 0x0E: 'ends the run (returns 3)',
         0x07: 'jump if rand()%100 < pct'}.get(i.op)
    return c


def ranges(ids):
    ids = sorted(ids)
    out, s = [], None
    for n, v in enumerate(ids):
        if s is None:
            s = p = v
        elif v == p + 1:
            p = v
        else:
            out.append('%d' % s if s == p else '%d-%d' % (s, p))
            s = p = v
    if s is not None:
        out.append('%d' % s if s == p else '%d-%d' % (s, p))
    return ','.join(out)


def listing(name, base, data, insns, reach, regs_at, cards, only=None,
            outcomes=None):
    targets = {i.target for i in insns.values() if i.target is not None}
    labels = {t: 'L_%04X' % t for t in targets}
    lines = []
    for a in sorted(insns):
        if only is not None and a not in only:
            continue
        i = insns[a]
        if a in targets:
            who = reach.get(a)
            extra = ''
            if only is None and who is not None:
                extra = '   ; reached by duelists %s' % (ranges(who) or 'none')
            lines.append('')
            lines.append('%s:%s' % (labels[a], extra))
        regs = regs_at.get(a)
        text = fmt_insn(i, regs, cards, labels)
        c = comment(i, regs, cards)
        if outcomes is not None and a in outcomes:
            o = outcomes[a]
            if o == {True}:
                c = (c + '; ' if c else '') + 'always taken here'
            elif o == {False}:
                c = (c + '; ' if c else '') + 'never taken here'
        lines.append('  %08X %04X  %-16s %s%s' % (
            base + a, a, i.raw.hex(' '), text, ('   ; ' + c) if c else ''))
    return lines


def chance_split(insns, regs, pc):
    """From pc, follow Store / Jump / JumpRandom (with a known percent) and
    return {landing address: probability} for the first other instruction
    reached on each path.  This is how a chain of JumpRandom rolls turns
    into the effective chance of each strategy."""
    out = collections.defaultdict(float)
    work = [(pc, 1.0, 0, pc)]          # address, probability, steps, label
    while work:
        a, p, n, lab = work.pop()
        i = insns.get(a)
        r = regs.get(a)
        if i is None or r is None or n > 64:
            out[lab] += p
            continue
        if i.op == 0x3F:
            work.append((a + i.size, p, n + 1, lab))
        elif i.op == 0x01:
            work.append((i.target, p, n + 1, i.target))
        elif i.op == 0x07 and r[i.args[0]][0] is not UNK:
            q = min(max(r[i.args[0]][0], 0), 100) / 100.0
            if q:
                work.append((i.target, p * q, n + 1, i.target))
            if q < 1:
                work.append((a + i.size, p * (1 - q), n + 1, a + i.size))
        else:
            out[lab] += p
    return dict(out)


def chance_points(insns, regs):
    """JumpRandom chains in a duelist's reachable code: the first roll of
    each chain (a JumpRandom not reached by falling through another)."""
    pts = []
    for a in sorted(regs):
        i = insns[a]
        if i.op != 0x07:
            continue
        # walk back over Store lines to see if a JumpRandom falls into us
        prev = [b for b in regs if b < a and b + insns[b].size == a]
        b = prev[0] if prev else None
        while b is not None and insns[b].op == 0x3F:
            pv = [c for c in regs if c < b and c + insns[c].size == b]
            b = pv[0] if pv else None
        if b is not None and insns[b].op == 0x07:
            continue
        s = a
        while True:   # start the walk at the Store feeding the roll, if any
            pv = [c for c in regs if c < s and c + insns[c].size == s]
            if pv and insns[pv[0]].op == 0x3F:
                s = pv[0]
            else:
                break
        pts.append((a, chance_split(insns, regs, s)))
    return pts


def classify_gap(data, lo, hi):
    b = data[lo:hi]
    if all(x == 0 for x in b):
        return 'zero fill'
    # Does a linear decode of the gap run cleanly to its end?
    p, ok, n = lo, True, 0
    while p < hi:
        if data[p] not in OPS or p + op_len(data[p]) > hi:
            ok = False
            break
        p += op_len(data[p])
        n += 1
    if ok:
        return 'decodes cleanly (%d instructions), unreachable' % n
    return 'not a clean instruction stream (first byte 0x%02X)' % b[0]


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--mrg', default=os.path.join(ROOT, 'game', 'DATA', 'WA_MRG.MRG'))
    ap.add_argument('--out', default=os.path.join(ROOT, 'tmp', 'ai-scripts'))
    ap.add_argument('--cards', default=os.path.join(ROOT, 'notes', 'card-catalog.csv'),
                    help='optional id,name CSV for card names')
    args = ap.parse_args()
    mrg = open(args.mrg, 'rb').read()
    cards = load_cards(args.cards)
    odata = load_opponent_data()
    os.makedirs(args.out, exist_ok=True)
    report = []
    R = report.append

    chunks = {}
    for name, rel, base in CHUNKS:
        hashes = set()
        for t in range(TERRAIN_COUNT):
            off = (TERRAIN_FIRST_SECTOR + t * TERRAIN_SECTORS + rel) * SECTOR
            blob = mrg[off:off + CHUNK_SIZE]
            hashes.add(hashlib.sha256(blob).hexdigest())
            if t == 0:
                chunks[name] = (base, blob)
        R('%s chunk: %d distinct SHA-256 over the %d terrain records (%s)'
          % (name, len(hashes), TERRAIN_COUNT, sorted(hashes)[0][:16]))

    all_ok = True
    per_opp = {}
    for name, (base, data) in chunks.items():
        insns, problems = descend(data)
        all_ok &= not problems
        size = sum(i.size for i in insns.values())
        last = max(insns)
        R('')
        R('== %s script (0x%08X, 0x%X bytes) ==' % (name, base, len(data)))
        R('instructions decoded by recursive descent: %d, %d bytes'
          % (len(insns), size))
        R('unknown opcodes / bad targets / overlaps: %d' % len(problems))
        for p in problems:
            R('  ' + p)
        targets = [i.target for i in insns.values() if i.target is not None]
        bad = [t for t in targets if t not in insns]
        R('branch/call targets: %d (%d distinct), not on an instruction start: %d'
          % (len(targets), len(set(targets)), len(bad)))
        R('highest target 0x%04X (chunk size 0x%04X)' % (max(targets), len(data)))
        R('last instruction 0x%04X %s; code extent 0x0000-0x%04X'
          % (last, insns[last].name, last + insns[last].size))
        cov = bytearray(len(data))
        for a, i in insns.items():
            for b in range(a, a + i.size):
                cov[b] = 1
        gaps, a = [], 0
        while a < len(data):
            if not cov[a]:
                b = a
                while b < len(data) and not cov[b]:
                    b += 1
                gaps.append((a, b))
                a = b
            else:
                a += 1
        R('bytes: %d code + %d not reached = %d' % (
            sum(cov), len(data) - sum(cov), len(data)))
        for lo, hi in gaps:
            R('  gap 0x%04X-0x%04X (%d bytes): %s' % (
                lo, hi, hi - lo, classify_gap(data, lo, hi)))
            for other, (_, odat) in chunks.items():
                if other != name and data[lo:hi] == odat[lo:hi]:
                    R('    identical to the %s chunk at the same offsets'
                      % other)
        # A linear sweep from 0 must meet exactly the same instruction starts
        # up to the end of the code: no data is interleaved with the code.
        p, sweep = 0, set()
        while p < last + insns[last].size and data[p] in OPS:
            sweep.add(p)
            p += op_len(data[p])
        R('linear sweep 0x0000-0x%04X agrees with the descent: %s'
          % (p, 'yes' if sweep == set(insns) else 'NO'))
        all_ok &= sweep == set(insns)
        hist = collections.Counter(i.name for i in insns.values())
        R('opcodes used (%d of %d): %s' % (len(hist), len(OPS), ', '.join(
            '%s %d' % kv for kv in sorted(hist.items()))))
        unused = sorted(set(v[0] for v in OPS.values()) - set(hist))
        R('opcodes never used: %s' % ', '.join(unused))

        # Whole-program pass (duelist unknown) and one pass per duelist.
        regs_any, out_any, iss = interpret(insns, None, odata)
        for s in sorted(iss):
            R('  analysis (any duelist): ' + s)
        reach = collections.defaultdict(set)
        res = {}
        for o in OPPONENT_IDS:
            regs_o, out_o, iss_o = interpret(insns, o, odata)
            for s in sorted(iss_o):
                R('  analysis (duelist %d): %s' % (o, s))
                all_ok = False
            for a in regs_o:
                reach[a].add(o)
            res[o] = (regs_o, out_o)
        per_opp[name] = (base, data, insns, res, reach)
        never = [a for a in insns if not reach.get(a)]
        R('instructions reachable by no duelist 1-39: %d' % len(never))
        with open(os.path.join(args.out, '%s_full.txt' % name), 'w',
                  encoding='utf-8') as f:
            f.write('; %s script, 0x%08X, all code reachable from offset 0\n'
                    % (name, base))
            f.write('; register values shown are those known for every '
                    'duelist\n')
            f.write('\n'.join(listing(name, base, data, insns, reach,
                                      regs_any, cards)) + '\n')

    # Per-duelist files and index.
    index = ['# AI scripts by duelist', '',
             '| id | duelist | hand insns | field insns | opponent data |'
             ' duelist-only chance splits |',
             '|---:|---|---:|---:|---|---|']
    for o in OPPONENT_IDS:
        fn = '%02d_%s.txt' % (o, re.sub(r'[^A-Za-z0-9]+', '_', duelist(o)).strip('_'))
        lines = ['; duelist %d: %s' % (o, duelist(o)),
                 '; gDuel_aOpponentData[%d] = %s' % (o, odata[o]),
                 ';   [0] hand/fusion window (Ai_GetHandSize); script fields '
                 '0..7 read values[1]*100, values[2..8]', '']
        counts, own = [], []
        for name in ('hand', 'field'):
            base, data, insns, res, reach = per_opp[name]
            regs_o, out_o = res[o]
            lines.append('==== %s script: %d reachable instructions ===='
                         % (name, len(regs_o)))
            for a, split in chance_points(insns, regs_o):
                txt = ', '.join('L_%04X %.1f%%' % (t, 100 * q) for t, q in
                                sorted(split.items(), key=lambda kv: -kv[1]))
                mine = reach.get(a) == {o}
                lines.append('; chance at %04X%s: %s' % (
                    a, ' (this duelist only)' if mine else '', txt))
                if mine:
                    own.append('%s %04X: %s' % (name, a, txt))
            lines += listing(name, base, data, insns, {}, regs_o, cards,
                             only=set(regs_o), outcomes=out_o)
            lines.append('')
            counts.append(len(regs_o))
        with open(os.path.join(args.out, fn), 'w', encoding='utf-8') as f:
            f.write('\n'.join(lines) + '\n')
        index.append('| %d | [%s](%s) | %d | %d | %s | %s |' % (
            o, duelist(o), fn, counts[0], counts[1], odata[o],
            '<br>'.join(own)))
    with open(os.path.join(args.out, 'index.md'), 'w', encoding='utf-8') as f:
        f.write('\n'.join(index) + '\n')
    R('')
    R('card names: %s' % ('%d from %s' % (len(cards), args.cards) if cards
                          else 'none (ids only)'))
    R('result: %s' % ('OK' if all_ok else 'PROBLEMS FOUND'))
    with open(os.path.join(args.out, 'verify.txt'), 'w', encoding='utf-8') as f:
        f.write('\n'.join(report) + '\n')
    print('\n'.join(report))
    return 0 if all_ok else 1


if __name__ == '__main__':
    sys.exit(main())
