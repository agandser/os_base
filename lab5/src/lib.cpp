#include "lib.h"

#include <algorithm>
#include <sys/time.h>

/*
 * Проверяем, есть ли данные в stdin (не блокируемся).
 * Возвращаем true, если можно читать из stdin без блокировок.
 */
bool inputAvailable() {
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    select(STDIN_FILENO + 1, &read_fds, nullptr, nullptr, &tv);
    return (FD_ISSET(STDIN_FILENO, &read_fds) != 0);
}

/*
 * Возвращает текущее время (time_t).
 */
std::time_t t_now() {
    return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

/*
 * Создаем и настраиваем Node в текущем процессе:
 *  - Устанавливаем id, pid, is_child
 *  - Инициализируем ZMQ-сокет
 *  - Выполняем zmq_bind (если is_child=false) или zmq_connect (если is_child=true)
 */
Node createNode(int id, bool is_child) {
    Node resultNode;
    resultNode.id = id;
    resultNode.pid = getpid();
    resultNode.is_child = is_child;

    resultNode.context = zmq_ctx_new();
    resultNode.socket = zmq_socket(resultNode.context, ZMQ_DEALER);

    // Адрес для подключения/привязки
    resultNode.address = "tcp://127.0.0.1:" + std::to_string(5555 + id);

    if (is_child) {
        zmq_connect(resultNode.socket, resultNode.address.c_str());
    } else {
        zmq_bind(resultNode.socket, resultNode.address.c_str());
    }

    return resultNode;
}

/*
 * Создает новый процесс (дочерний) и в нем запускает "computing".
 * В родительском процессе инициализируем и возвращаем структуру Node (с bind).
 */
Node createProcess(int id) {
    pid_t childPid = fork();
    if (childPid == 0) {
        // Мы в дочернем процессе
        execl("./computing", "computing", std::to_string(id).c_str(), nullptr);
        // Если execl не сработал:
        std::cerr << "execl failed" << std::endl;
        _exit(1); 
    } else if (childPid == -1) {
        // Не удалось вызвать fork
        std::cerr << "Fork failed" << std::endl;
        _exit(1);
    }
    // Родительский процесс
    Node newNode = createNode(id, false);
    newNode.pid = childPid;
    return newNode;
}

/*
 * Отправка сообщения m через сокет (не блокируя).
 */
void send_mes(Node &node, message m) {
    zmq_msg_t tmpMsg;
    zmq_msg_init_size(&tmpMsg, sizeof(m));
    std::memcpy(zmq_msg_data(&tmpMsg), &m, sizeof(m));

    zmq_msg_send(&tmpMsg, node.socket, ZMQ_DONTWAIT);
    zmq_msg_close(&tmpMsg);
}

/*
 * Попытка чтения сообщения из сокета (не блокируя).
 * Если нет доступных сообщений, возвращаем message(None, -1, -1).
 */
message get_mes(Node &node) {
    zmq_msg_t msgBuffer;
    zmq_msg_init(&msgBuffer);

    // Пробуем прочитать сообщение
    int msgBytes = zmq_msg_recv(&msgBuffer, node.socket, ZMQ_DONTWAIT);
    if (msgBytes == -1) {
        zmq_msg_close(&msgBuffer);
        return message(None, -1, -1);
    }

    message receivedMsg;
    std::memcpy(&receivedMsg, zmq_msg_data(&msgBuffer), sizeof(receivedMsg));

    zmq_msg_close(&msgBuffer);
    return receivedMsg;
}
