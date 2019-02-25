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
#include <Plan/PlanPlateau.hpp>
#include "PawnRule.hpp"

using namespace std;
using namespace sf;

class Plateau : public Drawable, public Transformable
{
public:
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
    Plateau(const PlanPlateau &planPlateau);
    Plateau(const Plateau &plateau);
    ~Plateau();
    bool saveToFile() const;
    bool saveToFile(const string &_fileName);
    PlanPlateau getPlan() const;
    const Tab &getTab(const Uint32 x, const Uint32 y) const;
    const Vector2u &getSize() const;
    bool checkMove(const Vector2u &pawnPos, const Vector2u &movePos) const;
    bool move(const Vector2u &pawnPos, const Vector2u &movePos);
    bool setStatus(const Vector2u &pawnPos, const bool &basicOnly = false); // ?
    void cleanStatus();
    Vector2u convertMousePos(const Vector2i &mousePos) const;

private:
    Plateau(const Vector2u &_size);
    void draw(RenderTarget &target, RenderStates states) const;
    void affBasicStatus(RenderTarget &target, RenderStates &states) const;
    void affAdvancedStatus(RenderTarget &target, RenderStates &states, const vector<Vector2u> &advancedStatus, const Color &color) const;
    void moveNoCheck(const Vector2u &pawnPos, const Vector2u &movePos);
    void setBasicStatus(const Vector2u &pawnPos, const vector<PawnRule::Direp> &direpTab, const BasicStatus &status);
    void setDangerStatus(const Vector2u &pawnPos, const Vector2u &movePos);
    void setRevengeStatus(const Vector2u &pawnPos, const Vector2u &movePos);

public:
    PawnMap *pawnMap;
    IntRect affRect;
    Vector2u select;
    Vector2u info;

private:
    string fileName;
    Vector2u size;
    Tab **tab;
};

#endif
