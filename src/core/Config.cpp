#include "Config.h"

#include <tinyxml2.h>

#include "Logger.h"

namespace core {

namespace {

// Small helpers so missing attributes fall back to the existing default
// instead of silently zero-initializing.
float attrFloat(tinyxml2::XMLElement* elem, const char* name, float fallback) {
  if (!elem) return fallback;
  float value = fallback;
  elem->QueryFloatAttribute(name, &value);
  return value;
}

int attrInt(tinyxml2::XMLElement* elem, const char* name, int fallback) {
  if (!elem) return fallback;
  int value = fallback;
  elem->QueryIntAttribute(name, &value);
  return value;
}

bool attrBool(tinyxml2::XMLElement* elem, const char* name, bool fallback) {
  if (!elem) return fallback;
  bool value = fallback;
  elem->QueryBoolAttribute(name, &value);
  return value;
}

std::string attrString(tinyxml2::XMLElement* elem, const char* name, const std::string& fallback) {
  if (!elem) return fallback;
  const char* value = elem->Attribute(name);
  return value ? std::string(value) : fallback;
}

}  // namespace

bool Config::loadFromFile(const std::string& path, Config& outConfig) {
  tinyxml2::XMLDocument doc;
  if (doc.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS) {
    Logger::log("Failed to parse config XML: " + std::string(doc.ErrorStr()));
    return false;
  }

  tinyxml2::XMLElement* root = doc.FirstChildElement("Settings");
  if (!root) {
    Logger::log("Config XML missing root <Settings> element");
    return false;
  }

  Config config;  // start from defaults, overwrite only what's present

  tinyxml2::XMLElement* windowElem = root->FirstChildElement("Window");
  config.title = attrString(windowElem, "title", config.title);
  config.width = attrInt(windowElem, "width", config.width);
  config.height = attrInt(windowElem, "height", config.height);
  config.fullscreen = attrBool(windowElem, "fullscreen", config.fullscreen);

  tinyxml2::XMLElement* graphicsElem = root->FirstChildElement("Graphics");
  config.target_fps = attrInt(graphicsElem, "target_fps", config.target_fps);
  config.vsync = attrBool(graphicsElem, "vsync", config.vsync);

  outConfig = config;
  return true;
}

}  // namespace core