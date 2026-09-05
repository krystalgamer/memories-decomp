#!/usr/bin/env python3
"""Extract the fusion, equip and drop tables from a retail disc image.

    extract_tables.py <disc.bin> [out_dir]

The disc argument is a raw MODE2/2352 .bin (what the .cue names). Sector
numbers below are relative to the start of DATA/WA_MRG.MRG, whose LBA comes
from config/slus_01411/disc_layout.json.

Writes tables.json next to the CSVs; compile_tables.py turns that into the
CSVs in this directory. Every invariant the notes state is asserted here, so a
run that completes is a run that agreed with them.
"""
import json
import os
import sys

RAW, HDR, USER = 2352, 24, 2048     # MODE2/2352: 24-byte header, 2048 of data
WA_LBA = 10102                      # DATA/WA_MRG.MRG
DUEL_SECTOR = 0x16C6                # duel blob, 235 sectors, one copy per terrain
DUELIST_SECTOR = 0x1D33             # per-duelist block, 3 sectors each
DUELISTS = 40
EQUIP_OFF, EQUIP_LEN = 0x22000, 0x2800
FUSION_OFF, FUSION_LEN = 0x24800, 0x10000
POOLS = (('deck', 0x000), ('pow', 0x5B4), ('bcd', 0xB68), ('tec', 0x111C))
CARDS = 722


def read_sectors(path, lba, count):
    out = bytearray()
    with open(path, 'rb') as f:
        for i in range(count):
            f.seek((lba + i) * RAW + HDR)
            block = f.read(USER)
            if len(block) != USER:
                sys.exit('disc image is short at sector %d' % (lba + i))
            out += block
    return bytes(out)


def decode_fusion(base):
    """Transcription of Duel_CheckFusion (0x80019A60), src/game/duel_card_checks.c.

    A pair is stored once, under the smaller id, with the larger as partner.
    Ids are ten bits; the four high halves are packed two bits each into the
    first byte of every five-byte group, which carries two pairs.
    """
    recipes = []
    for a in range(CARDS + 1):
        off = int.from_bytes(base[a * 2:a * 2 + 2], 'little')
        if off == 0:
            continue
        p = off
        n = base[p]
        if n == 0:                       # counts of 256..511 take a second byte
            n = 0x1FF - base[p + 1]
            p += 1
        p += 1
        while n > 0:
            g = base[p]
            recipes.append((a, ((g << 8) & 0x300) | base[p + 1],
                               ((g << 6) & 0x300) | base[p + 2]))
            if n > 1:                    # an odd count leaves the second pair unused
                recipes.append((a, ((g << 4) & 0x300) | base[p + 3],
                                   ((g << 2) & 0x300) | base[p + 4]))
            p += 5
            n -= 2
    return recipes


def decode_equip(base):
    """Transcription of Duel_CheckEquip (0x80019A08): u16 key, u16 count, u16 list."""
    pairs = []
    p = 0
    while True:
        key = int.from_bytes(base[p:p + 2], 'little')
        if key == 0:
            return pairs
        n = int.from_bytes(base[p + 2:p + 4], 'little')
        p += 4
        for i in range(n):
            pairs.append((key, int.from_bytes(base[p + 2 * i:p + 2 * i + 2], 'little')))
        p += 2 * n


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    disc = sys.argv[1]
    out_dir = sys.argv[2] if len(sys.argv) > 2 else os.path.dirname(os.path.abspath(__file__))

    blob = read_sectors(disc, WA_LBA + DUEL_SECTOR, 235)
    recipes = decode_fusion(blob[FUSION_OFF:FUSION_OFF + FUSION_LEN])
    equips = decode_equip(blob[EQUIP_OFF:EQUIP_OFF + EQUIP_LEN])

    drops = {}
    for did in range(DUELISTS):
        block = read_sectors(disc, WA_LBA + DUELIST_SECTOR + 3 * did, 3)
        drops[did] = {
            key: [int.from_bytes(block[off + 2 * i:off + 2 * i + 2], 'little')
                  for i in range(CARDS)]
            for key, off in POOLS
        }

    # The invariants notes/research/the-game.md states.
    results = {r for _, _, r in recipes}
    assert len(recipes) == 25131, len(recipes)
    assert len(results) == 204, len(results)
    assert len(equips) == 4041, len(equips)
    assert len({e for e, _ in equips}) == 34
    assert all(1 <= i <= CARDS for row in recipes for i in row[1:])
    for did, pools in drops.items():
        for key, weights in pools.items():
            assert sum(weights) == 2048, (did, key, sum(weights))

    path = os.path.join(out_dir, 'tables.json')
    json.dump({'recipes': recipes, 'equips': equips,
               'drops': {str(k): v for k, v in drops.items()}}, open(path, 'w'))
    print('%s: %d recipes, %d results, %d equip pairs, %d weight tables at 2048'
          % (path, len(recipes), len(results), len(equips), DUELISTS * len(POOLS)))


if __name__ == '__main__':
    main()
