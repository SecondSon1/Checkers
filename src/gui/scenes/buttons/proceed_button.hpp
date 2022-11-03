#pragma once

#include "../../templates/button.hpp"

class ConfigScene;

class ProceedButton : public Button {
 public:
  ProceedButton(size_t x, size_t y, size_t width, size_t height, sf::Font & font)
      : Button(x, y, width, height), font_(font), pressed_(false),
        normal_button_color_(0xEA, 0xE5, 0xD5, 0xFF) {}

 public:
  bool WasPressed() const noexcept {
    return pressed_;
  }

 protected:
  void DrawButton(sf::Color & button_color, sf::Color & outline_color, sf::Text & text) noexcept override {
    button_color = normal_button_color_;
    outline_color = sf::Color::Black;
    text.setString("Proceed");
    text.setFont(font_);
    text.setCharacterSize(50);
    text.setFillColor(sf::Color::Black);
  }


  void Callback() noexcept override {
    pressed_ = true;
  }

 private:
  sf::Font font_;
  sf::Color normal_button_color_;

  bool pressed_;
};