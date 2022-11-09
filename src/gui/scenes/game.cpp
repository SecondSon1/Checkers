#include "game.hpp"
#include "../window.hpp"
#include "game_over.hpp"

#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>
#include <cmath>


void GameScene::Init() {
  // Upmost 215 px - top padding + move indicator + maybe time
  board_size_ = std::min(GetWindowPtr()->GetWidth(),
                               GetWindowPtr()->GetHeight() - 215) - 30;
  square_size_ = board_size_ / 8;
  board_size_ = square_size_ * 8;

  info_width_ = GetWindowPtr()->GetWidth() - 30;
  info_height_ = 150;

  if (!board_texture_.create(board_size_, board_size_) ||
      !pieces_texture_.create(board_size_, board_size_) ||
      !moves_texture_.create(board_size_, board_size_) ||
      !info_texture_.create(info_width_, info_height_))
    throw std::runtime_error("Unable to create button_texture_");

  board_position_ = sf::Vector2f(static_cast<float>(GetWindowPtr()->GetWidth() - board_size_) / 2.f,
                              static_cast<float>(GetWindowPtr()->GetHeight() - board_size_ - 15));
  info_position_ = sf::Vector2f(static_cast<float>(GetWindowPtr()->GetWidth() - info_width_) / 2.f, 15.f);

  DrawBoard(board_texture_);

  move_pool_ = game_.GetBoard().GetAllLegalMoves(game_.GetCurrentlyMoving());
}


void GameScene::Draw(const sf::RenderWindow & window, sf::RenderTexture & texture) {
  // padding 15 px on all sides
  sf::Color transparent(0, 0, 0, 0);
  pieces_texture_.clear(transparent);
  DrawPieces(pieces_texture_);
  DrawFloatingPiece(pieces_texture_,
                    sf::Vector2f(sf::Mouse::getPosition(window)) - board_position_);
  Animate(pieces_texture_);
  pieces_texture_.display();

  moves_texture_.clear(transparent);
  DrawPossibleMoves(moves_texture_);
  moves_texture_.display();

  info_texture_.clear(transparent);
  DrawInfo(info_texture_);
  info_texture_.display();

  sf::Sprite board_sprite(board_texture_),
             piece_sprite(pieces_texture_.getTexture()),
             moves_sprite(moves_texture_.getTexture()),
             info_sprite(info_texture_.getTexture());

  board_sprite.setPosition(board_position_);
  texture.draw(board_sprite);
  moves_sprite.setPosition(board_position_);
  texture.draw(moves_sprite);
  piece_sprite.setPosition(board_position_);
  texture.draw(piece_sprite);
  info_sprite.setPosition(info_position_);
  texture.draw(info_sprite);
}


Position GameScene::GetPositionFromMouse(const sf::RenderWindow & window) const {
  sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
  mouse_pos.x = static_cast<int32_t>(round(static_cast<float>(mouse_pos.x) - board_position_.x));
  mouse_pos.y = static_cast<int32_t>(round(static_cast<float>(mouse_pos.y) - board_position_.y));
  if (mouse_pos.x < 0 || mouse_pos.y < 0 || mouse_pos.x >= board_size_ || mouse_pos.y >= board_size_)
    throw PositionOutOfBoundsException();
  size_t x = mouse_pos.x / square_size_;
  size_t y = mouse_pos.y / square_size_;
  return {7-y, x};
}


sf::Vector2f GameScene::GetCoordsFromPosition(const Position & pos) const {
  return sf::Vector2f(sf::Vector2<size_t>(pos.GetY() * square_size_, (7 - pos.GetX()) * square_size_));
}


