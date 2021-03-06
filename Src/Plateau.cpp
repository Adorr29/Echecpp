/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** Plateau.cpp
*/

#include <fstream>
#include "Plateau.hpp"
#include "Helper.hpp"

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
        for (Uint32 j = 0; j < size.y; j++) {
            tab[i][j].exist = true;
            tab[i][j].pawn.color = j < size.y / 2 ? Color::Black : Color::White;
            tab[i][j].pawn.first = true;
            tab[i][j].pawn.angle = j < size.y / 2 ? Angle::Down : Angle::Up;
        }
}

Plateau::Plateau(const PlanPlateau &planPlateau)
    : Plateau(planPlateau.getSize())
{
    for (Uint32 i = 0; i < size.x; i++)
        for (Uint32 j = 0; j < size.y; j++) {
            tab[i][j].exist = planPlateau.getExist(i, j);
            tab[i][j].pawn = planPlateau.getPawn(i, j);
        }
    fileName = planPlateau.getFileName();
}

Plateau::Plateau(const Plateau &plateau)
    : Plateau(plateau.size)
{
    pawnMap = plateau.pawnMap;
    affRect = plateau.affRect;
    select = plateau.select; // ?
    info = plateau.info; // ?
    for (Uint32 i = 0; i < size.x; i++)
        for (Uint32 j = 0; j < size.y; j++) {
            tab[i][j].exist = plateau.tab[i][j].exist;
            tab[i][j].pawn = plateau.tab[i][j].pawn;
        }
    fileName = plateau.fileName;
}

Plateau::~Plateau()
{
    for (Uint32 i = 0; i < size.x; i++)
        delete [] tab[i];
    delete [] tab;
}

bool Plateau::saveToFile() const
{
    ofstream file(fileName, ios::binary);

    if (!(file.write((char*)&size, sizeof(Vector2u))))
        return false;
    for (Uint32 i = 0; i < size.x; i++)
        for (Uint32 j = 0; j < size.y; j++) {
            if (!(file.write((char*)&tab[i][j].exist, sizeof(bool)))) // ?
                return false;
            /*if (!(file.write((char*)&tab[i][j].pawn, sizeof(PawnParam)))) // ?
              return false;*/
            if (!(file.write(tab[i][j].pawn.type.c_str(), sizeof(char) * (tab[i][j].pawn.type.size() + 1)))) // ?
                return false;
            if (!(file.write((char*)&tab[i][j].pawn.color, sizeof(Color)))) // ?
                return false;
            if (!(file.write((char*)&tab[i][j].pawn.first, sizeof(bool)))) // ?
                return false;
            if (!(file.write((char*)&tab[i][j].pawn.angle, sizeof(Angle)))) // ?
                return false;
        }
    return true;
}

bool Plateau::saveToFile(const string &_fileName)
{
    fileName = _fileName;
    return saveToFile();
}

PlanPlateau Plateau::getPlan() const
{
    PlanPlateau planPlateau;

    planPlateau.setSize(size);
    for (Uint32 i = 0; i < size.x; i++)
        for (Uint32 j = 0; j < size.y; j++) {
            planPlateau.setExist(i, j, tab[i][j].exist);
            planPlateau.setPawn(i, j, tab[i][j].pawn);
        }
    planPlateau.setFileName(fileName);
    return planPlateau;
}

const Plateau::Tab &Plateau::getTab(const Vector2u &pos) const
{
    //if (x >= size.x || y >= size.y) // TODO
    return tab[pos.x][pos.y];
}

const Vector2u &Plateau::getSize() const
{
    return size;
}

bool Plateau::checkMove(const Vector2u &pawnPos, const Vector2u &movePos) const
{
    if (pawnPos.x >= size.x || pawnPos.y >= size.y || movePos.x >= size.x || movePos.y >= size.y)
        return false;
    if (!tab[pawnPos.x][pawnPos.y].exist || !tab[movePos.x][movePos.y].exist)
        return false;
    if (pawnPos == movePos || tab[pawnPos.x][pawnPos.y].pawn.type == "")
        return false;
    if (tab[pawnPos.x][pawnPos.y].basicStatus != BasicStatus::My)
        return false;
    if (tab[movePos.x][movePos.y].basicStatus != BasicStatus::Move && tab[movePos.x][movePos.y].basicStatus != BasicStatus::Eat)
        return false;
    return true;
}

bool Plateau::move(const Vector2u &pawnPos, const Vector2u &movePos)
{
    if (!checkMove(pawnPos, movePos))
        return false;
    moveNoCheck(pawnPos, movePos);
    return true;
}

