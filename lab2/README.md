# Четно-Нечетная Параллельная Сортировка

## Сборка
```bash
g++ -std=c++20 -pthread sort.cpp -o sort
```
## Запуск
```bash
time ./sort 4 test.txt output.txt
```
## Просмотр количества потоков
```bash
ps aux | grep sort

ps -o nlwp <PID>
```