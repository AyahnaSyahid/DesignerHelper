file(GLOB POLMAKERSOURCES *.cpp *.h *.ui)

add_library(PolaroidMaker SHARED ${POLMAKERSOURCES} )

set_target_properties(PolaroidMaker PROPERTIES 
                      OUTPUT_NAME "PolaroidMaker"
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
                      PREFIX "")

install(TARGETS PolaroidMaker
        RUNTIME DESTINATION bin)