bool Plateau::setStatus(const Vector2u &pawnPos, const bool &basicOnly) // ?
{
    const PawnRule *pawnRule;
    Angle angle;

    if (pawnPos.x >= size.x || pawnPos.y >= size.y)
        return false;
    if (!tab[pawnPos.x][pawnPos.y].exist)
        return false;
    cleanStatus();
    if (!pawnMap)
        return false;
    pawnRule = pawnMap->getPawnRule(tab[pawnPos.x][pawnPos.y].pawn.type);
    if (!pawnRule) {
        tab[pawnPos.x][pawnPos.y].basicStatus = BasicStatus::My;
        return false;
    }
    angle = tab[pawnPos.x][pawnPos.y].pawn.angle;
    setBasicStatus(pawnPos, pawnRule->getMove(angle), BasicStatus::Move);
    setBasicStatus(pawnPos, pawnRule->getEat(angle), BasicStatus::Eat);
    tab[pawnPos.x][pawnPos.y].basicStatus = BasicStatus::My;
    if (basicOnly)
        return true;
    for (Uint32 i = 0; i < size.x; i++)
        for (Uint32 j = 0; j < size.y; j++)
            if (tab[i][j].basicStatus) {
                setDangerStatus(pawnPos, Vector2u(i, j)); // to move in setBasicStatus for opti
                if (pawnRule->isProperty("royalty"))
                    if (tab[i][j].basicStatus != BasicStatus::My && tab[i][j].dangerStatus.size()) {
                        tab[i][j].basicStatus = BasicStatus::None;
                        tab[i][j].dangerStatus.clear();
                    }
                // TODO
            }
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

Plateau::Plateau(const Vector2u &_size)
    : pawnMap(NULL), affRect(IntRect(480, 60, 960, 960)), select(_size), info(_size), size(_size)
{
    tab = new Tab* [size.x];
    for (Uint32 i = 0; i < size.x; i++)
        tab[i] = new Tab [size.y];
    cleanStatus();
}

void Plateau::draw(RenderTarget &target, RenderStates states) const
{
    const Vector2f rectSize(affRect.width / size.x, affRect.height / size.y);
    const Uint32 outlineThickness = rectSize.x / 60;
    RectangleShape rectangle(rectSize);
    RectangleShape infoRect(Vector2f(rectSize.x * 0.15, rectSize.y * 0.15));

    states.transform *= getTransform();
    infoRect.setOutlineThickness(outlineThickness);
    infoRect.setOrigin(infoRect.getSize().x / 2, infoRect.getSize().y / 2);
    for (Uint32 j = 0; j < size.y; j++)
        for (Uint32 i = 0; i < size.x; i++)
            if (tab[i][j].exist) {
                Vector2f rectPos(rectSize.x * i, rectSize.y * j);
                Color infoColor;

                rectPos.x += affRect.left;
                rectPos.y += affRect.top;
                rectangle.setPosition(rectPos);
                rectangle.setFillColor((i + j) % 2 ? Color(59, 59, 59) : Color(207, 207, 207));
                target.draw(rectangle, states);
                if (pawnMap)
                    pawnMap->aff(target, tab[i][j].pawn, FloatRect(rectPos.x, rectPos.y, rectSize.x, rectSize.y));
                if (tab[i][j].dangerStatus.size()) { // basicOnly ?
                    infoColor = Color::Red;
                    rectPos += Vector2f(rectSize.x * 0.2, rectSize.y * 0.2);
                }
                if (tab[i][j].revengeStatus.size()) { // basicOnly ?
                    infoColor = Color::Blue;
                    rectPos += Vector2f(rectSize.x * 0.8, rectSize.y * 0.2);
                }
                if (tab[i][j].dangerStatus.size() || tab[i][j].revengeStatus.size()) { // basicOnly ?
                    infoRect.setPosition(rectPos);
                    infoRect.setOutlineColor(infoColor);
                    infoRect.setFillColor(Color(infoColor.r, infoColor.g, infoColor.b, infoColor.a / 10));
                    target.draw(infoRect, states);
                }
            }
    affBasicStatus(target, states);
    //if (basicOnly) return; // TODO // ?
    if (info.x >= size.x && info.y >= size.y)
        return;
    affAdvancedStatus(target, states, tab[info.x][info.y].dangerStatus, Color::Red);
    affAdvancedStatus(target, states, tab[info.x][info.y].revengeStatus, Color::Blue);
}

void Plateau::affBasicStatus(RenderTarget &target, RenderStates &states) const
{
    const Vector2f rectSize(affRect.width / size.x, affRect.height / size.y);
    const Uint32 outlineThickness = rectSize.x / 60;
    RectangleShape rectangle(Vector2f(rectSize.x - outlineThickness * 2, rectSize.y - outlineThickness * 2));

    states.transform *= getTransform();
    for (Uint32 j = 0; j < size.y; j++)
        for (Uint32 i = 0; i < size.x; i++)
            if (tab[i][j].exist) {
                Vector2f rectPos(rectSize.x * i + outlineThickness, rectSize.y * j + outlineThickness);
                Color color = Color::Transparent;

                rectPos.x += affRect.left;
                rectPos.y += affRect.top;
                rectangle.setPosition(rectPos);
                if (tab[i][j].basicStatus == BasicStatus::My)
                    color = Color::Blue;
                else if (tab[i][j].basicStatus == BasicStatus::Move)
                    color = Color::Green;
                else if (tab[i][j].basicStatus == BasicStatus::Eat)
                    color = Color::Red;
                rectangle.setOutlineThickness(outlineThickness);
                rectangle.setOutlineColor(color);
                rectangle.setFillColor(Color(color.r, color.g, color.b, color.a / 10));
                target.draw(rectangle, states);
            }
}

void Plateau::affAdvancedStatus(RenderTarget &target, RenderStates &states, const vector<Vector2u> &advancedStatus, const Color &color) const
{
    const Vector2f rectSize(affRect.width / size.x, affRect.height / size.y);
    RectangleShape rectangle(Vector2f(rectSize.x * 0.75, rectSize.y * 0.75));
    const Uint32 outlineThickness = rectSize.x / 60;

    states.transform *= getTransform();
    rectangle.setOutlineThickness(outlineThickness);
    rectangle.setOutlineColor(color);
    rectangle.setFillColor(Color(color.r, color.g, color.b, color.a / 10));
    rectangle.setOrigin(rectangle.getSize().x / 2, rectangle.getSize().y / 2);
    rectangle.rotate(45);
    for (const Vector2u &statusPos : advancedStatus) {
        Vector2f rectPos(rectSize.x * statusPos.x, rectSize.y * statusPos.y);

        rectPos.x += affRect.left + rectSize.x * 0.5;
        rectPos.y += affRect.top + rectSize.y * 0.5;
        rectangle.setPosition(rectPos);
        target.draw(rectangle, states);
    }
}

void Plateau::moveNoCheck(const Vector2u &pawnPos, const Vector2u &movePos)
{
    tab[movePos.x][movePos.y].pawn = tab[pawnPos.x][pawnPos.y].pawn;
    tab[movePos.x][movePos.y].pawn.first = false;
    if (pawnPos != movePos)
        tab[pawnPos.x][pawnPos.y].pawn.type = "";
    if (pawnPromotion(movePos))
        tab[movePos.x][movePos.y].pawn.type = "Queen"; // tmp
}

bool Plateau::pawnPromotion(const Vector2u &pawnPos) const
{
    const PawnRule *pawnRule = getPawnRule(pawnPos);
    bool further = false;

    if (!pawnRule || !pawnRule->isProperty("Promotion"))
        return false;
    if (tab[pawnPos.x][pawnPos.y].pawn.angle == Angle::Left && pawnPos.x == 0)
        further = true;
    else if (tab[pawnPos.x][pawnPos.y].pawn.angle == Angle::Up && pawnPos.y == 0)
        further = true;
    else if (tab[pawnPos.x][pawnPos.y].pawn.angle == Angle::Right && pawnPos.x == size.x - 1)
        further = true;
    else if (tab[pawnPos.x][pawnPos.y].pawn.angle == Angle::Down && pawnPos.y == size.y - 1)
        further = true;
    else if (!tab[pawnPos.x][pawnPos.y].exist)
        further = true;
    return further;
}

const PawnRule *Plateau::getPawnRule(const Vector2u &pawnPos) const
{
    if (!pawnMap)
        return NULL;
    return pawnMap->getPawnRule(tab[pawnPos.x][pawnPos.y].pawn.type);
}

void Plateau::setBasicStatus(const Vector2u &pawnPos, const vector<PawnRule::Direp> &direpTab, const BasicStatus &status)
{
    for (const PawnRule::Direp &direp : direpTab) {
        if (direp.first && !tab[pawnPos.x][pawnPos.y].pawn.first)
            continue;
        for (size_t i = 1; direp.rep ? i <= direp.rep : true; i++) {
            Vector2u pos(i * direp.dir.x + pawnPos.x, i * direp.dir.y + pawnPos.y);

            if (direp.dir.x < 0 && (Uint32)(-direp.dir.x) > pawnPos.x)
                break;
            if (direp.dir.y < 0 && (Uint32)(-direp.dir.y) > pawnPos.y)
                break;
            if (pos.x >= size.x || pos.y >= size.y)
                break;
            if (!tab[pos.x][pos.y].exist)
                break;
            if (tab[pos.x][pos.y].pawn.type != "") {
                if (status == BasicStatus::Eat)
                    if (tab[pos.x][pos.y].pawn.color != tab[pawnPos.x][pawnPos.y].pawn.color)
                        tab[pos.x][pos.y].basicStatus = status;
                break;
            }
            if (status == BasicStatus::Move)
                tab[pos.x][pos.y].basicStatus = status;
        }
    }
}

void Plateau::setDangerStatus(const Vector2u &pawnPos, const Vector2u &movePos)
{
    Plateau plateau(*this);

    plateau.moveNoCheck(pawnPos, movePos);
    for (Uint32 i = 0; i < plateau.size.x; i++)
        for (Uint32 j = 0; j < plateau.size.y; j++)
            if (plateau.tab[i][j].pawn.type != "" && plateau.tab[i][j].pawn.color != plateau.tab[movePos.x][movePos.y].pawn.color) {
                plateau.setStatus(Vector2u(i, j), true); // !
                if (plateau.tab[movePos.x][movePos.y].basicStatus == BasicStatus::Eat)
                    tab[movePos.x][movePos.y].dangerStatus.push_back(Vector2u(i, j));
            }
}

void setRevengeStatus(const Vector2u &pawnPos, const Vector2u &movePos)
{
    (void)pawnPos;
    (void)movePos;
    // TODO ?
}
