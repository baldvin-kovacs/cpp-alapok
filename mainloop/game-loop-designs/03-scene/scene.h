#ifndef RUNNABLE_H
#define RUNNABLE_H

#include <SFML/Graphics.hpp>

class Scene {
 public:
  // Returns true if the game should continue.
  bool run(sf::RenderWindow& window);

 protected:
  // Handles an event. Returns true if this scene should continue,
  // false if this scene should end.
  virtual bool handle_event(sf::Event const& event) = 0;
  // Handles state changes. Returns true if this scene schould continue,
  // false if this scene should end.
  virtual bool state_update();
  virtual void paint(sf::RenderTarget& rt) = 0;
};

#endif  // RUNNABLE_H
