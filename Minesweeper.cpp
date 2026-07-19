#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <iostream>
#include <random>
#include <variant>

int randint(int &randnum, int min, int max) {
    std::random_device randomi;
    std::mt19937 gen(randomi());
    std::uniform_int_distribution<> distr(min, max);
    randnum = distr(gen);
    return randnum;
}

std::vector<int> minecounter(std::vector<int> &neighbourhood, bool &check, int &index) {
        if (index % 20 == 0) {
            neighbourhood = {            index - 20, index - 19, 
                                                     index + 1, 
                                         index + 20, index + 21};
        }
        else {
            for (int i = 1; i < 11; i++) {
                if (index == ((20 * i) - 1)) {
                    neighbourhood = {index - 21, index - 20, 
                                     index - 1, 
                                     index + 19, index + 20            };
                    check = true;
                }
            }
            if (check == false) {
                neighbourhood = {index - 21, index - 20, index - 19, 
                                 index - 1,              index + 1, 
                                 index + 19, index + 20, index + 21};
            }
            check = false;
        }
    return neighbourhood;
}

int main() {
    
    using namespace std;

    cout << "There are 31 mines on this board" << endl;

    sf::RenderWindow window(sf::VideoMode({600, 300}), "Minesweeper");
    window.setFramerateLimit(60);

    int randnum = 0, Opencount = 0, targetX = 0, targetY = 0, targetIndex = 0, minecount = 0, monoCount = 0, ifNum = 0;
    bool randombool = false, check = false, end = false, tobedone = false;
    vector<sf::RectangleShape> tiles;
    vector<int> randomcheck, neighbourhood;
    vector<variant<monostate, int>> Opencoder(200);
    vector<variant<monostate, sf::RectangleShape, sf::Text>> Open(200);

    sf::Font font;
    if (!font.openFromFile("C:/test_sfml/Roboto-VariableFont_wdth,wght.ttf")) {
        cerr << "Error 404: Font file not found" << endl;
        return -1;
    }

    for (float row = 0.f; row < 300.f; row += 30.f) {
        for (float col = 0.f; col < 600.f; col += 30.f) {
            sf::RectangleShape tile({30.f, 30.f});
            tile.setFillColor(sf::Color(200, 200, 200));
            tile.setPosition({col, row});
            tiles.push_back(tile);
        }
    }

    sf::RectangleShape Hline({600.f, 2.f});
    Hline.setFillColor(sf::Color(128, 128, 128));
    sf::RectangleShape Vline({2.f, 300.f});
    Vline.setFillColor(sf::Color(128, 128, 128));

    sf::RectangleShape mine({15.f, 15.f});
    mine.setFillColor(sf::Color::Black);

    sf::RectangleShape Cloth({20.f, 10.f});
    Cloth.setFillColor(sf::Color::Red);
    sf::RectangleShape Pole({2.f, 20.f});
    Pole.setFillColor(sf::Color::Black);

    sf::Text win(font);
    win.setString("You won!: You have cleared all the tiles");
    win.setFillColor(sf::Color::Black);
    win.setCharacterSize(23);
    win.setPosition({103, 135});
    sf::Text lose(font);
    lose.setString("You lost!: You stepped on a mine");
    lose.setFillColor(sf::Color::Black);
    lose.setCharacterSize(23);
    lose.setPosition({128, 135});

    while (randomcheck.size() != 31) {
        randint(randnum, 0, 199);
        for (auto const& num : randomcheck) {
            if (num == randnum) {
                randombool = true;
            }
        }
        if (randombool == false) {
            randomcheck.push_back(randnum);
        }
        randombool = false;
    }

    for (auto const& nums : randomcheck) {
        Open[nums] = mine;
    }

    for (int index = 0; index < 200; index++) {
        if (holds_alternative<std::monostate>(Open[index])) {
            minecounter(neighbourhood, check, index);
            for (auto const& neighbour : neighbourhood) {
                if (neighbour > -1 && neighbour < 200) {
                    if (holds_alternative<sf::RectangleShape>(Open[neighbour])) {minecount++;}
                }
            }
            if (minecount > 0) {
                sf::Text Minecount(font);
                Minecount.setString(to_string(minecount));
                Minecount.setFillColor(sf::Color::Green);
                Minecount.setStyle(sf::Text::Style::Bold);
                Minecount.setCharacterSize(14);
                Open[index] = Minecount;
            }
            minecount = 0;
            neighbourhood.clear();
        }
    }

    while (window.isOpen()) {
        window.clear(sf::Color::White);
        while (const optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                tobedone = false;
                targetX = window.mapPixelToCoords(mouse->position).x / 30;
                targetY = window.mapPixelToCoords(mouse->position).y / 30;
                targetIndex = (20 * targetY) + targetX;
                if (holds_alternative<int>(Opencoder[targetIndex])) {
                    tobedone = true;
                    ifNum = get<int>(Opencoder[targetIndex]);
                }
                if (mouse->button == sf::Mouse::Button::Left) {
                    if (tobedone) {
                        if (ifNum != 2) {
                            Opencoder[targetIndex] = 1;
                        }
                    }
                    else {Opencoder[targetIndex] = 1;}
                }
                else {
                    if (tobedone) {
                        if (ifNum == 1) {
                            Opencoder[targetIndex] = 2;
                        }
                        else {Opencoder[targetIndex] = monostate{};}
                    }
                    else {Opencoder[targetIndex] = 2;}
                }
            }
        }

        for (auto const& Tile : tiles) {
            window.draw(Tile);
        }

        for (float gap = 0.f; gap < 600.f; gap += 30.f) {
            Vline.setPosition({gap, 0.f});
            window.draw(Vline);
            if (gap < 300.f) {
                Hline.setPosition({0.f, gap});
                window.draw(Hline);
            }
        }

        for (int e = 0; e < 200; e++) {
            if (holds_alternative<int>(Opencoder[e])) {
                if (get<int>(Opencoder[e]) == 1) {
                    tiles[e].setFillColor(sf::Color::White);
                    if (holds_alternative<sf::RectangleShape>(Open[e])) {
                        get<sf::RectangleShape>(Open[e]).setPosition({tiles[e].getPosition().x + 8.f, tiles[e].getPosition().y + 8.f});
                        window.draw(get<sf::RectangleShape>(Open[e]));
                        window.draw(lose);
                        end = true;
                    }
                    else if (holds_alternative<sf::Text>(Open[e])) {
                        get<sf::Text>(Open[e]).setPosition({tiles[e].getPosition().x + 8.f, tiles[e].getPosition().y + 8.f});
                        window.draw(get<sf::Text>(Open[e]));
                    }
                    else {
                        minecounter(neighbourhood, check, e);
                        for (auto const& Value : neighbourhood) {
                            if (Value > -1 && Value < 200) {
                                Opencoder[Value] = 1;
                            }
                        }
                        neighbourhood.clear();
                    }
                }
                else if (get<int>(Opencoder[e]) == 2) {
                    sf::Vector2f truPos(tiles[e].getPosition());
                    Pole.setPosition({truPos.x + 6.f, truPos.y + 6.f});
                    Cloth.setPosition({truPos.x + 6.f, truPos.y + 6.f});
                    window.draw(Pole);
                    window.draw(Cloth);
                }
            }
        }

        for (int r = 0; r < 200; r++) {
            if (holds_alternative<monostate>(Opencoder[r])) {monoCount++;}
            else if (holds_alternative<int>(Opencoder[r]) && get<int>(Opencoder[r]) == 2 && holds_alternative<sf::RectangleShape>(Open[r])) {
                monoCount++;
            }
        }
        if (monoCount == 31 && (!end)) {
            window.draw(win);
            end = true;
        }
        monoCount = 0;

        window.display();
        if (end) {sf::sleep(sf::seconds(3)); return 0;}
    }
    return 0;
}