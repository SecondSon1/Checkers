#pragma once

#include "../bot.hpp"
#include <random>

class HumanBot : public Bot {
 public:
  HumanBot(PieceColor color)
      : Bot(color), rng_(1810), uid_(0, std::numeric_limits<int32_t>::max()) {}

  [[nodiscard]] Move GetNextMove(const Board & board) noexcept override {
    std::vector<Move> all_moves = board.GetAllLegalMoves(GetColor());
    return all_moves[uid_(rng_) % all_moves.size()];
  }

 private:
  std::mt19937 rng_;
  std::uniform_int_distribution<> uid_;
};