/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** Plateau.cpp
*/

#include <fstream>
#include "Plateau.hpp"

Plateau::Plateau()
    : Plateau(Vector2u(8, 8))
{
    tab[0][0].pawn.type = "Rook";
    tab[1][0].pawn.type = "Knight";
    tab[2][0].pawn.type = "Bishop";
    for (Uint32 i = 0; i < size.x / 2; i++)
        tab[i][1].pawn.type = "Pawn";
    for (Uint32 i = 0; i < size.x / 2; i++)
        for (Uint32 j = 0; j < size.y; j++)
            tab[size.x - 1 - i][j].pawn.type = tab[i][j].pawn.type;
    tab[3][0].pawn.type = "Queen";
    tab[4][0].pawn.type = "King";
    for (Uint32 i = 0; i < size.x; i++)
        for (Uint32 j = 0; j < size.y / 2; j++)
            tab[i][size.y - 1 - j].pawn.type = tab[i][j].pawn.type;
    for (Uint32 i = 0; i < size.x; i++)
        for (Uint32 j = 0; j < size.y; j++)
            tab[i][j].pawn.color = j < size.y / 2 ? Color::Black : Color::White;
}

Plateau::Plateau(const Vector2u &_size)
    : affRect(IntRect(480, 60, 960, 960)), size(_size)
{
    tab = new Tab* [size.x];
    for (Uint32 i = 0; i < size.x; i++)
        tab[i] = new Tab [size.y];
}

Plateau::Plateau(const Plateau &plateau)
    : Plateau(plateau.size)
{
    for (Uint32 i = 0; i < size.x; i++)
        for (Uint32 j = 0; j < size.y; j++)
            tab[i][j].pawn = plateau.tab[i][j].pawn;
}

Plateau::~Plateau()
{
    for (Uint32 i = 0; i < size.x; i++)
        delete [] tab[i];
    delete [] tab;
}

bool Plateau::loadFromFile(const string &_fileName)
{
    ifstream file(_fileName, ios::binary);

    if (!file)
        return false;
    if (!(file.read((char*)&size, sizeof(Vector2u))))
        return false;
    for (Uint32 i = 0; i < size.x; i++)
        for (Uint32 j = 0; j < size.y; j++)
            if (!(file.read((char*)&size, sizeof(Vector2u))))
                return false;
    fileName = _fileName;
    return true;
}

const Vector2u &Plateau::getSize() const
{
    return size;
}

bool Plateau::move(const Vector2u &pawnPos, const Vector2u &movePos)
{
    if (pawnPos.y >= size.x || pawnPos.y >= size.y || movePos.y >= size.x || movePos.y >= size.y)
        return false;
    if (tab[pawnPos.x][pawnPos.y].pawn.type == "")
        return false;
    tab[movePos.x][movePos.y].pawn = tab[pawnPos.x][pawnPos.y].pawn;
    tab[movePos.x][movePos.y].pawn.first = false;
    return true;
}

bool Plateau::setStatus(const Vector2u &pawnPos)
{
    const PawnRule *pawnRule;

    if (pawnPos.x >= size.x || pawnPos.y >= size.y)
        return false;
    pawnRule = pawnMap.getPawnRule(tab[pawnPos.x][pawnPos.y].pawn.type);
    if (!pawnRule)
        return false;
    cleanStatus();
    for (const PawnRule::Direp &move : pawnRule->getMove())
        for (size_t i = 1; move.rep ? i <= move.rep : true; i++) {
            Vector2u pos(i * move.dir.x + pawnPos.x, i * move.dir.y + pawnPos.y);

            if (move.dir.x < 0 && (Uint32)(-move.dir.x) > pawnPos.x)
                break;
            if (move.dir.y < 0 && (Uint32)(-move.dir.y) > pawnPos.y)
                break;
            if (pos.x >= size.x || pos.y >= size.y)
                break;
            tab[pos.x][pos.y].basicStatus = BasicStatus::Move;
        }
    for (const PawnRule::Direp &eat : pawnRule->getEat())
        for (size_t i = 1; eat.rep ? i <= eat.rep : true; i++) {
            Vector2u pos(i * eat.dir.x + pawnPos.x, i * eat.dir.y + pawnPos.y);

            if (eat.dir.x < 0 && (Uint32)(-eat.dir.x) > pawnPos.x)
                break;
            if (eat.dir.y < 0 && (Uint32)(-eat.dir.y) > pawnPos.y)
                break;
            if (pos.x >= size.x || pos.y >= size.y)
                break;
            tab[pos.x][pos.y].basicStatus = BasicStatus::Eat;
        }
    tab[pawnPos.x][pawnPos.y].basicStatus = BasicStatus::My;
    return true;
}

void Plateau::cleanStatus()
{
    for (Uint32 j = 0; j < size.y; j++)
        for (Uint32 i = 0; i < size.x; i++) {
            tab[i][j].basicStatus = BasicStatus::None;
            tab[i][j].dangerStatus.clear();
            tab[i][j].revengeStatus.clear();
        }
}

Vector2u Plateau::convertMousePos(const Vector2i &mousePos) const
{
    Vector2u pos;

    if (mousePos.x < affRect.left || mousePos.y < affRect.top)
        return size;
    if (mousePos.x >= affRect.left + affRect.width || mousePos.y >= affRect.top + affRect.height)
        return size;
    pos.x = (mousePos.x - affRect.left) / (affRect.width / size.x);
    pos.y = (mousePos.y - affRect.top) / (affRect.height / size.y);
    return pos;
}

void Plateau::draw(RenderTarget &target, RenderStates states) const
{
    Vector2f rectSize(affRect.width / size.x, affRect.height / size.y);
    RectangleShape rectangle(rectSize);
    const Uint32 outlineThickness = rectSize.x / 60;
    RectangleShape statusRect(Vector2f(rectSize.x - outlineThickness * 2, rectSize.y - outlineThickness * 2));

    states.transform *= getTransform();
    for (Uint32 j = 0; j < size.y; j++)
        for (Uint32 i = 0; i < size.x; i++) {
            Vector2f rectPos(rectSize.x * i, rectSize.y * j);
            Color statusColor = Color::Transparent;

            rectPos.x += affRect.left;
            rectPos.y += affRect.top;
            rectangle.setPosition(rectPos);
            rectangle.setFillColor((i + j) % 2 ? Color(60, 60, 60) : Color(210, 210, 210));
            target.draw(rectangle, states);
            pawnMap.aff(target, tab[i][j].pawn, FloatRect(rectPos.x, rectPos.y, rectSize.x, rectSize.y));
            statusRect.setPosition(Vector2f(rectPos.x + outlineThickness, rectPos.y + outlineThickness));
            if (tab[i][j].basicStatus == BasicStatus::My)
                statusColor = Color::Blue;
            else if (tab[i][j].basicStatus == BasicStatus::Move)
                statusColor = Color::Green;
            else if (tab[i][j].basicStatus == BasicStatus::Eat)
                statusColor = Color::Red;
            statusRect.setOutlineThickness(outlineThickness);
            statusRect.setOutlineColor(statusColor);
            statusRect.setFillColor(Color(statusColor.r, statusColor.g, statusColor.b, statusColor.a / 10));
            target.draw(statusRect, states);
        }
}
