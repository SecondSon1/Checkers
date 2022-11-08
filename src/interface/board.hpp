#pragma once

#include "space.hpp"

class Board {
 public:
  explicit Board(std::string config);
  Board(const Board & another_board) = default;

 public:
  void MakeMove(const Move & move);
  void UndoMove(const Move & move);

  Space operator [] (Position position) const noexcept {
    return board_[position.GetX()][position.GetY()];
  }

  [[nodiscard]] bool HasSomeoneWon() const;

  [[nodiscard]] std::vector<Move> GetAllLegalMoves(PieceColor current_color) const;

  bool operator == (const Board & rhs) const {
    for (size_t i = 0; i < 8; ++i)
      for (size_t j = 0; j < 8; ++j)
        if (board_[i][j].GetPiece() != rhs.board_[i][j].GetPiece())
          return false;
    return true;
  }

 private:
  Space & operator [] (Position position) noexcept {
    return board_[position.GetX()][position.GetY()];
  }

 private:
  Space board_[8][8];
};