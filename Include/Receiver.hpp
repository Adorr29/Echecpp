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
#include "Plan/PlanPlateau.hpp"
//#include "PawnRule.hpp"
#include "Plateau.hpp"
//#include "Player.hpp"

using namespace std;
using namespace sf;

class Receiver
{
public:
    //Receiver() {} // ??
    //~Receiver() {} // ??

    struct PlateauReceive
    {
        string fileName;
        Vector2u size;
        vector<bool> exist;
        vector<PawnParam> pawn;
    };

    struct PlayerReceive
    {
        Color color;
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
        PlateauPlan,
        Player,
        Pawn,
        YourTurn,
        Move,
        Error
    };

    PacketType type;

    //union // ???
    //{
    PlanPlateau planPlateau;
    PlayerReceive player;
    PawnReceive pawn;
    MoveReceive move;
    ErrorReceive error;
    //};
};

bool receiveFormatted(TcpSocket &socket, Receiver &receiver);
Packet &operator>>(Packet &packet, Receiver::PacketType &packetType);

#endif
