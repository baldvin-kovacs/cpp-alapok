#include <SFML/Graphics.hpp>
#include <cstdint>
#include <cstdlib>  // for EXIT_FAILURE
#include <cwctype>
#include <vector>
#include <functional>

class Button : public sf::Drawable {
  public:
    Button(sf::Font const& font, std::string label, sf::Vector2f const& pos)
      : txt{sf::String::fromUtf8(label.begin(), label.end()), font, 50}, result{label} {
      txt.setPosition(pos);
      txt.setFillColor(sf::Color::Green);
      sf::FloatRect txtBounds = txt.getGlobalBounds();
      box.setPosition(sf::Vector2f{pos.x - 10.0f, pos.y - 10.0f});
      box.setSize(sf::Vector2f{txtBounds.width + 30.0f, 80.0f});
      box.setOutlineColor(sf::Color::Green);
      box.setOutlineThickness(2.0f);
      box.setFillColor(sf::Color::Black);
    }

    std::string get_result() const {
      return result;
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
      if (selected) {
        target.draw(box);
      }
      target.draw(txt);
    }

    bool selected = false;
  private:
    sf::RectangleShape box;
    sf::Text txt;
    std::string result;
};

// Returns true if player requested another round
bool game_over(sf::RenderWindow &window) {
  sf::Font font;
  if (!font.loadFromFile("../fonts/KodeMono-SemiBold.ttf")) {
    return EXIT_FAILURE;
  }

  sf::Text game_over_text("Game over", font, 50.0f);
  game_over_text.setPosition(sf::Vector2f{50.0f, 50.0f});
  game_over_text.setFillColor(sf::Color::Green);

  std::vector<Button> buttons;
  buttons.push_back(Button{font, "Play again", sf::Vector2f{50.0f, 400.0f}});
  buttons.push_back(Button{font, "Exit", sf::Vector2f{500.0f, 400.0f}});

  int num_selected = 0;
  buttons[0].selected = true;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;
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
              if (buttons[num_selected].get_result() == "Play again") {
                return true;
              } else {
                return false;
              }
              break;
            case sf::Keyboard::Escape:
              window.close();
              break;
            default:
              break;
          }
        }; break;
        default:
          break;
      }
    }

    window.clear(sf::Color::Black);

    window.draw(game_over_text);
    for (auto const& button: buttons) {
      window.draw(button);
    }

    window.display();

  }

  return false;
}
