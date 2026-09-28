//
// Created by admin on 18.09.2026.
//

#ifndef DTOP_HANDLER_H
#define DTOP_HANDLER_H


#include <ncursesw/ncurses.h>
#include <string>

void draw_progress_bar(WINDOW* win, int x, int y, const std::string& label, uint8_t percentage);


#endif //DTOP_HANDLER_H
