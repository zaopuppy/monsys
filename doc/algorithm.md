
一些平时想到的问题
=================================================
都是算法相关, 但有些可能体现出来变成数学问题了


矩阵乘法顺序安排
-------------------------------------------------
有N个矩阵相乘:

    M1.M2.M3. ... .Nn

* 有多少种可能的乘法组合?
* 需要确定怎样的组合方式, 所需要的计算量最小?

#### 有多少种可能的组合? ####

令T(n)为n个矩阵可能的乘法组合, n个矩阵为

    M1.M2.M3. ... .Mn

假定某一次特定的乘法组合中, 最后一次乘法为

    (M1.M2...Mi)(Mi+1.Mi+2...Mn)

显然这一次乘法组合中

    T(n) = T(i)T(n-i)

由于i可以赋值为1~n-1, 所以

    T(n) = sum of T(i)T(n-i), where i from 1 to n-i


集合的排列
-------------------------------------------------

集合的组合
-------------------------------------------------

完全采用随机的方法进行排序, 时间复杂度期望是多少?
-------------------------------------------------
    random_sort(list)
    {
      if (list has been sorted)
        return list;

      idx1 = Math.random(list.length);
      idx2 = Math.random(list.length);
      swap(idx1, idx2, list);

      return random_sort(list);
    }

假定list的长度为n, idx1的概率为1/n, idx2的概率为1/(n-1)

生成大量随机字串, 要求不可重复
-------------------------------------------------

假设需要2^80个字符串, 长度为80, 简单的随机创建, 生成后检查是否有重复是不行了(代价太高, 越到后面越慢)

暂时想到的两个思路

* 模拟现实生活种顺序分配, 但是乱序回收的情况. 先顺序生成一定规则的字符串(不需要一次性全部产生), 然后随机分配回收时间, 分配给定时器, 然后到时回收.

* 按照一定算法一次性生成大量不重复的序列(可以是有序但是被打乱的), 然后逐个分发, 用光后再次生成. 存在的问题是下一次生成的和上一次生成的有重复. 可以采用生成两组不重复的解决





如何破解WEP加密协议
-------------------------------------------------
