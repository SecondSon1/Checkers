#include "move.hpp"
#include "pieces.hpp"

#include <cassert>

Move::Move(Position start_pos, std::initializer_list<Position> intermediate_pos,
           Position end_pos, std::initializer_list<std::shared_ptr<Piece>> taken)
           : start_pos_(start_pos), intermediate_pos_(intermediate_pos), end_pos_(end_pos), taken_pieces_(taken),
             piece_before_promotion_(nullptr), promotion_position_(0) {
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

  lhs.taken_pieces_.reserve(lhs.taken_pieces_.size() + rhs.taken_pieces_.size());
  lhs.taken_pieces_.insert(lhs.taken_pieces_.end(),
                           rhs.taken_pieces_.begin(), rhs.taken_pieces_.end());

  if (lhs.DoesPromotionHappen() && rhs.DoesPromotionHappen())
    assert(false);

  if (!lhs.DoesPromotionHappen() && rhs.DoesPromotionHappen()) {
    lhs.piece_before_promotion_ = rhs.piece_before_promotion_;
    lhs.promotion_position_ = rhs.promotion_position_;
  }
  return lhs;
}

Move Move::RemoveFirstStep() {
  assert(!intermediate_pos_.empty() && !taken_pieces_.empty());
  Move removed(start_pos_, intermediate_pos_[0], { taken_pieces_[0] });
  start_pos_ = intermediate_pos_[0];
  intermediate_pos_.erase(intermediate_pos_.begin());
  taken_pieces_.erase(taken_pieces_.begin());
  if (piece_before_promotion_ != nullptr && promotion_position_ == intermediate_pos_[0]) {
    removed.PromotionHappens(std::move(piece_before_promotion_),
                             promotion_position_);
  }
  return removed;
}
