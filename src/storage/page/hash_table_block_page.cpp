//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_block_page.cpp
//
// Identification: src/storage/page/hash_table_block_page.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/hash_table_block_page.h"
#include "storage/index/generic_key.h"

namespace bustub {

    //* occupied_ : The ith bit of occupied_ is 1 if the ith index of array_ has ever been occupied.
    //* readable_ : The ith bit of readable_ is 1 if the ith index of array_ holds a readable value.
    //* array_ : The array that holds the key-value pairs.
    // 维护occupied和readable的原因是 , 删除某个值并不一定删除array_里的数据 , 只是在readable和occupied里标记
    // 然后是多线程的情况下,避免竞争, 写入的时候要检查occupied (可能是tombstone) , tombstone是occupied的但不可读 ,



template <typename KeyType, typename ValueType, typename KeyComparator>
KeyType HASH_TABLE_BLOCK_TYPE::KeyAt(slot_offset_t bucket_ind) const {
    MappingType kv=array_[bucket_ind];
    return kv.first;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
ValueType HASH_TABLE_BLOCK_TYPE::ValueAt(slot_offset_t bucket_ind) const {
    MappingType kv=array_[bucket_ind];
    return kv.second;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
bool HASH_TABLE_BLOCK_TYPE::Insert(slot_offset_t bucket_ind, const KeyType &key, const ValueType &value) {
    if (IsOccupied(bucket_ind)) {
        return false;
    }

    array_[bucket_ind] = {key, value};

    SetAsOccupied(bucket_ind);
    SetAsReadable(bucket_ind);
    return true;
}



    template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BLOCK_TYPE::Remove(slot_offset_t bucket_ind) {
    //删除某个bucket , 先要检查readable
    if (!IsReadable(bucket_ind)) {
        return;
    }
    //标记删除
    readable_[bucket_ind/8] &= ~(1<<bucket_ind%8);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
bool HASH_TABLE_BLOCK_TYPE::IsOccupied(slot_offset_t bucket_ind) const {
    return occupied_[bucket_ind / 8] & (1 << bucket_ind % 8);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
bool HASH_TABLE_BLOCK_TYPE::IsReadable(slot_offset_t bucket_ind) const {
    return readable_[bucket_ind / 8] & (1 << bucket_ind % 8);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BLOCK_TYPE::SetAsReadable(slot_offset_t bucket_ind)  {
    readable_[bucket_ind / 8] |= (1 << bucket_ind % 8);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BLOCK_TYPE::SetAsOccupied(slot_offset_t bucket_ind)  {
    occupied_[bucket_ind / 8] |= (1 << bucket_ind % 8);
}


// DO NOT REMOVE ANYTHING BELOW THIS LINE
template class HashTableBlockPage<int, int, IntComparator>;
template class HashTableBlockPage<GenericKey<4>, RID, GenericComparator<4>>;
template class HashTableBlockPage<GenericKey<8>, RID, GenericComparator<8>>;
template class HashTableBlockPage<GenericKey<16>, RID, GenericComparator<16>>;
template class HashTableBlockPage<GenericKey<32>, RID, GenericComparator<32>>;
template class HashTableBlockPage<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
