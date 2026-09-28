//
// Created by admin on 18.09.2026.
//

#ifndef DTOP_DCOMMON_H
#define DTOP_DCOMMON_H

#include <cstdint>

// Путь к файлу сокета в системе
const char* const SOCKET_PATH = "/tmp/sim_server.sock";

// Daemon --> UI-Client
struct MetricsPacket {
  uint32_t iteration;       // Номер кадра/секунды для проверки связи
  uint8_t  cpu_usage;       // Тестовое число от 0 до 100
  uint8_t  ram_usage;       // Тестовое число от 0 до 100
  char     status_text[32]; // Текстовый статус работы демона (например, "RUNNING" или "PAUSED")
};

// UI-Client --> Daemon
struct ControlPacket {
  uint8_t command_id; // 1 - Стоп, 2 - Пауза, 3 - Старт, 4 - Изменить интервал
  uint32_t argument;  // Дополнительный параметр (например, 500 для 500мс)
};

// @todo - позже. Использовать хэдер как проверку - что за данные к нам пришли? Идентификация пакета по magic.
// Общий заголовок для всех пакетов в системе
//struct PacketHeader {
//  uint16_t magic;    // Уникальная метка протокола, например 0x4D53 (от "MS" - Monitor System)
//  uint8_t  packet_type; // Идентификатор структуры: 1 = ControlPacket, 2 = MetricsPacket
//  uint16_t length;   // Длина данных, которые идут следом
//};

// Теперь пакет управления выглядит так:
//struct ControlPacket {
//  PacketHeader header;
//  uint8_t      command_id;
//};





#endif //DTOP_DCOMMON_H
