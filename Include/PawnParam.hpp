/*
** EPITECH PROJECT, 2019
** for_norme
** File description:
** PawnParam.hpp
*/

#ifndef PAWNPARAM_HPP
#define PAWNPARAM_HPP

#include <string>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;


struct PawnParam
{
public:
    enum Angle
    {
        Up,
        Right,
        Down,
        Left
    };

public:
    string type;
    Color color;
    bool first;
    Angle angle;
};

#endif
