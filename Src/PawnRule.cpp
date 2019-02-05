/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** PawnRule.hpp
*/

#include <sstream>
#include <fstream>
#include "PawnRule.hpp"
#include "Helper.hpp"

const vector<PawnRule::Direp> PawnRule::getMove(const Angle &angle) const
{
    vector<Direp> moveRotate;

    for (const Direp &direp : move)
        moveRotate.push_back(rotate(direp, angle));
    return moveRotate;
}

const vector<PawnRule::Direp> PawnRule::getEat(const Angle &angle) const
{
    vector<Direp> eatRotate;

    for (const Direp &direp : eat)
        eatRotate.push_back(rotate(direp, angle));
    return eatRotate;
}

const string &PawnRule::getProperty() const
{
    return property;
}

PawnRule::Direp PawnRule::rotate(const PawnRule::Direp &direp, const Angle &angle) const
{
    PawnRule::Direp rotatDirep = direp;

    // default is Down
    if (angle == Angle::Up) {
        rotatDirep.dir.x = -direp.dir.x;
        rotatDirep.dir.y = -direp.dir.y;
    }
    else if (angle == Angle::Right) {
        rotatDirep.dir.x = direp.dir.y;
        rotatDirep.dir.y = -direp.dir.x;
    }
    else if (angle == Angle::Left) {
        rotatDirep.dir.x = -direp.dir.y;
        rotatDirep.dir.y = direp.dir.x;
    }
    return rotatDirep;
}

bool PawnRule::load(const string &path)
{
    ifstream file(path + "/Property");
    string line;

    if (!file)
        return false;
    move.clear();
    eat.clear();
    property = "";
    while (getline(file, line))
        if (line.find(':') != string::npos) {
            istringstream param(line.substr(line.find(':') + 1));
            string type(line.substr(0, line.find(':')));
            Direp direp;

            if (!(param >> direp.dir.x))
                direp.dir.x = 0;
            if (!(param >> direp.dir.y))
                direp.dir.y = 0;
            if (!(param >> direp.rep))
                direp.rep = 0;
            direp.first = type.find("first") != string::npos;
            if (type.find("move") != string::npos)
                move.push_back(direp);
            if (type.find("eat") != string::npos)
                eat.push_back(direp);
        }
        else
            property += line + '|';
    return true;
}

const string &Pawn::getName() const
{
    return name;
}

const PawnRule &Pawn::getRule() const
{
    return rule;
}

const Texture &Pawn::getTexture() const
{
    return texture;
}

bool Pawn::load(const string &path)
{
    name = lastWordOfPath(path);
    if (!rule.load(path))
        return false;
    if (!texture.loadFromFile(path + "/Texture.png"))
        return false;
    return true;
}

PawnMap::PawnMap(const string &path)
{
    reload(path);
}

const PawnRule *PawnMap::getPawnRule(const string &name) const
{
    if (pawn.find(name) == pawn.end())
        return nullptr;
    return &pawn.at(name).getRule();
}

void PawnMap::reload(const string &path)
{
    const string fullPath(path + "/*");
    glob_t pglob;

    pawn.clear();
    if (glob(fullPath.c_str(), GLOB_MARK, NULL, &pglob) == 0)
        for (size_t i = 0; i < pglob.gl_pathc; i++) {
            string pawnPath(pglob.gl_pathv[i]);
            Pawn newPawn;

            if (pawnPath[pawnPath.size() - 1] == '/' && newPawn.load(pawnPath))
                pawn.emplace(lastWordOfPath(pawnPath), newPawn);
        }
    globfree(&pglob);
}

bool PawnMap::aff(RenderTarget &target, const PawnParam &param, const FloatRect &rect) const
{
    return aff(target, vector<PawnParam>{param}, {rect});
}

bool PawnMap::aff(RenderTarget &target, const vector<PawnParam> &paramList, const vector<FloatRect> &rectList) const
{
    RectangleShape rectangle;
    bool result = true;

    if (paramList.size() != rectList.size())
        return false;
    for (size_t i = 0; i < paramList.size(); i++) {
        if (pawn.find(paramList[i].type) == pawn.end()) {
            result = false;
            continue;
        }
        const Texture &texture = pawn.at(paramList[i].type).getTexture();
        const IntRect whiteRect = IntRect(0, 0, texture.getSize().x / 2, texture.getSize().y);
        const IntRect blackRect = IntRect(texture.getSize().x / 2, 0, texture.getSize().x / 2, texture.getSize().y);
        Vector2f pos(rectList[i].left, rectList[i].top);

        rectangle.setTexture(&texture);
        rectangle.setPosition(pos);
        rectangle.setSize(Vector2f(rectList[i].width, rectList[i].height));
        if (paramList[i].color == Color::Black)
            rectangle.setTextureRect(blackRect);
        else {
            rectangle.setTextureRect(whiteRect);
            rectangle.setFillColor(paramList[i].color);
        }
        target.draw(rectangle);
    }
    return result;
}
