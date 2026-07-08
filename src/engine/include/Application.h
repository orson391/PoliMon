#pragma once

#include <filesystem>
#include <functional>
#include <string>

#include "Config.h"
#include "xml_reader.h"

struct SDL_Window;
struct SDL_Renderer;

namespace engine {

class Application {
 public:
  using FrameCallback = std::function<void(float dtSeconds)>;

  Application(const std::string& title = "SDL3 Modular App", int width = 800, int height = 600);
  virtual ~Application();

  bool initialize();
  void run();

  SDL_Renderer* renderer() const { return renderer_; }
  const core::Config& config() const { return config_; }
  bool reloadConfigFromFile(const std::string& path = {});

 protected:
  virtual void onStart();
  virtual void onClear();
  virtual void onInput();
  virtual void onUpdate(float dtSeconds);
  virtual void onRender(SDL_Renderer* renderer);
  virtual void onClose();
  virtual void onConfigReload(const core::Config& newConfig);

 private:
  core::Config config_;
  bool running_{false};
  SDL_Window* window_{nullptr};
  SDL_Renderer* renderer_{nullptr};
  std::filesystem::file_time_type lastConfigWrite_{};
  std::string configPath_;
  bool configLoadedAtLeastOnce_{false};
};

}  // namespace engine
