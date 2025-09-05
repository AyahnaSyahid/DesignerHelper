# add_executable(PolaroidUi WIN32 main.cpp
                          # res/polmakerui.rc)
add_executable(PolaroidUi main.cpp
                          res/polmakerui.rc)
# add_executable(PolaroidUi WIN32 main.cpp)

install(TARGETS PolaroidUi
        RUNTIME DESTINATION bin)