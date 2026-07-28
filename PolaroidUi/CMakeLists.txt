if($<STREQUAL:$<STRING:TOUPPER,${CMAKE_BUILD_TYPE}>,DEBUG>)
    message(STATUS "${CMAKE_BUILD_TYPE} build detected")
    add_executable(PolaroidUi main.cpp
                            res/polmakerui.rc)
else()    
    message(STATUS "${CMAKE_BUILD_TYPE} build detected")
    if (WIN32)
        add_executable(PolaroidUi WIN32 main.cpp
                                res/polmakerui.rc)
    else()
        add_executable(PolaroidUi main.cpp
                            res/polmakerui.rc)
    endif()
endif()

set_target_properties(PolaroidUi PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

install(TARGETS PolaroidUi
        RUNTIME DESTINATION bin)