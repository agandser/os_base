#include <iostream>
#include <dlfcn.h>
#include <stdexcept>

typedef float (*SinIntegralFunc)(float, float, float);
typedef int* (*SortFunc)(int*, int);

int main() {
    const char* libSinPaths[2] = {"./libSinIntegral_Rect.so", "./libSinIntegral_Trapezoid.so"};
    const char* libSortPaths[2] = {"./libSort_Bubble.so", "./libSort_Quick.so"};
    int currentLibIndex = 0;

    void* handleSin = dlopen(libSinPaths[currentLibIndex], RTLD_LAZY);
    if (!handleSin) {
        std::cerr << "Не удалось загрузить библиотеку SinIntegral: " << dlerror() << "\n";
        return 1;
    }

    void* handleSort = dlopen(libSortPaths[currentLibIndex], RTLD_LAZY);
    if (!handleSort) {
        std::cerr << "Не удалось загрузить библиотеку Sort: " << dlerror() << "\n";
        dlclose(handleSin);
        return 1;
    }

    // Получение адресов функций
    SinIntegralFunc SinIntegral = (SinIntegralFunc)dlsym(handleSin, "SinIntegral");
    SortFunc Sort = (SortFunc)dlsym(handleSort, "Sort");

    if (!SinIntegral || !Sort) {
        std::cerr << "Не удалось найти функции в библиотеках.\n";
        dlclose(handleSin);
        dlclose(handleSort);
        return 1;
    }

    std::string command;
    while (true) {
        std::cout << "Введите команду(0 - переключение, 1 - интеграл, 2 - сортировка): ";
        if (!(std::cin >> command)) break;
        try {
            if (command == "0") {
                // Переключение реализации библиотек
                dlclose(handleSin);
                dlclose(handleSort);
                currentLibIndex = 1 - currentLibIndex; // Переключение между 0 и 1
                handleSin = dlopen(libSinPaths[currentLibIndex], RTLD_LAZY);
                handleSort = dlopen(libSortPaths[currentLibIndex], RTLD_LAZY);
                if (!handleSin || !handleSort) {
                    std::cerr << "Не удалось переключить библиотеки: " << dlerror() << "\n";
                    return 1;
                }
                SinIntegral = (SinIntegralFunc)dlsym(handleSin, "SinIntegral");
                Sort = (SortFunc)dlsym(handleSort, "Sort");
                if (!SinIntegral || !Sort) {
                    std::cerr << "Не удалось найти функции после переключения.\n";
                    dlclose(handleSin);
                    dlclose(handleSort);
                    return 1;
                }
                std::cout << "Реализации переключены.\n";
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

    dlclose(handleSin);
    dlclose(handleSort);
    return 0;
}