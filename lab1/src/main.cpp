#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

// Функция для записи строки в pipe
void to_pipe(int* fd, const std::string& str) {
    if (write(fd[1], str.c_str(), str.size()) < 0) {
        perror("Can't write to the pipe");
        exit(4);
    }
    char null_terminator = '\0'; // передаём null-терминатор
    if (write(fd[1], &null_terminator, sizeof(char)) < 0) {
        perror("Can't write to the pipe");
        exit(4);
    }
}

int main() {
    std::cout << "Enter file's name for child process 1: ";
    std::string file1_name;
    std::getline(std::cin, file1_name);

    std::cout << "Enter file's name for child process 2: ";
    std::string file2_name;
    std::getline(std::cin, file2_name);

    // Открытие файлов для дочерних процессов
    int file1 = open(file1_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int file2 = open(file2_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file1 < 0 || file2 < 0) {
        perror("Can't open file");
        exit(1);
    }

    // Создание двух каналов
    int fd1[2], fd2[2];
    if (pipe(fd1) < 0 || pipe(fd2) < 0) {
        perror("Can't create pipe");
        exit(2);
    }

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("Can't create child process");
        exit(3);
    }

    if (pid1 > 0) { // Родительский процесс
        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("Can't create child process");
            exit(3);
        }

        if (pid2 > 0) { // Родительский процесс
            close(fd1[0]);
            close(fd2[0]);

            while (true) {
                std::string s;
                std::getline(std::cin, s);

                if (s.empty()) { // Завершение работы при пустой строке
                    to_pipe(fd1, s);
                    to_pipe(fd2, s);
                    break;
                }

                // Если длина строки больше 10 символов, отправляем её в pipe2, иначе — в pipe1
                if (s.size() > 10) {
                    to_pipe(fd2, s);
                } else {
                    to_pipe(fd1, s);
                }
            }

            close(fd1[1]);
            close(fd2[1]);

        } else { // Дочерний процесс 2
            close(fd1[0]);
            close(fd1[1]);
            close(fd2[1]);

            if (dup2(fd2[0], STDIN_FILENO) < 0) {
                perror("Can't redirect stdin for child process");
                exit(5);
            }
            if (dup2(file2, STDOUT_FILENO) < 0) {
                perror("Can't redirect stdout for child process");
                exit(5);
            }
            execl("./child", "./child", NULL);

            perror("Can't execute child process");
            exit(6);
        }
    } else { // Дочерний процесс 1
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);

        if (dup2(fd1[0], STDIN_FILENO) < 0) {
            perror("Can't redirect stdin for child process");
            exit(5);
        }
        if (dup2(file1, STDOUT_FILENO) < 0) {
            perror("Can't redirect stdout for child process");
            exit(5);
        }
        execl("./child", "./child", NULL);

        perror("Can't execute child process");
        exit(6);
    }

    return 0;
}
