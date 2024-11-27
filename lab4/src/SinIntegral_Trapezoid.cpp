#include <cmath>
#include <stdexcept>

extern "C" float SinIntegral(float A, float B, float e) {
    if (e <= 0.0f) {
        throw std::invalid_argument("Шаг интегрирования должен быть положительным");
    }
    float sum = 0.0f;
    for (float x = A; x < B; x += e) {
        float x_next = x + e;
        if (x_next > B) x_next = B;
        sum += (std::sin(x) + std::sin(x_next)) * (x_next - x) / 2.0f;
    }
    return sum;
}