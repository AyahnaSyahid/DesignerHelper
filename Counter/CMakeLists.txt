add_library(Counter SHARED counter.h
                           counter.cpp
                           Counter_global.h
                           dialogdetailcounter.cpp
                           dialogdetailcounter.h
                           dialogdetailcounter.ui
                           refillform.cpp
                           refillform.h
                           refillform.ui
                           refillrequest.cpp
                           refillrequest.h
                           refillrequest.ui
                           )
target_compile_definitions(Counter PRIVATE COUNTER_LIBRARY)
target_link_libraries(Counter PUBLIC Qt5::Widgets Qt5::Sql)
set_target_properties(Counter PROPERTIES OUTPUT_NAME "Counter"
                      PREFIX "")

install(TARGETS Counter 
        RUNTIME DESTINATION bin)