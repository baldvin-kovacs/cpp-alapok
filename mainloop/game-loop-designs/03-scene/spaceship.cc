#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cstdlib>
#include <iostream>

#include "game_over.h"
#include "jatek.h"
#include "nev_bekero.h"

int main() {
  const std::string title{"Spaceship"};
  sf::RenderWindow window(sf::VideoMode(800, 600),
                          sf::String::fromUtf8(title.begin(), title.end()));
  window.setFramerateLimit(60);

  sf::Font font;
  if (!font.loadFromFile("../fonts/KodeMono-SemiBold.ttf")) {
    return EXIT_FAILURE;
  }

  NevBekero nev_bekero{font};
  if (!nev_bekero.run(window)) {
    return EXIT_SUCCESS;
  }
  std::cerr << "Név: " << nev_bekero.get_nev() << std::endl;

  while (true) {
    Jatek jatek;
    if (!jatek.run(window)) {
      break;
    }
    std::cerr << "Győzelem: " << jatek.get_result() << std::endl;

    GameOver game_over(font);
    if (!game_over.run(window)) {
      break;
    }
    std::cerr << "Folytatás: " << game_over.get_continue() << std::endl;
    if (!game_over.get_continue()) {
      break;
    }
  }

  return EXIT_SUCCESS;
}