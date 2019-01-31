//
// EPITECH PROJECT, 2018
// Echec++
// File description:
// Main.cpp
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "Plateau.hpp"
#include "PawnRule.hpp"

using namespace std;
using namespace sf;

PawnMap pawnMap = PawnMap();

int main(void)
{
    RenderWindow window(VideoMode::getDesktopMode(), "Echec", Style::Fullscreen);
    //Pawn pawn("Resources/Pawn/Rook");
    Plateau plateau;

    window.setFramerateLimit(30);
    while (window.isOpen()) {
        for (Event event; window.pollEvent(event);) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::MouseMoved) {
                Vector2u pos = plateau.convertMousePos(Vector2i(event.mouseMove.x, event.mouseMove.y));
                if (pos != plateau.getSize())
                    plateau.setStatus(pos);
            }
        }
        window.clear();
        window.draw(plateau);
        window.display();
    }
    return 0;
}
