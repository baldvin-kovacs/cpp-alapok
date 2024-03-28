#include "game_over.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cstdint>
#include <cstdlib>  // for EXIT_FAILURE
#include <cwctype>
#include <functional>
#include <iostream>
#include <vector>

GameOver::Button::Button(sf::Font const& font, std::string label,
                         sf::Vector2f const& pos)
    : txt{sf::String::fromUtf8(label.begin(), label.end()), font, 50},
      result{label} {
  txt.setPosition(pos);
  txt.setFillColor(sf::Color::Green);
  sf::FloatRect txtBounds = txt.getGlobalBounds();
  box.setPosition(sf::Vector2f{pos.x - 10.0f, pos.y - 10.0f});
  box.setSize(sf::Vector2f{txtBounds.width + 30.0f, 80.0f});
  box.setOutlineColor(sf::Color::Green);
  box.setOutlineThickness(2.0f);
  box.setFillColor(sf::Color::Black);
}

void GameOver::Button::draw(sf::RenderTarget& target,
                            sf::RenderStates states) const {
  if (selected) {
    target.draw(box);
  }
  target.draw(txt);
}

GameOver::GameOver(sf::Font const& font)
    : game_over_text{"Game over", font, 50} {
  game_over_text.setPosition(sf::Vector2f{50.0f, 50.0f});
  game_over_text.setFillColor(sf::Color::Green);

  buttons.push_back(Button{font, "Play again", sf::Vector2f{50.0f, 400.0f}});
  buttons.push_back(Button{font, "Exit", sf::Vector2f{500.0f, 400.0f}});
  num_selected = 0;
  buttons[0].selected = true;
}

bool GameOver::handle_event(sf::Event const& event) {
  switch (event.type) {
    case sf::Event::KeyPressed: {
      switch (event.key.code) {
        case sf::Keyboard::Left:
          // fallthrough
        case sf::Keyboard::Right:
          buttons[num_selected].selected = false;
          num_selected = (num_selected + 1) % 2;
          buttons[num_selected].selected = true;
          break;
        case sf::Keyboard::Enter:
          result = num_selected == 0;
          return false;
        default:
          break;
      }
    }; break;
    default:
      break;
  }
  return true;
}

void GameOver::paint(sf::RenderTarget& rt) {
  rt.clear(sf::Color::Black);

  rt.draw(game_over_text);
  for (auto const& button : buttons) {
    rt.draw(button);
  }
}
