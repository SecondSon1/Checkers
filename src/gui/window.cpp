#include "window.hpp"

#include <thread>
#include <SFML/Window/Event.hpp>
#include "scene.hpp"

Window::Window(size_t width, size_t height,
               std::string title, sf::Color background_color, bool resizable,
               std::unique_ptr<Scene> scene_ptr)
               : width_(width), height_(height), background_color_(background_color),
                 scene_ptr_(std::move(scene_ptr)), running_(false),
                 window_(sf::VideoMode(width_, height_), title,
                         sf::Style::Titlebar | sf::Style::Close | (resizable ? sf::Style::Resize : 0)),
                 front_buffer_(), back_buffer_() {
  if (width_ == 0 || height_ == 0)
    throw std::invalid_argument("Width and height must be non-zero");
  if (scene_ptr_ == nullptr)
    throw std::invalid_argument("Scene must be provided to the window");
  window_.setVerticalSyncEnabled(true);
  front_buffer_ = std::make_unique<sf::RenderTexture>();
  back_buffer_ = std::make_unique<sf::RenderTexture>();
  if (!front_buffer_->create(width, height) || !back_buffer_->create(width, height))
    throw std::runtime_error("Unable to create window buffers");
}

void Window::Run() {
  scene_ptr_->SetWindowPtr(std::move(shared_from_this()));
  scene_ptr_->Init();

  window_.setActive(false);
  running_ = true;
  std::thread drawing_thread([this]() -> void {
    DrawingThreadFunction(window_);
  });
  std::thread logic_thread([this]() -> void {
    LogicThreadFunction(window_);
  });

  MainThreadFunction();

  drawing_thread.join();
  logic_thread.join();
  window_.close();
}

void Window::DrawingThreadFunction(sf::RenderWindow & window) {
  window.setActive(true);

  while (running_) {
    window.clear();

    back_buffer_->clear(background_color_);
    {
      std::lock_guard<std::mutex> guard(scene_mutex_);
      scene_ptr_->Draw(window, *back_buffer_);
    }
    back_buffer_->display();
    front_buffer_.swap(back_buffer_);

    sf::Sprite sprite(front_buffer_->getTexture());
    window.draw(sprite);

    window.display();
  }
}

void Window::LogicThreadFunction(sf::RenderWindow &window) {

  while (running_) {

    sf::Event evt{};
    while (!event_deque_.Empty()) {
      evt = event_deque_.PopFront();
      scene_ptr_->HandleEvent(window, evt);
    }

    scene_ptr_->HandleLogic(window);

  }


}


void Window::MainThreadFunction() {

  while (running_) {

    {
      std::lock_guard<std::mutex> guard(scene_mutex_);
      if (scene_ptr_->NewSceneAwaits()) {
        std::unique_ptr<Scene> new_scene = scene_ptr_->RetrieveNewScenePtr();
        new_scene->SetWindowPtr(shared_from_this());
        new_scene->Init();
        scene_ptr_.swap(new_scene);
      }
    }

    sf::Event evt{};
    while (window_.pollEvent(evt)) {
      if (evt.type == sf::Event::Closed) {
        running_ = false;
        break;
      }

      event_deque_.PushBack(evt);
    }

    if (scene_ptr_->ShouldClose()) {
      running_ = false;
    }

  }

}