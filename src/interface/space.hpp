#pragma once

#include <memory>
#include "pieces.hpp"

class Space {
 public:
  Space() : occupied_(false), piece_(nullptr) {}
  Space(std::shared_ptr<Piece> ptr) : occupied_(true), piece_(std::move(ptr)) {}

 public:
  [[nodiscard]] bool IsOccupied() const noexcept { return occupied_; }
  [[nodiscard]] bool IsEmpty() const noexcept { return !occupied_; }
  [[nodiscard]] std::shared_ptr<const Piece> GetPiece() const noexcept { return piece_; }
  [[nodiscard]] std::shared_ptr<Piece> GetPiece() noexcept { return piece_; }

  void Clear() noexcept {
    occupied_ = false;
    piece_ = nullptr;
  }

  void Set(std::shared_ptr<Piece> ptr) noexcept {
    occupied_ = true;
    piece_ = std::move(ptr);
  }

 private:
  bool occupied_;
  std::shared_ptr<Piece> piece_;
};