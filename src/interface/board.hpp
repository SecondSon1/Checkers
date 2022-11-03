#pragma once

#include "space.hpp"

class Board {
 public:
  explicit Board(std::string config);

 public:
  void MakeTheMove(const Move & move);
  // TODO: Make "undo move"

  Space operator [] (Position position) const noexcept {
    return board_[position.GetX()][position.GetY()];
  }

  [[nodiscard]] bool HasSomeoneWon() const;

  [[nodiscard]] std::vector<Move> GetAllLegalMoves(PieceColor current_color) const;

 private:
  Space & operator [] (Position position) noexcept {
    return board_[position.GetX()][position.GetY()];
  }

  void TryPromoting(Position position);

 private:
  Space board_[8][8];
};