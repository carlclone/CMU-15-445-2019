

# Subtle

1.死锁问题,在数据库层面会给你提供死锁检测和超时机制,回滚某个事务,但是在数据库代码实现层面,如果你同时在某棵索引树的节点上产生死锁,那就真的没办法了(就是一个代码bug,必须程序员的锅).所以死锁最好的解决办法就是避免死锁,一个好的原则是保证加锁的顺序一致,这样永远也不会产生死锁