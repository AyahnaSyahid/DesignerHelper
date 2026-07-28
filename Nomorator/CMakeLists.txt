add_executable(Nomorator main.cpp
                        mainwindow.cpp
                        mainwindow.h
                        mainwindow.ui
                        src/minmaxspinbox.h
                        src/minmaxspinbox.cpp
                        src/ncontroller.h
                        src/ncontroller.cpp
                        src/nomorator.h
                        src/nomorator.cpp
                        src/nomoratorscene.h
                        src/nomoratorscene.cpp
                        src/nomoratorview.h
                        src/nomoratorview.cpp
                        )
target_link_libraries(Nomorator PUBLIC Qt5::Widgets)
target_include_directories(Nomorator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src)

set_target_properties( Nomorator PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin" )