#ifndef GAME_OVER_H
#define GAME_OVER_H

#include "scene.h"

class GameOver : public Scene {
 public:
  GameOver(sf::Font const& font);
  virtual bool handle_event(sf::Event const& event) override;
  virtual void paint(sf::RenderTarget& rt) override;

  bool get_continue() { return result; }

 private:
  bool result = false;

  sf::Text game_over_text;

  class Button : public sf::Drawable {
   public:
    Button(sf::Font const& font, std::string label, sf::Vector2f const& pos);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    bool selected = false;

   private:
    sf::RectangleShape box;
    sf::Text txt;
    std::string result;
  };

  std::vector<Button> buttons;
  int num_selected;
};

#endif  // GAME_OVER_H
