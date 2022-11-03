#pragma once

#include <cstdint>
#include <string>
#include "exceptions.hpp"

class Position {
 public:
  explicit Position(size_t index) : Position(static_cast<int32_t>(index / 4),
                                             static_cast<int32_t>(index % 4 * 2 + (index / 4 % 2 == 1))) {}

  Position(size_t x, size_t y) : Position(static_cast<int32_t>(x), static_cast<int32_t>(y)) {}
  Position(int32_t x, int32_t y) : x_(x), y_(y) {
    if (x < 0 || y < 0 || x > 7 || y > 7 || (x + y) % 2 == 1)
      throw PositionOutOfBoundsException();
  }

  [[nodiscard]] uint8_t GetX() const noexcept { return x_; }
  [[nodiscard]] uint8_t GetY() const noexcept { return y_; }

  [[nodiscard]] std::string ToString() const noexcept {
    return static_cast<char>(y_ + 'a') + std::to_string(x_ + 1);
  }

 public:
  bool operator == (const Position & position) const noexcept {
    return x_ == position.x_ && y_ == position.y_;
  }
  bool operator != (const Position & position) const noexcept { return !(*this == position); }

 private:
  uint8_t x_;
  uint8_t y_;
};