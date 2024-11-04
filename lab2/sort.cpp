#include <iostream>
#include <vector>
#include <pthread.h>
#include <cstdlib>
#include <fstream>
#include <getopt.h>
#include <algorithm>
#include <cmath>

using namespace std;

// Структура для передачи аргументов в потоки
struct ThreadArgs {
    vector<int>* data;
    int left;
    int right;
    int level;
};

// Глобальные переменные
int MAX_THREADS;
int current_threads = 0;
pthread_mutex_t thread_count_mutex;

// Прототипы функций
void batcherOddEvenMergeSort(vector<int>& data, int left, int right, int level);
void batcherOddEvenMerge(vector<int>& data, int left, int right, int r);
void compareExchange(vector<int>& data, int i, int j);
void* threadFunc(void* args);

// Опции командной строки
string input_filename;
string output_filename;

// Функция для чтения данных из файла или стандартного ввода
bool readData(vector<int>& data, istream& in) {
    int n;
    if (!(in >> n)) {
        cerr << "Не удалось прочитать количество элементов." << endl;
        return false;
    }
    data.resize(n);
    for (int i = 0; i < n; ++i) {
        if (!(in >> data[i])) {
            cerr << "Не удалось прочитать элемент " << i << "." << endl;
            return false;
        }
    }
    return true;
}

// Функция для записи данных в файл или стандартный вывод
void writeData(const vector<int>& data, ostream& out) {
    out << data.size() << endl;
    for (size_t i = 0; i < data.size(); ++i) {
        out << data[i] << (i + 1 < data.size() ? ' ' : '\n');
    }
}

// Главная функция
int main(int argc, char* argv[]) {
    // Значения по умолчанию
    MAX_THREADS = 1;

    // Разбор опций командной строки
    int option;
    while ((option = getopt(argc, argv, "t:i:o:")) != -1) {
        switch (option) {
            case 't':
                MAX_THREADS = atoi(optarg);
                break;
            case 'i':
                input_filename = optarg;
                break;
            case 'o':
                output_filename = optarg;
                break;
            default:
                cerr << "Использование: " << argv[0] << " [-t max_threads] [-i input_file] [-o output_file]" << endl;
                return 1;
        }
    }

    // Инициализация мьютекса
    pthread_mutex_init(&thread_count_mutex, nullptr);

    // Чтение входных данных
    vector<int> data;
    if (!input_filename.empty()) {
        ifstream infile(input_filename);
        if (!infile) {
            cerr << "Не удалось открыть входной файл: " << input_filename << endl;
            return 1;
        }
        if (!readData(data, infile)) {
            return 1;
        }
        infile.close();
    } else {
        cout << "Введите количество элементов, а затем сами элементы:" << endl;
        if (!readData(data, cin)) {
            return 1;
        }
    }

    // Запуск сортировки
    batcherOddEvenMergeSort(data, 0, data.size() - 1, 0);

    // Запись отсортированных данных
    if (!output_filename.empty()) {
        ofstream outfile(output_filename);
        if (!outfile) {
            cerr << "Не удалось открыть выходной файл: " << output_filename << endl;
            return 1;
        }
        writeData(data, outfile);
        outfile.close();
    } else {
        cout << "Отсортированный массив:" << endl;
        writeData(data, cout);
    }

    // Уничтожение мьютекса
    pthread_mutex_destroy(&thread_count_mutex);

    return 0;
}

// Функция сортировки Батчера "нечетно-четное слияние"
void batcherOddEvenMergeSort(vector<int>& data, int left, int right, int level) {
    int size = right - left + 1;
    if (size <= 1) return;

    int mid = (left + right) / 2;

    bool spawn_thread = false;

    pthread_mutex_lock(&thread_count_mutex);
    if (current_threads < MAX_THREADS) {
        spawn_thread = true;
        current_threads++;
    }
    pthread_mutex_unlock(&thread_count_mutex);

    if (spawn_thread) {
        // Создание потока для левой половины
        pthread_t thread;
        ThreadArgs* args = new ThreadArgs{&data, left, mid, level + 1};
        pthread_create(&thread, nullptr, threadFunc, args);

        // Сортировка правой половины в текущем потоке
        batcherOddEvenMergeSort(data, mid + 1, right, level + 1);

        // Ожидание завершения сортировки левой половины
        pthread_join(thread, nullptr);

        pthread_mutex_lock(&thread_count_mutex);
        current_threads--;
        pthread_mutex_unlock(&thread_count_mutex);

        delete args;
    } else {
        // Сортировка обеих половин в текущем потоке
        batcherOddEvenMergeSort(data, left, mid, level + 1);
        batcherOddEvenMergeSort(data, mid + 1, right, level + 1);
    }

    // Слияние двух половин
    batcherOddEvenMerge(data, left, right, right - left + 1);
}

// Функция "нечетно-четное слияние" Батчера
void batcherOddEvenMerge(vector<int>& data, int left, int right, int size) {
    if (size <= 1) return;
    int mid = size / 2;

    // Сравнение и обмен элементов с расстоянием mid
    for (int i = left; i + mid <= right; ++i) {
        compareExchange(data, i, i + mid);
    }

    // Рекурсивное слияние двух половин
    batcherOddEvenMerge(data, left, left + mid - 1, mid);
    batcherOddEvenMerge(data, left + mid, right, mid);
}

// Функция сравнения и обмена
void compareExchange(vector<int>& data, int i, int j) {
    if (j >= data.size()) return; // Избегаем выхода за пределы массива
    if (data[i] > data[j]) {
        swap(data[i], data[j]);
    }
}

// Функция потока
void* threadFunc(void* args) {
    ThreadArgs* arg = static_cast<ThreadArgs*>(args);
    batcherOddEvenMergeSort(*arg->data, arg->left, arg->right, arg->level);
    return nullptr;
}
