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

const vector<PawnRule::Direp> &PawnRule::getMove() const
{
    return move;
}

const vector<PawnRule::Direp> &PawnRule::getEat() const
{
    return eat;
}

bool PawnRule::load(const string &path)
{
    ifstream file(path + "/Property");
    string line;

    if (!file)
        return false;
    move.clear();
    eat.clear();
    param = "";
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
            param += line + '|';
    /*cout << "name: " << path + "/Property" << endl;
    for (auto tmp : move)
        cout << "move: " << tmp.dir.x << " " << tmp.dir.y << " " << tmp.rep << endl;
    for (auto tmp : eat)
        cout << "eat: " << tmp.dir.x << " " << tmp.dir.y << " " << tmp.rep << endl;
        cout << "param: \"" << param << "\"" << endl << endl;*/
    return true;
}

/*void PawnRule::ee(const Vector2u &pos, const Plateau &plateau) const
{
    for (size_t i = 0; i < move.size(); i++)
        for (size_t j = 0; true; j++) {
            const Vector2u &size = plateau.getSize();
            Uint32 x = j * move[i].dir.x + pos.x;
            Uint32 y = j * move[i].dir.y + pos.y;

            if ((Uint32)(-move[j].dir.x) > pos.x || (Uint32)(-move[j].dir.y) > pos.y || x >= size.x || y >= size.y)
                break;
            //plateau[x][y] = MOVE;
            //plateau[x][y] = EAT;
        }
        }*/

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
    for (size_t i = 0; i < paramList.size(); i++)
        if (pawn.find(paramList[i].type) != pawn.end()) {
            const Texture &texture = pawn.at(paramList[i].type).getTexture();
            const IntRect whiteRect = IntRect(0, 0, texture.getSize().x / 2, texture.getSize().y);
            const IntRect blackRect = IntRect(texture.getSize().x / 2, 0, texture.getSize().x / 2, texture.getSize().y);

            rectangle.setTexture(&texture);
            rectangle.setPosition(rectList[i].left, rectList[i].top);
            rectangle.setSize(Vector2f(rectList[i].width, rectList[i].height));
            if (paramList[i].color == Color::Black)
                rectangle.setTextureRect(blackRect);
            else {
                rectangle.setTextureRect(whiteRect);
                rectangle.setFillColor(paramList[i].color);
            }
            target.draw(rectangle);
        }
        else
            result = false;
    return result;
}
