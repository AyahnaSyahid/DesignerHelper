file(GLOB POLMAKERSOURCES *.cpp *.h *.ui)

add_library(PolaroidMaker SHARED ${POLMAKERSOURCES} )

set_target_properties(PolaroidMaker PROPERTIES 
                      OUTPUT_NAME "PolaroidMaker"
                      PREFIX "")

install(TARGETS PolaroidMaker
        RUNTIME DESTINATION bin)