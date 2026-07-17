#include "ResourcePaths.h"

#include <SDL3/SDL.h>

#include <initializer_list>

namespace core {

namespace {

std::filesystem::path basePathFromSDL() {
  if (const char* base = SDL_GetBasePath()) return std::filesystem::path(base);

  return std::filesystem::current_path();
}

std::filesystem::path firstExistingChild(const std::filesystem::path& root,
                                         std::initializer_list<const char*> children) {
  for (const char* child : children) {
    const auto candidate = root / child;
    if (std::filesystem::exists(candidate)) {
      return candidate;
    }
  }
  return root / *children.begin();
}

std::filesystem::path findLayoutRoot(const std::filesystem::path& start) {
  auto current = start;
  for (int depth = 0; depth < 8; ++depth) {
    if (std::filesystem::exists(current / "asset") ||
        std::filesystem::exists(current / "assets") ||
        std::filesystem::exists(current / "config")) {
      return current;
    }

    const auto parent = current.parent_path();
    if (parent.empty() || parent == current) {
      break;
    }
    current = parent;
  }
  return start;
}

}  // namespace

std::filesystem::path ResourcePaths::executableDir() {
  return basePathFromSDL();
}

std::filesystem::path ResourcePaths::assetRoot() {
  const auto root = findLayoutRoot(executableDir());
  return firstExistingChild(root, {"asset", "assets"});
}

std::filesystem::path ResourcePaths::texture(const std::string& file) {
  return assetRoot() / file;
}

std::filesystem::path ResourcePaths::map(const std::string& file) {
  return assetRoot() / file;
}

std::filesystem::path ResourcePaths::config(const std::string& file) {
  const auto root = findLayoutRoot(executableDir());
  const auto configDir = root / "config";
  if (std::filesystem::exists(configDir)) {
    return configDir / file;
  }
  return root / file;
}

}  // namespace core
