/*
** EPITECH PROJECT, 2019
** Echec++
** File description:
** Main.cpp
*/

#include <iostream>
#include <string>
#include <vector>
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

bool getTurn(Player &player, string &turnName)
{
    Receiver receive;

    if (!player.receive(receive)) {
        cerr << "receive turn faild" << endl;
        return false;
    }
    else if (receive.type == Receiver::Turn)
        turnName = receive.turn.name;
    else
        return false;
    cout << "turnName: " << turnName << endl;
    return true;
}

int game(RenderWindow &window, Plateau &plateau)
{
    Player my("human", (PORT + PORTADD) % 2 == 0 ? "me" : "no me", PORT % 2 == 0 ? Color::White : Color::Black); // tmp
    Vector2u select = plateau.getSize();
    string turnName;

    cerr << "my name: " << my.getName() << endl;
    for (size_t i = 0; i < 10 && !my.connectClient("10.19.254.109", PORT + i); i++);
    if (!getTurn(my, turnName))
        cerr << "first getTurn faild" << endl;
    cerr << "turnName: " << turnName << endl;
    while (window.isOpen()) {
        if (turnName != my.getName()) {
            Receiver receive;

            cout << "wait opo" << endl;
            if (!my.receive(receive))
                cerr << "receive get opo move faild" << endl;
            else if (receive.type == Receiver::Move) {
                plateau.setStatus(receive.move.pawnPos);
                plateau.move(receive.move.pawnPos, receive.move.movePos);
                if (select != plateau.getSize())
                    plateau.setStatus(select);
                else
                    plateau.cleanStatus();
                if (!getTurn(my, turnName))
                    cerr << "receive turn after opo faild" << endl;
                cout << "turnName: " << turnName << endl;
            }
        }
        else
            for (Event event; window.pollEvent(event);) {
                if (event.type == Event::Closed)
                    window.close();
                if (event.type == Event::MouseButtonPressed) {
                    Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
                    Vector2u pos = plateau.convertMousePos(mousePos);

                    if (pos != plateau.getSize()) {
                        if (select != plateau.getSize() && my.getName() == turnName && plateau.checkMove(select, pos)) {
                            Receiver receive;

                            cout << select.x << " " << select.y << " | " << pos.x << " " << pos.y << endl;
                            if (!my.sendMove(select, pos))
                                cerr << "send move faild" << endl;
                            if (!my.receive(receive))
                                cerr << "receive check server faild" << endl;
                            else if (receive.type == Receiver::Move) {
                                plateau.move(receive.move.pawnPos, receive.move.movePos);
                                select = plateau.getSize();
                                plateau.cleanStatus();
                                if (!getTurn(my, turnName))
                                    cout << "getTurn faild" << endl;
                            }
                        }
                        else {
                            select = pos;
                            plateau.setStatus(pos);
                        }



                        /*if (select != plateau.getSize() && plateau.move(select, pos)) {
                          select = plateau.getSize();
                          plateau.cleanStatus();
                          }
                          else {
                          select = pos;
                          plateau.setStatus(pos);
                          }*/
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
        Player *player = new Player("human", !i ? "me" : "no me", !i ? Color::White : Color::Black);

        if (!player->connectServer(PORT + i)) {
            cerr << "connect faild (i:" << i << ")" << endl;
            return 84;
        }
        playerList.push_back(player);
    }
    cout << "all players here:" << endl;
    for (size_t i = 0; i < playerList.size(); i++)
        cout << " - i:" << i << " " << playerList[i]->getName() << endl;
    for (Player *player : playerList)
        if (!player->sendTurn(playerList[turn]->getName())) {
            cerr << "send first turn faild (name:" << player->getName() << ")" << endl;
            return 84;
        }
    while (true) { // tmp
        Receiver receive;
        Vector2u pos1;
        Vector2u pos2;

        cerr << "turn of " << playerList[turn]->getName() << endl;
        if (!playerList[turn]->receive(receive)) {
            cerr << "receive move faild" << endl;
            break;
        }
        else if (receive.type == Receiver::Move) {
            pos1 = receive.move.pawnPos;
            pos2 = receive.move.movePos;
        }
        else {
            cerr << "bad receive" << endl;
            break;
        }
        cout << playerList[turn]->getName() << ": " << pos1.x << " " << pos1.y << " | " << pos2.x << " " << pos2.y << endl;
        if (!plateau.setStatus(pos1) || !plateau.move(pos1, pos2)) {
            cerr << "bad pos" << endl;
            if (playerList[turn]->sendError(Receiver::IllegalMove)) {
                cerr << "error send faild" << endl;
                break;
            }
            continue;
        }
        for (Player *player : playerList)
            if (!player->sendMove(pos1, pos2)) {
                cerr << "send faild" << endl;
                break;
            }
        turn++;
        if (turn >= playerList.size())
            turn = 0;
        cerr << "PASS => " << "turn:" << turn << " name:" << playerList[turn]->getName() << endl;
        for (Player *player : playerList)
            if (!player->sendTurn(playerList[turn]->getName())) {
                cerr << "send faild" << endl;
                break;
            }
    }
    for (size_t i = 0; i < playerList.size(); i++)
        delete playerList[i];
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 1)
        return client();
    if (argc == 2 && (string(argv[1]) == "server" || string(argv[1]) == "s"))
        return server();
    else { // tmp
        PORTADD = 1;
        return client();
    }
    return 84;
}
