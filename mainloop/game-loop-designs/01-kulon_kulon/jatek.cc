#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cmath>
#include <cstdint>
#include <cstdlib>  // for EXIT_FAILURE
#include <deque>
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

class LovedekRenderer {
 public:
  LovedekRenderer() {
    if (!render_texture.create(lovedek_r, lovedek_r)) {
      throw std::runtime_error{"Could not allocate texture for the bullet!"};
    }
    sf::CircleShape c{5.0f};
    c.setFillColor(sf::Color::Green);
    render_texture.clear(sf::Color::Black);
    render_texture.draw(c);
    render_texture.display();
  }
  sf::Texture const& texture() const { return render_texture.getTexture(); }

 private:
  sf::RenderTexture render_texture;
};

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

int main() {
  const std::string title{"Játék"};
  sf::RenderWindow window(sf::VideoMode(window_width, window_height),
                          sf::String::fromUtf8(title.begin(), title.end()));
  window.setFramerateLimit(60);

  sf::Texture urhajo_texture;
  const std::string urhajo_png = "../assets/urhajo.png";
  if (!urhajo_texture.loadFromFile(urhajo_png)) {
    std::cerr << "Could not load: " << urhajo_png << std::endl;
    exit(EXIT_FAILURE);
  }
  sf::Sprite urhajo(urhajo_texture);
  const sf::Vector2f urhajo_size{urhajo_texture.getSize()};
  sf::Vector2f urhajo_pos{0.0f, window_height - urhajo_size.y};
  const float urhajo_x_max = window_width - urhajo_size.x;
  int urhajo_direction = 0;
  constexpr float urhajo_speed = 1e-3;

  LovedekRenderer lovedek_renderer;
  std::deque<sf::Sprite> lovedekek;

  bool key_right_down = false;
  bool key_left_down = false;
  int key_direction = 0;

  bool key_space_down = false;
  bool lovedek_is_first = true;
  sf::Clock lovedek_clock;

  sf::RectangleShape target{sf::Vector2f{45.0f, 15.0f}};
  target.setFillColor(sf::Color::Red);
  target.setPosition(window_width - target.getSize().x, 0.0f);
  int target_direction = -1;

  sf::Clock clock;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        case sf::Event::KeyPressed:
          switch (event.key.code) {
            case sf::Keyboard::Escape:
              window.close();
              break;
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
    }

    if (key_right_down && !key_left_down) {
      urhajo_direction = 1;
    } else if (!key_right_down && key_left_down) {
      urhajo_direction = -1;
    } else if (key_right_down && key_left_down) {
      urhajo_direction = key_direction;
    } else {
      urhajo_direction = 0;
    }

    if (key_space_down &&
        (lovedek_is_first ||
         lovedek_clock.getElapsedTime().asMilliseconds() > lovedek_suruseg)) {
      sf::Sprite lovedek(lovedek_renderer.texture());
      lovedek.setPosition(urhajo_pos.x + urhajo_size.x / 2.0f - 5.0f,
                          urhajo_pos.y - 3.0f);
      lovedekek.push_back(lovedek);
      lovedek_is_first = false;
      lovedek_clock.restart();
    }

    int64_t elapsed = clock.restart().asMicroseconds();
    float urhajo_dx = urhajo_speed * static_cast<float>(elapsed);
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
        std::cout << "Talált!" << std::endl;
        window.close();
      }
    }

    if (target.getPosition().y > window_height) {
      std::cout << "Vereség" << std::endl;
      window.close();
    }

    window.clear(sf::Color::Black);
    urhajo.setPosition(urhajo_pos);
    window.draw(urhajo);

    window.draw(target);

    for (auto const& lovedek : lovedekek) {
      window.draw(lovedek);
    }
    window.display();
  }
}
