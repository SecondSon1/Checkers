#pragma once

#include "../bot.hpp"

class CalculatingBot : public Bot {
 public:
  CalculatingBot(PieceColor color)
      : Bot(color) {}

  [[nodiscard]] Move GetNextMove(const Board & board) noexcept override;

 public:
  static bool MoveComparison(const Move & lhs, const Move & rhs, PieceColor color);

 private:
  int32_t Evaluate(Board & board, int32_t current_color_index, int32_t alpha, int32_t beta, int32_t depth);

 private:
  PieceColor colors_[2];
};