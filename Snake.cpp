#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <iostream>

int randint(int &randnum, int min, int max) {
    std::random_device randomi;
    std::mt19937 gen(randomi());
    std::uniform_int_distribution<> distr(min, max);
    randnum = distr(gen);
    return randnum;
}

int main() {
    
    using namespace std;

    cout << "The game will automatically close/crash when you lose or win bcuz I am too lazy to code the easy win/lose detection" << endl;

    sf::RenderWindow window(sf::VideoMode({300, 180}), "Snake");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("C:/test_sfml/Roboto-VariableFont_wdth,wght.ttf")) {
        cerr << "Something went wrong" << endl;
        return -1;
    }

    int randnum = 0, direction = -20, randcount = 0, tail = 0, decl = 0;
    bool randomcheck = true;
    vector<sf::RectangleShape> boxes;
    vector<int> snake = {127, 147, 167}, illl, illll, illlll, illllll;
    for (float row = 0.f; row < 180.f; row += 15.f) {
        for (float col  = 0.f; col < 300.f; col += 15.f) {
            sf::RectangleShape box({15.f, 15.f});
            box.setPosition({col, row});
            boxes.push_back(box);
        }
    }

    sf::CircleShape apple(5.5f);
    apple.setFillColor(sf::Color::Red);

    for (int ill0 = 0; ill0 < 20; ill0++) {
        illl.push_back(ill0);
        illll.push_back(ill0 + 220);
    }

    for (int ill1 = 0; ill1 < 240; ill1 += 20) {
        illlll.push_back(ill1);
        illllll.push_back(ill1 + 19);
    }

    while (window.isOpen()) {
        window.clear(sf::Color::Black);
        while (const optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::W) {
                    if (direction != 20) {
                        direction = -20;
                    }
                }
                else if (key->code == sf::Keyboard::Key::A) {
                    if (direction != 1) {
                        direction = -1;
                    }
                }
                else if (key->code == sf::Keyboard::Key::D) {
                    if (direction != -1) {
                        direction = 1;
                    }
                }
                else if (key->code == sf::Keyboard::Key::S) {
                    if (direction != -20) {
                        direction = 20;
                    }
                }
            }
        }
        
        for (auto& dabba : boxes) {
            dabba.setFillColor(sf::Color::White);
        }
        for (auto const& num : snake) {
            boxes[num].setFillColor(sf::Color::Blue);
        }

        for (auto const& tile : boxes) {
            window.draw(tile);
        }

        if (randomcheck) {
            while (true) {
                randint(randnum, 0, 239);
                for (auto const& nos : snake) {
                    if (randnum != nos) {randcount++;}
                }
                if (randcount == snake.size()) {
                    apple.setPosition({boxes[randnum].getPosition().x + 2.f, boxes[randnum].getPosition().y + 2.f});
                    randcount = 0;
                break;
                }
                else {randcount = 0;}
            }
            randomcheck = false;
        }
        window.draw(apple);

        window.display();
        sf::sleep(sf::seconds(1));
        for (int move = snake.size() - 1; move > 0; move -= 1) {
            if (move == snake.size() - 1) {
                tail = snake[move] - snake[move - 1];
            }
            snake[move] = snake[move - 1];
        }
        snake[0] += direction;
        sf::Vector2f applepos(apple.getPosition().x - 2.f, apple.getPosition().y - 2.f);
        if (boxes[snake[0]].getPosition() == applepos) {
            snake.push_back(snake[snake.size() - 1] + tail);
            for (auto const& value : illl) {
                if ((value == snake[snake.size() - 1] || value == snake[0]) && direction == -20) {
                    decl = -1;
                }
            }
            for (auto const& value : illll) {
                if ((value == snake[snake.size() - 1] || value == snake[0]) && direction == 20) {
                    decl = -1;
                }
            }
            for (auto const& value : illlll) {
                if ((value == snake[snake.size() - 1] || value == snake[0]) && direction == -1) {
                    decl = -1;
                }
            }
            for (auto const& value : illllll) {
                if ((value == snake[snake.size() - 1] || value == snake[0]) && direction == 1) {
                    decl = -1;
                }
            }
            randomcheck = true;
        }
    }
    
    return 0;
}