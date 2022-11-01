cmake_minimum_required(VERSION 3.24)

project(ParsingStateMachine)

Include(FetchContent)
FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG        v3.0.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)

add_library(${PROJECT_NAME}
	src/ParsingStateMachine.h
	src/ParsingStateMachine.cpp
)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

add_executable("${PROJECT_NAME}Test"
	test/ParsingStateMachineTest.cpp
)
target_include_directories("${PROJECT_NAME}Test"
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src
)
set_target_properties("${PROJECT_NAME}Test" PROPERTIES CXX_STANDARD 20)
target_link_libraries("${PROJECT_NAME}Test" PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests("${PROJECT_NAME}Test")

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin/ VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
