# Четно-Нечетная Параллельная Сортировка

## Сборка
```bash
g++ -std=c++20 -pthread sort.cpp -o sort
```
## Запуск
```bash
time ./sort -t 4 -i test.txt -o output.txt
```
## Просмотр количества потоков
```bash
ps aux | grep sort

ps -o nlwp <PID>
```