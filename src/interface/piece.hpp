#pragma once

#include <vector>
#include "move.hpp"

enum class PieceColor {
  kWhite, kBlack
};

enum class PieceType {
  kMan, kKing
};

class Board;

class Piece {
 public:
  Piece(PieceColor color, Position position, PieceType type) : color_(color), position_(position), type_(type) {}
  virtual ~Piece() = default;

 public:
  [[nodiscard]] PieceColor GetColor() const noexcept { return color_; }
  [[nodiscard]] Position GetPosition() const noexcept { return position_; }
  [[nodiscard]] PieceType GetType() const noexcept { return type_; }

  void MoveTo(Position new_position) noexcept {
    position_ = new_position;
  }

  [[nodiscard]] virtual std::vector<Move> GetMoves(const Board & board) const noexcept = 0;

  [[nodiscard]] std::vector<Move> FilterMoves(const std::vector<Move> & moves) const noexcept {
    std::vector<Move> result;
    for (const Move & move : moves)
      if (move.GetStartPosition() == position_)
        result.push_back(move);
    return result;
  }

 private:
  PieceColor color_;
  Position position_;
  PieceType type_;
};

class Man : public Piece {
 public:
  Man(PieceColor color, Position position) : Piece(color, position, PieceType::kMan) {}

 public:
  [[nodiscard]] std::vector<Move> GetMoves(const Board & board) const noexcept override;
};

class King : public Piece {
 public:
  King(PieceColor color, Position position) : Piece(color, position, PieceType::kKing) {}

 public:
  [[nodiscard]] std::vector<Move> GetMoves(const Board & board) const noexcept override;
};