# Linear probe Hashing
线性探测哈希,如果某个新插入的key的哈希位置发生冲突,则在该哈希位置的下一位插入(多个冲突则再往下一位,甚至形成circular buffer)

需要解决的问题:删除一个key之后会形成一个空洞,将某个相同哈希值的连续列表断开,造成查询失败
解决方案:在该空洞处标记一个tombstone,则相同哈希值的列表在scan到该处时不会停止,可以继续往下查询


优点:实现简单,易于将哈希表从磁盘的physical page中存储和读取

# hash table在磁盘中的存储结构
一个header page
多个block page

# CPP相关

## 资料
极客时间 C++
A tour of C++
CPP Reference

## 在CPP的字节数组上进行位操作

假设是size为2的CHAR数组

`[00000000,00000000]`

想象成连续16位的bitset

如果需要将第三个0的位 置为1 ,

先locate字节 , 3/8 = 0 ,在index=0的字节上 ,
再和 00100000进行或操作
如何获得00100000 , 将二进制的1左移(3%8位=5位)
00000000|00100000=00100000

将某位置为0

与00100000进行取反操作 (~)
00100000~00100000=00000000


## 强制类型转换

Page 转成 BlockPage 问题, 
由于 HashTableBlockPage 用了模板类,  
不能在reinterpret_cast里使用类型推导

转换后是在同一个内存地址上进行解释吗?
Page 是 lvalue , 所以是在同一个内存地址中 , 修改后可被其他线程看到
TODO;加读/写锁

## lvalue 和 rvalue

## CPP 如何检测某段代码的内存变化

## 测试reinterpret_cast后的实例值变化是否可以被多个线程看到

