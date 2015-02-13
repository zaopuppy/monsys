#!/usr/bin/env python3
# -*- coding: utf-8 -*-


# c : Candidate
# sup: Support

from itertools import permutations
from collections import defaultdict


TDB = (
    ('10', ('Beer', 'Nuts', 'Diaper')),
    ('20', ('Beer', 'Coffee', 'Diaper')),
    ('30', ('Beer', 'Diaper', 'Eggs')),
    ('40', ('Nuts', 'Eggs', 'Milk')),
    ('50', ('Nuts', 'Coffee', 'Diaper', 'Eggs', 'Milk')),
)


def issubsetof(set1, set2):
    return False


def apriori(db, minsup, minconf):
    k_items = {}

    # 1-items
    num_of_records = len(db)
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

    # 2-items
    # generate candidate pairs
    two_item_sets = tuple(map(set, permutations(k_items[1].keys(), 2)))
    print(two_item_sets)
    two_item_dic = defaultdict(int)
    for rec in db:
        for s in two_item_sets:
            if s.issubset(set(rec[1])):
                two_item_dic[s] += 1

    print(two_item_dic)

    return k_items


def main():
    print(apriori(TDB, 0.5, 0.5))


if __name__ == '__main__':
    quit(main())

