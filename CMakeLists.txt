CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

project(DesignerHelper)

SET(CMAKE_BUILD_TYPE Debug)

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Sql AxContainer Gui REQUIRED)

add_subdirectory(Counter)
add_subdirectory(DesignUtil)
add_subdirectory(Nomorator)
add_subdirectory(PolaroidMaker)
add_dependencies(PolaroidMaker Counter)

target_include_directories(PolaroidMaker PRIVATE Counter)
target_compile_definitions(PolaroidMaker PRIVATE POLAROIDMAKER_LIBRARY)
target_link_libraries(PolaroidMaker PUBLIC Qt5::Widgets Counter)

add_subdirectory(PolaroidUi)
add_dependencies(PolaroidUi PolaroidMaker Counter)
target_include_directories(PolaroidUi PRIVATE PolaroidMaker Counter)
target_link_libraries(PolaroidUi PRIVATE Qt5::Widgets PolaroidMaker Counter)