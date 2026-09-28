//
// Created by admin on 11.09.2026.
//
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <clocale>
#include <csignal>
#include <poll.h>
#include <sys/socket.h>


#include <ncursesw/ncurses.h>
#include <panel.h>

#include "dcommon.h"
#include "Connect.h"
#include "ClientHandler.h"
#include "Logger.h"


int main() {

    Logger logger("CLIENT");
    logger.log_message("Client's log starts");

    setlocale(LC_ALL, "");

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    timeout(0); // getch не блокирует поток
    start_color();
    //keypad(stdscr, TRUE);
    //set_escdelay(50);

    init_pair(1, COLOR_GREEN, COLOR_YELLOW);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);

    refresh();

    WINDOW* win_header = newwin(3,80,0,0);
    WINDOW* win_monitor = newwin(10,80,3,0);
    box(win_header, 0,0);
    box(win_monitor, 0,0);

    // рисуем шапку
    wprintw(win_header, "--- SYSTEM MONITOR ---\n");
    wprintw(win_header, "[P] Pause | [R] Replay | [K] Kill daemon | [Q] Quit UI\n");
    box(win_header, 0,0);
    wrefresh(win_header);


    logger.log_message("отрисовали шапку. Пробуем коннект к серверу...");


    Connect connect(SOCKET_PATH);

    logger.log_message("подключились к сокету");

    struct pollfd fds[1];
    fds[0].fd = connect.sock_fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    bool running = true;

    while (running) {

        // Handle user input...
        int ch = getch();
        if (ch != ERR) {
            ControlPacket cpack{};
            bool send_cmd = false;

            if (ch == 'p' || ch == 'P') {
                cpack.command_id = 1;
                send_cmd = true;
            }
            else if (ch == 'r' || ch == 'R') {
                cpack.command_id = 2;
                send_cmd = true;
            }
            else if (ch == 'k' || ch == 'K') {
                cpack.command_id = 3;
                send_cmd = true;
            }
            else if (ch == 'q' || ch == 'Q') {
                running = false;
            }

            if (send_cmd && connect.sock_fd > 0) {
                // вызов send упрятан в класс Connect
                send(connect.sock_fd, &cpack, sizeof(cpack), 0);
            }
        }

        int ret = poll(fds, 1, 50);
        // в буфере что-то есть
        if (ret > 0) {

            logger.log_message("в буфере что-то есть. revents: " + std::to_string(fds[0].revents));

            if (fds[0].revents & POLLIN) {

                logger.log_message("вошли в POLLIN");

                MetricsPacket metrics{};

                ssize_t bytes = recv(connect.sock_fd, &metrics, sizeof(metrics), 0);

                logger.log_message("получили из сокета " + std::to_string(bytes) + " байт");
                logger.log_message("metrics.status_text " + std::string(metrics.status_text)
                    + ". RAM usage:" + std::to_string(metrics.ram_usage)
                    + ". CPU usage:" + std::to_string(metrics.cpu_usage)
                );

                // Если == 0, значит демон закрыл соединение
                // Если -1, значит была ошибка сети
                if (bytes <= 0) {
                    running = false;
                    break;
                }

                werase(win_monitor);
                mvwprintw(win_monitor, 1, 2, "Кадр связи: #%05d", metrics.iteration);
                mvwprintw(win_monitor, 3, 2, "Статус демона: ");

                std::string status_str(metrics.status_text, sizeof(metrics.status_text));
                // красим статус
                if (status_str.find("RUNNING") != std::string::npos) {
                    wattron(win_monitor, COLOR_PAIR(1));
                    wprintw(win_monitor, "%s", metrics.status_text);
                    wattroff(win_monitor, COLOR_PAIR(1));
                } else {
                    wattron(win_monitor, COLOR_PAIR(2));
                    wprintw(win_monitor, "%s", metrics.status_text);
                    wattroff(win_monitor, COLOR_PAIR(2));
                }

                // рисуем бары
                draw_progress_bar(win_monitor, 5, 2, "Загрузка CPU", metrics.cpu_usage);
                draw_progress_bar(win_monitor, 6, 2, "Загрузка RAM", metrics.ram_usage);

                wrefresh(win_monitor);
            }

            if (fds[0].revents & (POLLERR | POLLHUP)) {

                logger.log_message("ошибка POLLERR|POLLHUP");

                running = false;
            }

            fds[0].revents = 0;
        }

        else if (ret < 0) {
            if (errno == EINTR) continue;

            logger.log_message("ошибка ret < 0");

            break;
        }

    }

    delwin(win_header);
    delwin(win_monitor);
    flushinp();
    endwin();

    return 0;
}




