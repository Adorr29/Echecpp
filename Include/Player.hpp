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

#define IP IpAddress::getLocalAddress() // ??
#define PORT 53000 // ?

class Player // ?
{
public:
    Player(const Color &_color);
    const Color &getColor() const;
    bool connectClient(const IpAddress &ip = IP, unsigned short port = PORT);
    bool connectServer(TcpListener &listener); // ?
    bool sendPlanPlateau(const PlanPlateau &planPlateau);
    bool sendYourTurn();
    bool sendMove(const Vector2u &pawnPos, const Vector2u &movePos);
    bool sendError(const Receiver::ErrorType &error);
    bool receive(Receiver &receiver);

private:
    Color color;
    TcpSocket socket;
};

#endif
