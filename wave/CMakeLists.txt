cmake_minimum_required(VERSION 3.19)
project(wave LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

include_directories("C:/dev/ffmpeg/include")
link_directories("C:/dev/ffmpeg/lib")

qt_add_executable(wave
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    mp4creator.h mp4creator.cpp
)

target_link_libraries(wave
    PRIVATE
    Qt::Core
    Qt::Widgets
    avcodec
    avdevice
    avfilter
    avformat
    avutil
    swresample
    swscale
)

include(GNUInstallDirs)

install(TARGETS wave
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET wave
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
