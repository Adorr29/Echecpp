/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** Main.cpp
*/

#include <iostream>
#include <string>
#include <vector>
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

bool waitMyTurnLock = false;

bool waitMyTurn(Plateau *plateau, Player *my, bool *myTurn) // (return ?)
{
    Receiver receive;

    waitMyTurnLock = true;
    if (!my->receive(receive)) {
        cerr << "receive faild" << endl;
        waitMyTurnLock = false;
        return false;
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
        return false;
    }
    waitMyTurnLock = false;
    return true;
}

int game(RenderWindow &window, Player &my, Plateau &plateau)
{
    bool myTurn = false;
    Thread waitMyTurnThread(bind(&waitMyTurn, &plateau, &my, &myTurn));

    cerr << "game launche" << endl;
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

bool menu(RenderWindow &window)
{
    Player my(true ? Color::White : Color::Black); // tmp
    Receiver receive;

    for (size_t i = 0; i < 10 && !my.connectClient(IpAddress::getLocalAddress(), PORT + i); i++);
    if (!my.receive(receive))
        return false;
    if (receive.type == Receiver::PlateauPlan) {
        cerr << "receive.type == Receiver::PlateauPlan" << endl;
        Plateau plateau(receive.planPlateau);

        cerr << "Plateau plateau(receive.planPlateau)" << endl;
        game(window, my, plateau);
    }
    else
        return false;
    return true;
}

int client()
{
    RenderWindow window(VideoMode::getDesktopMode(), "Echec++", Style::Fullscreen);

    window.setFramerateLimit(30);
    menu(window);
    return 0;
}

bool serverWaitMove(Plateau &plateau, const vector<Player*> &playerList, const size_t &turn)
{
    Receiver receive;
    Vector2u pos1;
    Vector2u pos2;

    while (true) {
        if (!playerList[turn]->receive(receive)) {
            cerr << "receive move faild" << endl;
            return false;
        }
        if (receive.type == Receiver::Move) {
            pos1 = receive.move.pawnPos;
            pos2 = receive.move.movePos;
        }
        else {
            cerr << "bad receive type" << endl;
            return false;
        }
        cout << pos1.x << " " << pos1.y << " | " << pos2.x << " " << pos2.y << endl;
        // TODO : check color of pawn on pos1
        if (plateau.setStatus(pos1) && plateau.move(pos1, pos2)) {
            if (!playerList[turn]->sendError(Receiver::Ok)) {
                cerr << "OK send faild" << endl;
                return false;
            }
        }
        else {
            cerr << "bad move" << endl;
            if (!playerList[turn]->sendError(Receiver::IllegalMove)) {
                cerr << "error send faild" << endl;
                return false;
            }
            continue;
        }
        break;
    }
    for (Player *player : playerList)
        if (!player->sendMove(pos1, pos2)) {
            cerr << "send move faild" << endl;
            return false;
        }
    return true;
}

int server(Plateau &plateau)
{
    PlanPlateau planPlateau = plateau.getPlan();
    size_t turn = 0;
    vector<Player*> playerList;
    TcpListener listener;

    for (size_t i = 0; i < 2; i++) { // tmp
        Player *player = new Player(!i ? Color::White : Color::Black);

        if (!player->connectServer(PORT + i)) {
            cerr << "connect faild (i:" << i << ")" << endl;
            return 84;
        }
        if (!player->sendPlanPlateau(planPlateau)) {
            cerr << "faild to send plan plateau" << endl;
            return 84;
        }
        playerList.push_back(player);
    }
    cout << "all players here" << endl;
    while (true) { // tmp
        if (!playerList[turn]->sendYourTurn())
            cerr << "sendTurn faild (turn: " << turn << ")" << endl;
        if (!serverWaitMove(plateau, playerList, turn))
            break;
        turn++;
        if (turn >= playerList.size())
            turn = 0;
    }
    for (size_t i = 0; i < playerList.size(); i++)
        delete playerList[i];
    return 0;
}

bool create_server(const string &fileName = "")
{
    PlanPlateau planPlateau;

    if (fileName == "") {
        Plateau plateau;

        return server(plateau);
    }
    if (!planPlateau.loadFromFile(fileName))
        return false;
    Plateau plateau(planPlateau);

    return server(plateau);
}

int main(int argc, char **argv)
{
    if (argc == 1)
        return client();
    else if ((argc == 2 || argc == 3) && (string(argv[1]) == "server" || string(argv[1]) == "s"))
        return create_server(argc == 3 ? string(argv[2]) : "");
    return 84;
}
