/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** Main.cpp
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <SFML/Network.hpp>
#include "Player.hpp"
#include "Plateau.hpp"
#include "PawnRule.hpp"
#include "Helper.hpp"
#include "Receiver.hpp"

using namespace std;
using namespace sf;

bool waitMyTurnLock = false;

bool waitMyTurn(Plateau *plateau, Player *my, bool *myTurn) // (return ?)
{
    Receiver receive;

    waitMyTurnLock = true;
    if (!my->receive(receive)) {
        cerr << "receive faild" << endl;
        waitMyTurnLock = false;
        exit(84); // tmp
        return false;
    }
    if (receive.type == Receiver::YourTurn) {
        *myTurn = true;
        cerr << "my turn !" << endl;
    }
    else if (receive.type == Receiver::Move) {
        plateau->setStatus(receive.move.pawnPos, true); // ?
        plateau->move(receive.move.pawnPos, receive.move.movePos);
        if (plateau->select != plateau->getSize())
            plateau->setStatus(plateau->select, true); // ?
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

bool game(RenderWindow &window, Player &my, Plateau &plateau)
{
    bool myTurn = false;
    Thread waitMyTurnThread(bind(&waitMyTurn, &plateau, &my, &myTurn));

    while (window.isOpen()) {
        if (!myTurn && !waitMyTurnLock)
            waitMyTurnThread.launch();
        for (Event event; window.pollEvent(event);) {
            if (event.type == Event::Closed)
                window.close();
            else if (event.type == Event::MouseMoved)
                plateau.info = plateau.convertMousePos(Vector2i(event.mouseMove.x, event.mouseMove.y));
            else if (event.type == Event::MouseButtonPressed) {
                Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                Vector2u pos = plateau.convertMousePos(mousePos);

                if (pos != plateau.getSize()) {
                    plateau.info = pos;
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

struct IaMove
{
    Vector2u pawnPos;
    Vector2u movePos;
    Int32 score;
};

void iaStockMove(vector<IaMove> &moveList, const Plateau &plateau, const Uint32 x, const Uint32 y)
{
    for (Uint32 i = 0; i < plateau.getSize().x; i++)
        for (Uint32 j = 0; j < plateau.getSize().y; j++)
            if (plateau.getTab(i, j).basicStatus == Plateau::Move || plateau.getTab(i, j).basicStatus == Plateau::Eat) {
                IaMove iaMove;

                iaMove.pawnPos = Vector2u(x, y);
                iaMove.movePos = Vector2u(i, j);
                    iaMove.score = 0;
                if (plateau.getTab(i, j).basicStatus == Plateau::Move)
                    cout << "Move: ";
                if (plateau.getTab(i, j).basicStatus == Plateau::Eat) {
                    cout << "Eat:  ";
                    iaMove.score = 1; // TODO
                }
                cout << "(" << x << ";" << y << ") => (" << i << ";" << j << ")" << endl;
                moveList.push_back(iaMove);
            }
}

void iaPlay(const Player &my, Plateau &plateau, Vector2u &pawnPos, Vector2u &movePos)
{
    vector<IaMove> moveList;

    for (Uint32 i = 0; i < plateau.getSize().x; i++)
        for (Uint32 j = 0; j < plateau.getSize().y; j++)
            if (plateau.getTab(i, j).pawn.color == my.getColor()) {
                plateau.setStatus(Vector2u(i, j), true); // ?
                iaStockMove(moveList, plateau, i, j);
            }
    cout << "moveList size: " << moveList.size() << endl;
    if (!moveList.size())
        return;
    // TODO
    sort(moveList.begin(), moveList.end(), [](IaMove &a, IaMove &b){return a.score > b.score;});
    pawnPos = moveList[0].pawnPos;
    movePos = moveList[0].movePos;
}

bool ia(Player &my, Plateau &plateau)
{
    bool myTurn = false;

    while (true) { // tmp
        Receiver receive;
        Vector2u pawnPos;
        Vector2u movePos;

        while (!myTurn)
            waitMyTurn(&plateau, &my, &myTurn);
        // ia TODO
        iaPlay(my, plateau, pawnPos, movePos); // ?
        cout << pawnPos.x << " " << pawnPos.y << " | " << movePos.x << " " << movePos.y << endl;
        if (!my.sendMove(pawnPos, movePos)) {
            cerr << "send move faild" << endl;
            return false;
        }
        if (!my.receive(receive)) {
            cerr << "receive check server faild" << endl;
            return false;
        }
        if (receive.type == Receiver::Error) {
            if (receive.error.type == Receiver::Ok) {
                plateau.select = plateau.getSize();
                plateau.cleanStatus();
                cerr << "end of my turn" << endl;
                myTurn = false;
            }
            else if (receive.error.type == Receiver::IllegalMove) {
                cerr << "Illegal Move" << endl;
                // TODO
            }
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
    return true;
}

bool create_ia(const IpAddress &ip = IpAddress::getLocalAddress())
{
    PawnMap pawnMap;
    Player my(false ? Color::White : Color::Black); // tmp
    Receiver receive;

    for (size_t i = 0; i < 100 && !my.connectClient(ip, PORT + i); i++);
    if (!my.receive(receive))
        return false;
    if (receive.type == Receiver::PlateauPlan) {
        Plateau plateau(receive.planPlateau);

        plateau.pawnMap = &pawnMap;
        if (!ia(my, plateau))
        return false;
    }
    else
        return false;
    return true;
}

bool menu(RenderWindow &window, const IpAddress &ip = IpAddress::getLocalAddress())
{
    PawnMap pawnMap;
    Player my(true ? Color::White : Color::Black); // tmp
    Receiver receive;

    for (size_t i = 0; i < 100 && !my.connectClient(ip, PORT + i); i++);
    if (!my.receive(receive))
        return false;
    if (receive.type == Receiver::PlateauPlan) {
        Plateau plateau(receive.planPlateau);

        plateau.pawnMap = &pawnMap;
        if (!game(window, my, plateau))
            return false;
    }
    else
        return false;
    return true;
}

int client(const IpAddress &ip = IpAddress::getLocalAddress()) // tmp
{
    RenderWindow window(VideoMode::getDesktopMode(), "Echec++", Style::Fullscreen);

    srand(time(NULL));
    window.setFramerateLimit(30);
    menu(window, ip);
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
        if (plateau.setStatus(pos1, true) && plateau.move(pos1, pos2)) { // (", true" ?)
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
    unsigned short port = PORT + rand() % 100; // tmp

    for (size_t i = 0; i < 2; i++) { // tmp
        Player *player = new Player(!i ? Color::White : Color::Black);

        if (!player->connectServer(port + i)) {
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
    PawnMap pawnMap;
    PlanPlateau planPlateau;

    srand(time(NULL)); // tmp
    if (fileName == "") {
        Plateau plateau;

        plateau.pawnMap = &pawnMap;
        return server(plateau);
    }
    if (!planPlateau.loadFromFile(fileName))
        return false;
    Plateau plateau(planPlateau);

    plateau.pawnMap = &pawnMap;
    return server(plateau);
}

int main(int argc, char **argv)
{
    if ((argc == 2 || argc == 3) && (string(argv[1]) == "server" || string(argv[1]) == "s"))
        return create_server(argc == 3 ? string(argv[2]) : "");
    else if ((argc == 2 || argc == 3) && string(argv[1]) == "ia") {
        if (argc == 2)
            return create_ia();
        else
            return create_ia(IpAddress(string(argv[2])));
    }
    else if (argc == 2 && string(argv[1]) == "ip")
        cout << "LocalAddress: " << IpAddress::getLocalAddress() << endl;
    else if ((argc == 1 || argc == 2)) {
        if (argc == 1)
            return client();
        else
            return client(IpAddress(string(argv[1])));
    }
    return 84;
}
