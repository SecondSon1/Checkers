#include "move.hpp"

Move::Move(Position start_pos, std::initializer_list<Position> intermediate_pos,
           Position end_pos, std::initializer_list<Position> taken)
           : start_pos_(start_pos), intermediate_pos_(intermediate_pos), end_pos_(end_pos), taken_pos_(taken) {
  if (intermediate_pos_.empty()) {
    if (start_pos_ == end_pos_)
      throw EmptyMoveException();
  } else {
    bool bad_move_among_intermediates = false;
    for (size_t index = 1; index < intermediate_pos_.size(); ++index) {
      if (intermediate_pos_[index - 1] == intermediate_pos_[index]) {
        bad_move_among_intermediates = true;
        break;
      }
    }
    if (bad_move_among_intermediates || start_pos_ == intermediate_pos_[0] || intermediate_pos_.back() == end_pos_)
      throw EmptyMoveException();
  }
}

Move operator + (Move lhs, const Move & rhs) {
  if (lhs.end_pos_ != rhs.start_pos_)
    throw MovesNotCompatibleException();
  lhs.intermediate_pos_.reserve(lhs.intermediate_pos_.size() + 1 + rhs.intermediate_pos_.size());
  lhs.intermediate_pos_.push_back(lhs.end_pos_);
  lhs.intermediate_pos_.insert(lhs.intermediate_pos_.end(),
                               rhs.intermediate_pos_.begin(), rhs.intermediate_pos_.end());
  lhs.end_pos_ = rhs.end_pos_;

  lhs.taken_pos_.reserve(lhs.taken_pos_.size() + rhs.taken_pos_.size());
  lhs.taken_pos_.insert(lhs.taken_pos_.end(),
                        rhs.taken_pos_.begin(), rhs.taken_pos_.end());
  return lhs;
}