void GameScene::HandleEvent(sf::RenderWindow & window, sf::Event & evt) {
  {
    std::lock_guard guard(animation_mutex_);
    if (animation_state_ == AnimationState::kGoing)
      return;
  }

  if (evt.type == sf::Event::KeyPressed) {
    if (evt.key.code == sf::Keyboard::Space) { // If a bot is to move than it does
      if ((game_.GetCurrentlyMoving() == PieceColor::kWhite && white_human_player_ == nullptr) ||
          (game_.GetCurrentlyMoving() == PieceColor::kBlack && black_human_player_ == nullptr))
        move_entered_ = true;
      return;
    } else if (evt.key.code == sf::Keyboard::Backspace) { // Roll back last move
      if (!game_.GetMoveHistory().empty()) {
        game_.GoBack();
        sequential_move_in_progress_ = false;
        chosen_piece_ = nullptr;
        possible_moves_.clear();
        move_pool_ = game_.GetBoard().GetAllLegalMoves(game_.GetCurrentlyMoving());
      }
    }
  }
  if (evt.type != sf::Event::MouseButtonPressed && evt.type != sf::Event::MouseButtonReleased)
    return;
  if (evt.mouseButton.button != sf::Mouse::Left)
    return;

  try {
    Position pos = GetPositionFromMouse(window);

    std::lock_guard<std::mutex> guard_piece(chosen_piece_mutex_);
    std::lock_guard<std::mutex> guard_moves(possible_moves_mutex_);

    if (evt.type == sf::Event::MouseButtonPressed) {

      if (game_.GetBoard()[pos].IsEmpty())
        return;

      std::shared_ptr<Piece> piece_ptr = game_.GetBoard()[pos].GetPiece();
      if (piece_ptr->GetColor() != game_.GetCurrentlyMoving() ||
          ((game_.GetCurrentlyMoving() == PieceColor::kWhite ? white_human_player_ : black_human_player_) == nullptr))
        return;

      chosen_piece_ = std::move(piece_ptr);
      if (!sequential_move_in_progress_)
        possible_moves_ = chosen_piece_->FilterMoves(move_pool_);
      floating_offset_ = sf::Vector2f(sf::Mouse::getPosition(window));
      floating_offset_ -= board_position_;
      floating_offset_ -= sf::Vector2f(static_cast<float>(chosen_piece_->GetPosition().GetY() * square_size_),
                                       static_cast<float>((7 - chosen_piece_->GetPosition().GetX()) * square_size_));

    } else if (evt.type == sf::Event::MouseButtonReleased) {

      std::vector<Move> possible_move_prefixes;
      std::shared_ptr<Piece> taken;
      Move first_move(Position(0), Position(1));
      for (const Move & move : possible_moves_) {
        if (move.GetIntermediatePositions().empty() && pos == move.GetEndPosition()) {
          std::shared_ptr<Human> & human_player_ptr = game_.GetCurrentlyMoving() == PieceColor::kWhite ?
                                                      white_human_player_ : black_human_player_;
          human_player_ptr->SetNextMove(move);
          move_entered_ = true;
          possible_moves_.clear();
          chosen_piece_ = nullptr;
          return;
        }
        if (!move.GetIntermediatePositions().empty() && pos == move.GetIntermediatePositions()[0]) {
          possible_move_prefixes.push_back(move);
          assert(!move.GetTakenPieces().empty());
          if (taken == nullptr)
            taken = move.GetTakenPieces()[0];
          else
            assert(taken == move.GetTakenPieces()[0]);
        }
      }

      if (possible_move_prefixes.empty())
        throw PositionOutOfBoundsException();

      possible_moves_.clear();
      while (!possible_move_prefixes.empty()) {
        Move move = std::move(possible_move_prefixes.back());
        possible_move_prefixes.pop_back();
        first_move = std::move(move.RemoveFirstStep());
        possible_moves_.push_back(move);
      }

      std::shared_ptr<Human> & human_player_ptr = game_.GetCurrentlyMoving() == PieceColor::kWhite ?
                                                  white_human_player_ : black_human_player_;
      human_player_ptr->SetNextMove(first_move);
      game_.ProceedWithIntermediateMove();
      sequential_move_in_progress_ = true;

    }
  } catch (const PositionOutOfBoundsException & e) {
    if (evt.type == sf::Event::MouseButtonReleased && chosen_piece_ != nullptr) {
      std::lock_guard<std::mutex> guard_piece(chosen_piece_mutex_);
      std::lock_guard<std::mutex> guard_moves(possible_moves_mutex_);
      chosen_piece_ = nullptr;
      if (!sequential_move_in_progress_)
        possible_moves_.clear();
    }
  }
}


