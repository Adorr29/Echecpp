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
#include <SFML/Network.hpp>
#include "PawnRule.hpp" // tmp

using namespace std;
using namespace sf;

class Plateau : public Drawable, public Transformable
{
private:
    enum BasicStatus : Uint32
    {
        None,
        My,
        Move,
        Eat
    };

    struct Tab
    {
        bool exist;
        PawnParam pawn;
        BasicStatus basicStatus;
        vector<Vector2u> dangerStatus;
        vector<Vector2u> revengeStatus;
    };

public:
    Plateau();
    Plateau(const Plateau &plateau);
    Plateau(const Packet &packet);
    ~Plateau();
    bool loadFromFile(const string &_fileName);
    Packet &loadFromPacket(Packet &packet);
    Packet &saveToPacket(Packet &packet) const;
    const Vector2u &getSize() const;
    bool checkMove(const Vector2u &pawnPos, const Vector2u &movePos) const;
    bool move(const Vector2u &pawnPos, const Vector2u &movePos);
    bool setStatus(const Vector2u &pawnPos);
    void cleanStatus();
    Vector2u convertMousePos(const Vector2i &mousePos) const;

private:
    Plateau(const Vector2u &_size);
    void draw(RenderTarget &target, RenderStates states) const;
    void setBasicStatus(const Vector2u &pawnPos, const vector<PawnRule::Direp> &direpTab, const BasicStatus &status);
    //void affBasicStatus() const;

public:
    IntRect affRect;
    Vector2u select;

private:
    string fileName;
    Vector2u size;
    Tab **tab;
};

Packet &operator<<(Packet &packet, const Plateau &plateau);
Packet &operator>>(Packet &packet, Plateau &plateau);

#endif
