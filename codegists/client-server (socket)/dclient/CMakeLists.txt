#
#

add_executable(dclient
        main_client.cpp
        ClientHandler.cpp
        ../dcommon/Connect.cpp
        ../dcommon/Logger.h
)

# клиент для демона будет на ncurses
find_package(Curses REQUIRED)
# Подключаем заголовочные файлы и линкуем библиотеку к проекту
#   будет искать в: /usr/includes, /usr/local/includes и других
target_include_directories(dclient PRIVATE ${CURSES_INCLUDE_DIR})

# ncursesw - обязательна для кириллицы (wide - для UTF-8)
# panelw - панели добавляют окнам Z-глубину
#   будет искать в: /usr/lib, /usr/local/lib/ и других
target_link_libraries(dclient PRIVATE ncursesw panelw ${CURSES_LIBRARIES})



# общая либа (IPC итд)
target_link_libraries(dclient PRIVATE dcommon)





