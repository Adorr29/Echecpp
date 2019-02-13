/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** Helper.cpp
*/

#include "Helper.hpp"

string lastWordOfPath(const string &path)
{
    string name;
    size_t offset = path.rfind("/");

    if (offset != string::npos) {
        name = path.substr(0, offset);
        offset = name.rfind("/");
        if (offset != string::npos)
            name = name.substr(offset + 1);
    }
    return (name);
}

Packet &operator<<(Packet &packet, const Color &color)
{
    return packet << color.r << color.g << color.b << color.a;
}

Packet &operator>>(Packet &packet, Color &color)
{
    return packet >> color.r >> color.g >> color.b >> color.a;
}

Packet &operator<<(Packet &packet, const PawnParam &pawnParam)
{
    return packet << pawnParam.type
                  << pawnParam.color
                  << pawnParam.first
                  << pawnParam.angle;
}

Packet &operator>>(Packet &packet, PawnParam &pawnParam)
{
    Uint32 tmp; // ??

    packet >> pawnParam.type
           >> pawnParam.color
           >> pawnParam.first
           >> tmp;
    pawnParam.angle = (Angle)tmp;
    return packet;
}
