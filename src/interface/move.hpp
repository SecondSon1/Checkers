#pragma once

#include <vector>
#include "position.hpp"
#include "exceptions.hpp"

class Piece;

class Move {
 public:
  Move(Position start_pos, Position end_pos)
       : Move(start_pos, {}, end_pos, {}) {}

  Move(Position start_pos, Position end_pos, std::initializer_list<std::shared_ptr<Piece>> taken)
       : Move(start_pos, {}, end_pos, taken) {}

  Move(Position start_pos, std::initializer_list<Position> intermediate_pos,
       Position end_pos, std::initializer_list<std::shared_ptr<Piece>> taken);

  [[nodiscard]] Position GetStartPosition() const noexcept { return start_pos_; }
  [[nodiscard]] Position GetEndPosition() const noexcept { return end_pos_; }
  [[nodiscard]] const std::vector<Position> & GetIntermediatePositions() const noexcept { return intermediate_pos_; }
  [[nodiscard]] const std::vector<std::shared_ptr<Piece>> & GetTakenPieces() const noexcept { return taken_pieces_; }
  [[nodiscard]] bool DoesPromotionHappen() const noexcept { return piece_before_promotion_ != nullptr; }
  [[nodiscard]] std::shared_ptr<Piece> GetPieceBeforePromotion() const noexcept { return piece_before_promotion_; }
  [[nodiscard]] Position GetPromotionPosition() const noexcept { return promotion_position_; }

  void PromotionHappens(std::shared_ptr<Piece> piece_before_promotion, Position promotion_position) noexcept {
    piece_before_promotion_ = std::move(piece_before_promotion);
    promotion_position_ = promotion_position;
  }

  [[nodiscard]] bool BelongsTo(const Position & position) const { return start_pos_ == position; }
  void ApplyTo(Position & position) const {
    if (!BelongsTo(position))
      return;
    position = end_pos_;
  }

  friend Move operator + (Move lhs, const Move & rhs);
  Move& operator += (const Move & rhs) { return *this = std::move(*this + rhs); }
  Move RemoveFirstStep();

 private:
  Position start_pos_;
  std::vector<Position> intermediate_pos_;
  Position end_pos_;
  std::vector<std::shared_ptr<Piece>> taken_pieces_;
  std::shared_ptr<Piece> piece_before_promotion_;
  Position promotion_position_;
};