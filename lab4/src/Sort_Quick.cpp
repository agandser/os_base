#include <algorithm>
#include <stdexcept>

extern "C" int* Sort(int* array, int size) {
    if (!array) {
        throw std::invalid_argument("Массив не может быть null");
    }
    if (size <= 0) {
        throw std::invalid_argument("Размер массива должен быть положительным");
    }
    std::sort(array, array + size);
    return array;
}