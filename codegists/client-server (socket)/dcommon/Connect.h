//
// Created by admin on 18.09.2026.
//

#ifndef DTOP_CONNECT_H
#define DTOP_CONNECT_H

#include <unistd.h>
#include <iostream>
#include <cerrno>
#include <stdexcept>
#include <string>
#include <cstring>
#include <clocale>
#include <cstdio>
#include <poll.h>
#include <sys/socket.h>
#include <sys/un.h>

struct MetricsPacket;
struct ControlPacket;

/**
 * Создает сокет с режимом подключения connect (сторона клиента)
 */
class Connect {

public:

    int sock_fd = -1;

    explicit Connect(const std::string& path)
    {
        sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sock_fd==-1) {
            throw std::runtime_error("Ошибка создания сокета: "+std::string(std::strerror(errno)));
        }

        struct sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        std::strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path)-1);

        // Блокирующий вызов connect() — ждем сервер
        // @todo - сделать неблокирующим, т.е. ожидающим запуска сервера
        if (connect(sock_fd, (struct sockaddr*)& addr, sizeof(addr)) == -1) {
            close(sock_fd);
            throw std::runtime_error("Сервер не запущен или недоступен по пути: "+path);
        }
    }

    ~Connect()
    {
        if (sock_fd!=-1) {
            close(sock_fd);
        }
    }

    Connect(const Connect&) = delete;
    Connect& operator=(const Connect&) = delete;
};

#endif //DTOP_CONNECT_H
