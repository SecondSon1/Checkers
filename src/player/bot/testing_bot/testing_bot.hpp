#pragma once

#include "../bot.hpp"
#include <random>

class TestingBot : public Bot {
 public:
  TestingBot(PieceColor color, int depth, int king_val, int norm_val, int blocked_val, uint32_t seed)
      : Bot(color), depth_(depth), king_value_(king_val), normal_value_(norm_val),
        blocked_value_(blocked_val), rng_(seed) {}

  [[nodiscard]] Move GetNextMove(const Board & board) noexcept override;

 public:
  static bool MoveComparison(const Move & lhs, const Move & rhs, PieceColor color);

 private:
  int32_t Evaluate(Board & board, int32_t current_color_index, int32_t alpha, int32_t beta, int32_t depth);

 private:
  PieceColor colors_[2];
  int depth_;
  int king_value_;
  int normal_value_;
  int blocked_value_;
  std::mt19937 rng_;
};