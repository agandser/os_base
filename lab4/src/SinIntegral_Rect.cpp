#include <cmath>
#include <stdexcept>

extern "C" float SinIntegral(float A, float B, float e) {
    if (e <= 0.0f) {
        throw std::invalid_argument("Шаг интегрирования должен быть положительным");
    }
    float sum = 0.0f;
    for (float x = A; x < B; x += e) {
        sum += std::sin(x) * e;
    }
    return sum;
}