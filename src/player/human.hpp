#pragma once

#include "player.hpp"

class Human : public Player {
 public:
  Human(PieceColor color) : Player(PlayerType::kHuman, color),
                            next_move_(Position(0, 0), Position(1, 1)) {}

  Move GetNextMove(const Board & board) noexcept override {
    return next_move_;
  }

 private:
  friend class GameScene;

  void SetNextMove(Move next_move) {
    next_move_ = std::move(next_move);
  }

  Move next_move_;
};