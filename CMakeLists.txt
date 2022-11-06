cmake_minimum_required (VERSION 3.0.0)
project (RedArchiever
    VERSION 0.1.0
    LANGUAGES CXX
)

set (CMAKE_CXX_STANDARD 17)

#######################
# Archiver
#######################
file (GLOB ${PROJECT_NAME}_SOURCES Sources/*.cpp)
add_executable(RedArchiever ${${PROJECT_NAME}_SOURCES} Main.cpp)

#######################
# GTest
#######################
ADD_SUBDIRECTORY (googletest)
enable_testing ()
include_directories ("googletest/googletest/include" ${gtest_SOURCE_DIR})

#######################
# Unit Tests Archiver
#######################
file(GLOB ${PROJECT_NAME}_TEST_SOURCES Sources/Tests/*.cpp)
add_executable(test_${PROJECT_NAME} ${${PROJECT_NAME}_TEST_SOURCES} ${${PROJECT_NAME}_SOURCES})
target_link_libraries(test_${PROJECT_NAME} gtest pthread)