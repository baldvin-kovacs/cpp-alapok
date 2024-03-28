#include <SFML/Graphics.hpp>
#include <cstdlib>  // for EXIT_FAILURE
#include <cwctype>
#include <iostream>
#include <vector>

constexpr float char_input_box_width = 80.0f;
constexpr float char_input_box_height = 120.0f;

sf::RectangleShape make_char_input_box(sf::Vector2f const& pos) {
  sf::RectangleShape rect{
      sf::Vector2f{char_input_box_width, char_input_box_height}};
  rect.setOutlineColor(sf::Color::Green);
  rect.setOutlineThickness(1.0f);
  rect.setFillColor(sf::Color::Black);
  rect.setPosition(pos);
  return rect;
}

enum class Phase {
  GET_CHAR,
  REQUEST_ENTER,
};

constexpr int num_name_chars = 3;

struct State {
  std::vector<sf::Uint32> chars;

  Phase phase() {
    if (chars.size() >= num_name_chars) {
      return Phase::REQUEST_ENTER;
    }
    return Phase::GET_CHAR;
  }
};

std::string nev_bekero(sf::RenderWindow &window) {
  sf::Font font;
  if (!font.loadFromFile("../fonts/KodeMono-SemiBold.ttf")) {
    std::cerr << "font not found" << std::endl;
    exit(EXIT_FAILURE);
  }
  sf::Text enter_name_text("Enter your name", font, 50.0f);
  enter_name_text.setPosition(sf::Vector2f{50.0f, 50.0f});
  enter_name_text.setFillColor(sf::Color::Green);

  sf::Text hit_enter_text(">>> Hit enter to play <<<", font, 50.0f);
  hit_enter_text.setPosition(sf::Vector2f{25.0f, 450.0f});
  hit_enter_text.setFillColor(sf::Color::Green);

  std::vector<sf::RectangleShape> boxes;
  constexpr float chars_left = 200.0f;
  constexpr float chars_top = 200.0f;
  constexpr float chars_dx = 120.0f;
  for (int i = 0; i < num_name_chars; ++i) {
    boxes.push_back(make_char_input_box(
        sf::Vector2f{chars_left + chars_dx * i, chars_top}));
  }

  std::vector<sf::Text> chars;

  sf::RectangleShape cursor{sf::Vector2f{char_input_box_width - 20.0f, 2}};
  cursor.setFillColor(sf::Color::Green);

  State state;

  sf::Clock clock;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;
        case sf::Event::KeyPressed:
          if (state.phase() == Phase::REQUEST_ENTER) {
            if (event.key.code == sf::Keyboard::Enter) {
              auto name =
                  sf::String::fromUtf32(state.chars.begin(), state.chars.end())
                      .toUtf8();
              return std::string(name.begin(), name.end());
            }
          };
          break;
        case sf::Event::TextEntered:
          if (state.phase() == Phase::GET_CHAR) {
            std::wint_t chr = std::towupper(event.text.unicode);
            state.chars.push_back(chr);
            sf::Text new_char(sf::String(static_cast<sf::Uint32>(chr)), font, 100.0f);
            new_char.setPosition(sf::Vector2f{
                chars_left + chars_dx * chars.size() + 10.0f, chars_top});
            new_char.setFillColor(sf::Color::Green);
            chars.push_back(new_char);
          };
          break;
        default:
          // pass
          break;
      }
    }

    window.clear(sf::Color::Black);

    window.draw(enter_name_text);
    for (auto const& box : boxes) {
      window.draw(box);
    }

    for (auto const& chr : chars) {
      window.draw(chr);
    }

    int32_t elapsed = clock.getElapsedTime().asMilliseconds() / 50;
    if (elapsed % 10 < 6) {
      switch (state.phase()) {
        case Phase::GET_CHAR:
          cursor.setPosition(
              sf::Vector2f{chars_left + chars_dx * state.chars.size() + 10.0f,
                           chars_top + char_input_box_height - 15.0f});
          window.draw(cursor);
          break;
        case Phase::REQUEST_ENTER:
          window.draw(hit_enter_text);
          break;
        default:
          break;
      }
    }

    window.display();
  }

  return "ERR";
}