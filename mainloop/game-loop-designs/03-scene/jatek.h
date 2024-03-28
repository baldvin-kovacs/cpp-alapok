#ifndef JATEK_H
#define JATEK_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <deque>
#include <vector>

#include "scene.h"

class Jatek : public Scene {
 public:
  Jatek();
  virtual bool handle_event(sf::Event const& event) override;
  virtual bool state_update() override;
  virtual void paint(sf::RenderTarget& rt) override;

  bool get_result() const { return result; }

 private:
  bool result = false;

  sf::Clock clock;
  sf::Texture urhajo_texture;
  sf::Sprite urhajo;
  const float urhajo_x_max;
  std::deque<sf::Sprite> lovedekek;

  class LovedekRenderer {
   public:
    LovedekRenderer();
    sf::Texture const& texture() const;

   private:
    sf::RenderTexture render_texture;
  };

  LovedekRenderer lovedek_renderer;

  bool key_right_down = false;
  bool key_left_down = false;
  int key_direction = 0;

  bool key_space_down = false;
  bool lovedek_is_first = true;
  sf::Clock lovedek_clock;

  sf::RectangleShape target;
  int target_direction = -1;

  int compute_urhajo_direction();
};

#endif  // JATEK_H
