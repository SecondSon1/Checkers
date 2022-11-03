#include "../piece.hpp"
#include "../board.hpp"

constexpr int32_t delta_x[] = { 1, 1, -1, -1 };
constexpr int32_t delta_y[] = { 1, -1, 1, -1 };

void ManTakes(std::vector<Move> & path,
              std::vector<std::vector<Move>> & paths,
              Position position,
              const Board & board,
              std::vector<std::vector<bool>> & taken,
              const Position & starting_position,
              const PieceColor & starting_color) {

  auto current_x = static_cast<int32_t>(position.GetX());
  auto current_y = static_cast<int32_t>(position.GetY());

  bool taken_at_least_one = false;

  for (size_t d = 0; d < 4; ++d) {
    try {
      Position middle_position(current_x + delta_x[d], current_y + delta_y[d]);

      if (board[middle_position].IsOccupied()) {
        std::shared_ptr<Piece> piece_ptr = board[middle_position].GetPiece();
        if (piece_ptr->GetColor() == starting_color ||
            taken[middle_position.GetX()][middle_position.GetY()])
          continue;

        Position last_position(current_x + delta_x[d] * 2, current_y + delta_y[d] * 2);
        if (board[last_position].IsOccupied() && starting_position != last_position)
          continue;

        taken_at_least_one = true;
        taken[middle_position.GetX()][middle_position.GetY()] = true;
        path.push_back(Move(position, last_position, { middle_position }));

        ManTakes(path, paths, last_position, board, taken, starting_position, starting_color);

        path.pop_back();
        taken[middle_position.GetX()][middle_position.GetY()] = false;
      }
    } catch (const PositionOutOfBoundsException & e) {}
  }

  if (!taken_at_least_one && !path.empty()) // Because if you can take, you MUST take
    paths.push_back(path);
}

std::vector<Move> Man::GetMoves(const Board & board) const noexcept {
  std::vector<Move> result;

  size_t d_from = (GetColor() == PieceColor::kBlack) * 2;
  size_t d_to = d_from + 2;
  for (size_t d = d_from; d < d_to; ++d) {
    try {
      Position move_pos(static_cast<int32_t>(GetPosition().GetX()) + delta_x[d],
                        static_cast<int32_t>(GetPosition().GetY()) + delta_y[d]);
      if (board[move_pos].IsEmpty())
        result.emplace_back(GetPosition(), move_pos);
    } catch (const PositionOutOfBoundsException & e) {}
  }

  std::vector<Move> temp_path;
  std::vector<std::vector<bool>> taken(8, std::vector<bool>(8));
  std::vector<std::vector<Move>> paths;
  ManTakes(temp_path, paths, GetPosition(), board, taken,
           GetPosition(), GetColor());
  for (std::vector<Move> & path : paths) {
    for (size_t index = 1; index < path.size(); ++index) {
      path[0] += path[index];
    }
    result.push_back(path[0]);
  }

  return result;
}
