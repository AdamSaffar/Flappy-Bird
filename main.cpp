#include <iostream>
#include <SFML/graphics.hpp>

class Pipes {

};
class Bird {

};
int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Flappy Bird");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;

            }
        }
    }
}