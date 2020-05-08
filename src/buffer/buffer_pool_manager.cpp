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
  for (size_t i = 0; i < pool_size_; i++) {
    if (pages_[i].page_id_ == page_id) {
      replacer_->Pin(i);
      return &pages_[i];
    }
  }

  // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
  //        Note that pages are always found from the free list first.
  frame_id_t replacementPageFrameId;
  Page *replacementPage;
  if (free_list_.size() != 0) {
    replacementPageFrameId = free_list_.front();
    free_list_.pop_front();
    replacementPage = &pages_[replacementPageFrameId];
  }

  // find a replacement page (R) from  the replacer.
  else {
    replacer_->Victim(&replacementPageFrameId);
    replacementPage = &pages_[replacementPageFrameId];
  }

  // For FetchPageImpl,you should return NULL if no page is available in the free list
  // and all other pages are currently pinned.
  if (replacementPage == nullptr) {
    return nullptr;
  }

  // 2.     If R is dirty, write it back to the disk.
  if (replacementPage->IsDirty()) {
    disk_manager_->WritePage(replacementPageFrameId, replacementPage->GetData());
  }
  // 3.     Delete R from the page table and insert P.
  // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
  replacementPage->page_id_ = page_id;
  replacementPage->is_dirty_ = false;
  disk_manager_->ReadPage(page_id, replacementPage->data_);
  return replacementPage;
}

/**
 * Unpin the target page from the buffer pool.
 * @param page_id id of page to be unpinned
 * @param is_dirty true if the page should be marked as dirty, false otherwise
 * @return false if the page pin count is <= 0 before this call, true otherwise
 */
bool BufferPoolManager::UnpinPageImpl(page_id_t page_id, bool is_dirty) {


  return false;
}

bool BufferPoolManager::FlushPageImpl(page_id_t page_id) {
  // Make sure you call DiskManager::WritePage!
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
  }

    // find a replacement page (R) from  the replacer.
  else {
    auto result=replacer_->Victim(&replacementPageFrameId);
    //一个空闲frame都找不到了
    if (result==false) {
      return nullptr;
    }
    replacementPage = &pages_[replacementPageFrameId];
  }

  //在磁盘中allocate一个page,获得page id
  page_id_t newPageId = disk_manager_->AllocatePage();

  //更新 bufferpool 里 frame 的 page 信息
  replacementPage->ResetMemory(); //清空原 frame 里的数据,避免污染
  replacementPage->page_id_ = newPageId;
  replacementPage->is_dirty_=true; //新数据, 空的 page 也要flush
  replacementPage->pin_count_=1; //

  //track page->frame的映射

  //返回响应
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
