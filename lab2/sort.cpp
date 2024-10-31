#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
#include <algorithm>

using namespace std;

mutex mtx; // Mutex для защиты переменной 'swapped'

void oddEvenSortParallel(vector<int>& arr, int maxThreads) {
    int n = arr.size();
    bool swapped = true;

    while (swapped) {
        swapped = false;

        // Две фазы: четная и нечетная
        for (int phase = 0; phase < 2; ++phase) {
            int threadsToUse = maxThreads;

            vector<thread> threads(threadsToUse);
            int chunkSize = (n + threadsToUse - 1) / threadsToUse;

            // Создаем потоки
            for (int t = 0; t < threadsToUse; ++t) {
                int startIndex = t * chunkSize + phase;
                int endIndex = min((t + 1) * chunkSize + phase, n - 1);

                if (startIndex >= n - 1) continue;

                threads[t] = thread([&, startIndex, endIndex]() {
                    bool localSwapped = false;
                    for (int i = startIndex; i < endIndex; i += 2) {
                        if (arr[i] > arr[i + 1]) {
                            swap(arr[i], arr[i + 1]);
                            localSwapped = true;
                        }
                    }
                    if (localSwapped) {
                        lock_guard<mutex> lock(mtx);
                        swapped = true;
                    }
                });
            }

            // Ждем завершения потоков
            for (int t = 0; t < threadsToUse; ++t) {
                if (threads[t].joinable()) {
                    threads[t].join();
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <maxThreads> <inputFile> <outputFile>" << endl;
        return 1;
    }

    int maxThreads = stoi(argv[1]);
    string inputFileName = argv[2];
    string outputFileName = argv[3];

    vector<int> arr;

    // Чтение массива из файла
    ifstream inputFile(inputFileName);
    if (!inputFile) {
        cerr << "Error: Cannot open input file " << inputFileName << endl;
        return 1;
    }

    int num;
    while (inputFile >> num) {
        arr.push_back(num);
    }
    inputFile.close();

    if (arr.empty()) {
        cerr << "Error: Input array is empty or invalid." << endl;
        return 1;
    }
    /*
    cout << "Original array: ";
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;
*/
    oddEvenSortParallel(arr, maxThreads);
/*
    cout << "Sorted array: ";
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;
*/
    // Запись отсортированного массива в файл
    ofstream outputFile(outputFileName);
    if (!outputFile) {
        cerr << "Error: Cannot open output file " << outputFileName << endl;
        return 1;
    }

    for (int num : arr) {
        outputFile << num << " ";
    }
    outputFile.close();

    return 0;
}
