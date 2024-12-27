#include "lib.h"
#include <sstream>

int main(int argc, char *argv[])
{
    Node I = createNode(atoi(argv[1]), true);

    while (true) {
        message m = get_mes(I);
        if (m.command == None) {
            usleep(100000);
            continue;
        }

        switch (m.command) {
        case Ping:
            if (m.id == I.id) {
                send_mes(I, {Ping, I.id, 1});
            }
            break;
        case ExecSum:
        {
            if (m.id == I.id) {
                std::istringstream iss(m.st);
                int n = m.num;
                int sum = 0;
                for (int i = 0; i < n; i++) {
                    int val;
                    if (!(iss >> val)) {
                        // Предполагаем корректный ввод.
                        break;
                    }
                    sum += val;
                }
                // Возвращаем результат
                send_mes(I, {ExecSum, I.id, sum});
            }
        }
            break;
        default:
            break;
        }
        usleep(100000);
    }
    return 0;
}
