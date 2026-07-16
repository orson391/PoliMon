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

  void setZoom(float zoom) {
    if (zoom > 0.0f) {
      zoom_ = zoom;
    }
  }

  float zoom() const { return zoom_; }

  void follow(float targetX, float targetY, float targetW, float targetH) {
    // Center camera on target using zoomed viewport dimensions
    float zoomWidth = viewportWidth_ / zoom_;
    float zoomHeight = viewportHeight_ / zoom_;

    float centerX = targetX + targetW * 0.5f;
    float centerY = targetY + targetH * 0.5f;

    x_ = centerX - zoomWidth * 0.5f;
    y_ = centerY - zoomHeight * 0.5f;

    if (hasBounds_) {
      if (x_ < 0.0f) x_ = 0.0f;
      if (y_ < 0.0f) y_ = 0.0f;
      if (x_ + zoomWidth > mapWidth_) x_ = mapWidth_ - zoomWidth;
      if (y_ + zoomHeight > mapHeight_) y_ = mapHeight_ - zoomHeight;

      // If map is smaller than zoomed viewport, center it
      if (mapWidth_ < zoomWidth) {
        x_ = (mapWidth_ - zoomWidth) * 0.5f;
      }
      if (mapHeight_ < zoomHeight) {
        y_ = (mapHeight_ - zoomHeight) * 0.5f;
      }
    }
  }

  float x() const { return x_; }
  float y() const { return y_; }
  float viewportWidth() const { return viewportWidth_; }
  float viewportHeight() const { return viewportHeight_; }

  // Check if a bounding box is visible within the zoomed camera viewport (frustum culling)
  bool isVisible(float worldX, float worldY, float width, float height) const {
    float zoomWidth = viewportWidth_ / zoom_;
    float zoomHeight = viewportHeight_ / zoom_;
    return (worldX + width >= x_ && worldX <= x_ + zoomWidth && worldY + height >= y_ &&
            worldY <= y_ + zoomHeight);
  }

 private:
  float x_ = 0.0f;
  float y_ = 0.0f;
  float viewportWidth_ = 800.0f;
  float viewportHeight_ = 600.0f;
  float mapWidth_ = 0.0f;
  float mapHeight_ = 0.0f;
  float zoom_ = 1.7f;  // Default zoom in (2x)
  bool hasBounds_ = false;
};

}  // namespace game::world
