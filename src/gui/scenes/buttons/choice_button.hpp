#pragma once

#include "../../templates/button.hpp"

class ChoiceButton : public Button, public std::enable_shared_from_this<ChoiceButton> {
 public:
  ChoiceButton(size_t x, size_t y, size_t width, size_t height, std::string label, sf::Font & font,
               std::shared_ptr<std::shared_ptr<ChoiceButton>> chosen_button_ptr)
      : Button(x, y, width, height), font_(font),
        normal_button_color_(0xEA, 0xE5, 0xD5, 0xFF),
        chosen_button_color_(0xC7, 0xB9, 0x8E, 0xFF),
        button_outline_color_(sf::Color::Black),
        chosen_button_ptr_(std::move(chosen_button_ptr)), label_(std::move(label)) {}

 protected:
  void DrawButton(sf::Color & button_color, sf::Color & outline_color, sf::Text & text) noexcept override {
    bool chosen = chosen_button_ptr_->get() == this;
    button_color = chosen ? chosen_button_color_ : normal_button_color_;
    outline_color = sf::Color::Black;
    text.setString(label_);
    text.setFont(font_);
    text.setCharacterSize(40);
    text.setFillColor(sf::Color::Black);
  }

  void Callback() noexcept override {
    *chosen_button_ptr_ = shared_from_this();
  }

 private:
  std::shared_ptr<std::shared_ptr<ChoiceButton>> chosen_button_ptr_;
  std::string label_;
  sf::Font font_;

  sf::Color normal_button_color_;
  sf::Color chosen_button_color_;
  sf::Color button_outline_color_;
};