
# Lab

- [x] Lab1
    - [x] Clock replacer
    - [x] Buffer pool manager
- [x] Lab2
    - [x] Hashtable header page
    - [x] Hashtable block page
    - [x] Hashtable linear probe
    - [x] Hashtable resize
- [x] Lab3
    - [x] Simple catalog
    - [x] Executors
- [ ] Lab4
    - [ ] Log Manager
    - [ ] System Recovery
    - [ ] Checkpoints

# Homework
- [x] SQL
- [x] Indexes
- [x] Join Algorithms
- [ ] Concurrency Control
- [ ] Distributed Databases

# simple catalog

static_cast<std::unique_ptr<TableMetadata>>(table)

```
图:
+-------------------------------------------------------------------+
| explanation:
| tuple : like a int or char field  ,binary data of a field
| record : one row of table                                         +
|                                                                   |
|  /**                                                              |
|   * Tuple format:                                                 |
|   * ------------------------------------+-------------------------+------                                           src/storage/table/table_heap.cpp
|   * | FIXED|SIZE or VARIED|SIZED OFFSET | PAYLOAD OF VARIED|SIZED FIELD |
|   * ------------------------------------+-------------------------+------
|   */                                                              |
+------------------------------------------+------------------------+-----------------------------------------+    +-----------------------------------------------+       +---------------------------------------------------+
                                           | +--------------------+                                 catalog   |    |tableHeap                                      |       |  tableMetaData                                    |
                                           | |tables              |      +------------+                       |    |   represent a physical table on disk          |       |   - schema                                        |
                                           | |  +--------------+  |      | names      |      +---------+---+  |    |   linked list of pages                        |       |   | name                                          |
                                           | |  |metadata      |  |      |            |      |table id -   |  |    |   job to operate the table record             |       |   | ptr to tableheap                              |
                                           | |  |  -tableheap  +<--------+  -table1   |      |generator    |  |    |                                               |       |   - table obj id                                  |
                                           | |  |              |  |      |  -table2   |      |             |  |    |                                               |       |                                                   |
                                           | |  |              |  |      |            |      |             |  |    |                                               |       |                                                   |
                                           | |  |              |  |      |            |      +-------------+  |    |                                               |       |                                                   |
                                           | |  |              |  |      |            |                       |    |                                               |       |                                                   |
                                           | |  +--------------+  |      |            |                       |    |   - first page ( in physical disk) , retrie   |       |                                                   |
                                           | |                    |      |            |                       |    |   ve from bpm                                 |       |                                                   |
                                           | |                    |      |            |                       |    |                                               |       |                                                   |
                                           | |                    |      |            |                       |    |   InsertTuple(tuple)                          |       |                                                   |
                                           | |                    |      +------------+                       |    |                                               |       |                                                   |
                                           | |                    |                                           |    |   MarkDelete(record id)                       |       |                                                   |
                                           | |                    |                                           |    |                                               |       |                                                   |
                                           | |                    |                                           |    |   UpdateTuple(tuple)                          |       |                                                   |
                                           | |                    |                                           |    |                                               |       |                                                   |
                                           | |                    |                                           |    |                                               |       |                                                   |
                                           | |                    |                                           |    |                                               |       |                                                   |
                                           | |                    |                                           |    |                                               |       |                                                   |
                                           | |                    |                                           |    |                                               |       |                                                   |
                                           | |                    |                                           |    +-----------------------------------------------+       |                                                   |
                                           | |                    |                                           |                                                            |                                                   |
                                           | |                    |                                           |                                                            |                                                   |
                                           | |                    |                                           |                                                            +---------------------------------------------------+
                                           | +--------------------+                                           |
                                           +------------------------------------------------------------------+
```

# 笔记

[DB Storage](https://github.com/carlclone/CMU-15-445-2019/blob/master/notes/1.db-storage.md)
[Buffer Pool Manager / Lab1]()
[Hash Table / Lab2]()
[Tree Index / 2018 Lab2]()
[Multiple Version Concurrency Control]()



# 资料

课程 schedule
英文字幕
中文字幕 

# 课程提到的书籍

Database System Concepts 6e
Modern BTree Technique
A tour of C++

# end

课程给的参考资料不像MIT6.824那么全 ,需要自己额外找 , 讲义只有slide没有note,所以视频也得看一遍自己做笔记 , 测试不是太全 , 在线提交的网站已经停了
C++也不熟,只能边学边做了
