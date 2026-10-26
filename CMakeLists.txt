cmake_minimum_required(VERSION 3.16)
project(TeamChatServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Путь к Qt задаётся через пресеты (QTDIR), переменную окружения или флаг:
#   cmake -DCMAKE_PREFIX_PATH="D:/QT_Online/6.11.1/msvc2022_64" ...
#   cmake -DCMAKE_PREFIX_PATH="$env{QTDIR}" --preset Debug-x64
# Хардкод пути убран: мешал сборке на других машинах и ОС.

find_package(Qt6 REQUIRED COMPONENTS Core Network WebSockets)

qt_standard_project_setup()

# MSVC: исходники в UTF-8
if(MSVC)
    add_compile_options(/utf-8)
endif()

set(PROJECT_SOURCES
    main.cpp
    ChatServer.h
    ChatServer.cpp
    message.h
    message.cpp
    user_message.h
    user_message.cpp
    voice_message.h
    voice_message.cpp
    system_message.h
    system_message.cpp
    chanel.h
    chanel.cpp
    system_messages/sm_server_info.h
    system_messages/sm_server_info.cpp
    system_messages/sm_chanels_info.h
    system_messages/sm_chanels_info.cpp
    system_messages/sm_chanel_history.h
    system_messages/sm_chanel_history.cpp
    system_messages/sm_get_chanel_history.h
    system_messages/sm_get_chanel_history.cpp
    system_messages/sm_commands.h
    system_messages/sm_commands.cpp
)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Network
        Qt6::WebSockets
)

# --- Развёртывание ---

# 1. Установка в правильную папку
install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 2. Генерируем скрипт развертывания
qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

# 3. Устанавливаем скрипт с абсолютным путем
install(SCRIPT ${deploy_script})
