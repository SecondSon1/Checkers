#include "board.hpp"
#include "../util.hpp"


Board::Board(std::string config) {
  bool ok = config.size() == 33 && config.back() != '.';
  for (char c : config)
    ok &= (ToLowerCase(c) == 'w') || (ToLowerCase(c) == 'b') || c == '.';
  if (!ok)
    throw InvalidConfigException();

  for (size_t index = 0; index < 32ul; ++index) {
    Position pos(index);
    char ch = config[index];
    if (ch == '.') continue;
    PieceColor color = (ch == 'b' || ch == 'B') ? PieceColor::kBlack : PieceColor::kWhite;
    bool king = ch == 'B' || ch == 'W';
    std::shared_ptr<Piece> ptr;
    if (king)
      ptr = std::make_shared<King>(color, pos);
    else
      ptr = std::make_shared<Man>(color, pos);
    (*this)[pos].Set(std::move(ptr));
  }
}


void Board::MakeMove(const Move & move) {
  std::shared_ptr<Piece> piece_ptr = (*this)[move.GetStartPosition()].GetPiece();
  piece_ptr->MoveTo(move.GetEndPosition());

  (*this)[move.GetStartPosition()].Clear();

  for (const std::shared_ptr<Piece> & taken : move.GetTakenPieces())
    (*this)[taken->GetPosition()].Clear();

  (*this)[move.GetEndPosition()].Set(piece_ptr);

  if (move.DoesPromotionHappen())
    (*this)[move.GetEndPosition()] = Space(
        std::make_shared<King>(piece_ptr->GetColor(), piece_ptr->GetPosition())
    );
}


void Board::UndoMove(const Move & move) {
  std::shared_ptr<Piece> piece_ptr = (*this)[move.GetEndPosition()].GetPiece();
  piece_ptr->MoveTo(move.GetStartPosition());

  (*this)[move.GetEndPosition()].Clear();

  for (const std::shared_ptr<Piece> & taken : move.GetTakenPieces())
    (*this)[taken->GetPosition()].Set(taken);

  (*this)[move.GetStartPosition()].Set(piece_ptr);

  if (move.DoesPromotionHappen())
    (*this)[move.GetStartPosition()] = Space(
        std::make_shared<Man>(piece_ptr->GetColor(), piece_ptr->GetPosition())
    );
}


bool Board::HasSomeoneWon() const {
  size_t white_count = 0;
  size_t black_count = 0;

  for (size_t i = 0; i < 8; ++i) {
    for (size_t j = 0; j < 8; ++j) {
      if (board_[i][j].IsEmpty()) continue;
      if (board_[i][j].GetPiece()->GetColor() == PieceColor::kWhite)
        white_count++;
      else
        black_count++;
    }
  }

  return white_count == 0 || black_count == 0;
}


std::vector<Move> Board::GetAllLegalMoves(PieceColor current_color) const {
  std::vector<Move> peaceful_moves;
  std::vector<Move> take_moves;

  for (const auto & file : board_) {
    for (const auto & rank : file) {
      if (rank.IsEmpty())
        continue;
      std::shared_ptr<const Piece> piece_ptr = rank.GetPiece();
      if (piece_ptr->GetColor() == current_color) {
        auto piece_moves = piece_ptr->GetMoves(*this);
        for (const Move & move : piece_moves) {
          if (move.GetTakenPieces().empty())
            peaceful_moves.push_back(move);
          else
            take_moves.push_back(move);
        }
      }
    }
  }

  return take_moves.empty() ? peaceful_moves : take_moves;
}