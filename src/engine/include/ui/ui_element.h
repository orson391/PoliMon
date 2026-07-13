#pragma once

#include <SDL3/SDL.h>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace engine::ui {
class UiElement {
 public:
  UiElement() = default;
  explicit UiElement(std::string id) : m_id(std::move(id)) {}
  virtual ~UiElement() = default;

  // Non-copyable (owns children via unique_ptr), movable if you need it later.
  UiElement(const UiElement&) = delete;
  UiElement& operator=(const UiElement&) = delete;

  virtual bool load(SDL_Renderer* renderer);
  virtual void update(float deltaTime);
  virtual void render(SDL_Renderer* renderer);

  virtual bool handleEvent(const SDL_Event& event);

  UiElement* addChild(std::unique_ptr<UiElement> child);

  void removeChild(const UiElement* child);

  UiElement* getParent() const { return m_parent; }
  const std::vector<std::unique_ptr<UiElement>>& getChildren() const { return m_children; }

  void setPosition(float x, float y);

  void setSize(float w, float h) {
    m_bounds.w = w;
    m_bounds.h = h;
    m_dirty = true;
  }

  const SDL_FRect& getBounds() const { return m_bounds; }

  bool contains(float x, float y) const {
    return x >= m_bounds.x && x <= m_bounds.x + m_bounds.w && y >= m_bounds.y &&
           y <= m_bounds.y + m_bounds.h;
  }

  void setVisible(bool visible) { m_visible = visible; }
  bool isVisible() const { return m_visible; }

  void setEnabled(bool enabled) { m_enabled = enabled; }
  bool isEnabled() const { return m_enabled; }

  const std::string& getId() const { return m_id; }
  void setId(std::string id) { m_id = std::move(id); }

 protected:
  std::string m_id;
  SDL_FRect m_bounds{0, 0, 0, 0};
  bool m_visible = true;
  bool m_enabled = true;
  bool m_dirty = true;  // set when layout needs recalculating

  UiElement* m_parent = nullptr;
  std::vector<std::unique_ptr<UiElement>> m_children;
};

}  // namespace engine::ui
