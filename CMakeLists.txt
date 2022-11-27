cmake_minimum_required(VERSION 3.22)

project(ParsingStateMachine)

Include(FetchContent)
FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG        v3.0.1 # or a later release
)
FetchContent_MakeAvailable(Catch2)

add_library(${PROJECT_NAME} INTERFACE
	src/rule/AnyRule.h
	src/rule/CharRule.h
	src/rule/EndRule.h
	src/rule/IfRule.h
	src/rule/NotRule.h
	src/rule/OneRule.h
	src/rule/OptRule.h
	src/rule/PlusRule.h
	src/rule/RangeRule.h
	src/rule/RepRule.h
	src/rule/SeqRule.h
	src/rule/StarRule.h
	src/rule/UntilRule.h
	src/Rule.h
	src/StandardRules.h
	src/Parser.h
	src/StdLogTraceAction.h
)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
target_include_directories(${PROJECT_NAME}
	INTERFACE ${CMAKE_CURRENT_LIST_DIR}
)

add_executable("${PROJECT_NAME}Test"
	test/ParsingStateMachineTest.cpp
)
set_target_properties("${PROJECT_NAME}Test" PROPERTIES CXX_STANDARD 20)
target_link_libraries("${PROJECT_NAME}Test" PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})

include(CTest)
include(Catch)
catch_discover_tests("${PROJECT_NAME}Test")

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin/ VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
