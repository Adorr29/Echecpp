/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** PawnRule.hpp
*/

#ifndef PAWNRULE_HPP
#define PAWNRULE_HPP

#include <string>
#include <vector>
#include <map>
#include <glob.h>
#include <SFML/Graphics.hpp>
#include "PawnParam.hpp"

using namespace std;
using namespace sf;

class PawnRule
{
public:
    struct Direp
    {
        Vector2i dir;
        size_t rep;
        bool first;
    };

public:
    const vector<Direp> getMove(const PawnParam::Angle &angle = PawnParam::Angle::Down) const;
    const vector<Direp> getEat(const PawnParam::Angle &angle = PawnParam::Angle::Down) const;
    const string &getProperty() const;
    bool load(const string &path);

private:
    Direp rotate(const Direp &direp, const PawnParam::Angle &angle = PawnParam::Angle::Down) const;

private:
    vector<Direp> move;
    vector<Direp> eat;
    string property;
};

class Pawn
{
public:
    const string &getName() const;
    const PawnRule &getRule() const;
    const Texture &getTexture() const;
    bool load(const string &path);

private:
    string name;
    PawnRule rule;
    Texture texture;
};

class PawnMap
{
public:
    PawnMap(const string &path = "Resources/Pawn");
    const PawnRule *getPawnRule(const string &name) const;
    void reload(const string &path = "Resources/Pawn");
    bool aff(RenderTarget &target, const PawnParam &param, const FloatRect &rect) const;
    bool aff(RenderTarget &target, const vector<PawnParam> &paramList, const vector<FloatRect> &rectList) const;

private:
    map<string, Pawn> pawn;
};

extern PawnMap pawnMap; // tmp

#endif
