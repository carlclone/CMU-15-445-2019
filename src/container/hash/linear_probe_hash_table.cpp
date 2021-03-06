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

    template<typename KeyType, typename ValueType, typename KeyComparator>
    HASH_TABLE_TYPE::LinearProbeHashTable(const std::string &name,
                                          BufferPoolManager *buffer_pool_manager,
                                          const KeyComparator &comparator,
                                          size_t num_buckets,
                                          HashFunction<KeyType> hash_fn)
            : buffer_pool_manager_(buffer_pool_manager), comparator_(comparator), hash_fn_(std::move(hash_fn)) {
        //初始化 header page
        Page *newPage = buffer_pool_manager_->NewPage(&header_page_id_);
        auto headerPage = reinterpret_cast<HashTableHeaderPage *>(newPage->GetData());
        headerPage->SetPageId(header_page_id_);


        headerPage->SetSize(num_buckets);

        auto blockNum = num_buckets / BLOCK_ARRAY_SIZE;

        //预先建立好 block pages
        page_id_t tmpPageId;
        for (unsigned i = 0; i < blockNum; i++) {
            //new page
            buffer_pool_manager->NewPage(&tmpPageId);
            //存储到 header 的 block page id 中
            headerPage->AddBlockPageId(tmpPageId);
            //un pin
            buffer_pool_manager->UnpinPage(tmpPageId, true);
        }

        buffer_pool_manager->UnpinPage(header_page_id_, true);
    }

    template<typename KeyType, typename ValueType, typename KeyComparator>
    LinearProbeHashTable<KeyType, ValueType, KeyComparator>::~LinearProbeHashTable() {
        buffer_pool_manager_->UnpinPage(header_page_id_, true);
    }

/*****************************************************************************
 * SEARCH
 *****************************************************************************/
    template<typename KeyType, typename ValueType, typename KeyComparator>
    bool HASH_TABLE_TYPE::GetValue(Transaction *transaction, const KeyType &key, std::vector<ValueType> *result) {
        //获取 header page
        Page *hPage = buffer_pool_manager_->FetchPage(header_page_id_);
        HashTableHeaderPage *headerPage = reinterpret_cast<HashTableHeaderPage *>(hPage->GetData());

        auto mmHash = hash_fn_.GetHash(key);

        //定位 block page
        auto targetBlockIndex = mmHash % headerPage->NumBlocks();
        auto blockPageId = headerPage->GetBlockPageId(targetBlockIndex);

        //如何把 page 转成 block page , 二进制强转
        //这个操作符能够在非相关的类型之间转换。操作结果只是简单的从一个类型的指针到别的类型的指针的值的二进制拷贝。
        Page *page = buffer_pool_manager_->FetchPage(blockPageId);
        HASH_TABLE_BLOCK_TYPE *blockPage = reinterpret_cast<HASH_TABLE_BLOCK_TYPE *>(page->GetData());

        //定位 slot
        auto res = false;
        auto targetSlotIndex = mmHash % BLOCK_ARRAY_SIZE;

        //从该 slot 位置开始找
        for (auto i = targetSlotIndex; i < BLOCK_ARRAY_SIZE; i++) {
            if (blockPage->IsValid(i) && comparator_(blockPage->KeyAt(i), key) == 0) {
                result->push_back(blockPage->ValueAt(i));
                res = true;
            }
        }


        buffer_pool_manager_->UnpinPage(blockPageId, true);
        //unpin header page
        buffer_pool_manager_->UnpinPage(header_page_id_, true);

        return res;
    }

/*****************************************************************************
 * INSERTION
 *****************************************************************************/
    template<typename KeyType, typename ValueType, typename KeyComparator>
    bool HASH_TABLE_TYPE::Insert(Transaction *transaction, const KeyType &key, const ValueType &value) {
        //获取 header page
        Page *hPage = buffer_pool_manager_->FetchPage(header_page_id_);
        HashTableHeaderPage *headerPage = reinterpret_cast<HashTableHeaderPage *>(hPage->GetData());

        auto mmHash = hash_fn_.GetHash(key);

        //定位 block page
        auto targetBlockIndex = mmHash % headerPage->NumBlocks();
        auto blockPageId = headerPage->GetBlockPageId(targetBlockIndex);

        //如何把 page 转成 block page , 二进制强转
        //这个操作符能够在非相关的类型之间转换。操作结果只是简单的从一个类型的指针到别的类型的指针的值的二进制拷贝。
        Page *page = buffer_pool_manager_->FetchPage(blockPageId);
        HASH_TABLE_BLOCK_TYPE *blockPage = reinterpret_cast<HASH_TABLE_BLOCK_TYPE *>(page->GetData());

        //定位 slot
        auto res = false;
        auto targetSlotIndex = mmHash % BLOCK_ARRAY_SIZE;



        //从该 slot 位置开始找到可插入的 slot , 如果 slot 没有被 occupied , 则可以插入
        for (auto i = targetSlotIndex; i < BLOCK_ARRAY_SIZE; i++) {
            //如果值存在,false
            if (blockPage->IsOccupied(i) && blockPage->ValueAt(i)==value) {
                res=false;
                break;
            }

            if (!blockPage->IsOccupied(i)) {
                blockPage->Insert(i, key, value);
                headerPage->count++;
                //LOG_INFO("inserted a vjkjk");
                res = true;
                break;
            }
        }

        //unpin all page
        buffer_pool_manager_->UnpinPage(blockPageId, true);
        buffer_pool_manager_->UnpinPage(header_page_id_, true);

        return res;
    }

