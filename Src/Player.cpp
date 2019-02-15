/*
** EPITECH PROJECT, 2019
** for_norme
** File description:
** Player.cpp
*/

#include "Player.hpp"
#include "Helper.hpp"

Player::Player(const Color &_color)
    : color(_color)
{
}

const Color &Player::getColor() const
{
    return color;
}

bool Player::connectClient(const string &ip, unsigned short port)
{
    if (socket.connect(ip, port) != Socket::Done)
        return false;
    return true;
}

bool Player::connectServer(unsigned short port)
{
    TcpListener listener;

    if (listener.listen(port) != Socket::Done)
        return false;
    if (listener.accept(socket) != Socket::Done)
        return false;
    return true;
}

bool Player::sendYourTurn()
{
    Packet packet;

    if (!(packet << Receiver::YourTurn))
        return false;
    return socket.send(packet) == Socket::Done;
}

bool Player::sendMove(const Vector2u &pawnPos, const Vector2u &movePos)
{
    Packet packet;

    if (!(packet << Receiver::Move))
        return false;
    if (!(packet << pawnPos << movePos))
        return false;
    return socket.send(packet) == Socket::Done;
}

bool Player::sendError(const Receiver::ErrorType &error)
{
    Packet packet;

    if (!(packet << Receiver::Error))
        return false;
    if (!(packet << error))
        return false;
    return socket.send(packet) == Socket::Done;
}

bool Player::receive(Receiver &receiver)
{
    return receiveFormatted(socket, receiver);
}
