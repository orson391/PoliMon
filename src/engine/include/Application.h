#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>  // used by rectangles_ below; was previously an implicit/transitive
                          // include, which is fragile (breaks if include order changes).

#include "Config.h"

struct SDL_Window;
struct SDL_Renderer;

namespace engine {

class Application {
 public:
  using FrameCallback = std::function<void(float dtSeconds)>;

  Application(const std::string& title = "PoliMon", int width = 800, int height = 600);
  virtual ~Application();

  // Application owns raw SDL_Window*/SDL_Renderer* handles with no reference
  // counting. Copying would produce two objects that both believe they own
  // (and will destroy) the same window/renderer, causing a double free and
  // use-after-free. Moving is disabled too since nothing in the codebase
  // needs to relocate an Application and disabling it avoids having to
  // reason about a "moved-from" running app. Delete both explicitly instead
  // of leaving the implicitly-generated (and unsafe) versions available.
  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;
  Application(Application&&) = delete;
  Application& operator=(Application&&) = delete;

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

 protected:
  std::unordered_map<int, core::RectElement> rectangles_;
  core::Config config_;
  float width_{0.0f};
  float height_{0.0f};

 private:
  bool running_{false};
  bool initialized_{false};
  SDL_Window* window_{nullptr};
  SDL_Renderer* renderer_{nullptr};
  std::filesystem::file_time_type lastConfigWrite_{};
  std::string configPath_;
  bool configLoadedAtLeastOnce_{false};

  // Throttles filesystem stat() calls made while polling for config file
  // changes; see reloadConfigFromFile() usage in onUpdate().
  float configCheckAccumulatorSeconds_{0.0f};
  static constexpr float kConfigCheckIntervalSeconds = 1.0f;
};

}  // namespace engine
