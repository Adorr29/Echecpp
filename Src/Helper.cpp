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