void GameScene::HandleLogic(sf::RenderWindow & window) {
  if (move_entered_) {

    bool finished = false;

    {
      std::lock_guard<std::mutex> animation_guard(animation_mutex_);

      if (animation_state_ == AnimationState::kGoing)
        return;

      if (animation_state_ == AnimationState::kFinished) {
        animation_state_ = AnimationState::kIdle;
        finished = true;
      } else if (!IsOneCurrentlyMovingHuman() && animation_state_ == AnimationState::kIdle) {
        SetUpAnimation();
        return;
      }

    }

    move_entered_ = false;
    sequential_move_in_progress_ = false;

    game_.Proceed();

    if (finished) {
      std::lock_guard<std::mutex> animation_guard(animation_mutex_);
      animated_piece_ = nullptr;
    }

    if (game_.IsRunning())
      move_pool_ = game_.GetBoard().GetAllLegalMoves(game_.GetCurrentlyMoving());
    else {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      SwitchScene(std::make_unique<GameOverScene>(
            game_.GetState() == GameState::kWhiteWon ? PieceColor::kWhite : PieceColor::kBlack,
            font_,
            GetWindowPtr()->GetBuffer()
          ));
    }
  }
}


float GameScene::GetMoveTime(const Move & move) const {
  float dx = static_cast<float>(move.GetStartPosition().GetX()) -
             static_cast<float>(move.GetEndPosition().GetX());
  float dy = static_cast<float>(move.GetStartPosition().GetY()) -
             static_cast<float>(move.GetEndPosition().GetY());

  return sqrt(dx * dx + dy * dy) / animated_move_speed_;
}


void GameScene::SetUpAnimation() {
  // already under mutex guard from context (invariant)
  animation_state_ = AnimationState::kGoing;
  animated_move_ = game_.GetNextMove();
  animated_currently_moving_ = animated_move_.GetIntermediatePositions().empty() ?
      animated_move_ : animated_move_.RemoveFirstStep();
  animation_start_time_ = std::chrono::steady_clock::now();
  animated_move_time_ = GetMoveTime(animated_currently_moving_);
  animated_piece_ = game_.GetBoard()[animated_currently_moving_.GetStartPosition()].GetPiece();
  assert(animated_piece_ != nullptr);
}


void GameScene::DrawBoard(sf::Texture & texture) {
  auto *pixels = new sf::Uint8[4 * board_size_ * board_size_];
  for (size_t x = 0; x < board_size_; x += square_size_) {
    for (size_t y = 0; y < board_size_; y += square_size_) {
      sf::Color color = ((7 - x / square_size_) + y / square_size_) % 2 == 0 ? black_square_color_ : white_square_color_;
      for (size_t i = x; i < x + square_size_; ++i) {
        for (size_t j = y; j < y + square_size_; ++j) {
          size_t ind = (i * board_size_ + j) * 4;
          pixels[ind] = color.r;
          pixels[ind + 1] = color.g;
          pixels[ind + 2] = color.b;
          pixels[ind + 3] = color.a;
        }
      }
    }
  }
  texture.update(pixels);
  delete[] pixels;
}


void GameScene::DrawPiece(sf::RenderTexture & texture,
                          const std::shared_ptr<Piece> & piece,
                          sf::Vector2f top_left_corner) {
  sf::CircleShape shape(static_cast<float>(square_size_) / 2.f - 10);
  shape.setFillColor(piece->GetColor() == PieceColor::kWhite ? white_piece_color_ : black_piece_color_);
  shape.setOutlineColor(piece->GetColor() == PieceColor::kWhite ? black_piece_color_ : white_piece_color_);
  shape.setOutlineThickness(3.f);
  float position = (static_cast<float>(square_size_) - shape.getRadius() * 2.f) / 2.f;
  sf::Vector2f screen_pos(position, position);
  shape.setPosition(top_left_corner + screen_pos);
  texture.draw(shape);

  if (piece->GetType() == PieceType::kKing) {
    sf::Text k_sym("K", font_, floor(shape.getRadius() * 1.3f));
    k_sym.setFillColor(shape.getOutlineColor());
    float sym_width = k_sym.getLocalBounds().width - k_sym.getLocalBounds().left;
    float sym_height = k_sym.getLocalBounds().height - k_sym.getLocalBounds().top;
    sf::Vector2f label_position((shape.getRadius() * 2 - sym_width) / 2.1f,
                                (shape.getRadius() * 2 - sym_height * 3) / 2.f);
    k_sym.setPosition(top_left_corner + screen_pos + label_position);
    texture.draw(k_sym);
  }
}


