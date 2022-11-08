#pragma once

#include "../bot.hpp"
#include <random>

class CalculatingBot : public Bot {
 public:
  CalculatingBot(PieceColor color)
      : Bot(color), rng_(2281337), uid_(0, std::numeric_limits<int32_t>::max()) {}

  [[nodiscard]] Move GetNextMove(const Board & board) noexcept override;

 private:
  std::mt19937 rng_;
  std::uniform_int_distribution<> uid_;
};