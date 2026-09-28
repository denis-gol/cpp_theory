//
// Created by admin on 15.09.2026.
//

#include "DaemonGuard.h"

#include <fstream> // std::ofstream
#include <stdexcept>
#include <sys/file.h> // open, flock
#include <unistd.h> // close
#include <cerrno>


DaemonGuard::DaemonGuard(const std::string& app_name)
    : lock_path("/tmp/" + app_name + ".lock")
    , pid_path("/tmp/" + app_name + ".pic")
{
    lock_fd = open(lock_path.c_str(), O_RDWR | O_CREAT, 0644);
    if (lock_fd == -1) {
        throw std::runtime_error("[ERROR] Can't open lock-file.");
    }

    if (flock(lock_fd, LOCK_EX | LOCK_NB) == -1) { // эксклюзивный неблокирующий доступ
        if (errno == EWOULDBLOCK) {
            close(lock_fd);
            throw std::runtime_error("[ERROR] Daemon already running.");
        }
        close(lock_fd);
        throw std::runtime_error("[ERROR] System error flock().");
    }

    // затрет старый файл, если был
    std::ofstream pid_file(pid_path);
    if (!pid_file.is_open()) {
        close(lock_fd);
        throw std::runtime_error("[ERROR] Can't create pid-file.");
    }

    // кладем в него PID текущего процесса
    pid_file << getpid() << "\n";
    pid_file.close();
}

DaemonGuard::~DaemonGuard() {
    if (lock_fd != -1) {
        unlink(pid_path.c_str());
        // просто закрываем дескриптор. Ядро само снимет блокировку,
        // а файл пусть остается - его удаление может привести к гонке.
        close(lock_fd);
    }
}
