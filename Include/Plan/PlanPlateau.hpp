/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** PlanPlateau.hpp
*/

#ifndef PLANPLATEAU_HPP
#define PLANPLATEAU_HPP

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "PawnRule.hpp" // ?

using namespace std;
using namespace sf;

class PlanPlateau
{
private:
    struct Tab
    {
        bool exist;
        PawnParam pawn;
    };

public:
    PlanPlateau();
    ~PlanPlateau();
    const string &getFileName() const;
    const Vector2u &getSize() const;
    const bool &getExist(Uint32 x, Uint32 y) const;
    const PawnParam &getPawn(Uint32 x, Uint32 y) const;
    void setFileName(const string &_fileName); // ?
    void setSize(const Vector2u &_size);
    void setExist(const Uint32 &x, const Uint32 &y, const bool &exist);
    void setPawn(const Uint32 &x, const Uint32 &y, const PawnParam &pawn);
    bool loadFromFile(const string &_fileName);

private:
    void destroy();

private:
    string fileName;
    Vector2u size;
    Tab **tab;
};

#endif
