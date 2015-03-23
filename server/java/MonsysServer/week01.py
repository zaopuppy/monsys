#!/usr/bin/env python3
# -*- coding: utf-8 -*-


# c : Candidate
# sup: Support

from itertools import combinations
from collections import defaultdict


TDB = (
    ('10', ('Beer', 'Nuts', 'Diaper')),
    ('20', ('Beer', 'Coffee', 'Diaper', 'Nuts')),
    ('30', ('Beer', 'Diaper', 'Eggs')),
    ('40', ('Beer', 'Nuts', 'Eggs', 'Milk')),
    ('50', ('Nuts', 'Coffee', 'Diaper', 'Eggs', 'Milk')),
)

# TDB = (
#     ('10', ('Beer', 'Nuts', 'Diaper')),
#     ('20', ('Beer', 'Coffee', 'Diaper', 'Nuts')),
#     ('30', ('Beer', 'Diaper', 'Eggs')),
#     ('40', ('Beer', 'Nuts', 'Eggs', 'Milk')),
#     ('50', ('Beer', 'Nuts', 'Diaper', 'Eggs', 'Milk')),
# )


def apriori(db, minsup, minconf):
    k_items = {}

    num_of_records = len(db)
    print("number of records is {}".format(num_of_records))

    # 1-items
    one_item_dic = defaultdict(int)
    for rec in db:
        for item in rec[1]:
            one_item_dic[item] += 1

    m = defaultdict(float)
    for k, v in one_item_dic.items():
        sup = float(v)/float(num_of_records)
        if sup >= minsup:
            m[k] = sup

    k_items[1] = m

    # FIXME: use `tuple()` will more memory, how can I know if the size of candidates is 0 or not?
    pair_size = 2
    candidates = tuple(map(set, combinations(k_items[1].keys(), pair_size)))
    while len(candidates) > 0:
        dic = defaultdict(int)
        for rec in db:
            for c in candidates:
                if c.issubset(set(rec[1])):
                    s_l = [str(_) for _ in c]
                    s_l.sort()
                    dic[', '.join(s_l)] += 1

        m = defaultdict(float)
        for k, v in dic.items():
            sup = float(v)/float(num_of_records)
            if sup >= minsup:
                m[k] = sup

        k_items[pair_size] = m
        pair_size += 1
        candidates = tuple(map(set, combinations(k_items[1].keys(), pair_size)))

    return k_items


def main():
    for k1, v1 in apriori(TDB, 0.5, 0.2).items():
        print(k1)
        for k2, v2 in v1.items():
            print("({}) => {}".format(k2, v2))


if __name__ == '__main__':
    quit(main())