void GameScene::DrawPieces(sf::RenderTexture & texture) {
  std::vector<Position> ignoring_positions;

  {
    std::lock_guard<std::mutex> guard_piece(chosen_piece_mutex_);
    if (chosen_piece_ != nullptr)
      ignoring_positions.push_back(chosen_piece_->GetPosition());
  }
  {
    std::lock_guard<std::mutex> guard_animation(animation_mutex_);
    if (animation_state_ == AnimationState::kGoing)
      ignoring_positions.push_back(animated_piece_->GetPosition());
  }

  for (size_t index = 0; index < 32; ++index) {
    Position pos(index);
    if (game_.GetBoard()[pos].IsEmpty() ||
        std::count(ignoring_positions.begin(), ignoring_positions.end(), pos) > 0)
      continue;
    std::shared_ptr<Piece> piece = game_.GetBoard()[pos].GetPiece();
    DrawPiece(texture, piece, GetCoordsFromPosition(pos));
  }
}


void GameScene::DrawFloatingPiece(sf::RenderTexture &texture, sf::Vector2f relative_mouse_coords) {
  if (chosen_piece_ == nullptr) return;
  std::lock_guard<std::mutex> guard(chosen_piece_mutex_);
  DrawPiece(texture, chosen_piece_, relative_mouse_coords - floating_offset_);
}


void GameScene::DrawSquareInColor(sf::RenderTexture & texture,
                                  const Position & position,
                                  const sf::Color & color) const {
  sf::RectangleShape rect(sf::Vector2f(sf::Vector2u(square_size_, square_size_)));
  rect.setPosition(GetCoordsFromPosition(position));
  rect.setFillColor(color);
  texture.draw(rect);
}


void GameScene::DrawPossibleMoves(sf::RenderTexture & texture) {
  std::lock_guard<std::mutex> guard_moves(possible_moves_mutex_);

  std::vector<std::vector<bool>> visited(8, std::vector<bool>(8, false));

  std::lock_guard<std::mutex> guard_piece(chosen_piece_mutex_);
  if (chosen_piece_ != nullptr)
    DrawSquareInColor(texture, chosen_piece_->GetPosition(), current_square_color_);
  else {
    std::vector<Move> & current_moves = (possible_moves_.empty() ? move_pool_ : possible_moves_);
    for (const Move & move : current_moves) {
      if (!visited[move.GetStartPosition().GetX()][move.GetStartPosition().GetY()]) {
        visited[move.GetStartPosition().GetX()][move.GetStartPosition().GetY()] = true;
        DrawSquareInColor(texture, move.GetStartPosition(), current_square_color_);
      }
    }
    return;
  }

  for (const Move & move : possible_moves_) {
    Position next_position = move.GetIntermediatePositions().empty() ?
        move.GetEndPosition() : move.GetIntermediatePositions()[0];
    if (visited[next_position.GetX()][next_position.GetY()])
      continue;
    visited[next_position.GetX()][next_position.GetY()] = true;
    DrawSquareInColor(texture, next_position, possible_move_square_color_);
  }
}


void GameScene::Animate(sf::RenderTexture & texture) {
  std::lock_guard<std::mutex> guard(animation_mutex_);
  if (animation_state_ != AnimationState::kGoing)
    return;
  auto ms_passed = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - animation_start_time_
      ).count());
  if (ms_passed > animated_move_time_) {
    ms_passed = animated_move_time_;
  }
  sf::Vector2f from = GetCoordsFromPosition(animated_currently_moving_.GetStartPosition());
  sf::Vector2f to = GetCoordsFromPosition(animated_currently_moving_.GetEndPosition());
  float t = (ms_passed / animated_move_time_);
  sf::Vector2f lerp = from + (to - from) * t;
  DrawPiece(texture, animated_piece_, lerp);

  if (FloatsEqual(ms_passed, animated_move_time_)) {
    if (animated_move_ == animated_currently_moving_) {
      animation_state_ = AnimationState::kFinished;
      return;
    }
    animated_currently_moving_ = animated_move_.GetIntermediatePositions().empty() ?
        animated_move_ : animated_move_.RemoveFirstStep();
    animated_move_time_ = GetMoveTime(animated_currently_moving_);
    animation_start_time_ = std::chrono::steady_clock::now();
  }
}


void GameScene::DrawInfo(sf::RenderTexture & texture) {
  sf::Text text;
  text.setFont(font_);
  std::string str;
  if (game_.IsRunning()) {
    switch (game_.GetCurrentlyMoving()) {
      case PieceColor::kWhite:
        str = "White's move";
        break;
      case PieceColor::kBlack:
        str = "Black's move";
        break;
    }
  }
  text.setString(str);
  text.setCharacterSize(50);
  text.setFillColor(sf::Color::Black);
  texture.draw(text);
}