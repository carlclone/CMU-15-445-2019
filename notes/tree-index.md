

# b tree如何处理non-unique key

# inner node的结构


# leaf node的结构


# 支持的索引

index(a,b,c)

(a) √  确定左边界和右边界
(a,b) √  确定左边界和右边界
(*,b) √  , 利用该索引可以确定 (*,b)<(c,c)的右边界 ,接下来则是优化器的判断,是全表顺序扫还是使用该索引
