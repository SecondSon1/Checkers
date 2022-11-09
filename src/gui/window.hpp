#pragma once

#include "scene.hpp"

class Window : public std::enable_shared_from_this<Window> {
 public:
  Window(size_t width_, size_t height_,
         std::string title, sf::Color background_color, bool resizable,
         std::unique_ptr<Scene> scene_ptr);

  void Run();

 public:
  size_t GetWidth() const { return width_; }
  size_t GetHeight() const { return height_; }
  sf::Color GetBackgroundColor() const { return background_color_; }
  bool IsRunning() const { return running_; }
  const sf::Texture & GetBuffer() const noexcept { return front_buffer_->getTexture(); }

 private:
  void DrawingThreadFunction(sf::RenderWindow & window);
  void LogicThreadFunction(sf::RenderWindow & window);

 private:
  size_t width_, height_;
  sf::Color background_color_;
  sf::RenderWindow window_;
  std::atomic<bool> running_;

  std::mutex scene_mutex_;
  std::unique_ptr<Scene> scene_ptr_;

  std::unique_ptr<sf::RenderTexture> front_buffer_;
  std::unique_ptr<sf::RenderTexture> back_buffer_;
};