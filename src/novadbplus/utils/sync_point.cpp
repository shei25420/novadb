// Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved.
// Please refer to the license text that comes with this novadb open source
// project for additional information.

//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.

#include "novadbplus/utils/sync_point.h"

#ifdef WITH_SYNC_POINT
namespace novadbplus {

SyncPoint* SyncPoint::GetInstance() {
  static SyncPoint sync_point;
  return &sync_point;
}

void SyncPoint::LoadDependency(const std::vector<Dependency>& dependencies) {
  std::unique_lock<std::mutex> lock(mutex_);
  successors_.clear();
  predecessors_.clear();
  cleared_points_.clear();
  for (const auto& dependency : dependencies) {
    successors_[dependency.predecessor].push_back(dependency.successor);
    predecessors_[dependency.successor].push_back(dependency.predecessor);
  }
  cv_.notify_all();
}

bool SyncPoint::PredecessorsAllCleared(const std::string& point) {
  for (const auto& pred : predecessors_[point]) {
    if (cleared_points_.count(pred) == 0) {
      return false;
    }
  }
  return true;
}

void SyncPoint::SetCallBack(const std::string point,
                            std::function<void(void*)> callback) {
  std::unique_lock<std::mutex> lock(mutex_);
  callbacks_[point] = callback;
}

void SyncPoint::ClearAllCallBacks() {
  std::unique_lock<std::mutex> lock(mutex_);
  while (num_callbacks_running_ > 0) {
    cv_.wait(lock);
  }
  callbacks_.clear();
}

void SyncPoint::EnableProcessing() {
  std::unique_lock<std::mutex> lock(mutex_);
  enabled_ = true;
}

void SyncPoint::DisableProcessing() {
  std::unique_lock<std::mutex> lock(mutex_);
  enabled_ = false;
}

void SyncPoint::ClearTrace() {
  std::unique_lock<std::mutex> lock(mutex_);
  cleared_points_.clear();
}

void SyncPoint::Process(const std::string& point, void* cb_arg) {
  std::unique_lock<std::mutex> lock(mutex_);

  if (!enabled_)
    return;

  auto callback_pair = callbacks_.find(point);
  if (callback_pair != callbacks_.end()) {
    num_callbacks_running_++;
    mutex_.unlock();
    callback_pair->second(cb_arg);
    mutex_.lock();
    num_callbacks_running_--;
    cv_.notify_all();
  }

  while (!PredecessorsAllCleared(point)) {
    cv_.wait(lock);
  }

  cleared_points_.insert(point);
  cv_.notify_all();
}
}  // namespace novadbplus
#endif  // WITH_SYNC_POINT
