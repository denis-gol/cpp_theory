//
// Created by admin on 15.09.2026.
//

#ifndef DTOP_DAEMONHANDLER_H
#define DTOP_DAEMONHANDLER_H

enum State {
  RUNNING,  // демон работает полностью
  PAUSED    // работает, но пропускает тяжелый шаг парсингау
};

void daemonize();

#endif //DTOP_DAEMONHANDLER_H
