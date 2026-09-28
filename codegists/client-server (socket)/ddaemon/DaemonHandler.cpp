//
// Created by admin on 15.09.2026.
//
#include <csignal> // setsid, sig_atomic_t, SIGTERM etc
#include <unistd.h> // chdir, fork, dup2
#include <sys/stat.h> // umask
#include <fstream> // std::ofstream, EXIT_FAILURE
#include <sys/file.h> // open, flock

#include "DaemonHandler.h"


extern volatile sig_atomic_t g_running;

void signal_handler(int signal) {
    if (signal == SIGTERM || signal == SIGINT) {
        g_running = false;
    }
}

/**
 * Создание демона (Unix-way)
 */
void daemonize() {
    pid_t pid;

    // первый fork (нужно отвязаться от терминала)
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    // завершаем parent
    if (pid > 0) exit(EXIT_SUCCESS);
    // здесь уже работает child_1

    // создаем сессию
    if (setsid() < 0) exit(EXIT_FAILURE);

    // Перехватываем сигналы остановки
    std::signal(SIGTERM, signal_handler);
    std::signal(SIGINT, signal_handler); // Ctrl+C
    std::signal(SIGHUP, SIG_IGN); // Игнорируем закрытие терминала, если оно прилетит

    // второй форк (от захвата tty)
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    // завершаем первого потомка
    if (pid > 0) exit(EXIT_SUCCESS);
    // здесь уже работает child_2

    // сброс маски прав
    umask(0);

    // меняем cwd на рут (чтобы не сломать, вдруг umount)
    if (chdir("/") < 0) exit(EXIT_FAILURE);

    // закрываем доступ стандартным потокам
    int null_fd = open("/dev/null", O_RDWR);
    if (null_fd != -1) {
        // безопасно клонируем дескрипторы
        if (dup2(null_fd, STDIN_FILENO) < 0)  // 0 -> /dev/null
            exit(EXIT_FAILURE);
        if (dup2(null_fd, STDOUT_FILENO) < 0) // 1 -> /dev/null
            exit(EXIT_FAILURE);
        if (dup2(null_fd, STDERR_FILENO) < 0) // 2 -> /dev/null
            exit(EXIT_FAILURE);

        close(null_fd);
    }
}
