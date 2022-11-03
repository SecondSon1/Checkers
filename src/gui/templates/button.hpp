#pragma once

#include <SFML/Graphics.hpp>

class Button {
 public:
  Button(size_t x, size_t y, size_t width, size_t height)
         : x_(x), y_(y), width_(width), height_(height), waiting_for_release_(false) {
    if (!button_texture_.create(width_, height_))
      throw std::runtime_error("Unable to create button button_texture_");
  }
  virtual ~Button() = default;

  [[nodiscard]] size_t GetX() const { return x_; }
  [[nodiscard]] size_t GetY() const { return y_; }
  [[nodiscard]] size_t GetWidth() const { return width_; }
  [[nodiscard]] size_t GetHeight() const { return height_; }

  void Draw(sf::RenderTexture & texture) noexcept;
  bool IsInside(sf::Vector2f mouse_pos_relative);
  void HandleEvent(const sf::Event & evt, sf::Vector2f mouse_pos_relative);

 protected:
  virtual void Callback() noexcept = 0;
  virtual void DrawButton(sf::Color & button_color, sf::Color & outline_color, sf::Text & text) noexcept = 0;

 private:
  size_t x_, y_;
  size_t width_, height_;

  bool waiting_for_release_;
  sf::RenderTexture button_texture_;
};