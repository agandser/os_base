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
        std::getline(std::cin, str, '\0');// Читаем до null-терминатора 
        

        if (str.empty()) { // Завершаем работу, если пустая строка
            break;
        }

        reverse_string(str);
        std::cout << str << std::endl; // Записываем реверсированную строку в файл
        std::cout.flush(); // Очищаем буфер вывода
    }
    return 0;
}
