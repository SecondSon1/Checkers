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

  white_square_color = sf::Color(238, 238, 215, 255);
  black_square_color = sf::Color(129, 147, 99, 255);

  white_piece_color = sf::Color(248, 248, 248, 255);
  black_piece_color = sf::Color(85, 83, 82, 255);

  current_square_color = sf::Color(247, 82, 82, 100);
  possible_move_square_color = sf::Color(201, 22, 22, 100);

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


void GameScene::HandleEvent(sf::RenderWindow & window, sf::Event & evt) {
  if (evt.type == sf::Event::KeyPressed) {
    if (evt.key.code == sf::Keyboard::Space) {
      if ((game_.GetCurrentlyMoving() == PieceColor::kWhite && white_human_player_ == nullptr) ||
          (game_.GetCurrentlyMoving() == PieceColor::kBlack && black_human_player_ == nullptr))
        move_finished_ = true;
      return;
    } else if (evt.key.code == sf::Keyboard::Backspace) {
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
      bool promotion = false;
      for (const Move & move : possible_moves_) {
        if (move.GetIntermediatePositions().empty() && pos == move.GetEndPosition()) {
          std::shared_ptr<Human> & human_player_ptr = game_.GetCurrentlyMoving() == PieceColor::kWhite ?
                                                      white_human_player_ : black_human_player_;
          human_player_ptr->SetNextMove(move);
          move_finished_ = true;
          possible_moves_.clear();
          chosen_piece_ = nullptr;
          return;
        }
        if (!move.GetIntermediatePositions().empty() && pos == move.GetIntermediatePositions()[0]) {
          possible_move_prefixes.push_back(move);
          if (move.DoesPromotionHappen() && move.GetPromotionPosition() == pos)
            promotion = true;
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
        move.RemoveFirstStep();
        possible_moves_.push_back(move);
      }

      std::shared_ptr<Human> & human_player_ptr = game_.GetCurrentlyMoving() == PieceColor::kWhite ?
                                                  white_human_player_ : black_human_player_;
      Move next_move(chosen_piece_->GetPosition(), pos, { taken });
      if (promotion)
        next_move.SetPromotionPosition(pos);
      human_player_ptr->SetNextMove(next_move);
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
  if (move_finished_) {
    move_finished_ = false;
    sequential_move_in_progress_ = false;
    game_.Proceed();

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


void GameScene::DrawBoard(sf::Texture & texture) {
  auto *pixels = new sf::Uint8[4 * board_size_ * board_size_];
  for (size_t x = 0; x < board_size_; x += square_size_) {
    for (size_t y = 0; y < board_size_; y += square_size_) {
      sf::Color color = ((7 - x / square_size_) + y / square_size_) % 2 == 0 ? black_square_color : white_square_color;
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
  shape.setFillColor(piece->GetColor() == PieceColor::kWhite ? white_piece_color : black_piece_color);
  shape.setOutlineColor(piece->GetColor() == PieceColor::kWhite ? black_piece_color : white_piece_color);
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
  std::lock_guard<std::mutex> guard_piece(chosen_piece_mutex_);

  for (size_t index = 0; index < 32; ++index) {
    Position pos(index);
    sf::Vector2f top_left_corner(static_cast<float>(pos.GetY() * square_size_),
                                 static_cast<float>((7 - pos.GetX()) * square_size_));
    if (game_.GetBoard()[pos].IsEmpty() ||
        (chosen_piece_ != nullptr && chosen_piece_->GetPosition() == pos))
      continue;
    std::shared_ptr<Piece> piece = game_.GetBoard()[pos].GetPiece();
    DrawPiece(texture, piece, top_left_corner);
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
  sf::Vector2f top_left_corner(
        static_cast<float>(position.GetY() * square_size_),
        static_cast<float>((7 - position.GetX()) * square_size_)
      );
  sf::RectangleShape rect(sf::Vector2f(sf::Vector2u(square_size_, square_size_)));
  rect.setPosition(top_left_corner);
  rect.setFillColor(color);
  texture.draw(rect);
}


void GameScene::DrawPossibleMoves(sf::RenderTexture & texture) {
  std::lock_guard<std::mutex> guard_moves(possible_moves_mutex_);

  std::vector<std::vector<bool>> visited(8, std::vector<bool>(8, false));

  {
    std::lock_guard<std::mutex> guard_piece(chosen_piece_mutex_);
    if (chosen_piece_ != nullptr)
      DrawSquareInColor(texture, chosen_piece_->GetPosition(), current_square_color);
    else {
      std::vector<Move> & current_moves = (possible_moves_.empty() ? move_pool_ : possible_moves_);
      for (const Move & move : current_moves) {
        if (!visited[move.GetStartPosition().GetX()][move.GetStartPosition().GetY()]) {
          visited[move.GetStartPosition().GetX()][move.GetStartPosition().GetY()] = true;
          DrawSquareInColor(texture, move.GetStartPosition(), current_square_color);
        }
      }
      return;
    }
  }

  for (const Move & move : possible_moves_) {
    Position next_position = move.GetIntermediatePositions().empty() ?
        move.GetEndPosition() : move.GetIntermediatePositions()[0];
    if (visited[next_position.GetX()][next_position.GetY()])
      continue;
    visited[next_position.GetX()][next_position.GetY()] = true;
    DrawSquareInColor(texture, next_position, possible_move_square_color);
  }
}


void GameScene::DrawInfo(sf::RenderTexture & texture) {
  sf::Text text;
  text.setFont(font_);
  std::string str;
  switch (game_.GetState()) {
    case GameState::kRunning:
      switch (game_.GetCurrentlyMoving()) {
        case PieceColor::kWhite:
          str = "White's move";
          break;
        case PieceColor::kBlack:
          str = "Black's move";
          break;
      }
      break;
    case GameState::kDraw:
      str = "Draw! (Lol how)";
      break;
    case GameState::kWhiteWon:
      str = "White won!";
      break;
    case GameState::kBlackWon:
      str = "Black won!";
      break;
  }
  text.setString(str);
  text.setCharacterSize(50);
  text.setFillColor(sf::Color::Black);
  texture.draw(text);
}