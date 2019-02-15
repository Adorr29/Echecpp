/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** Receiver.hpp
*/

#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
//#include "PawnRule.hpp"
//#include "Plateau.hpp"

using namespace std;
using namespace sf;

class Receiver
{
public:
    //Receiver() {} // ??
    //~Receiver() {} // ??

    struct PlateauReceive
    {
        // TODO
    };

    struct PlayerReceive
    {
        // TODO
    };

    struct PawnReceive
    {
        // TODO
    };

    struct MoveReceive
    {
        Vector2u pawnPos;
        Vector2u movePos;
    };

    enum ErrorType : Uint32
    {
        Ok,
        IllegalMove
    };

    struct ErrorReceive
    {
        ErrorType type;
        //string message; // ?
    };

    enum PacketType : Uint32
    {
        Plateau,
        Player,
        Pawn,
        YourTurn,
        Move,
        Error
    };

    PacketType type;

    //union // ???
    //{
        PlateauReceive plateau;
        PlayerReceive player;
        PawnReceive pawn;
        MoveReceive move;
        ErrorReceive error;
    //};
};

bool receiveFormatted(TcpSocket &socket, Receiver &receiver);
Packet &operator>>(Packet &packet, Receiver::PacketType &packetType);

#endif
