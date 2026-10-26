cmake_minimum_required(VERSION 3.16)
project(TeamChatServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Путь к Qt (если не находится автоматически)
set(CMAKE_PREFIX_PATH "D:/QT_Online/6.11.1/msvc2022_64" ${CMAKE_PREFIX_PATH})

find_package(Qt6 REQUIRED COMPONENTS Core WebSockets)

qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp
    ChatServer.h
    ChatServer.cpp
    message.h
    message.cpp
    user_message.h
    user_message.cpp
    text_chanel.h
    text_chanel.cpp
    system_message.h
    system_message.cpp
    system_messages/sm_server_info.h
    system_messages/sm_server_info.cpp
    system_messages/sm_chanels_info.h
    system_messages/sm_chanels_info.cpp
    system_messages/sm_chanel_history.h
    system_messages/sm_chanel_history.cpp
    system_messages/sm_get_chanel_history.h
    system_messages/sm_get_chanel_history.cpp
)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt6::Core
        Qt6::WebSockets
)

# --- ПРАВИЛЬНОЕ РАЗВЕРТЫВАНИЕ ---

# 1. Указываем установку в правильную папку
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