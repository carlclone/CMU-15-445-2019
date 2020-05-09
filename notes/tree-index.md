

# b tree如何处理non-unique key

# parent node page

> 结构



# inner node page


# leaf node page


# 支持的索引

index(a,b,c)

(a) √  确定左边界和右边界
(a,b) √  确定左边界和右边界
(*,b) √  , 利用该索引可以确定 (*,b)<(c,c)的右边界 ,接下来则是优化器的判断,是全表顺序扫还是使用该索引




# 17/18年的B tree index代码阅读
https://15445.courses.cs.cmu.edu/fall2018/project2/

19年版本是hash index ,找到了17年的B+tree Lab, 学习一下实现细节

