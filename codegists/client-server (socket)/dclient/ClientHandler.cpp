//
// Created by admin on 18.09.2026.
//


#include "ClientHandler.h"

void draw_progress_bar(WINDOW* win, int x, int y, const std::string& label, uint8_t percentage)
{
    // @todo - Заглушка функции отрисовки
    const int BAR_WIDTH = 20;
    mvwprintw(win, x, y, "%s: [%d%%]", label.c_str(), percentage);



}
