cmake_minimum_required(VERSION 3.17)
project(kp)

set(CMAKE_CXX_STANDARD 20)

# Поиск необходимых пакетов
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

# Использование pkg-config для ZeroMQ
pkg_check_modules(ZMQ REQUIRED libzmq)

# Добавление исполняемых файлов
add_executable(server
    server.cpp
    game.h
    player.h
    myMQ.h
)

add_executable(client
    client.cpp
    game.h
    player.h
    myMQ.h
)

# Включение директорий для заголовочных файлов
target_include_directories(server PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(client PRIVATE ${ZMQ_INCLUDE_DIRS})

# Линковка с библиотеками
target_link_libraries(server ${ZMQ_LIBRARIES} Threads::Threads)
target_link_libraries(client ${ZMQ_LIBRARIES} Threads::Threads)
