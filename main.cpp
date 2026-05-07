#include <iostream>
#include <SFML/graphics.hpp>

class Pipes {

};
class Bird {
private:
    const float birdPosX{}; // Never change birds X position
    float birdPosY{};
    float birdVelocity{0.0f};
    float birdAcceleration{0.0f};
    sf::CircleShape shape;
public:
    // initalize bird position in constructor list
    Bird(unsigned int screenWidth, unsigned int screenHeight) : birdPosX(static_cast<float>(screenWidth)), birdPosY(static_cast<float>(screenHeight)) {
        shape.setRadius(25.0f);
        shape.setFillColor(sf::Color::Red);
        shape.setOrigin(25.0f,25.0f);

        shape.setPosition(birdPosX, birdPosY);
    }

    void jump() {
        birdVelocity = -10.0f; // increase velocity
    }
    void onNewFrame() {
        birdVelocity += 0.5f; // constant gravitational pull on bird
        birdPosY += birdVelocity; // update position based on current speed
        shape.setPosition(birdPosX, birdPosY);
        //std::cout << "Bird Pos Y: " << birdPosY << '\n';
    }
    // pass reference to window
    void draw(sf::RenderWindow &window) {
        window.draw(shape);
    }
};
int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Flappy Bird");
    window.setFramerateLimit(20); // 20 FPS

    float width = static_cast<float>(window.getSize().x) / 7.5f;
    float height = static_cast<float>(window.getSize().y  / 2.0f);
    // initialize bird object
    Bird playerBird(width, height);
    //int count = 0;
    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::TextEntered:
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                        /*
                         * Enter update velocity logic
                         */
                        playerBird.jump();
                        //std::cout << static_cast<char>(event.text.unicode) << '\n';
                    }
                    break;
                case sf::Event::LostFocus:
                    //myGame.pause();
                    break;
                case sf::Event::GainedFocus:
                    //myGame.play();
                    break;
                case sf::Event::MouseMoved:
                    std::cout <<"X pos: " << event.mouseMove.x << '\n';
                    std::cout << "Y pos: " << event.mouseMove.y << '\n';
                default:
                    break;

            }
        }
        playerBird.onNewFrame();

        // clear current frame
        window.clear(sf::Color::Black);
        // call draw func
        playerBird.draw(window);
        // end current frame
        window.display();

    }
    return 0;
}