#pragma once

#include <vector>
#include "position.hpp"
#include "exceptions.hpp"

class Move {
 public:
  Move(Position start_pos, Position end_pos)
       : Move(start_pos, {}, end_pos, {}) {}

  Move(Position start_pos, Position end_pos, std::initializer_list<Position> taken)
       : Move(start_pos, {}, end_pos, taken) {}

  Move(Position start_pos, std::initializer_list<Position> intermediate_pos,
       Position end_pos, std::initializer_list<Position> taken);

  [[nodiscard]] Position GetStartPosition() const noexcept { return start_pos_; }
  [[nodiscard]] Position GetEndPosition() const noexcept { return end_pos_; }
  [[nodiscard]] const std::vector<Position> & GetIntermediatePositions() const noexcept { return intermediate_pos_; }
  [[nodiscard]] const std::vector<Position> & GetTakenPositions() const noexcept { return taken_pos_; }

  [[nodiscard]] bool BelongsTo(const Position & position) const { return start_pos_ == position; }
  void ApplyTo(Position & position) const {
    if (!BelongsTo(position))
      return;
    position = end_pos_;
  }

  friend Move operator + (Move lhs, const Move & rhs);
  Move& operator += (const Move & rhs) { return *this = std::move(*this + rhs); }
  void RemoveFirstStep() {
    assert(!intermediate_pos_.empty() && !taken_pos_.empty());
    start_pos_ = intermediate_pos_[0];
    intermediate_pos_.erase(intermediate_pos_.begin());
    taken_pos_.erase(taken_pos_.begin());
  }

 private:
  Position start_pos_;
  std::vector<Position> intermediate_pos_;
  Position end_pos_;
  std::vector<Position> taken_pos_;
};