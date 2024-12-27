#include "lib.h"
#include <sstream>
#include <vector>

// Узлы в виде BST
struct BSTNode {
    int id;
    Node node;
    BSTNode *left;
    BSTNode *right;
    BSTNode(int _id, Node _n) : id(_id), node(_n), left(nullptr), right(nullptr) {}
};

BSTNode* insertBST(BSTNode* root, int id, Node node) {
    if (!root) return new BSTNode(id, node);
    if (id < root->id) root->left = insertBST(root->left, id, node);
    else if (id > root->id) root->right = insertBST(root->right, id, node);
    return root;
}

BSTNode* findBST(BSTNode* root, int id) {
    if (!root) return nullptr;
    if (id == root->id) return root;
    else if (id < root->id) return findBST(root->left, id);
    else return findBST(root->right, id);
}

std::vector<message> saved_mes;
std::vector<Node> all_nodes;

int main() {
    BSTNode *root = nullptr;

    std::string command;
    while (true) {
        // Проверка ответов от узлов
        for (auto &nd : all_nodes) {
            message m = get_mes(nd);
            if (m.command == None) continue;
            for (auto it = saved_mes.begin(); it != saved_mes.end(); ++it) {
                if (it->command == m.command && it->id == m.id) {
                    // Нашли связанное сообщение
                    if (m.command == Ping) {
                        // Узел доступен
                        std::cout << "Ok: 1" << std::endl;
                    } else if (m.command == ExecSum) {
                        // Результат суммы
                        std::cout << "Ok:" << m.id << ": " << m.num << std::endl;
                    }
                    saved_mes.erase(it);
                    break;
                }
            }
        }

        // Проверка таймаутов
        for (auto it = saved_mes.begin(); it != saved_mes.end();) {
            double diff = std::difftime(t_now(), it->sent_time);
            if (diff > 5) {
                // Таймаут
                if (it->command == Ping) {
                    // Узел не ответил на Ping
                    std::cout << "Ok: 0" << std::endl;
                } else if (it->command == ExecSum) {
                    // Узел не ответил на Exec
                    std::cout << "Error:" << it->id << ": Node is unavailable" << std::endl;
                }
                it = saved_mes.erase(it);
            } else {
                ++it;
            }
        }

        // Обработка команд пользователя
        if (!inputAvailable()) {
            usleep(100000);
            continue;
        }

        std::cin >> command;
        if (command == "create") {
            int id, parent_id = -1;
            std::cin >> id;
            if (std::cin.peek() != '\n') {
                std::cin >> parent_id; 
            }

            if (findBST(root, id)) {
                std::cout << "Error: Already exists" << std::endl;
                continue;
            }

            // Создаём узел
            Node child = createProcess(id);
            all_nodes.push_back(child);
            root = insertBST(root, id, child);
            std::cout << "Ok: " << child.pid << std::endl;

        } else if (command == "exec") {
            int id, n;
            std::cin >> id >> n;

            BSTNode* node_ptr = findBST(root, id);
            if (!node_ptr) {
                std::cout << "Error:" << id << ": Not found" << std::endl;
                // Считываем оставшиеся числа, чтобы очистить ввод
                for (int i=0; i<n; i++){int tmp; std::cin>>tmp;}
                continue;
            }

            std::ostringstream oss;
            for (int i=0; i<n; i++) {
                long long val;
                std::cin >> val;
                oss << val << " ";
            }
            std::string nums_str = oss.str();

            char buf[30];
            memset(buf,0,sizeof(buf));
            strncpy(buf, nums_str.c_str(), 29);

            message m(ExecSum, id, n, buf);
            saved_mes.push_back(m);
            send_mes(node_ptr->node, m);

        } else if (command == "ping") {
            int id;
            std::cin >> id;
            BSTNode* node_ptr = findBST(root, id);
            if (!node_ptr) {
                std::cout << "Error: Not found" << std::endl;
                continue;
            }
            message m(Ping, id, 0);
            saved_mes.push_back(m);
            send_mes(node_ptr->node, m);
        } else {
            std::cout << "Error: Command doesn't exist!" << std::endl;
        }
        usleep(100000);
    }
    return 0;
}
