#include "prac.h"

#include <stdio.h>

// std::vector<std::set<int>*> r;
using namespace std;


void printResult(const out_t &result)
{
  out_t::iterator iter = result.begin();

  for (int i = 0; iter != result.end(); ++iter, ++i) {
    printf("%d:\n", i);

    vector<int> *p = *iter;
    vector<int>::iterator iter1 = p->begin();
    for (; iter1 != p->end(); ++iter1) {
      printf("\t%d", *iter1);
    }
    printf("\n");
  }
}

void permutation(const in_t &s, out_t &out)
{
  printf("permutation(%lu)\n", s.size());

  if (s.size() == 0) {
    return;
  } else if (s.size() == 1) {
    printf("size == 1: %d\n", *s.begin());
    vector<int> *v = new vector<int>();
    v->push_back(*s.begin());
    out.insert(v);
    return;
  }

  set<int>::iterator iter = s.begin();
  for (; iter != s.end(); ++iter) {
    // printf("v: %d\n", *iter);
    in_t s1(s);
    out_t out1;
    s1.erase(*iter);
    permutation(s1, out1);

    out_t::iterator iter1 = out1.begin();
    vector<int> *p = NULL;
    for (; iter1 != out1.end(); ++iter1) {
      p = *iter1;
      p->push_back(*iter);
      out.insert(p);
    }
  }
}

int main(int argc, char const *argv[])
{
  in_t s;

  for (int i = 0; i < 6; ++i) {
    s.insert(i);
  }

  out_t out;
  permutation(s, out);

  printResult(out);

  return 0;
}

