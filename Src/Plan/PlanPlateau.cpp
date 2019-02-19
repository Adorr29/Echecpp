/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** PlanPlateau.cpp
*/

#include <fstream>
#include "Plan/PlanPlateau.hpp"

PlanPlateau::PlanPlateau()
    : fileName(""), size(0, 0), tab(NULL)
{
}

PlanPlateau::~PlanPlateau()
{
    destroy();
}

const string &PlanPlateau::getFileName() const
{
    return fileName;
}

const Vector2u &PlanPlateau::getSize() const
{
    return size;
}

const bool &PlanPlateau::getExist(Uint32 x, Uint32 y) const
{
    return tab[x][y].exist;
}

const PawnParam &PlanPlateau::getPawn(Uint32 x, Uint32 y) const
{
    return tab[x][y].pawn;
}

void PlanPlateau::setFileName(const string &_fileName) // ?
{
    fileName = _fileName;
}

void PlanPlateau::setSize(const Vector2u &_size)
{
    size = _size;
    tab = new Tab* [size.x];
    for (Uint32 i = 0; i < size.x; i++)
        tab[i] = new Tab [size.y];
}

void PlanPlateau::setExist(const Uint32 &x, const Uint32 &y, const bool &exist)
{
    tab[x][y].exist = exist;
}

void PlanPlateau::setPawn(const Uint32 &x, const Uint32 &y, const PawnParam &pawn)
{
    tab[x][y].pawn = pawn;
}

bool PlanPlateau::loadFromFile(const string &_fileName)
{
    ifstream file(_fileName, ios::binary);

    if (!file)
        return false;
    fileName = _fileName;
    destroy();
    if (!file.read((char*)(&size), sizeof(Vector2u)))
        return false;
    //setSize(size); // ?
    tab = new Tab* [size.x];
    for (Uint32 i = 0; i < size.x; i++) {
        tab[i] = new Tab [size.y];
        for (Uint32 j = 0; j < size.y; j++) {
            if (!file.read((char*)(&tab[i][j].exist), sizeof(bool)))
                return false;
            tab[i][j].pawn.type = "";
            for (char c; file.read(&c, sizeof(char)) && c; tab[i][j].pawn.type += c);
            if (!file.read((char*)(&tab[i][j].pawn.color), sizeof(Color)))
                return false;
            if (!file.read((char*)(&tab[i][j].pawn.first), sizeof(bool)))
                return false;
            if (!file.read((char*)(&tab[i][j].pawn.angle), sizeof(Angle)))
                return false;
        }
    }
    return true;
}

void PlanPlateau::destroy()
{
    if (tab == NULL)
        return;
    for (Uint32 i = 0; i < size.x; i++)
        delete [] tab[i];
    delete [] tab;
}
