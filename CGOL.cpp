#include <SFML/Graphics.hpp>
#include <vector>

int main() {
    
    using namespace std;

    sf::RenderWindow window(sf::VideoMode({800, 480}), "Conway's Game Of Life: Recreation II");
    window.setFramerateLimit(60);
    
    int bx, by, indeks;
    vector<sf::RectangleShape> Cells;
    vector<int> neighbourhood, Hi, Bye;
    bool check = false;
    int NeighbourCount = 0;

    for (float row = 0.f; row < 24.f; row++) {
        for (float col = 0.f; col < 40.f; col++) {
            sf::RectangleShape cell({20.f, 20.f});
            cell.setPosition({col * 20.f, row *20.f});
            cell.setFillColor(sf::Color::White);
            Cells.push_back(cell);
        }
    }

    sf::RectangleShape Hline({800.f, 2.f});
    Hline.setFillColor(sf::Color::Black);
    sf::RectangleShape Vline({2.f, 480.f});
    Vline.setFillColor(sf::Color::Black);

    while (window.isOpen()) {

        window.clear(sf::Color::White);

        while (const optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* KeyPress = event->getIf<sf::Event::KeyPressed>()) {
                if (KeyPress->code == sf::Keyboard::Key::L) {
                    for (int index = 0; index < 960; index++) {
                        if (index % 40 == 0) {
                            neighbourhood = {            index - 40, index - 39, 
                                                                     index + 1, 
                                                         index + 40, index + 41};
                        }
                        else {
                            for (int i = 1; i < 25; i++) {
                                if (index == ((40 * i) - 1)) {
                                    neighbourhood = {index - 41, index - 40, 
                                                     index - 1, 
                                                     index + 39, index + 40            };
                                    check = true;
                                }
                            }
                            if (check == false) {
                                    neighbourhood = {index - 41, index - 40, index - 39, 
                                                     index - 1,              index + 1, 
                                                     index + 39, index + 40, index + 41};
                                }
                        }
                        for (auto const& neighbours : neighbourhood) {
                            if ((neighbours > -1) && (neighbours < 960)) {
                                if (Cells[neighbours].getFillColor() == sf::Color::Black) {NeighbourCount++;}
                            }
                        }
                        check = false;
                        if ((NeighbourCount < 2) || (NeighbourCount > 3)) {
                            Bye.push_back(index);
                        }
                        else if (NeighbourCount == 3) {
                            Hi.push_back(index);
                        }
                        NeighbourCount = 0;
                    }
                    for (auto const& Values : Bye) {
                        Cells[Values].setFillColor(sf::Color::White);
                    }
                    for (auto const& values : Hi) {
                        Cells[values].setFillColor(sf::Color::Black);
                    }
                    Bye.clear();
                    Hi.clear();
                }
            }
            if (const auto* MousePOS = event->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f PosMouse = window.mapPixelToCoords(MousePOS->position);
                bx = PosMouse.x / 20;
                by = PosMouse.y / 20;
                indeks = (40 * by) + bx;
                if (MousePOS->button == sf::Mouse::Button::Left) {
                    Cells[indeks].setFillColor(sf::Color::Black);
                }
                if (MousePOS->button == sf::Mouse::Button::Right) {
                    Cells[indeks].setFillColor(sf::Color::White);
                }
            }
        }

        for (auto const& cell : Cells) {
            window.draw(cell);
        }
        for (float xpos = 0.f; xpos < 820.f; xpos += 20.f) {
            Vline.setPosition({xpos, 0.f});
            window.draw(Vline);
        }
        for (float ypos = 0.f; ypos < 500.f; ypos += 20.f) {
            Hline.setPosition({0.f, ypos});
            window.draw(Hline);
        }
        window.display();
    }

    return 0;
}