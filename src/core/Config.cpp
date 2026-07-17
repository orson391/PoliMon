#include "Config.h"

#include <algorithm>

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

int clampIntWithLog(const std::string& field, int value, int minValue, int maxValue) {
  const int clamped = std::clamp(value, minValue, maxValue);
  if (clamped != value) {
    Logger::log("Config XML invalid value for " + field + ": " + std::to_string(value) +
                " clamped to " + std::to_string(clamped));
  }
  return clamped;
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

  config.width = clampIntWithLog("Window.width", config.width, 320, 100000);
  config.height = clampIntWithLog("Window.height", config.height, 240, 100000);
  config.target_fps = clampIntWithLog("Graphics.target_fps", config.target_fps, 1, 500);

  tinyxml2::XMLElement* uiElem = root->FirstChildElement("UI");
  if (uiElem) {
    int index = 0;
    for (auto* rectElem = uiElem->FirstChildElement("Rectangle"); rectElem;
         rectElem = rectElem->NextSiblingElement("Rectangle")) {
      RectElement rect;
      rect.id = attrInt(rectElem, "id", index);
      rect.x = attrFloat(rectElem, "x", 0.0f);
      rect.y = attrFloat(rectElem, "y", 0.0f);
      rect.width = attrFloat(rectElem, "width", 0.0f);
      rect.height = attrFloat(rectElem, "height", 0.0f);
      rect.r = static_cast<unsigned char>(clampIntWithLog(
          "UI.Rectangle[" + std::to_string(index) + "].r", attrInt(rectElem, "r", 255), 0, 255));
      rect.g = static_cast<unsigned char>(clampIntWithLog(
          "UI.Rectangle[" + std::to_string(index) + "].g", attrInt(rectElem, "g", 255), 0, 255));
      rect.b = static_cast<unsigned char>(clampIntWithLog(
          "UI.Rectangle[" + std::to_string(index) + "].b", attrInt(rectElem, "b", 255), 0, 255));
      rect.a = static_cast<unsigned char>(clampIntWithLog(
          "UI.Rectangle[" + std::to_string(index) + "].a", attrInt(rectElem, "a", 255), 0, 255));
      rect.filled = attrBool(rectElem, "filled", true);
      rect.normalized = attrBool(rectElem, "normalized", false);
      config.rectangles.push_back(rect);
      index++;
    }
  }

  outConfig = config;
  return true;
}

}  // namespace core
