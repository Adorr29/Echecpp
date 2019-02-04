/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** Helper.hpp
*/

#ifndef HELPER_HPP
#define HELPER_HPP

#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "PawnParam.hpp"

using namespace std;
using namespace sf;

string lastWordOfPath(const string &path);
Packet &operator<<(Packet &packet, const Color &color);
Packet &operator>>(Packet &packet, Color &color);
Packet &operator<<(Packet &packet, const PawnParam &pawnParam);
Packet &operator>>(Packet &packet, PawnParam &pawnParam);

template<typename T>
Packet &operator<<(Packet &packet, const Vector2<T> &vector)
{
    return packet << vector.x << vector.y;
}

template<typename T>
Packet &operator>>(Packet &packet, Vector2<T> &vector)
{
    return packet >> vector.x >> vector.y;
}

#endif
