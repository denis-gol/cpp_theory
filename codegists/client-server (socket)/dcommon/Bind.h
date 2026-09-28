//
// Created by admin on 25.09.2026.
//

#ifndef DTOP_BIND_H
#define DTOP_BIND_H

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
 * Создает сокет с режимом подключения bind (сторона сервера)
 *      Создает файл типа s, переключает сокет в режим прослушивания
 */
class Bind {
private:
    int sock_fd = -1;

public:
    explicit Bind(const std::string& path)
    {
        sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sock_fd==-1) {
            throw std::runtime_error("Ошибка создания сокета: "+std::string(std::strerror(errno)));
        }

        struct sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        std::strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path)-1);

        // Обязательно удаляем старый файл сокета, если он остался от прошлого запуска
        unlink(path.c_str());

        // биндим адрес к сокету (создается файл)
        if (bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
            std::perror("[SERVER] bind() failed");
            close(sock_fd);
            throw std::runtime_error("Сервер не запущен или недоступен по пути: "+path);
        }

        if (listen(sock_fd, 5) == -1) {
            std::perror("[SERVER] listen() failed");
            close(sock_fd);
            throw std::runtime_error("Ошибка при переключении в режим прослушивания listen.");
        }
    }

    ~Bind()
    {
        if (sock_fd!=-1) {
            close(sock_fd);
        }
    }

    Bind(const Bind&) = delete;
    Bind& operator=(const Bind&) = delete;

    // Блокирующий вызов accept(). Процесс «заснет», пока UI не вызовет connect()
    // @todo - можно тоже сделать неблокирующим! Через fcntl + poll
    int accept_connection() {
        int client_fd = ::accept(sock_fd, nullptr, nullptr);
        if (client_fd == -1) {
            std::perror("[SERVER] accept() failed");
            close(sock_fd);
            sock_fd = -1; // после close, все еще содержит число. Вручную указываем, что уже неактуально!
            throw std::runtime_error("Ошибка при подключении (accept).");
        }
        return client_fd;
    };

};

#endif //DTOP_BIND_H
