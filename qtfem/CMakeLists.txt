cmake_minimum_required(VERSION 3.19)
project(qtfem LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

include_directories("C:/dev/eigen")

qt_add_executable(qtfem
    main.cpp
)

find_package(Qt6 ${USED_QT_VERSION} REQUIRED COMPONENTS
    Core
    Widgets
    Multimedia
    MultimediaWidgets
)

target_link_libraries(qtfem
    PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Multimedia
    Qt::MultimediaWidgets
)

include(GNUInstallDirs)

install(TARGETS qtfem
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET qtfem
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
