#include "Application.h"

#include <SDL3/SDL.h>
#include <glad/glad.h>

#include <filesystem>
#include <memory>
#include <string>

#include "Logger.h"
#include "ResourcePaths.h"
#include "gl_context/renderer.h"

namespace engine {

namespace {

std::string resolveConfigPath() {
  return core::ResourcePaths::config("settings.xml").string();
}

}  // namespace

Application::Application(const std::string& title, int width, int height)
    : config_{}, configPath_{resolveConfigPath()}, rectangles_(128) {
  config_.title = title;
  config_.width = width;
  config_.height = height;
}

Application::~Application() {
  // Destroy renderer-owned GL objects while the context is still current.
  renderer_.reset();
  if (glContext_) SDL_GL_DestroyContext(glContext_);
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

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  Uint32 windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
  if (config_.fullscreen) {
    windowFlags |= SDL_WINDOW_FULLSCREEN;
  }

  window_ = SDL_CreateWindow(config_.title.c_str(), config_.width, config_.height, windowFlags);
  if (!window_) {
    core::Logger::log("Window creation failed: " + std::string(SDL_GetError()));
    SDL_Quit();
    return false;
  }

  width_ = config_.width;
  height_ = config_.height;

  glContext_ = SDL_GL_CreateContext(window_);
  if (!glContext_ || !SDL_GL_MakeCurrent(window_, glContext_)) {
    core::Logger::log("OpenGL context creation failed: " + std::string(SDL_GetError()));
    SDL_DestroyWindow(window_);
    window_ = nullptr;
    SDL_Quit();
    return false;
  }

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
    core::Logger::log("Failed to load OpenGL functions.");
    SDL_GL_DestroyContext(glContext_);
    glContext_ = nullptr;
    SDL_DestroyWindow(window_);
    window_ = nullptr;
    SDL_Quit();
    return false;
  }

  auto glRenderer = std::make_unique<graphics::gl_context::Renderer>();
  if (!glRenderer->initialize(config_.width, config_.height)) {
    core::Logger::log("OpenGL renderer initialization failed.");
    SDL_GL_DestroyContext(glContext_);
    glContext_ = nullptr;
    SDL_DestroyWindow(window_);
    window_ = nullptr;
    SDL_Quit();
    return false;
  }
  renderer_ = std::move(glRenderer);
  SDL_GL_SetSwapInterval(1);

  for (const auto& rect : config_.rectangles) {
    rectangles_[rect.id] = rect;
  }

  running_ = true;
  initialized_ = true;
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
  if (!core::Config::loadFromFile(configPath, loaded)) {
    return false;
  }

  config_ = loaded;
  lastConfigWrite_ = currentWrite;
  configLoadedAtLeastOnce_ = true;

  if (window_) {
    SDL_SetWindowTitle(window_, config_.title.c_str());
    SDL_SetWindowSize(window_, config_.width, config_.height);
    SDL_SetWindowFullscreen(window_, config_.fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

    width_ = config_.width;
    height_ = config_.height;
  }

  // Rebuild rectangles_ from scratch on every successful reload. Previously
  // this only overwrote/inserted entries for ids present in the new config,
  // so any rectangle removed from settings.xml between reloads stayed
  // resident in the map forever (a slow, config-driven leak of small POD
  // entries, and a source of stale rectangles being rendered).
  rectangles_.clear();
  for (const auto& rect : config_.rectangles) {
    rectangles_[rect.id] = rect;
  }

  onConfigReload(config_);
  return true;
}

void Application::run() {
  if (!initialized_) {
    return;
  }

  onStart();

  while (running_) {
    static Uint64 lastTime = SDL_GetPerformanceCounter();
    const Uint64 currentTime = SDL_GetPerformanceCounter();
    const float dtSeconds =
        static_cast<float>(currentTime - lastTime) / SDL_GetPerformanceFrequency();
    lastTime = currentTime;

    onInput();
    onUpdate(dtSeconds);

    renderer_->setViewport(static_cast<int>(width_), static_cast<int>(height_));
    renderer_->beginFrame({20.0f / 255.0f, 20.0f / 255.0f, 20.0f / 255.0f, 1.0f});
    onRender(*renderer_);
    renderer_->endFrame();
    SDL_GL_SwapWindow(window_);
  }

  // onClose() was previously declared/overridden (GameApp logs "Game
  // closed") but never actually invoked anywhere, so subclass shutdown
  // hooks never ran. This is the natural place for it: once the loop exits
  // but before the Application (and its window/renderer) start tearing
  // down in ~Application().
  onClose();
}

void Application::onStart() {}

void Application::onClear() {}

void Application::onInput() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_EVENT_QUIT:
        running_ = false;
        break;

      case SDL_EVENT_WINDOW_RESIZED: {
        int newWidth = event.window.data1;
        int newHeight = event.window.data2;
        width_ = newWidth;
        height_ = newHeight;
        break;
      }

      default:
        break;
    }
  }
}

void Application::onUpdate(float dtSeconds) {
  // Polling the filesystem (exists() + last_write_time()) on every single
  // frame is an unnecessary syscall per frame (hundreds/thousands per
  // second). It isn't a memory leak by itself, but it fights the "zero
  // unnecessary work per frame" goal and adds avoidable frame-time jitter,
  // so throttle it to a fixed wall-clock interval instead.
  configCheckAccumulatorSeconds_ += dtSeconds;
  if (configCheckAccumulatorSeconds_ >= kConfigCheckIntervalSeconds) {
    configCheckAccumulatorSeconds_ = 0.0f;
    reloadConfigFromFile();
  }
}

void Application::onRender(graphics::IRenderer& renderer) {
  // Implementation for rendering
  (void)renderer;
}

void Application::onClose() {}

void Application::onConfigReload(const core::Config& newConfig) {
  (void)newConfig;
}

}  // namespace engine
