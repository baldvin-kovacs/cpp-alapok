#include "scene.h"

#include <SFML/Graphics.hpp>

bool Scene::run(sf::RenderWindow& window) {
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          return false;
        default:
          if (event.type == sf::Event::KeyPressed &&
              event.key.code == sf::Keyboard::Escape) {
            window.close();
            return true;
          }
          if (!handle_event(event)) {
            return true;
          }
      }
    }
    if (!state_update()) {
      return true;
    }
    window.clear(sf::Color::Black);
    paint(window);
    window.display();
  }
  return false;
}

bool Scene::state_update() { return true; }
