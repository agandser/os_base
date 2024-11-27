#include <algorithm>
#include <stdexcept>

extern "C" int* Sort(int* array, int size) {
    if (!array) {
        throw std::invalid_argument("Массив не может быть null");
    }
    if (size <= 0) {
        throw std::invalid_argument("Размер массива должен быть положительным");
    }
    for(int i = 0; i < size-1; ++i) {
        for(int j = 0; j < size-i-1; ++j) {
            if(array[j] > array[j+1]) {
                std::swap(array[j], array[j+1]);
            }
        }
    }
    return array;
}