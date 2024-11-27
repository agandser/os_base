#include <iostream>
#include <stdexcept>

// Прототипы функций из динамической библиотеки
extern "C" float SinIntegral(float A, float B, float e);
extern "C" int* Sort(int* array, int size);

int main() {
    std::string command;
    while (true) {
        std::cout << "Введите команду (0 - переключение не поддерживается, 1 - интеграл, 2 - сортировка): ";
        if (!(std::cin >> command)) break;
        try {
            if (command == "0") {
                std::cout << "Переключение реализации не поддерживается в программе №1.\n";
            }
            else if (command == "1") {
                float A, B, e;
                std::cout << "Введите A, B, e: ";
                std::cin >> A >> B >> e;
                float result = SinIntegral(A, B, e);
                std::cout << "Интеграл: " << result << "\n";
            }
            else if (command == "2") {
                int size;
                std::cout << "Введите размер массива: ";
                std::cin >> size;
                if (size <= 0) {
                    std::cerr << "Размер массива должен быть положительным.\n";
                    continue;
                }
                int* array = new int[size];
                std::cout << "Введите элементы массива: ";
                for(int i = 0; i < size; ++i) {
                    std::cin >> array[i];
                }
                int* sorted = Sort(array, size);
                std::cout << "Отсортированный массив: ";
                for(int i = 0; i < size; ++i) {
                    std::cout << sorted[i] << " ";
                }
                std::cout << "\n";
                delete[] array;
            }
            else {
                std::cout << "Неверная команда.\n";
            }
        } catch (const std::exception& ex) {
            std::cerr << "Ошибка: " << ex.what() << "\n";
        }
    }
    return 0;
}