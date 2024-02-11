#构建目标
get_filename_component(BUILD_TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_executable(${BUILD_TARGET_NAME})

#目标包含目录
target_include_directories(${BUILD_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
#目标源文件
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src PRIVATE_SRC)
target_sources(${BUILD_TARGET_NAME} PRIVATE ${PRIVATE_SRC})

#Qt
find_package(Qt6Core CONFIG REQUIRED)
target_link_libraries(${BUILD_TARGET_NAME} PRIVATE Qt::Core Qt6::Core Qt::CorePrivate Qt6::CorePrivate)
find_package(Qt6Gui CONFIG REQUIRED)
target_link_libraries(${BUILD_TARGET_NAME} PRIVATE Qt::Gui Qt6::Gui Qt::GuiPrivate Qt6::GuiPrivate)
find_package(Qt6Widgets CONFIG REQUIRED)
target_link_libraries(${BUILD_TARGET_NAME} PRIVATE Qt::Widgets Qt6::Widgets Qt::WidgetsPrivate Qt6::WidgetsPrivate)
file(GLOB UI_FILES ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui)#获取UI文件路径
qt_wrap_ui(UI_H ${UI_FILES})#打包UI文件
target_sources(${BUILD_TARGET_NAME} PRIVATE ${UI_H})#编译打包的UI文件
target_include_directories(${BUILD_TARGET_NAME} PRIVATE ${CMAKE_BINARY_DIR}/${BUILD_TARGET_NAME})#包含编译UI文件的输出目录
#LibHV
find_package(libhv CONFIG REQUIRED)
target_link_libraries(${BUILD_TARGET_NAME} PRIVATE hv_static)
