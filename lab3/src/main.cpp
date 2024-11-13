#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>

// Структура для обмена данными через отображаемый файл
struct shared_data {
    sem_t sem_parent; // Семафор для родительского процесса (сигнал о готовности данных)
    sem_t sem_child;  // Семафор для дочернего процесса (сигнал о завершении обработки)
    char buffer[1024];
    int terminate;    // Флаг для завершения работы
};

int main() {
    // Запрос имен файлов у пользователя
    std::cout << "Enter file's name for child process 1: ";
    std::string file1_name;
    std::getline(std::cin, file1_name);

    std::cout << "Enter file's name for child process 2: ";
    std::string file2_name;
    std::getline(std::cin, file2_name);

    // Открытие файлов для записи
    int file1 = open(file1_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int file2 = open(file2_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file1 < 0 || file2 < 0) {
        perror("Can't open file");
        exit(1);
    }

    // Создание отображаемых файлов (shared memory objects)
    const char *shm_name1 = "/shm_child1";
    const char *shm_name2 = "/shm_child2";

    int shm_fd1 = shm_open(shm_name1, O_CREAT | O_RDWR, 0666);
    int shm_fd2 = shm_open(shm_name2, O_CREAT | O_RDWR, 0666);

    if (shm_fd1 == -1 || shm_fd2 == -1) {
        perror("Can't create shared memory object");
        exit(1);
    }

    // Установка размера отображаемых файлов
    ftruncate(shm_fd1, sizeof(shared_data));
    ftruncate(shm_fd2, sizeof(shared_data));

    // Отображение файлов в память
    shared_data *shm_ptr1 = (shared_data *) mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd1, 0);
    shared_data *shm_ptr2 = (shared_data *) mmap(NULL, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd2, 0);

    if (shm_ptr1 == MAP_FAILED || shm_ptr2 == MAP_FAILED) {
        perror("Can't mmap shared memory");
        exit(1);
    }

    // Инициализация семафоров
    sem_init(&shm_ptr1->sem_parent, 1, 0);
    sem_init(&shm_ptr1->sem_child, 1, 0);
    sem_init(&shm_ptr2->sem_parent, 1, 0);
    sem_init(&shm_ptr2->sem_child, 1, 0);

    // Установка флагов завершения работы
    shm_ptr1->terminate = 0;
    shm_ptr2->terminate = 0;

    // Создание дочерних процессов
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("Can't fork");
        exit(1);
    }

    if (pid1 == 0) {
        // Дочерний процесс 1
        // Закрытие неиспользуемых отображаемых файлов
        munmap(shm_ptr2, sizeof(shared_data));
        close(shm_fd2);

        // Перенаправление стандартного вывода в файл
        if (dup2(file1, STDOUT_FILENO) < 0) {
            perror("Can't redirect stdout for child process 1");
            exit(1);
        }
        close(file1);
        close(file2); // Не нужен в этом процессе

        execl("./child", "./child", shm_name1, NULL);
        perror("Can't execute child process 1");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("Can't fork");
        exit(1);
    }

    if (pid2 == 0) {
        // Дочерний процесс 2
        // Закрытие неиспользуемых отображаемых файлов
        munmap(shm_ptr1, sizeof(shared_data));
        close(shm_fd1);

        // Перенаправление стандартного вывода в файл
        if (dup2(file2, STDOUT_FILENO) < 0) {
            perror("Can't redirect stdout for child process 2");
            exit(1);
        }
        close(file2);
        close(file1); // Не нужен в этом процессе

        execl("./child", "./child", shm_name2, NULL);
        perror("Can't execute child process 2");
        exit(1);
    }

    // Родительский процесс
    close(file1);
    close(file2);

    while (true) {
        std::string s;
        std::getline(std::cin, s);

        if (s.empty()) {
            // Установка флагов завершения работы
            shm_ptr1->terminate = 1;
            shm_ptr2->terminate = 1;

            // Сигнализация дочерним процессам
            sem_post(&shm_ptr1->sem_parent);
            sem_post(&shm_ptr2->sem_parent);
            break;
        }

        if (s.size() > 10) {
            // Отправка данных дочернему процессу 2
            strcpy(shm_ptr2->buffer, s.c_str());
            sem_post(&shm_ptr2->sem_parent); // Сигнал дочернему процессу
            sem_wait(&shm_ptr2->sem_child);  // Ожидание завершения обработки
        } else {
            // Отправка данных дочернему процессу 1
            strcpy(shm_ptr1->buffer, s.c_str());
            sem_post(&shm_ptr1->sem_parent); // Сигнал дочернему процессу
            sem_wait(&shm_ptr1->sem_child);  // Ожидание завершения обработки
        }
    }

    // Ожидание завершения дочерних процессов
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    // Очистка ресурсов
    sem_destroy(&shm_ptr1->sem_parent);
    sem_destroy(&shm_ptr1->sem_child);
    sem_destroy(&shm_ptr2->sem_parent);
    sem_destroy(&shm_ptr2->sem_child);

    munmap(shm_ptr1, sizeof(shared_data));
    munmap(shm_ptr2, sizeof(shared_data));
    close(shm_fd1);
    close(shm_fd2);
    shm_unlink(shm_name1);
    shm_unlink(shm_name2);

    return 0;
}
