/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** Plateau.hpp
*/

#ifndef PLATEAU_HPP
#define PLATEAU_HPP

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "PawnRule.hpp" // tmp

using namespace std;
using namespace sf;

class Plateau : public Drawable, public Transformable
{
private:
    enum BasicStatus
    {
        None,
        My,
        Move,
        Eat
    };
    struct Tab
    {
        PawnParam pawn;
        BasicStatus basicStatus;
        vector<Vector2u> dangerStatus;
        vector<Vector2u> revengeStatus;
    };

public:
    Plateau();
    Plateau(const Vector2u &_size); // add a config tab
    Plateau(const string &fileName);
    Plateau(const Plateau &plateau);
    ~Plateau();
    bool loadFromFile(const string &_fileName);
    const Vector2u &getSize() const;
    bool move(const Vector2u &pawnPos, const Vector2u &movePos);
    bool setStatus(const Vector2u &pawnPos);
    void cleanStatus();
    Vector2u convertMousePos(const Vector2i &mousePos) const;

private:
    void draw(RenderTarget &target, RenderStates states) const;
    //void affBasicStatus() const;

public:
    IntRect affRect;

private:
    string fileName;
    Vector2u size;
    Tab **tab;
};

#endif