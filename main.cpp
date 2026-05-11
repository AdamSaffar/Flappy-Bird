#include <iostream>
#include <SFML/graphics.hpp>
#include <random>
#include <cstdlib>
#include <ctime>
#include <deque>

struct PipePair {
    sf::RectangleShape bottomPipe;
    sf::RectangleShape topPipe;
    float randHight = std::rand() % (720 - 100 + 1) + 100;
    float xPos;
    bool passed = false;

    PipePair(float x, float gapY, float gapHeight, float pipeWidth, float screenHeight) : xPos(x) {
        // stretches down to the top of the gap
        topPipe.setSize(sf::Vector2f(pipeWidth, gapY));
        topPipe.setPosition(xPos, 0.0f);
        topPipe.setFillColor(sf::Color::Green);

        // 2. Bottom Pipe: Starts at the bottom of the gap, stretches to the floor
        float bottomHeight = screenHeight - (gapY + gapHeight);
        bottomPipe.setSize(sf::Vector2f(pipeWidth, bottomHeight));
        bottomPipe.setPosition(xPos, gapY + gapHeight);
        bottomPipe.setFillColor(sf::Color::Green);
    }

    void move(float speed) {
        xPos -= speed;
        topPipe.setPosition(xPos, topPipe.getPosition().y);
        bottomPipe.setPosition(xPos, bottomPipe.getPosition().y);
    }

    void draw(sf::RenderWindow &window) {
        window.draw(topPipe);
        window.draw(bottomPipe);
    }
};
class Pipes {
private:
    std::deque<PipePair> pipeQueue;
    float screenHeight;
    float screenWidth;
    // constant vars
    const float pipeSpacing{550.0f}; // distance between pipes
    const float pipeWidth{100.0f};
    const float gapHeight{200.0f}; // size of hole the bird flies through
    const float pipeSpeedSeconds{300.0f}; // tested at 5.0 speed, 60fps = 300 per sec

public:
    Pipes(float sWidth, float sHeight) : screenHeight(sHeight), screenWidth(sWidth) {
        for (int i = 0; i < 4; i++) {
            float startX = screenWidth + (i * pipeSpacing);
            spawnPipe(startX);
        }
    }
    void spawnPipe(float startX) {
        int minGapY = 100;
        int maxGapY = static_cast<int>(screenHeight - gapHeight - 100);
        float randGapY = static_cast<float>(std::rand() % (maxGapY - minGapY + 1) + minGapY);

        pipeQueue.push_back(PipePair(startX, randGapY, gapHeight, pipeWidth, screenHeight));
    }
    // called every frame
    void update(float dt) {
        for (auto& pipe : pipeQueue) {
            // Distance = speed * time
            pipe.move(pipeSpeedSeconds * dt);
        }

        if (!pipeQueue.empty() && pipeQueue.front().xPos < -pipeWidth) {
            // remove old pipe
            pipeQueue.pop_front();

            // find where last pipe is and spawn the new one behind it
            float newX = pipeQueue.back().xPos + pipeSpacing;
            spawnPipe(newX);
        }
    }
    bool checkCollision(const sf::FloatRect& birdHitbox) {
        for (auto& pipe : pipeQueue) {
            if (pipe.topPipe.getGlobalBounds().intersects(birdHitbox) || pipe.bottomPipe.getGlobalBounds().intersects(birdHitbox)) {
                return true; // collision detected
            }
        }
        return false; // no collision
    }

    int checkScore(float birdPosX) {
        int pointsScoredThisFrame = 0;

        for (auto& pipe : pipeQueue) {
            if (!pipe.passed && birdPosX > pipe.xPos + pipeWidth) {
                pipe.passed = true; // mark as scored so we don't count it twice
                pointsScoredThisFrame++;
            }
        }
        return pointsScoredThisFrame;
    }
    void draw(sf::RenderWindow& window) {
        for (auto& pipe : pipeQueue) {
            pipe.draw(window);
        }
    }
};
class Bird {
private:
    const float birdPosX{}; // Never change birds X position
    float birdPosY{};
    float birdVelocity{0.0f};
    float birdAcceleration{0.0f};
    sf::CircleShape shape;

    float windowHeight;
public:
    // initalize bird position in constructor list
    Bird(float screenWidth, float screenHeight) : birdPosX(static_cast<float>(screenWidth) / 7.5f), birdPosY(static_cast<float>(screenHeight) / 2.0f) {
        windowHeight = screenHeight;
        shape.setRadius(25.0f);
        shape.setFillColor(sf::Color::Red);
        shape.setOrigin(25.0f,25.0f);

        shape.setPosition(birdPosX, birdPosY);
    }

    void jump() {
        /*
        // We can only jump if the bird is falling
        if (birdVelocity >= 0) {
            birdVelocity = -10.0f; // increase velocity
        }
        */
        birdVelocity = -400.0f;
    }
    void onNewFrame(float dt) {
        float gravity = 1000.0f; // constant gravitational pull on bird

        birdVelocity += gravity * dt;
        birdPosY += birdVelocity * dt;
        shape.setPosition(birdPosX, birdPosY);
        //std::cout << "Bird Pos Y: " << birdPosY << '\n';
    }
    // pass reference to window
    void draw(sf::RenderWindow &window) {
        window.draw(shape);
    }
    sf::FloatRect getBounds() const{
        return shape.getGlobalBounds();
    }
    float getX() const {
        return birdPosX;
    }
    bool floorCeilingCollision() {
        if (birdPosY >= windowHeight || birdPosY <= 0) {
            return true;
        } else {
            return false;
        }
    }
};
int main() {
    std::srand(static_cast<unsigned>(std::time(0)));
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Flappy Bird");
    //window.setFramerateLimit(60); // 60 FPS
    window.setVerticalSyncEnabled(true); // set frame rate equal to monitors frequency
    sf::Clock clock;

    float width = static_cast<float>(window.getSize().x);
    float height = static_cast<float>(window.getSize().y);
    // initialize bird object
    Bird playerBird(width, height);
    Pipes pipeGen(width, height);

    int currentScore = 0;
    bool isGameOver = false;

    while (window.isOpen()) {
        sf::Time elapsed = clock.restart(); // reset clock at each frame
        float dt = elapsed.asSeconds(); // time elapsed between a single frame
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::TextEntered:
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                        if (isGameOver == true) {
                            break; // dont let bird jump
                        }
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
                    //std::cout <<"X pos: " << event.mouseMove.x << '\n';
                    //std::cout << "Y pos: " << event.mouseMove.y << '\n';
                default:
                    break;

            }
        }
        if (!isGameOver) {
            playerBird.onNewFrame(dt);
            pipeGen.update(dt);
            // check for floor/ceiling and pipe collision
            if (playerBird.floorCeilingCollision() || pipeGen.checkCollision(playerBird.getBounds())) {
                isGameOver = true;
                std::cout << "GAME OVER! Final Score: " << currentScore << '\n';
            }

            int points = pipeGen.checkScore(playerBird.getX());
            if (points > 0) {
                currentScore += points;
            }
        }

        // clear current frame
        window.clear(sf::Color::Black);
        // call draw func
        playerBird.draw(window);
        pipeGen.draw(window);
        // end current frame
        window.display();

    }
    return 0;
}