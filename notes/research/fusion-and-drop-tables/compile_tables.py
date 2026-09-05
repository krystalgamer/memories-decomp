#!/usr/bin/env python3
"""Turn extract_tables.py's tables.json into the CSVs in this directory.

    compile_tables.py [tables.json]

Card names come from notes/card-catalog.csv; duelist names are the block order
established in notes/research/the-game.md §6.4.
"""
import collections
import csv
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..', '..'))

DUELISTS = """1 Simon Muran|2 Teana|3 Jono|4 Villager 1|5 Villager 2|6 Villager 3|7 Seto
|8 Heishin|9 Rex Raptor|10 Weevil Underwood|11 Mai Valentine|12 Bandit Keith|13 Shadi
|14 Yami Bakura|15 Pegasus|16 Isis|17 Kaiba|18 Mage Soldier|19 Jono 2nd|20 Teana 2nd
|21 Ocean Mage|22 High Mage Secmeton|23 Forest Mage|24 High Mage Anubisius|25 Mountain Mage
|26 High Mage Atenza|27 Desert Mage|28 High Mage Martis|29 Meadow Mage|30 High Mage Kepura
|31 Labyrinth Mage|32 Seto 2nd|33 Guardian Sebek|34 Guardian Neku|35 Heishin 2nd
|36 Seto 3rd|37 DarkNite|38 Nitemare|39 Duel Master K"""
POOL_NAME = {'pow': 'S/A POW', 'bcd': 'B/C/D', 'tec': 'S/A TEC', 'deck': 'deck'}


def main():
    src = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, 'tables.json')
    raw = json.load(open(src))
    cards = {int(r['id']): r['name'] for r in
             csv.DictReader(open(os.path.join(ROOT, 'notes', 'card-catalog.csv')))}
    name = lambda i: cards.get(i, '(id %d)' % i)

    duelist = {0: '(unused; copy of Simon Muran)'}
    for token in DUELISTS.replace('\n', '').split('|'):
        i, n = token.strip().split(' ', 1)
        duelist[int(i)] = n

    def write(fname, header, rows):
        with open(os.path.join(HERE, fname), 'w', newline='') as f:
            w = csv.writer(f)
            w.writerow(header)
            w.writerows(rows)

    recipes = [tuple(r) for r in raw['recipes']]
    write('fusions.csv',
          ['material_a_id', 'material_a', 'material_b_id', 'material_b',
           'result_id', 'result'],
          [[a, name(a), b, name(b), r, name(r)] for a, b, r in sorted(recipes)])

    by_result = collections.defaultdict(list)
    for a, b, r in recipes:
        by_result[r].append((a, b))
    write('fusions_by_result.csv',
          ['result_id', 'result', 'recipe_count', 'materials'],
          [[r, name(r), len(by_result[r]),
            '; '.join('%s + %s' % (name(a), name(b)) for a, b in sorted(by_result[r]))]
           for r in sorted(by_result, key=lambda r: (-len(by_result[r]), r))])

    uses = collections.Counter()
    for a, b, _ in recipes:
        uses[a] += 1
        uses[b] += 1
    write('fusions_by_material.csv', ['card_id', 'card', 'recipes_using_it'],
          [[i, name(i), uses.get(i, 0)] for i in sorted(cards)])

    write('equips.csv', ['equip_id', 'equip', 'monster_id', 'monster'],
          [[e, name(e), m, name(m)] for e, m in raw['equips']])

    drops = {int(k): v for k, v in raw['drops'].items()}
    rows = []
    for did in sorted(drops):
        for key in ('pow', 'bcd', 'tec', 'deck'):
            for idx, weight in enumerate(drops[did][key]):
                if weight:
                    rows.append([did, duelist[did], POOL_NAME[key], idx + 1,
                                 name(idx + 1), weight,
                                 round(weight * 100.0 / 2048, 4)])
    write('drops.csv',
          ['duelist_id', 'duelist', 'pool', 'card_id', 'card', 'weight',
           'chance_percent'], rows)

    summary = []
    for did in sorted(drops):
        row = [did, duelist[did]]
        for key in ('pow', 'bcd', 'tec', 'deck'):
            nz = [w for w in drops[did][key] if w]
            row += [len(nz), max(nz)]
        summary.append(row)
    write('drops_summary.csv',
          ['duelist_id', 'duelist', 'pow_cards', 'pow_best', 'bcd_cards', 'bcd_best',
           'tec_cards', 'tec_best', 'deck_cards', 'deck_best'], summary)

    print('fusions %d, results %d, equips %d, drop rows %d'
          % (len(recipes), len(by_result), len(raw['equips']), len(rows)))


if __name__ == '__main__':
    main()
