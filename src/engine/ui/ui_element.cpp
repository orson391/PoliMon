#include "ui/ui_element.h"

namespace engine::ui {

bool UiElement::load(graphics::IRenderer& renderer) {
  for (auto& child : m_children) {
    if (!child->load(renderer)) return false;
  }
  return true;
}

void UiElement::update(float deltaTime) {
  if (!m_visible) return;
  for (auto& child : m_children) {
    child->update(deltaTime);
  }
}

void UiElement::render(graphics::IRenderer& renderer) {
  if (!m_visible) return;
  for (auto& child : m_children) {
    child->render(renderer);
  }
}

bool UiElement::handleEvent(const SDL_Event& event) {
  if (!m_visible || !m_enabled) return false;
  for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
    if ((*it)->handleEvent(event)) return true;
  }
  return false;
}

UiElement* UiElement::addChild(std::unique_ptr<UiElement> child) {
  child->m_parent = this;
  m_children.push_back(std::move(child));
  return m_children.back().get();
}

void UiElement::removeChild(const UiElement* child) {
  for (auto it = m_children.begin(); it != m_children.end(); ++it) {
    if (it->get() == child) {
      m_children.erase(it);
      break;
    }
  }
}

void UiElement::setPosition(float x, float y) {
  m_bounds.x = x;
  m_bounds.y = y;
  m_dirty = true;
}

}  // namespace engine::ui
