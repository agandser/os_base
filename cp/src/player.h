#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <csignal>
#include "myMQ.h"

// Размер игрового поля
const int BOARD_SIZE = 10;

/*
 *Класс, описывающий игрока и его поле
 */
class Player {
public:
    // Поле 10x10
    std::vector<std::vector<char>> board;
    // Номер игрока (1 или 2)
    int num;

    Player() {
        board = std::vector<std::vector<char>>(BOARD_SIZE, std::vector<char>(BOARD_SIZE, ' '));
        num = 0;
    }

    /*
     *Простейшее размещение кораблей (один однопалубник)
     */
    void placeShips(zmq::socket_t& player_socket, pid_t first_player_pid, pid_t second_player_pid) {
        // Спросим у клиента ориентацию
        send_mes(player_socket, "Введите ориентацию (V/H)");
        std::string orientation = get_mes(player_socket);
        {
            // Удалим лишние пробелы
            std::stringstream tmpSS(orientation);
            tmpSS >> orientation;
        }

        // Попросим координаты
        send_mes(player_socket, "Разместите 1-палубный корабль (x y)");
        std::string coords = get_mes(player_socket);

        bool alive = try_recv(first_player_pid, second_player_pid);
        if (!alive) {
            std::cout << "[Server] Игра прервана из-за смерти процесса\n";
            kill(first_player_pid, SIGTERM);
            kill(second_player_pid, SIGTERM);
            exit(0);
        }

        // Парсим полученные координаты (формат "coords:x:y")
        int x = -1, y = -1;
        {
            std::stringstream ss(coords);
            std::string tmp;
            std::getline(ss, tmp, ':'); // "coords"
            std::getline(ss, tmp, ':'); // x
            x = std::stoi(tmp);
            std::getline(ss, tmp, ':'); // y
            y = std::stoi(tmp);
        }

        if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
            send_mes(player_socket, "Error: Неверные координаты");
            get_mes(player_socket); // читаем "ok"
            return;
        }

        // Размещаем корабль
        board[x][y] = 'O';

        // Покажем доску
        send_mes(player_socket, "board" + getBoard());
        get_mes(player_socket); // "ok"
    }

    /*
     *Возвращает поле игрока в виде строки (открытое, с кораблями)
     */
    std::string getBoard() const {
        std::stringstream ss;
        ss << "\n  0 1 2 3 4 5 6 7 8 9\n";
        for (int i = 0; i < BOARD_SIZE; ++i) {
            ss << i << " ";
            for (int j = 0; j < BOARD_SIZE; ++j) {
                ss << board[i][j] << " ";
            }
            ss << "\n";
        }
        ss << "\n";
        return ss.str();
    }

    /*
     *Возвращает поле игрока без отображения кораблей (скрытые «O»)
     */
    std::string getClearBoard() const {
        std::stringstream ss;
        ss << "\n  0 1 2 3 4 5 6 7 8 9\n";
        for (int i = 0; i < BOARD_SIZE; ++i) {
            ss << i << " ";
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board[i][j] == 'O') {
                    ss << "  ";
                } else {
                    ss << board[i][j] << " ";
                }
            }
            ss << "\n";
        }
        ss << "\n";
        return ss.str();
    }
};