/*****************************************************************************
 * REMOVE
 *****************************************************************************/
    template<typename KeyType, typename ValueType, typename KeyComparator>
    bool HASH_TABLE_TYPE::Remove(Transaction *transaction, const KeyType &key, const ValueType &value) {
        //类似 GetValue , 先找到 , 然后比对 Value 一致后删除
        //从 header 获取 metadata
        //获取 header page
        Page *hPage = buffer_pool_manager_->FetchPage(header_page_id_);
        HashTableHeaderPage *headerPage = reinterpret_cast<HashTableHeaderPage *>(hPage->GetData());

        auto mmHash = hash_fn_.GetHash(key);

        //定位 block page
        auto targetBlockIndex = mmHash % headerPage->NumBlocks();
        auto blockPageId = headerPage->GetBlockPageId(targetBlockIndex);

        //如何把 page 转成 block page , 二进制强转
        //这个操作符能够在非相关的类型之间转换。操作结果只是简单的从一个类型的指针到别的类型的指针的值的二进制拷贝。
        Page *page = buffer_pool_manager_->FetchPage(blockPageId);
        HASH_TABLE_BLOCK_TYPE *blockPage = reinterpret_cast<HASH_TABLE_BLOCK_TYPE *>(page->GetData());

        //定位 slot
        auto res = false;
        auto targetSlotIndex = mmHash % BLOCK_ARRAY_SIZE;

        //如果 slot 是 occupied 和 readable 的
        for (auto i = targetSlotIndex; i < BLOCK_ARRAY_SIZE; i++) {
            if (blockPage->IsValid(i) && comparator_(blockPage->KeyAt(i), key) == 0 &&
                blockPage->ValueAt(i) == value
                    ) {
                //标记删除
                blockPage->Remove(i);
                headerPage->count--;
                res = true;
            }
        }

        //unpin all page
        buffer_pool_manager_->UnpinPage(blockPageId, true);
        buffer_pool_manager_->UnpinPage(header_page_id_, true);

        return res;
    }

/*****************************************************************************
 * RESIZE
 *****************************************************************************/
    template<typename KeyType, typename ValueType, typename KeyComparator>
    void HASH_TABLE_TYPE::Resize(size_t initial_size) {
//        //创建新的 hashtable , 遍历所有 block page 和所有 slot , 重新执行 insert 到新的 hashtable
//        //记得删除旧的 header 和 blocks page
//        //然后更新该 hash table 的状态(属性)
//        auto headerPage = reinterpret_cast<HashTableHeaderPage * >(buffer_pool_manager_->FetchPage(
//                header_page_id_)->GetData());
//
//        //新的 block pages
//        page_id_t tmpPageId;
//        for (unsigned i = 0; i < headerPage->GetSize(); i++) {
//            //new page
//            buffer_pool_manager_->NewPage(&tmpPageId);
//            //存储到 header 的 block page id 中
//            headerPage->AddBlockPageId(tmpPageId);
//            //un pin
//            buffer_pool_manager_->UnpinPage(tmpPageId, true);
//        }
//
//        auto oldSize = headerPage->GetSize();
//
//        headerPage->SetSize(initial_size);
//
//        //遍历所有page,所有slot,hash到新的位置 , 可以复用Insert方法
//        for (size_t i = 0; i < oldSize; i++) {
//            size_t oldBlockPageId = headerPage->GetBlockPageId(i);
//            auto blockPage = reinterpret_cast<HASH_TABLE_BLOCK_TYPE *> (buffer_pool_manager_->FetchPage(
//                    oldBlockPageId)->GetData());
//
//            //遍历page的所有slot
//            for (size_t j = 0; j < BLOCK_ARRAY_SIZE; j++) {
//                if (blockPage->IsReadable(i)) {
//                    auto val = blockPage->ValueAt(i);
//                    auto key = blockPage->KeyAt(i);
//                    Insert(nullptr, key, val);
//                }
//            }
//
//            buffer_pool_manager_->UnpinPage(oldBlockPageId, false);
//            buffer_pool_manager_->DeletePage(oldBlockPageId);
//        }
//        buffer_pool_manager_->UnpinPage(header_page_id_, true);
    }

/*****************************************************************************
 * GETSIZE
 *****************************************************************************/
    template<typename KeyType, typename ValueType, typename KeyComparator>
    size_t HASH_TABLE_TYPE::GetSize() {
        //从 header 获取 metadata
        //获取 header page
        Page *hPage = buffer_pool_manager_->FetchPage(header_page_id_);
        HashTableHeaderPage *headerPage = reinterpret_cast<HashTableHeaderPage *>(hPage->GetData());
        auto size = headerPage->count;
        buffer_pool_manager_->UnpinPage(header_page_id_, false);
        return size;
    }

    template
    class LinearProbeHashTable<int, int, IntComparator>;

    template
    class LinearProbeHashTable<GenericKey<4>, RID, GenericComparator<4>>;

    template
    class LinearProbeHashTable<GenericKey<8>, RID, GenericComparator<8>>;

    template
    class LinearProbeHashTable<GenericKey<16>, RID, GenericComparator<16>>;

    template
    class LinearProbeHashTable<GenericKey<32>, RID, GenericComparator<32>>;

    template
    class LinearProbeHashTable<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
