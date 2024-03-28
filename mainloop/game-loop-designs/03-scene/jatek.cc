#include "jatek.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cmath>
#include <cstdint>
#include <cstdlib>  // for EXIT_FAILURE
#include <iostream>
#include <stdexcept>
#include <vector>

constexpr float window_width = 800.0f;
constexpr float window_height = 600.0f;
constexpr float lovedek_r = 10.0f;
constexpr float lovedek_r_squared = lovedek_r * lovedek_r;
constexpr float lovedek_suruseg = 500.0f;  // Hány millisecundumonként lőhet?
constexpr float target_speed = 1.6e-3;
constexpr float target_dy = 30.0f;
constexpr float urhajo_speed = 1e-3;

Jatek::LovedekRenderer::LovedekRenderer() {
  if (!render_texture.create(lovedek_r, lovedek_r)) {
    throw std::runtime_error{"Could not allocate texture for the bullet!"};
  }
  sf::CircleShape c{5.0f};
  c.setFillColor(sf::Color::Green);
  render_texture.clear(sf::Color::Black);
  render_texture.draw(c);
  render_texture.display();
}

sf::Texture const& Jatek::LovedekRenderer::texture() const {
  return render_texture.getTexture();
}

float distance_squared(sf::Vector2f const& a, sf::Vector2f const& b) {
  return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

bool utkozott(sf::Vector2f const& target_pos, sf::Vector2f const& target_size,
              sf::Vector2f const& lovedek) {
  if (lovedek.x >= target_pos.x && lovedek.x <= target_pos.x + target_size.x) {
    if (lovedek.y >= target_pos.y - lovedek_r &&
        lovedek.y <= target_pos.y + target_size.y + lovedek_r) {
      return true;
    }
  }

  if (lovedek.y >= target_pos.y && lovedek.y <= target_pos.y + target_size.y) {
    if (lovedek.x >= target_pos.x - lovedek_r &&
        lovedek.x <= target_pos.x + target_size.x + lovedek_r) {
      return true;
    }
  }

  if (distance_squared(lovedek, target_pos) <= lovedek_r_squared) {
    return true;
  }
  if (distance_squared(
          lovedek, sf::Vector2f{target_pos.x, target_pos.y + target_size.y}) <=
      lovedek_r_squared) {
    return true;
  }
  if (distance_squared(
          lovedek, sf::Vector2f{target_pos.x + target_size.x, target_pos.y}) <=
      lovedek_r_squared) {
    return true;
  }
  if (distance_squared(lovedek, sf::Vector2f{target_pos.x + target_size.x,
                                             target_pos.y + target_size.y}) <=
      lovedek_r_squared) {
    return true;
  }

  return false;
}

sf::Texture load_urhajo() {
  const std::string urhajo_png = "../assets/urhajo.png";
  sf::Texture t;
  if (!t.loadFromFile(urhajo_png)) {
    std::cerr << "Could not load: " << urhajo_png << std::endl;
    exit(EXIT_FAILURE);
  }
  return t;
}

Jatek::Jatek()
    : urhajo_texture{load_urhajo()},
      urhajo{urhajo_texture},
      urhajo_x_max{window_width - urhajo_texture.getSize().x},
      target{sf::Vector2f{45.0f, 15.0f}} {
  urhajo.setPosition(0.0f, window_height - urhajo_texture.getSize().y);

  target.setFillColor(sf::Color::Red);
  target.setPosition(window_width - target.getSize().x, 0.0f);
}

bool Jatek::handle_event(sf::Event const& event) {
  switch (event.type) {
    case sf::Event::KeyPressed:
      switch (event.key.code) {
        case sf::Keyboard::Escape:
          return false;
        case sf::Keyboard::Right:
          key_direction = 1;
          key_right_down = true;
          break;
        case sf::Keyboard::Left:
          key_direction = -1;
          key_left_down = true;
          break;
        case sf::Keyboard::Space:
          key_space_down = true;
          break;
        default:
          break;
      }
      break;
    case sf::Event::KeyReleased:
      switch (event.key.code) {
        case sf::Keyboard::Right:
          key_right_down = false;
          break;
        case sf::Keyboard::Left:
          key_left_down = false;
          break;
        case sf::Keyboard::Space:
          key_space_down = false;
          lovedek_is_first = true;
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  return true;
}

int Jatek::compute_urhajo_direction() {
  if (key_right_down && !key_left_down) {
    return 1;
  } else if (!key_right_down && key_left_down) {
    return -1;
  } else if (key_right_down && key_left_down) {
    return key_direction;
  }
  return 0;
}

bool Jatek::state_update() {
  int urhajo_direction = compute_urhajo_direction();

  if (key_space_down &&
      (lovedek_is_first ||
       lovedek_clock.getElapsedTime().asMilliseconds() > lovedek_suruseg)) {
    sf::Sprite lovedek(lovedek_renderer.texture());
    lovedek.setPosition(
        urhajo.getPosition().x + urhajo_texture.getSize().x / 2.0f - 5.0f,
        urhajo.getPosition().y - 3.0f);
    lovedekek.push_back(lovedek);
    lovedek_is_first = false;
    lovedek_clock.restart();
  }

  int64_t elapsed = clock.restart().asMicroseconds();
  float urhajo_dx = urhajo_speed * static_cast<float>(elapsed);
  sf::Vector2f urhajo_pos = urhajo.getPosition();
  switch (urhajo_direction) {
    case 1:
      urhajo_pos.x += urhajo_dx;
      if (urhajo_pos.x > urhajo_x_max) {
        urhajo_pos.x = urhajo_x_max;
      }
      break;
    case -1:
      urhajo_pos.x -= urhajo_dx;
      if (urhajo_pos.x < 0.0f) {
        urhajo_pos.x = 0.0f;
      }
      break;
    default:
      break;
  }
  urhajo.setPosition(urhajo_pos);

  constexpr float lovedek_speed = 1e-3;
  float lovedek_dx = -lovedek_speed * static_cast<float>(elapsed);
  for (auto& lovedek : lovedekek) {
    lovedek.move(0.0f, lovedek_dx);
  }
  while (lovedekek.size() > 0 && lovedekek.front().getPosition().y < -10.0f) {
    lovedekek.pop_front();
  }

  float target_dx = target_speed * static_cast<float>(elapsed);
  sf::Vector2f target_pos = target.getPosition();
  switch (target_direction) {
    case -1:
      target_pos.x -= target_dx;
      if (target_pos.x < 0.0f) {
        target_direction = 1;
        target_pos.x = 0.0f;
        target_pos.y += target_dy;
      }
      break;
    case 1:
      target_pos.x += target_dx;
      if (target_pos.x > window_width - target.getSize().x) {
        target_direction = -1;
        target_pos.x = window_width - target.getSize().x;
        target_pos.y += target_dy;
      }
      break;
    default:
      break;
  }
  target.setPosition(target_pos);

  for (auto& lovedek : lovedekek) {
    if (utkozott(target.getPosition(), target.getSize(),
                 lovedek.getPosition())) {
      result = true;
      return false;
    }
  }

  if (target.getPosition().y > window_height) {
    result = false;
    return false;
  }

  return true;
}

void Jatek::paint(sf::RenderTarget& rt) {
  rt.draw(urhajo);
  rt.draw(target);
  for (auto const& lovedek : lovedekek) {
    rt.draw(lovedek);
  }
}