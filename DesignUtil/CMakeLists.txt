add_library(DesignUtil DesignUtil.h
                       DesignUtil.cpp
                       DesignUtil_global.h
                       )
target_compile_definitions(DesignUtil PRIVATE DESIGNUTIL_LIBRARY=1)
target_link_libraries(DesignUtil PUBLIC Qt5::Widgets Qt5::Sql)