//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.cpp
//
// Identification: src/buffer/clock_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/clock_replacer.h"

namespace bustub {

ClockReplacer::ClockReplacer(size_t num_pages) {
  //钟表指针
  clockHand = 0;
  pageSize = num_pages;

  //
}

ClockReplacer::~ClockReplacer() = default;

//找到一个祭品
// Starting from the current position of clock hand,
// find the first frame that is both in the `ClockReplacer` and with its ref flag set to false.
// If a frame is in the `ClockReplacer`, but its ref flag is set to true, change it to false instead.
// This should be the only method that updates the clock hand.
bool ClockReplacer::Victim(frame_id_t *frame_id) {
  for (int i = clockHand; i < pageSize; i++) {
    if (!inClock[i]) {
      continue;
    }
    if (refFlag[i] == false) {
      *frame_id = i;
      forwardClockHand();
      return true;
      break;
    }
    if (refFlag[i] == true) {
      refFlag[i] = false;
      forwardClockHand();
    }
  }
  return false;
}

//This method should be called after a page is pinned to a frame in the BufferPoolManager.
// It should remove the frame containing the pinned page from the ClockReplacer.
void ClockReplacer::Pin(frame_id_t frame_id) {
  inClock[frame_id]= false;
}

void ClockReplacer::Unpin(frame_id_t frame_id) {
  inClock[frame_id]= true;
  refFlag[frame_id]= true;
}

size_t ClockReplacer::Size() {
  int size=0;
  for (int i=0;i<pageSize;i++) {
    if (inClock[i]) {
      size++;
    }
  }
  return size;
}
void ClockReplacer::forwardClockHand() {
  clockHand = clockHand+1%pageSize;
}

}  // namespace bustub
