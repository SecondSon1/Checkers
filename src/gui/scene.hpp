#pragma once

#include <SFML/Graphics.hpp>

class Window;

class Scene {
 public:
  Scene() : new_scene_(nullptr), should_close_(false) {}
  virtual ~Scene() = default;

  // TODO: make private
  virtual void Init() = 0;
  virtual void Draw(const sf::RenderWindow & window, sf::RenderTexture & texture) = 0;
  virtual void HandleEvent(sf::RenderWindow & window, sf::Event & evt) = 0;
  virtual void HandleLogic(sf::RenderWindow & window) = 0;

 protected:
  void SwitchScene(std::unique_ptr<Scene> ptr) {
    new_scene_ = std::move(ptr);
  }

  void Exit() {
    should_close_ = true;
  }

  [[nodiscard]] std::shared_ptr<Window> GetWindowPtr() const { return window_ptr_; }

 private:
  friend class Window;

  void SetWindowPtr(std::shared_ptr<Window> ptr) {
    window_ptr_ = std::move(ptr);
  }

  [[nodiscard]] bool NewSceneAwaits() const {
    return new_scene_ != nullptr;
  }

  [[nodiscard]] bool ShouldClose() const {
    return should_close_;
  }

  [[nodiscard]] std::unique_ptr<Scene> RetrieveNewScenePtr() {
    return std::move(new_scene_);
  }

 private:
  std::shared_ptr<Window> window_ptr_;
  std::unique_ptr<Scene> new_scene_;
  bool should_close_;
};