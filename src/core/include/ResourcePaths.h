#pragma once

#include <filesystem>
#include <string>

namespace core {

class ResourcePaths {
 public:
  static std::filesystem::path executableDir();
  static std::filesystem::path assetRoot();

  static std::filesystem::path texture(const std::string& file);
  static std::filesystem::path map(const std::string& file);
  static std::filesystem::path config(const std::string& file);
};

}  // namespace core
