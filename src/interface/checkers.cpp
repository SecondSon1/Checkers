#include "checkers.hpp"

Move Checkers::Proceed() {

  Move move = ProceedWithIntermediateMove();
  currently_moving_ = currently_moving_ == PieceColor::kWhite ?
                      PieceColor::kBlack : PieceColor::kWhite;

  // Lol I thought that when you don't have moves it`s stalemate like in chess
  // Turns out it counts as a win for another player. Smarter every day ig
  if (board_.HasSomeoneWon() || board_.GetAllLegalMoves(currently_moving_).empty())
    state_ = (currently_moving_ == PieceColor::kBlack) ? GameState::kWhiteWon : GameState::kBlackWon;

  return move;
}

Move Checkers::ProceedWithIntermediateMove() {
  if (IsOver())
    throw GameIsOverException();

  Move move = ((currently_moving_ == PieceColor::kWhite) ? white_player_ : black_player_)->GetNextMove(board_);
  board_.MakeTheMove(move);

  return move;
}
