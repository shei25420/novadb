// Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved.
// Please refer to the license text that comes with this novadb open source
// project for additional information.

#ifndef SRC_novadbPLUS_LOCK_MGL_MGL_H__
#define SRC_novadbPLUS_LOCK_MGL_MGL_H__

#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>
#include <string>

#include "novadbplus/lock/mgl/lock_defines.h"
#include "novadbplus/lock/mgl/mgl_mgr.h"

namespace novadbplus {

namespace mgl {

class LockSchedCtx;

// multi granularity lock
// each lock can lock only one target, use multiple MGLocks
// if you want to lock different targets.
class MGLock {
 public:
  explicit MGLock(MGLockMgr* mgr);
  MGLock(const MGLock&) = delete;
  MGLock(MGLock&&) = delete;
  MGLock& operator=(const MGLock&) const = delete;
  ~MGLock();
  LockRes lock(const std::string& target, LockMode mode, uint64_t timeoutMs);
  void unlock();
  uint64_t getHash() const {
    return _targetHash;
  }
  LockMode getMode() const {
    return _mode;
  }
  LockRes getStatus() const;
  const std::string& getTarget() const {
    return _target;
  }
  std::string toString() const;
  const std::string& getThreadId() const {
    return _threadId;
  }

 private:
  friend class LockSchedCtx;
  void setLockResult(LockRes res, std::list<MGLock*>::iterator iter);
  void releaseLockResult();
  std::list<MGLock*>::iterator getLockIter() const;
  void notify();
  bool waitLock(uint64_t timeoutMs);

  const uint64_t _id;
  std::string _target;
  uint64_t _targetHash;
  LockMode _mode;

  // wrote by MGLockMgr
  mutable std::mutex _mutex;
  std::condition_variable _cv;
  LockRes _res;
  std::list<MGLock*>::iterator _resIter;
  MGLockMgr* _lockMgr;
  std::string _threadId;

  static std::atomic<uint64_t> _idGen;
  static std::list<MGLock*> _dummyList;
};

}  // namespace mgl

}  // namespace novadbplus

#endif  // SRC_novadbPLUS_LOCK_MGL_MGL_H__
