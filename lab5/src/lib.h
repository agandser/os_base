#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/select.h>
#include "zmq.h"
#include <vector>

/*
 * Проверка готовности ввода с консоли (не блокирующая).
 * return true, если есть данные для чтения; иначе false.
 */
bool inputAvailable();

/*
 * Возвращает текущее время в формате time_t.
 */
std::time_t t_now();

/*
 * Возможные команды для взаимодействия между управляющим и вычислительными узлами
 */
enum com : char {
    None = 0,   // Пустое сообщение (отсутствие команды)
    Ping = 1,   // Проверка доступности узла
    ExecSum = 2 // Вычисление суммы чисел
};

/*
 * Класс для хранения сообщения, передаваемого между узлами
 */
class message {
public:
    message() 
        : command(None), id(-1), num(0), sent_time(0) {
        std::memset(st, 0, sizeof(st));
    }

    message(com _cmd, int _id, int _num)
        : command(_cmd), id(_id), num(_num), sent_time(t_now()) {
        std::memset(st, 0, sizeof(st));
    }

    message(com _cmd, int _id, int _num, const char* s)
        : command(_cmd), id(_id), num(_num), sent_time(t_now()) {
        std::memset(st, 0, sizeof(st));
        std::strncpy(st, s, 29);
    }

    com command;           // Тип команды
    int id;                // Идентификатор узла, к которому обращаемся
    int num;               // Числовое поле (исп. для количества чисел или результата)
    std::time_t sent_time; // Время отправки сообщения (для таймаута)
    char st[30];           // Строковое поле для передачи набора чисел
};

/*
 * Класс, описывающий "узел" с точки зрения ZMQ-связи:
 *  - id узла,
 *  - pid процесса,
 *  - сокет и контекст ZeroMQ,
 *  - флаг is_child (дочерний/вычислительный или управляющий).
 */
class Node {
public:
    int id;            // ID узла
    pid_t pid;         // PID процесса
    void *context;     // контекст ZeroMQ
    void *socket;      // сокет ZeroMQ
    bool is_child;     // true, если это вычислительный узел
    std::string address; // адрес (tcp://127.0.0.1:порт)

    bool operator==(const Node &other) const {
        return (id == other.id && address == other.address);
    }
};

/*
 * Создать структуру Node в текущем процессе (с учетом is_child)
 * и настроить bind/connect на порт (5555 + id).
 */
Node createNode(int id, bool is_child);

/*
 * Создает новый процесс через fork() + execl("./computing", ...).
 * В родительском процессе возвращает Node (с bind), а в дочернем
 * происходит замена образа процесса на computing.
 */
Node createProcess(int id);

/*
 * Отправить сообщение m через сокет node.socket (ZMQ_DEALER, неблокирующе).
 */
void send_mes(Node &node, message m);

/*
 * Получить сообщение из сокета node.socket (ZMQ_DEALER, неблокирующе).
 * Возвращает message с command=None, если сообщений нет.
 */
message get_mes(Node &node);

