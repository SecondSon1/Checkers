#pragma once

#include "../interface/move.hpp"
#include "../interface/board.hpp"

enum class PlayerType {
  kHuman, kBot
};

class Player {
 public:
  Player() = delete;
  virtual ~Player() = default;

  virtual Move GetNextMove(const Board & board) noexcept = 0;

  [[nodiscard]] PlayerType GetType() const { return type_; }
  [[nodiscard]] PieceColor GetColor() const { return color_; }

 protected:
  Player(PlayerType type, PieceColor color) : type_(type), color_(color) {}

 private:
  PlayerType type_;
  PieceColor color_;
};