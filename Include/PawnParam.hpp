/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** PawnParam.hpp
*/

#ifndef PAWNPARAM_HPP
#define PAWNPARAM_HPP

#include <string>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

enum Angle
{
    Up,
    Right,
    Down,
    Left
};

struct PawnParam
{
    string type;
    Color color;
    bool first;
    Angle angle;
};

#endif
