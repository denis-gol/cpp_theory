#
#

add_executable(ddaemon
        main_daemon.cpp
        DaemonGuard.cpp
        DaemonGuard.h
        DaemonHandler.cpp
        DaemonHandler.h
        ../dcommon/Logger.h
)

# подключаем общую часть
target_link_libraries(ddaemon PRIVATE dcommon)

# если вдруг использовать потоки (пока не планировал)
#find_package(Threads REQUIRED)
#target_link_libraries(ddaemon PRIVATE Threads::Threads)
