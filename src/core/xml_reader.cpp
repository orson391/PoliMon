#include "xml_reader.h"

#include <tinyxml2.h>

#include <algorithm>
#include <cstring>
#include <string>

#include "Config.h"

namespace {

void clear_config(XmlReaderConfig& config) {
  std::memset(&config, 0, sizeof(config));
}

void copy_string(char* destination, size_t destination_size, const std::string& source) {
  const auto length = std::min<size_t>(destination_size - 1, source.size());
  std::memcpy(destination, source.data(), length);
  destination[length] = '\0';
}

}  // namespace

extern "C" int xml_reader_load_from_file(const char* path, XmlReaderConfig* out_config) {
  if (!path || !out_config) {
    return 0;
  }

  clear_config(*out_config);

  core::Config config;
  if (!core::Config::loadFromFile(path, config)) {
    return 0;
  }

  copy_string(out_config->window.title, sizeof(out_config->window.title), config.title);
  out_config->window.width = config.width;
  out_config->window.height = config.height;
  out_config->window.fullscreen = config.fullscreen ? 1 : 0;
  out_config->graphics.target_fps = config.target_fps;
  out_config->graphics.vsync = config.vsync ? 1 : 0;

  const int rectCount =
      std::min<int>(static_cast<int>(config.rectangles.size()), XML_READER_MAX_RECTS);

  for (int i = 0; i < rectCount; ++i) {
    const core::RectElement& src = config.rectangles[static_cast<size_t>(i)];
    XmlReaderRect& dst = out_config->rects[i];
    dst.id = src.id;
    dst.x = src.x;
    dst.y = src.y;
    dst.width = src.width;
    dst.height = src.height;
    dst.r = src.r;
    dst.g = src.g;
    dst.b = src.b;
    dst.a = src.a;
    dst.filled = src.filled ? 1 : 0;
    dst.normalized = src.normalized ? 1 : 0;
  }
  out_config->rect_count = rectCount;

  return 1;
}

extern "C" int xml_reader_reload_from_file(const char* path, XmlReaderConfig* out_config) {
  return xml_reader_load_from_file(path, out_config);
}