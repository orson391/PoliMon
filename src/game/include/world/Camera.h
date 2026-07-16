#pragma once

#include <algorithm>
#include <cmath>

namespace game::world {

class Camera {
 public:
  Camera() = default;

  void setViewport(float width, float height) {
    viewportWidth_ = width;
    viewportHeight_ = height;
  }

  void setMapBounds(float width, float height) {
    mapWidth_ = width;
    mapHeight_ = height;
    hasBounds_ = true;
  }

  void follow(float targetX, float targetY, float targetW, float targetH) {
    // Center camera on target
    float centerX = targetX + targetW * 0.5f;
    float centerY = targetY + targetH * 0.5f;

    x_ = centerX - viewportWidth_ * 0.5f;
    y_ = centerY - viewportHeight_ * 0.5f;

    if (hasBounds_) {
      if (x_ < 0.0f) x_ = 0.0f;
      if (y_ < 0.0f) y_ = 0.0f;
      if (x_ + viewportWidth_ > mapWidth_) x_ = mapWidth_ - viewportWidth_;
      if (y_ + viewportHeight_ > mapHeight_) y_ = mapHeight_ - viewportHeight_;

      // If map is smaller than viewport, center it
      if (mapWidth_ < viewportWidth_) {
        x_ = (mapWidth_ - viewportWidth_) * 0.5f;
      }
      if (mapHeight_ < viewportHeight_) {
        y_ = (mapHeight_ - viewportHeight_) * 0.5f;
      }
    }
  }

  float x() const { return x_; }
  float y() const { return y_; }
  float viewportWidth() const { return viewportWidth_; }
  float viewportHeight() const { return viewportHeight_; }

  // Check if a bounding box is visible within the camera viewport (frustum culling)
  bool isVisible(float worldX, float worldY, float width, float height) const {
    return (worldX + width >= x_ && worldX <= x_ + viewportWidth_ &&
            worldY + height >= y_ && worldY <= y_ + viewportHeight_);
  }

 private:
  float x_ = 0.0f;
  float y_ = 0.0f;
  float viewportWidth_ = 800.0f;
  float viewportHeight_ = 600.0f;
  float mapWidth_ = 0.0f;
  float mapHeight_ = 0.0f;
  bool hasBounds_ = false;
};

}  // namespace game::world
