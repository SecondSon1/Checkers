#include "config.hpp"
#include "../window.hpp"
#include "game.hpp"
#include "../../player/bot/random_bot/random_bot.hpp"
#include "../../player/bot/calculating_bot/calculating_bot.hpp"
#include "../../player/bot/human_bot/human_bot.hpp"

void ConfigScene::Init() {
  width_ = GetWindowPtr()->GetWidth() - 30;
  info_height_ = 250;
  choice_height_ = GetWindowPtr()->GetHeight() - 45 - info_height_;
  info_position_ = sf::Vector2f(15.f, 15.f);
  choice_position_ = sf::Vector2f(15.f, static_cast<float>(30 + info_height_));

  if (!info_texture_.create(width_, info_height_) ||
      !choice_texture_.create(width_, choice_height_))
    throw std::runtime_error("Unable to create info and choice textures in Config scene");

  button_width_ = width_ / 5 * 2;
  button_height_ = 75;

  std::string labels[] = {
      "Human Player",
      "Random Bot",
      "Calculating Bot",
      "\"Human\" Bot"
  };
  white_choice_buttons_.reserve(4);
  black_choice_buttons_.reserve(4);
  size_t next_y = 0;

  white_chosen_ = std::make_shared<std::shared_ptr<ChoiceButton>>(nullptr);
  black_chosen_ = std::make_shared<std::shared_ptr<ChoiceButton>>(nullptr);

  for (const std::string & label : labels) {
    white_choice_buttons_.push_back(std::make_shared<ChoiceButton>(
          0, next_y, button_width_, button_height_,
          label, font_, white_chosen_
        ));
    black_choice_buttons_.push_back(std::make_shared<ChoiceButton>(
          width_ - button_width_, next_y, button_width_, button_height_,
          label, font_, black_chosen_
        ));
    next_y += button_height_ + 15;
  }

  size_t proceed_button_width = width_ / 5 * 3;
  proceed_button_ = std::make_shared<ProceedButton>(
        (width_ - proceed_button_width) / 2, next_y + 15, proceed_button_width, 90, font_
      );

  info_texture_.clear(sf::Color(255, 255, 255, 0));
  DrawInfo(info_texture_);
  info_texture_.display();
}


void ConfigScene::Draw(const sf::RenderWindow & window, sf::RenderTexture & texture) {
  choice_texture_.clear(sf::Color(255, 255, 255, 0));
  DrawChoices(choice_texture_);
  choice_texture_.display();

  sf::Sprite info_sprite(info_texture_.getTexture()),
             choice_sprite(choice_texture_.getTexture());
  info_sprite.setPosition(info_position_);
  choice_sprite.setPosition(choice_position_);
  texture.draw(info_sprite);
  texture.draw(choice_sprite);
}


void ConfigScene::HandleEvent(sf::RenderWindow & window, sf::Event &evt) {
  sf::Vector2f mouse_pos = sf::Vector2f(sf::Mouse::getPosition(window)) - choice_position_;
  for (const std::shared_ptr<ChoiceButton> & choice_button_ptr : white_choice_buttons_)
    choice_button_ptr->HandleEvent(evt, mouse_pos);
  for (const std::shared_ptr<ChoiceButton> & choice_button_ptr : black_choice_buttons_)
    choice_button_ptr->HandleEvent(evt, mouse_pos);
  proceed_button_->HandleEvent(evt, mouse_pos);
}


void ConfigScene::HandleLogic(sf::RenderWindow & window) {
  if (proceed_button_->WasPressed() && white_chosen_ != nullptr && black_chosen_ != nullptr) {
    std::shared_ptr<Player> white_player, black_player;
    size_t chosen_white = 0, chosen_black = 0;
    for (; chosen_white < 4; ++chosen_white)
      if (*white_chosen_ == white_choice_buttons_[chosen_white])
        break;
    for (; chosen_black < 4; ++chosen_black)
      if (*black_chosen_ == black_choice_buttons_[chosen_black])
        break;

    switch (chosen_white) {
      case 0:
        white_player = std::make_shared<Human>(PieceColor::kWhite);
        break;
      case 1:
        white_player = std::make_shared<RandomBot>(PieceColor::kWhite);
        break;
      case 2:
        white_player = std::make_shared<CalculatingBot>(PieceColor::kWhite);
        break;
      case 3:
        white_player = std::make_shared<HumanBot>(PieceColor::kWhite);
        break;
      default:
        throw std::runtime_error("somehow chosen button not found lol");
    }

    switch (chosen_black) {
      case 0:
        black_player = std::make_shared<Human>(PieceColor::kBlack);
        break;
      case 1:
        black_player = std::make_shared<RandomBot>(PieceColor::kBlack);
        break;
      case 2:
        black_player = std::make_shared<CalculatingBot>(PieceColor::kBlack);
        break;
      case 3:
        black_player = std::make_shared<HumanBot>(PieceColor::kBlack);
        break;
      default:
        throw std::runtime_error("somehow chosen button not found lol");
    }

    SwitchScene(std::make_unique<GameScene>(
          std::move(white_player), std::move(black_player), font_
        ));
    white_chosen_ = nullptr;
    black_chosen_ = nullptr;
  }
}


void ConfigScene::DrawInfo(sf::RenderTexture & texture) {

}


void ConfigScene::DrawChoices(sf::RenderTexture & texture) {
  for (const std::shared_ptr<ChoiceButton> & choice_button_ptr : white_choice_buttons_)
    choice_button_ptr->Draw(texture);
  for (const std::shared_ptr<ChoiceButton> & choice_button_ptr : black_choice_buttons_)
    choice_button_ptr->Draw(texture);
  proceed_button_->Draw(texture);
}