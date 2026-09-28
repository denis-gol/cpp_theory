//
// Created by admin on 25.09.2026.
//

#ifndef DTOP_LOGGER_H
#define DTOP_LOGGER_H

// вызов логгера
//
// #include "Logger.h"
//
// Logger logger("CLIENT");
// logger.log_message("Client's log starts");
//
//

#include <iostream>
#include <fstream>

const char* FILEPATH = "ui.log";

// @todo - переписать на templates

class Logger {
private:
    std::string filepath = FILEPATH;
    std::string prefix;
    std::ofstream log_file;

public:
    explicit Logger(std::string prefix):
      prefix(std::move(prefix)),
      log_file(std::ofstream(filepath, std::ios::app))
    {}

    void log_message(const std::string& msg) {
        if (log_file.is_open()) {
            log_file << "[" << prefix << "] " << msg << std::endl;
        }
    }

    void log_message(const int msg) {
        if (log_file.is_open()) {
            log_file << "[" << prefix << "] " << msg << std::endl;
        }
    }
};

#endif //DTOP_LOGGER_H
