#pragma once

#include "../bot.hpp"
#include <random>

class HumanBot : public Bot {
 public:
  HumanBot(PieceColor color)
      : Bot(color), rng_(std::random_device{}()) {}

  [[nodiscard]] Move GetNextMove(const Board & board) noexcept override;


 private:
  int32_t Evaluate(Board & board, int32_t current_color_index, int32_t alpha, int32_t beta, int32_t depth);

 private:
  std::mt19937 rng_;
  PieceColor colors_[2];
};