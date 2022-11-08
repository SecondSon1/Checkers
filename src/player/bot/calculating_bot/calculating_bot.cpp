#include "calculating_bot.hpp"

PieceColor current_color;
PieceColor colors[2];

bool MoveComparison(const Move & lhs, const Move & rhs) {
  if (lhs.DoesPromotionHappen() && !rhs.DoesPromotionHappen())
    return true;
  size_t left_taken = lhs.GetTakenPieces().size();
  size_t right_taken = rhs.GetTakenPieces().size();
  if (left_taken != right_taken)
    return left_taken < right_taken;

  int left_x = lhs.GetEndPosition().GetX();
  int relative_left_x = current_color == PieceColor::kWhite ? left_x : 7 - left_x;

  int right_x = rhs.GetEndPosition().GetX();
  int relative_right_x = current_color == PieceColor::kWhite ? right_x : 7 - right_x;

  return relative_left_x > relative_right_x;
}

int Evaluate(Board & board, int current_color_index, int alpha, int beta, int depth) {
  std::vector<Move> moves = board.GetAllLegalMoves(colors[current_color_index]);

  if (moves.empty())
    return -1e9;

  if (depth == 0) {

    int total[2] = { 0,0 };
    for (size_t index = 0; index < 32; ++index) {
      Position pos(index);
      if (((const Board &) board)[pos].IsOccupied()) {
        int cost = ((const Board &) board)[pos].GetPiece()->GetType() == PieceType::kKing ? 4 : 1;
        total[((const Board &) board)[pos].GetPiece()->GetColor() == colors[1]] += cost;
      }
    }

    return total[current_color_index] - total[current_color_index ^ 1];
  }

  current_color = colors[current_color_index];
  std::sort(moves.begin(), moves.end(), MoveComparison);

  for (const Move & move : moves) {
    board.MakeMove(move);

    int eval = -Evaluate(board, current_color_index ^ 1,
                         -beta, -alpha, depth - 1);

    board.UndoMove(move);

    if (eval > alpha)
      alpha = eval;

    if (beta <= alpha)
      return alpha;

  }

  return alpha;
}


Move CalculatingBot::GetNextMove(const Board & const_board) noexcept {
  Board board(const_board);

  colors[0] = GetColor();
  colors[1] = GetColor() == PieceColor::kWhite ? PieceColor::kBlack : PieceColor::kWhite;

  int alpha = -1e9;
  int beta = 1e9;
  auto our_moves = board.GetAllLegalMoves(GetColor());
  Move resulting_move = our_moves[0];

  for (const Move & move : our_moves) {
    board.MakeMove(move);

    int eval = -Evaluate(board, 1, -beta, -alpha, 7);

    if (eval > alpha) {
      alpha = eval;
      resulting_move = move;
    }

    board.UndoMove(move);
  }

  return resulting_move;

}
