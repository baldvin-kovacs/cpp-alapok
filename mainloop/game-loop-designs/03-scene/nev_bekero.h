#ifndef NEV_BEKERO_H
#define NEV_BEKERO_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>

#include "scene.h"

constexpr int num_name_chars = 3;

class NevBekero : public Scene {
 public:
  NevBekero(sf::Font const& font);
  virtual bool handle_event(sf::Event const& event) override;
  virtual void paint(sf::RenderTarget& rt) override;

  std::string get_nev() const { return nev; }

 private:
  sf::Font font;
  sf::Text enter_name_text;
  sf::Text hit_enter_text;
  std::vector<sf::RectangleShape> boxes;
  std::vector<sf::Text> chars;
  sf::RectangleShape cursor;
  sf::Clock clock;

  std::string nev;

  enum class Phase {
    GET_CHAR,
    REQUEST_ENTER,
  };

  struct State {
    std::vector<sf::Uint32> chars;

    Phase phase() {
      if (chars.size() >= num_name_chars) {
        return Phase::REQUEST_ENTER;
      }
      return Phase::GET_CHAR;
    }
  };

  State state;
};

#endif  // NEV_BEKERO_H
