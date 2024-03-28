#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>

#include "nev_bekero.h"
#include "jatek.h"
#include "game_over.h"

constexpr float window_width = 800.0f;
constexpr float window_height = 600.0f;

int main() {
  const std::string title{"Játék"};
  sf::RenderWindow window(sf::VideoMode(window_width, window_height),
                          sf::String::fromUtf8(title.begin(), title.end()));
  window.setFramerateLimit(60);

  std::string name = nev_bekero(window);
  std::cerr << "nev: " << name << std::endl;
  bool play_again = true;
  while (play_again) {
    bool result = jatek(window);
    std::cerr << "sikerult-e: " << result << std::endl; 
    play_again = game_over(window);
    std::cerr << "ujra jatszani? " << play_again << std::endl;
  }
  window.close();
}