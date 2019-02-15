/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** Main.cpp
*/

#include <iostream>
#include <string>
#include <vector>
#include <thread> // ??
#include <functional>
#include <SFML/Network.hpp>
#include "Player.hpp"
#include "Plateau.hpp"
#include "PawnRule.hpp"
#include "Helper.hpp"
#include "Receiver.hpp"

using namespace std;
using namespace sf;

PawnMap pawnMap = PawnMap(); // tmp

int PORTADD = 0; //tmp

bool waitMyTurnLock = false;

bool waitMyTurn(Plateau *plateau, Player *my, bool *myTurn) // (return ?)
{
    Receiver receive;

    waitMyTurnLock = true;
    if (!my->receive(receive)) {
        cerr << "receive faild" << endl;
        waitMyTurnLock = false;
        exit(84);return false;
    }
    if (receive.type == Receiver::YourTurn) {
        *myTurn = true;
        cerr << "my turn !" << endl;
    }
    else if (receive.type == Receiver::Move) {
        plateau->setStatus(receive.move.pawnPos);
        plateau->move(receive.move.pawnPos, receive.move.movePos);
        if (plateau->select != plateau->getSize())
            plateau->setStatus(plateau->select);
        else
            plateau->cleanStatus();
        cerr << "move receive" << endl;
    }
    else {
        cerr << "bad receive type (" << receive.type << ")" << endl;
        waitMyTurnLock = false;
        exit(84);return false;
    }
    waitMyTurnLock = false;
    return true;
}

int game(RenderWindow &window, Plateau &plateau)
{
    Player my(true ? Color::White : Color::Black); // tmp
    bool myTurn = false;
    Thread waitMyTurnThread(bind(&waitMyTurn, &plateau, &my, &myTurn));

    for (size_t i = 0; i < 10 && !my.connectClient("10.19.254.109", PORT + i); i++);
    while (window.isOpen()) {
        if (!myTurn && !waitMyTurnLock)
            waitMyTurnThread.launch();
        for (Event event; window.pollEvent(event);) {
            if (event.type == Event::Closed)
                window.close();
            else if (event.type == Event::MouseButtonPressed) {
                Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                Vector2u pos = plateau.convertMousePos(mousePos);

                if (pos != plateau.getSize()) {
                    if (plateau.select != plateau.getSize() && myTurn && plateau.checkMove(plateau.select, pos)) {
                        Receiver receive;

                        cout << plateau.select.x << " " << plateau.select.y << " | " << pos.x << " " << pos.y << endl;
                        if (!my.sendMove(plateau.select, pos)) {
                            cerr << "send move faild" << endl;
                            return false;
                        }
                        if (!my.receive(receive)) {
                            cerr << "receive check server faild" << endl;
                            return false;
                        }
                        if (receive.type == Receiver::Error) {
                            if (receive.error.type == Receiver::Ok) {
                                plateau.move(receive.move.pawnPos, receive.move.movePos);
                                plateau.select = plateau.getSize();
                                plateau.cleanStatus();
                                cerr << "end of my turn" << endl;
                                myTurn = false;
                            }
                            else if (receive.error.type == Receiver::IllegalMove)
                                cerr << "Illegal Move" << endl;
                            else {
                                cerr << "bad receive error type" << endl;
                                return false;
                            }
                        }
                        else {
                            cerr << "bad receive type" << endl;
                            return false;
                        }
                    }
                    else {
                        plateau.select = pos;
                        plateau.setStatus(pos);
                    }
                }
            }
        }
        window.clear();
        window.draw(plateau);
        window.display();
    }
    return 0;
}

int menu(RenderWindow &window)
{
    Plateau plateau;

    game(window, plateau);
    return 0;
}

int client()
{
    RenderWindow window(VideoMode::getDesktopMode(), "Echec++", Style::Fullscreen);

    window.setFramerateLimit(30);
    menu(window);
    return 0;
}

int server()
{
    Plateau plateau;
    size_t turn = 0;
    vector<Player*> playerList;
    TcpListener listener;

    for (size_t i = 0; i < 2; i++) { // tmp
        Player *player = new Player(!i ? Color::White : Color::Black);

        if (!player->connectServer(PORT + i)) {
            cerr << "connect faild (i:" << i << ")" << endl;
            return 84;
        }
        playerList.push_back(player);
    }
    cout << "all players here" << endl;
    while (true) { // tmp
        Receiver receive;
        Vector2u pos1;
        Vector2u pos2;

        if (!playerList[turn]->sendYourTurn())
            cerr << "sendTurn faild (turn: " << turn << ")" << endl;
        if (!playerList[turn]->receive(receive)) {
            cerr << "receive move faild" << endl;
            break;
        }
        if (receive.type == Receiver::Move) {
            pos1 = receive.move.pawnPos;
            pos2 = receive.move.movePos;
        }
        else {
            cerr << "bad receive type" << endl;
            break;
        }
        cout << turn << ": " << pos1.x << " " << pos1.y << " | " << pos2.x << " " << pos2.y << endl;
        // TODO : check color of pawn on pos1
        if (plateau.setStatus(pos1) && plateau.move(pos1, pos2)) {
            if (!playerList[turn]->sendError(Receiver::Ok)) {
                cerr << "OK send faild" << endl;
                break;
            }
        }
        else {
            cerr << "bad move" << endl;
            if (!playerList[turn]->sendError(Receiver::IllegalMove)) {
                cerr << "error send faild" << endl;
                break;
            }
            continue;
        }
        for (Player *player : playerList)
            if (!player->sendMove(pos1, pos2)) {
                cerr << "send move faild" << endl;
                break;
            }
        turn++;
        if (turn >= playerList.size())
            turn = 0;
    }
    for (size_t i = 0; i < playerList.size(); i++)
        delete playerList[i];
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 1)
        return client();
    else if (argc == 2 && (string(argv[1]) == "server" || string(argv[1]) == "s"))
        return server();
    return 84;
}
