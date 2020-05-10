//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// linear_probe_hash_table.cpp
//
// Identification: src/container/hash/linear_probe_hash_table.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "common/exception.h"
#include "common/logger.h"
#include "common/rid.h"
#include "container/hash/linear_probe_hash_table.h"

namespace bustub {

template <typename KeyType, typename ValueType, typename KeyComparator>
HASH_TABLE_TYPE::LinearProbeHashTable(const std::string &name,
                                      BufferPoolManager *buffer_pool_manager,
                                      const KeyComparator &comparator,
                                      size_t num_buckets,
                                      HashFunction<KeyType> hash_fn)
    : buffer_pool_manager_(buffer_pool_manager), comparator_(comparator), hash_fn_(std::move(hash_fn)) {
  //初始化 header page
  Page *newPage = buffer_pool_manager_->NewPage(&header_page_id_);
  headerPage->SetPageId(header_page_id_);
  //bucket 数(或者 block page 数)
  headerPage->SetSize(num_buckets);

  //预先建立好 block pages

}

template <typename KeyType, typename ValueType, typename KeyComparator>
LinearProbeHashTable<KeyType, ValueType, KeyComparator>::~LinearProbeHashTable() {
  buffer_pool_manager_->UnpinPage(header_page_id_, true);
}

/*****************************************************************************
 * SEARCH
 *****************************************************************************/
template <typename KeyType, typename ValueType, typename KeyComparator>
bool HASH_TABLE_TYPE::GetValue(Transaction *transaction, const KeyType &key, std::vector<ValueType> *result) {
  //从header page获取 metadata
  auto tailIndex = headerPage->NumBlocks() -1;
  auto mmHash = hash_fn_.GetHash(key);
  auto targetBlockIndex = mmHash / BLOCK_ARRAY_SIZE;




  //定位 block page
  auto blockPageId = headerPage->GetBlockPageId(targetBlockIndex);
  //如何把 page 转成 block page , 二进制强转
  //这个操作符能够在非相关的类型之间转换。操作结果只是简单的从一个类型的指针到别的类型的指针的值的二进制拷贝。
  auto page = buffer_pool_manager_->FetchPage(blockPageId);
  HASH_TABLE_BLOCK_TYPE* blockPage = reinterpret_cast<HASH_TABLE_BLOCK_TYPE *>(page);

  //定位 slot
  auto res=false;
  auto targetSlotIndex = mmHash % BLOCK_ARRAY_SIZE;

  //从该 slot 位置开始找
  for (auto i = targetSlotIndex; i < BLOCK_ARRAY_SIZE; i++) {
    if (blockPage->IsValid(i) && comparator_(blockPage->KeyAt(i), key) == 0 ) {
      result->push_back(blockPage->ValueAt(i));
      res=true;
    }
  }



  return res;
}
/*****************************************************************************
 * INSERTION
 *****************************************************************************/
template <typename KeyType, typename ValueType, typename KeyComparator>
bool HASH_TABLE_TYPE::Insert(Transaction *transaction, const KeyType &key, const ValueType &value) {
  //从 header 获取 metadata

  //计算 key hash找到插入的 block page

  //找到插入的 slot

  //从该 slot 开始找到一个可以插入的 slot

  return false;
}

/*****************************************************************************
 * REMOVE
 *****************************************************************************/
template <typename KeyType, typename ValueType, typename KeyComparator>
bool HASH_TABLE_TYPE::Remove(Transaction *transaction, const KeyType &key, const ValueType &value) {
  //类似 GetValue , 先找到 , 然后比对 Value 一致后删除
  return false;
}

/*****************************************************************************
 * RESIZE
 *****************************************************************************/
template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_TYPE::Resize(size_t initial_size) {
  //创建新的 hashtable , 遍历所有 block page 和所有 slot , 重新执行 insert 到新的 hashtable
  //记得删除旧的 header 和 blocks page
  //然后更新该 hash table 的状态(属性)
}

/*****************************************************************************
 * GETSIZE
 *****************************************************************************/
template <typename KeyType, typename ValueType, typename KeyComparator>
size_t HASH_TABLE_TYPE::GetSize() {
  return headerPage->NumBlocks();
}

template class LinearProbeHashTable<int, int, IntComparator>;

template class LinearProbeHashTable<GenericKey<4>, RID, GenericComparator<4>>;
template class LinearProbeHashTable<GenericKey<8>, RID, GenericComparator<8>>;
template class LinearProbeHashTable<GenericKey<16>, RID, GenericComparator<16>>;
template class LinearProbeHashTable<GenericKey<32>, RID, GenericComparator<32>>;
template class LinearProbeHashTable<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
