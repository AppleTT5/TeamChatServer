include("C:/Utility/Project/my_works/TeamChatServer/out/build/debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/TeamChatServer-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase;qtwebsockets")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "C:/Utility/Project/my_works/TeamChatServer/out/build/debug/TeamChatServer.exe"
    GENERATE_QT_CONF
)
