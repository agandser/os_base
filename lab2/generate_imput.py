import random

def generate_input_file(filename, num_count, min_value, max_value):
    with open(filename, 'w') as f:
        f.write(f"{num_count}\n")  # Записываем количество чисел
        for _ in range(num_count):
            f.write(f"{random.randint(min_value, max_value)} ")
        f.write("\n")  # Переход на новую строку после записи всех чисел

if __name__ == "__main__":
    filename = "test.txt"
    num_count = 12000000  # Количество случайных чисел
    min_value = 1   # Минимальное значение
    max_value = 10000000 # Максимальное значение

    generate_input_file(filename, num_count, min_value, max_value)
    print(f"Generated {filename} with {num_count} random integers between {min_value} and {max_value}.")
