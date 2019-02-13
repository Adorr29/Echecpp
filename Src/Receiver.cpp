/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** Receiver.hpp
*/

#include "Receiver.hpp"
#include "Helper.hpp"

bool receiveFormatted(TcpSocket &socket, Receiver &receiver)
{
    Packet packet;

    if (socket.receive(packet) != Socket::Done)
        return false;
    if (!(packet >> receiver.type))
        return false;
    if (receiver.type == Receiver::PacketType::Plateau) {
        // TODO
    }
    else if (receiver.type == Receiver::PacketType::Player) {
        // TODO
    }
    else if (receiver.type == Receiver::PacketType::Pawn) {
        // TODO
    }
    else if (receiver.type == Receiver::PacketType::Turn) {
        if (!(packet >> receiver.turn.name))
            return false;
    }
    else if (receiver.type == Receiver::PacketType::Move) {
        if (!(packet >> receiver.move.pawnPos))
            return false;
        if (!(packet >> receiver.move.movePos))
            return false;
    }
    else if (receiver.type == Receiver::PacketType::Error) {
        Uint32 tmp; // ?

        if (!(packet >> tmp))
            return false;
        receiver.error.type = (Receiver::ErrorType)tmp;
    }
    else
        return false;
    return true;
}

Packet &operator>>(Packet &packet, Receiver::PacketType &packetType)
{
    Uint32 tmp; // ??

    packet >> tmp;
    packetType = (Receiver::PacketType)tmp;
    return packet;
}
