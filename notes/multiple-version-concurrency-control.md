
# MVCC介绍

因为没有锁,所以读不阻塞写,写不阻塞读

维护了一张公共的record version表 , 和一张事务状态表

每个事务对版本的可见判断, 取决于logical timestamp 和isolation level

