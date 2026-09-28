//
// Created by admin on 15.09.2026.
//
#ifndef DTOP_DAEMONGUARD_H
#define DTOP_DAEMONGUARD_H

#include <string>

class DaemonGuard {
private:
    int lock_fd = -1;

    // файл для системы - флаг блокировки
    const std::string lock_path;

    // файл для админа - в нем будет лежать PID демона
    const std::string pid_path;

public:
    explicit DaemonGuard(const std::string& app_name);
    ~DaemonGuard();

    DaemonGuard(const DaemonGuard&) = delete;
    DaemonGuard& operator=(const DaemonGuard&) = delete;
};

#endif //DTOP_DAEMONGUARD_H
