#include "Application.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <filesystem>
#include <string>

#include "Logger.h"

namespace engine {

namespace {

std::string resolveConfigPath() {
  return "C:\\Projects\\VsCode\\PoliMon\\config\\settings.xml";
}

bool loadConfigFromXml(const std::string& path, core::Config& outConfig) {
  XmlReaderConfig xmlConfig{};
  if (!xml_reader_load_from_file(path.c_str(), &xmlConfig)) {
    return false;
  }

  outConfig.title = xmlConfig.window.title;
  outConfig.width = xmlConfig.window.width;
  outConfig.height = xmlConfig.window.height;
  outConfig.fullscreen = xmlConfig.window.fullscreen != 0;
  outConfig.target_fps = xmlConfig.graphics.target_fps;
  outConfig.vsync = xmlConfig.graphics.vsync != 0;
  return true;
}

}  // namespace

Application::Application(const std::string& title, int width, int height)
    : config_{}, configPath_{resolveConfigPath()} {
  config_.title = title;
  config_.width = width;
  config_.height = height;
}

Application::~Application() {
  if (renderer_) {
    SDL_DestroyRenderer(renderer_);
  }
  if (window_) {
    SDL_DestroyWindow(window_);
  }
  SDL_Quit();
}

bool Application::initialize() {
  const std::string configPath = configPath_.empty() ? resolveConfigPath() : configPath_;
  if (!configPath.empty()) {
    if (!reloadConfigFromFile(configPath)) {
      core::Logger::log("Failed to load config, using defaults");
    }
  }

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    core::Logger::log("SDL_Init failed: " + std::string(SDL_GetError()));
    return false;
  }

  Uint32 windowFlags = SDL_WINDOW_RESIZABLE;
  if (config_.fullscreen) {
    windowFlags |= SDL_WINDOW_FULLSCREEN;
  }

  window_ = SDL_CreateWindow(config_.title.c_str(), config_.width, config_.height, windowFlags);
  if (!window_) {
    core::Logger::log("Window creation failed: " + std::string(SDL_GetError()));
    SDL_Quit();
    return false;
  }

  renderer_ = SDL_CreateRenderer(window_, nullptr);
  if (!renderer_) {
    core::Logger::log("Renderer creation failed: " + std::string(SDL_GetError()));
    SDL_DestroyWindow(window_);
    window_ = nullptr;
    SDL_Quit();
    return false;
  }

  running_ = true;
  core::Logger::log("Application initialized with title='" + config_.title +
                    "' size=" + std::to_string(config_.width) + "x" +
                    std::to_string(config_.height) + " fps=" + std::to_string(config_.target_fps));
  return true;
}

bool Application::reloadConfigFromFile(const std::string& path) {
  const std::string configPath = path.empty() ? configPath_ : path;
  if (configPath.empty()) {
    return false;
  }

  std::error_code ec;
  const auto currentWrite = std::filesystem::exists(configPath)
                                ? std::filesystem::last_write_time(configPath, ec)
                                : std::filesystem::file_time_type{};
  if (ec) {
    core::Logger::log("Failed to inspect config file timestamp: " + ec.message());
    return false;
  }

  if (configLoadedAtLeastOnce_ && lastConfigWrite_ == currentWrite) {
    return true;
  }

  core::Config loaded;
  if (!loadConfigFromXml(configPath, loaded)) {
    return false;
  }

  config_ = loaded;
  lastConfigWrite_ = currentWrite;
  configLoadedAtLeastOnce_ = true;

  if (window_) {
    SDL_SetWindowTitle(window_, config_.title.c_str());
    SDL_SetWindowSize(window_, config_.width, config_.height);
    SDL_SetWindowFullscreen(window_, config_.fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
  }

  onConfigReload(config_);
  return true;
}

void Application::run() {
  onStart();

  while (running_) {
    static Uint64 lastTime = SDL_GetPerformanceCounter();
    const Uint64 currentTime = SDL_GetPerformanceCounter();
    const float dtSeconds =
        static_cast<float>(currentTime - lastTime) / SDL_GetPerformanceFrequency();
    lastTime = currentTime;

    onInput();
    onUpdate(dtSeconds);

    SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
    SDL_RenderClear(renderer_);

    onRender(renderer_);
    SDL_RenderPresent(renderer_);
  }
}

void Application::onStart() {}

void Application::onClear() {}

void Application::onInput() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      running_ = false;
    }
  }
}

void Application::onUpdate(float dtSeconds) {
  (void)dtSeconds;
  reloadConfigFromFile();
}

void Application::onRender(SDL_Renderer* renderer) {
  // Implementation for rendering
}

void Application::onClose() {}

void Application::onConfigReload(const core::Config& newConfig) {
  (void)newConfig;
}

}  // namespace engine
