#pragma once

#include <zmq.hpp>
#include <iostream>
#include <signal.h>
#include <string>

/*
 *Глобальная переменная, отвечающая за индекс «игрового» сокета на сервере.
 */
extern int PORT_ITER;

/*
 *Получить строку для подключения по TCP на заданный порт
 */
inline std::string GetConPort(int port) {
    return "tcp://127.0.0.1:" + std::to_string(port);
}

/*
 *Отправка строки через сокет ZeroMQ
 */
inline bool send_mes(zmq::socket_t& socket, const std::string& message_string) {
    zmq::message_t message(message_string.size());
    memcpy(message.data(), message_string.c_str(), message_string.size());
    return bool(socket.send(message, zmq::send_flags::none));
}

/*
 *Получение строки из сокета ZeroMQ
 */
inline std::string get_mes(zmq::socket_t& socket) {
    zmq::message_t message;
    bool ok = false;
    try {
        ok = bool(socket.recv(message, zmq::recv_flags::none));
    } catch(...) {
        ok = false;
    }
    if (!ok) {
        return "";
    }
    return std::string(static_cast<char*>(message.data()), message.size());
}

/*
 *Проверка, живы ли процессы (kill(pid, 0)).
 */
inline bool try_recv(pid_t first_player_pid, pid_t second_player_pid) {
    if (kill(first_player_pid, 0) != 0 || kill(second_player_pid, 0) != 0) {
        return false;
    }
    return true;
}
