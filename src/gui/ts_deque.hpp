#pragma once

#include <deque>
#include <mutex>

template<typename T>
class TSDeque {
 public:
  TSDeque() = default;
  TSDeque(const TSDeque<T> &) = delete;
  virtual ~TSDeque() { Clear(); }

  const T& Front() {
    std::scoped_lock<std::mutex> lock(mutex_);
    return deque_.front();
  }
  const T& Back() {
    std::scoped_lock<std::mutex> lock(mutex_);
    return deque_.back();
  }

  void PushFront(const T& item) {
    std::scoped_lock<std::mutex> lock(mutex_);
    deque_.push_front(item);
  }
  void PushBack(const T& item) {
    std::scoped_lock<std::mutex> lock(mutex_);
    deque_.push_back(item);
  }

  bool Empty() {
    std::scoped_lock<std::mutex> lock(mutex_);
    return deque_.empty();
  }
  size_t Size() {
    std::scoped_lock<std::mutex> lock(mutex_);
    return deque_.size();
  }
  void Clear() {
    std::scoped_lock<std::mutex> lock(mutex_);
    deque_.clear();
  }
  T PopFront() {
    std::scoped_lock<std::mutex> lock(mutex_);
    auto item = std::move(deque_.front());
    deque_.pop_front();
    return item;
  }
  T PopBack() {
    std::scoped_lock<std::mutex> lock(mutex_);
    auto item = std::move(deque_.back());
    deque_.pop_back();
    return item;
  }

 private:
  std::deque<T> deque_;
  std::mutex mutex_;
};