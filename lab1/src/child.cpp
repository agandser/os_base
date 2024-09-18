#include <iostream>
#include <string>
#include <algorithm>
#include <unistd.h>

// Функция для реверса строки
void reverse_string(std::string& str) {
    std::reverse(str.begin(), str.end());
}

int main() {
    while (true) {
        std::string str;
        char c;
        
        // Чтение строки до null-терминатора
        while (read(STDIN_FILENO, &c, 1) > 0 && c != '\0') {
            str += c;
        }

        if (str.empty()) { // Завершаем работу, если пустая строка
            break;
        }

        reverse_string(str);
        std::cout << str << std::endl; // Записываем реверсированную строку в файл
        std::cout.flush(); // Очищаем буфер вывода
    }
    return 0;
}
