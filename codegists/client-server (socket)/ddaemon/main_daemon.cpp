//
// Created by admin on 11.09.2026.
//

#include <csignal> // sig_atomic_t
#include <thread> // sleep_for
#include <fstream> // std::ofstream

#include "DaemonGuard.h"
#include "DaemonHandler.h"
#include "Bind.h"
#include "dcommon.h"
#include "Logger.h"

// атомик в C-style. Здесь нужен, т.к. мы обрабатываем сигналы (они асинхронны)
volatile sig_atomic_t g_running = 1;

//// пишем сюда (@todo - временно, демонстрация работы демона)
//const char* const LOG_FILE = "/tmp/ddaemon.log";
//
//// Функция для записи логов в файл по абсолютному пути (@todo - временно, демонстрация работы демона)
//void log_message(const std::string& message) {
//    std::ofstream log(LOG_FILE, std::ios_base::app);
//    if (log.is_open()) {
//        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//        std::string time_str = std::ctime(&now);
//        time_str.pop_back(); // Удаляем символ переноса строки из ctime
//        log << "[" << time_str << "] " << message << std::endl;
//    }
//}

#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <poll.h>
#include <cstring>

int main() {

    // @frag - вместо демона запускаем обычный процесс в терминале и изучаем работу сокетов

    std::cout << "[SERVER] Запуск тестового сервера...\n";
    Logger logger("SERVER");
    logger.log_message("Daemon's log starts");

    Bind bind(SOCKET_PATH);

    std::cout << "[SERVER] Ожидание подключения UI-клиента в соседнем терминале...\n";

    int client_fd = bind.accept_connection();
    std::cout << "[SERVER] UI-клиент успешно подключился!";
    logger.log_message("клиент успешно подключился. client_fd=" + std::to_string(client_fd));

    // Настраиваем poll для сервера, чтобы читать команды без зависания
    struct pollfd fds[1];
    fds[0].fd = client_fd;
    fds[0].events = POLLIN;

    uint32_t iter = 0;
    bool is_paused = false;
    bool running = true;

    while (running) {
        // Проверяем, прислал ли UI какую-то команду (таймаут 0 — проверяем мгновенно)
        int ret = poll(fds, 1, 0);
        if (ret>0 && (fds[0].revents & POLLIN)) {
            ControlPacket cpack{};
            ssize_t bytes = recv(client_fd, &cpack, sizeof(cpack), 0);

            if (bytes<=0) {
                std::cout << "[SERVER] UI-клиент разорвал соединение.\n";
                break;
            }

            // Выводим полученную команду прямо в терминал сервера!
            std::cout << "[SERVER] Получена команда ID: " << (int) cpack.command_id << " -> ";
            if (cpack.command_id==1) {
                std::cout << "Пауза\n";
                is_paused = true;
            }
            else if (cpack.command_id==2) {
                std::cout << "Возобновление\n";
                is_paused = false;
            }
            else if (cpack.command_id==3) {
                std::cout << "Завершение работы сервера по требованию UI!\n";
                running = false;
                break;
            }
        }

        // Если мы не на паузе, генерируем «метрики»
        if (!is_paused) {
            iter++;
        }

        MetricsPacket metrics{};
        metrics.iteration = iter;
        // Генерируем фейковую пилообразную загрузку для теста баров
        metrics.cpu_usage = (iter*7)%101;
        metrics.ram_usage = (40+(iter%30));

        if (is_paused) {
            std::strncpy(metrics.status_text, "PAUSED", sizeof(metrics.status_text));
        }
        else {
            std::strncpy(metrics.status_text, "RUNNING", sizeof(metrics.status_text));
        }

        // Отправляем пакет с метриками UI-клиенту
        send(client_fd, &metrics, sizeof(metrics), 0);

        // Имитируем шаг измерения в 200 миллисекунд
        usleep(2e5);
    }

    // Закрываем дескрипторы и чистим за собой файловую систему
    close(client_fd);
    // close(server_fd); // больше не надо, перешли на RAII
//        unlink(SOCKET_PATH);
    std::cout << "[SERVER] Сервер успешно остановлен.\n";


    // @frag - вместо демона запускаем обычный процесс в терминале и изучаем работу сокетов
//    daemonize();
//    DaemonGuard guard("dtop");
//
//    int iter = 0;
//
//    while (g_running) {
//        // Имитация полезной работы
//        log_message("демон работает. " + std::to_string(iter++));
//        std::this_thread::sleep_for(std::chrono::seconds(2));
//
//    }

    return 0;
}
