
计算集合的排列
=================================================

约定P(S, m, n)表示从一个大小为m的集合S中, 选取n个元素做排列.

一般的计算P(S, m, n)思维是这样子的:

* 从集合S中选取排列的第一个元素E, 有m种选择
* 对每一个选取的元素E, 计算P(S - E, m - 1, n - 1)的排列, 将元素E合并到结果中

很明显这是一个递归的过程, 用很容易写出这样的代码(C++代码, 假定元素类型为int):

    typedef std::set<int> in_t;
    typedef std::set< std::vector<int>* > out_t;

    void permutation(const in_t &s, out_t &out)
    {
      if (s.size() == 0) {
        return;
      } else if (s.size() == 1) {
        vector<int> *v = new vector<int>();
        v->push_back(*s.begin());
        out.insert(v);
        return;
      }

      set<int>::iterator iter = s.begin();
      for (; iter != s.end(); ++iter) {
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

代码比较糙, 用Erlang来写的话会是这样:

    permutation([]) ->
      [];
    permutation([_ | []] = Set) ->
      [Set];
    permutation(Set) ->
      lists:foldl(
        fun(X1, R1) ->
          S = permutation(Set -- [X1]),
          SS = lists:foldl(fun(X2, R2) -> [[X1 | X2] | R2] end, [], S),
          SS ++ R1
        end,
        [],
        Set).




