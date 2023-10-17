#include "human_bot.hpp"


int32_t HumanBot::Evaluate(Board & board, int32_t current_color_index, int32_t alpha, int32_t beta, int32_t depth) {
  std::vector<Move> moves = board.GetAllLegalMoves(colors_[current_color_index]);

  if (moves.empty())
    return -1e9;

  if (depth == 0) {

    int32_t total[2] = { 0,0 };
    for (size_t index = 0; index < 32; ++index) {
      Position pos(index);
      if (((const Board &) board)[pos].IsOccupied()) {
        int32_t cost = ((const Board &) board)[pos].GetPiece()->GetType() == PieceType::kKing ? 4 : 1;
        total[((const Board &) board)[pos].GetPiece()->GetColor() == colors_[1]] += cost;
      }
    }

    return total[current_color_index] - total[current_color_index ^ 1];
  }

  std::shuffle(moves.begin(), moves.end(), rng_);

  for (const Move & move : moves) {
    if ((rng_() & 63) == 0)
      continue; // humans make mistakes so our bot has to too

    board.MakeMove(move);

    int32_t eval = -Evaluate(board, current_color_index ^ 1,
                         -beta, -alpha, depth - 1);

    board.UndoMove(move);

    if (eval > alpha)
      alpha = eval;

    if (beta <= alpha)
      return alpha;

  }

  return alpha;
}


Move HumanBot::GetNextMove(const Board & const_board) noexcept {
  Board board(const_board);

  colors_[0] = GetColor();
  colors_[1] = GetColor() == PieceColor::kWhite ? PieceColor::kBlack : PieceColor::kWhite;

  int32_t alpha = -1e9;
  int32_t beta = 1e9;
  auto our_moves = board.GetAllLegalMoves(GetColor());
  Move resulting_move = our_moves[0];
  if (our_moves.size() == 1)
    return resulting_move;

  for (const Move & move : our_moves) {
    board.MakeMove(move);

    int32_t eval = -Evaluate(board, 1, -beta, -alpha, 5);

    if (eval > alpha) {
      alpha = eval;
      resulting_move = move;
    }

    board.UndoMove(move);
  }

  return resulting_move;

}
