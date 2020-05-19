//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager.h"

#include <include/common/logger.h>

#include <list>
#include <unordered_map>

namespace bustub {

    BufferPoolManager::BufferPoolManager(size_t pool_size, DiskManager *disk_manager, LogManager *log_manager)
            : pool_size_(pool_size), disk_manager_(disk_manager), log_manager_(log_manager) {
        // We allocate a consecutive memory space for the buffer pool.
        pages_ = new Page[pool_size_];
        replacer_ = new ClockReplacer(pool_size);

        // Initially, every page is in the free list.
        for (size_t i = 0; i < pool_size_; ++i) {
            free_list_.emplace_back(static_cast<int>(i));
        }
    }

    BufferPoolManager::~BufferPoolManager() {
        delete[] pages_;
        delete replacer_;
    }

    Page *BufferPoolManager::FetchPageImpl(page_id_t page_id) {
        // 1.     Search the page table for the requested page (P).
        // 1.1    If P exists, pin it and return it immediately.
        Page *replacementPage;
        frame_id_t replaceFrameId;
        if (page_table_.find(page_id) != page_table_.end()) {
            replaceFrameId = page_table_[page_id];
            replacementPage = &pages_[replaceFrameId];
//            LOG_INFO("page已经在pool中,直接返回 page %d frame %d",page_id,replaceFrameId);
        }

            // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
            //        Note that pages are always found from the free list first.
        else {
            if (free_list_.size() != 0) {
                replaceFrameId = free_list_.front();
                free_list_.pop_front();
                replacementPage = &pages_[replaceFrameId];
//                LOG_INFO("fetchpage free list中找了frame %d",replaceFrameId);
            } else {
                // find a replacement page (R) from  the replacer.
                auto success = replacer_->Victim(&replaceFrameId);
                if (success) {
                    replacementPage = &pages_[replaceFrameId];
//                    LOG_INFO("fetchpage replacer中找了frame %d",replaceFrameId);

                } else {
                    replacementPage = nullptr;
//                    LOG_INFO("fetchpage replacer中找不到frame ");
                }
            }
        }
        // For FetchPageImpl,you should return NULL if no page is available in the free list
        // and all other pages are currently pinned.
        if (replacementPage == nullptr) {
            return nullptr;
        }

        page_table_.erase(replacementPage->GetPageId());
        // 2.     If R is dirty, write it back to the disk.
        if (replacementPage->IsDirty()) {
            disk_manager_->WritePage(replacementPage->page_id_, replacementPage->GetData());
        }
        // 3.     Delete R from the page table and insert P.
        // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
        replacementPage->page_id_ = page_id;
        replacementPage->is_dirty_ = false;
        replacementPage->pin_count_++;
        disk_manager_->ReadPage(page_id, replacementPage->data_);
        page_table_[page_id] = replaceFrameId;
//        LOG_INFO("从disk中获取了%d 数据%s",page_id,replacementPage->data_);

        return replacementPage;
    }

/**
 * Unpin the target page from the buffer pool.
 * @param page_id id of page to be unpinned
 * @param is_dirty true if the page should be marked as dirty, false otherwise
 * @return false if the page pin count is <= 0 before this call, true otherwise
 */
    bool BufferPoolManager::UnpinPageImpl(page_id_t page_id, bool is_dirty) {

        frame_id_t frameId;
        //page不存在buffer pool中
        if (page_table_.find(page_id) == page_table_.end()) {
            return false;
        }
        frameId = page_table_[page_id];
        Page *page = &pages_[frameId];

        if (is_dirty) {
            page->is_dirty_ = true;
        }
        if (page->is_dirty_) {
            disk_manager_->WritePage(page->GetPageId(),page->GetData());
        }
        page->pin_count_--;

        if (page->pin_count_ == 0) {
            replacer_->Unpin(frameId);
        }

//        LOG_INFO("unpin了page %d frame %d",page_id,frameId);

        return true;
    }

    bool BufferPoolManager::FlushPageImpl(page_id_t page_id) {
        // Make sure you call DiskManager::WritePage!

        if (page_table_.find(page_id) == page_table_.end()) {
//            LOG_INFO("flush page 找不到");
            return false;
        }
        Page *page = &pages_[page_table_[page_id]];
        if (page->IsDirty()) {
//            LOG_INFO("flush page 写入page %d",page_id);
            disk_manager_->WritePage(page_id, page->GetData());
            page->is_dirty_=false;
            return true;
        }

        return false;
    }

//在 buffer pool 里分配一块内存给新的 page , 要和磁盘上的page一一对应

// 0.   Make sure you call DiskManager::AllocatePage!
// 1.   If all the pages in the buffer pool are pinned, return nullptr.
// 2.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
// 3.   Update P's metadata, zero out memory and add P to the page table.
// 4.   Set the page ID output parameter. Return a pointer to P.
    Page *BufferPoolManager::NewPageImpl(page_id_t *page_id) {

        //如果bufferpool 里的 page 都 pinned 了, return nullprt
        //遍历 pages , 检查 pin count
        frame_id_t replacementPageFrameId;
        Page *replacementPage;

        //从freelist或者replacer里找一个可用的frame
        if (free_list_.size() != 0) {
            replacementPageFrameId = free_list_.front();
            free_list_.pop_front();
            replacementPage = &pages_[replacementPageFrameId];
//            LOG_INFO("从freelist中找到可用的frame");
        }

            // find a replacement page (R) from  the replacer.
        else {
            auto result = replacer_->Victim(&replacementPageFrameId);
            //一个空闲frame都找不到了
            if (result == false) {
//                LOG_INFO("没有空闲frame了");
                return nullptr;
            }
//            LOG_INFO("从replacer中找到可用frame %d",replacementPageFrameId);
            replacementPage = &pages_[replacementPageFrameId];

        }
        page_table_.erase(replacementPage->GetPageId());


        //在磁盘中allocate一个page,获得page id
        page_id_t newPageId = disk_manager_->AllocatePage();

        //更新 bufferpool 里 frame 的 page 信息
        replacementPage->ResetMemory(); //清空原 frame 里的数据,避免污染
        replacementPage->page_id_ = newPageId;
        replacementPage->is_dirty_ = true; //新数据, 空的 page 也要flush
        replacementPage->pin_count_ = 1; //



        //track page->frame的映射
        page_table_[newPageId] = replacementPageFrameId;

        //返回响应
//        LOG_INFO("创建了一个新 page %d frame %d", *page_id,replacementPageFrameId);
        *page_id = newPageId;
        return replacementPage;
    }

    bool BufferPoolManager::DeletePageImpl(page_id_t page_id) {
        // 0.   Make sure you call DiskManager::DeallocatePage!
        // 1.   Search the page table for the requested page (P).
        // 1.   If P does not exist, return true.
        // 2.   If P exists, but has a non-zero pin-count, return false. Someone is using the page.
        // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free list.
        return false;
    }

    void BufferPoolManager::FlushAllPagesImpl() {
        // You can do it!
    }

}  // namespace bustub
