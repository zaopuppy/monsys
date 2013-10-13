#ifndef _PRAC_H__
#define _PRAC_H__

#include <set>
#include <vector>

typedef std::set<int> in_t;
typedef std::set< std::vector<int>* > out_t;

// P(a, b), starts with 0
void permutation(const in_t &s,
                 out_t &out);
// void permutation(const std::set<int> &s, std::set<int> &out);
// void permutation(std::set<int> &s);

#endif // _PRAC_H__

