#include <cassert>
#include <filesystem>
#include <fstream>

#include "Config.h"
#include "xml_reader.h"

int main() {
  const auto path = std::filesystem::temp_directory_path() / "sdl3_config_test.xml";
  {
    std::ofstream file(path);
    file << R"(<Settings>
  <Window title="Test Config" width="1024" height="768" fullscreen="true" />
  <Graphics target_fps="144" vsync="false" />
</Settings>)";
  }

  XmlReaderConfig nativeConfig{};
  const bool loaded = xml_reader_load_from_file(path.string().c_str(), &nativeConfig);
  assert(loaded);
  assert(std::string(nativeConfig.window.title) == "Test Config");
  assert(nativeConfig.window.width == 1024);
  assert(nativeConfig.window.height == 768);
  assert(nativeConfig.graphics.target_fps == 144);
  assert(nativeConfig.window.fullscreen);
  assert(!nativeConfig.graphics.vsync);

  {
    std::ofstream file(path);
    file << R"(<Settings>
  <Window title="Reloaded Config" width="1280" height="720" fullscreen="false" />
  <Graphics target_fps="240" vsync="true" />
</Settings>)";
  }

  const bool reloaded = xml_reader_reload_from_file(path.string().c_str(), &nativeConfig);
  assert(reloaded);
  assert(std::string(nativeConfig.window.title) == "Reloaded Config");
  assert(nativeConfig.window.width == 1280);
  assert(nativeConfig.window.height == 720);
  assert(nativeConfig.graphics.target_fps == 240);
  assert(!nativeConfig.window.fullscreen);
  assert(nativeConfig.graphics.vsync);

  core::Config config;
  const bool configLoaded = core::Config::loadFromFile(path.string(), config);
  assert(configLoaded);
  assert(config.title == "Reloaded Config");
  assert(config.width == 1280);
  assert(config.height == 720);
  assert(config.target_fps == 240);
  assert(!config.fullscreen);
  assert(config.vsync);

  {
    std::ofstream file(path);
    file << R"(<Settings>
  <Window title="Bad Config" width="100" height="120" fullscreen="false" />
  <Graphics target_fps="9999" vsync="true" />
  <UI>
    <Rectangle r="-10" g="999" b="12" a="300" />
  </UI>
</Settings>)";
  }

  const bool invalidLoaded = core::Config::loadFromFile(path.string(), config);
  assert(invalidLoaded);
  assert(config.width == 320);
  assert(config.height == 240);
  assert(config.target_fps == 500);
  assert(config.rectangles.size() == 1);
  assert(config.rectangles[0].r == 0);
  assert(config.rectangles[0].g == 255);
  assert(config.rectangles[0].b == 12);
  assert(config.rectangles[0].a == 255);

  std::filesystem::remove(path);
  return 0;
}
