#include <SFML/Graphics.hpp>
#include <iostream>
#include <SFML/System.hpp>

int main() {
 
    using namespace std;

    sf::RenderWindow window(sf::VideoMode({400, 300}), "Atari's pong game: Modified");
    window.setFramerateLimit(60);

    float UNIy, UNy = 131.25f;
    float Ballx = 192.5f;
    float Bally = 146.5f;
    float xchange = 0.3f;
    float ychange = 1.5f;
    float sec;
    float DetectedY;

    sf::Font font;
    if (!font.openFromFile("C:/test_sfml/Roboto-VariableFont_wdth,wght.ttf")) {
        cerr << "Something went wrong." << endl;
        return -1;
    }

    sf::Text result1(font);
    result1.setString("You won!");
    result1.setCharacterSize(28);
    result1.setPosition({140.f, 110.f});

    sf::Text result2(font);
    result2.setString("You lost!");
    result2.setCharacterSize(28);
    result2.setPosition({140.f, 110.f});

    sf::RectangleShape paddle1({5.f, 37.5f});
    paddle1.setFillColor(sf::Color::Blue);
    paddle1.setPosition({2.f, UNIy});

    sf::RectangleShape paddle2({5.f, 37.5f});
    paddle2.setFillColor(sf::Color::Red);
    paddle2.setPosition({393.f, UNy});

    sf::RectangleShape ball({7.5f, 7.5f});
    ball.setFillColor(sf::Color::Yellow);
    ball.setPosition({Ballx, Bally});

    while (window.isOpen()) {
 
            while (const optional event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>())
                    window.close();
                }
    //we move the paddle
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        if (UNIy > 0.f) UNIy -= 5.f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        if (UNIy < 262.f) UNIy += 5.f;
    }
    paddle1.setPosition({2.f, UNIy});
    //ball moves
    Ballx -= xchange;
    Bally -= ychange;
    if ((Bally <= 0.f) || (Bally >= 300.f)) {
        ychange = ychange * -1.f;
        ball.setPosition({Ballx, Bally});
    }
    ball.setPosition({Ballx, Bally});
    //collisions
    if (Ballx <= 7.f) {
        if ((Bally >= UNIy) && (Bally <= (UNIy + 37.5))) {
            if (Bally < (UNIy + 18.75)) {
                xchange = xchange * -1;
                if (xchange < 0.f) {xchange -= 0.5f;}
                else {xchange += 0.5f;}
                ychange = ychange * -1;
            }
            else if (Bally == (UNIy + 18.75)) {
                xchange = xchange * -1;
            }
            else if (Bally > (UNIy + 18.75)) {
                xchange = xchange * -1;
                if (xchange < 0.f) {xchange -= 0.15f;}
                else {xchange += 0.15f;}
                ychange = ychange * -1;
            }
        }
    }
    if ((Ballx + 7.5f) >= 392.f) {
        if ((Bally >= UNy) && (Bally <= (UNy + 37.5))) {
            if (Bally < (UNy + 18.75)) {
                xchange = xchange * -1;
                if (xchange < 0.f) {xchange -= 0.5f;}
                else {xchange += 0.5f;}
                ychange = ychange * -1;
            }
            else if (Bally == (UNy + 18.75)) {
                xchange = xchange * -1;
            }
            else if (Bally > (UNy + 18.75)) {
                xchange = xchange * -1;
                if (xchange < 0.f) {xchange -= 0.15f;}
                else {xchange += 0.15f;}
                ychange = ychange * -1;
            }
        }
    }
    //winners!
    if (Ballx < 0.f) {
        window.draw(result2);
        window.display();
        sf::sleep(sf::seconds(3));
        break;}
    else if ((Ballx + 7.5f) > 401.f) {
        window.draw(result1);
        window.display();
        sf::sleep(sf::seconds(3));
        break;}
    //The dumbmaker and the computer
    DetectedY = Bally;
    if (DetectedY > UNy) {
        if (UNy <= 262.f) {UNy += 5.f;}
        paddle2.setPosition({393.f, UNy});}
    else if (DetectedY < UNy) {
        if (UNy >= 6.f) {UNy -= 5.f;}
        paddle2.setPosition({393.f, UNy});}
    sf::Clock Timer;
    sf::Time count = Timer.getElapsedTime();
    sec = count.asSeconds();
    if (sec == 6.f) {
        while (sec < 9.f) {
            if (UNy >= 6.f) {UNy -= 5.f;}
            paddle2.setPosition({393.f, UNy});
        }
    }
    if (sec >= 9.f) {
        count = Timer.restart();
    }
 
    window.clear(sf::Color::Black);
    window.draw(paddle1);
    window.draw(paddle2);
    window.draw(ball);
    window.display();
    }

    return 0;
}