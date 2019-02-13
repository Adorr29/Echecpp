/*
** EPITECH PROJECT, 2019
** for_norme
** File description:
** Player.hpp
*/

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "Receiver.hpp"

using namespace std;
using namespace sf;

#define PORT 53000 // ?

class Player
{
public:
    Player(const string &_type, const string &_name, const Color &_color);
    const string &getType() const;
    const string &getName() const;
    const Color &getColor() const;
    bool connectClient(const string &ip, unsigned short port = PORT);
    bool connectServer(unsigned short port = PORT);
    bool sendTurn(const string &name);
    bool sendMove(const Vector2u &pawnPos, const Vector2u &movePos);
    bool sendError(const Receiver::ErrorType &error);
    bool receive(Receiver &receiver);

private:
    string type;
    string name;
    Color color;
    TcpSocket socket;
};

#endif
