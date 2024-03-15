cmake_minimum_required(VERSION 3.22)

project(ParsingStateMachine)

Include(FetchContent)
FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG v3.5.3 # or a later release
)
FetchContent_MakeAvailable(Catch2)

add_library(${PROJECT_NAME} STATIC
	src/rule/AnyRule.h
	src/rule/CharRule.h
	src/rule/DiscardRule.h
	src/rule/EndRule.h
	src/rule/IfRule.h
	src/rule/NotRule.h
	src/rule/OneRule.h
	src/rule/OptRule.h
	src/rule/PlusRule.h
	src/rule/RangeRule.h
	src/rule/ReparseRule.h
	src/rule/RepRule.h
	src/rule/SeqRule.h
	src/rule/StarRule.h
	src/rule/SuccessRule.h
	src/rule/UntilRule.h
	src/LogTraceAction.h
	src/Parser.h
	src/Parser.cpp
	src/Rule.h
	src/StandardRules.h
)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
target_include_directories(${PROJECT_NAME}
	PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)

add_executable("${PROJECT_NAME}Test"
	test/rule/AnyRuleTest.cpp
	test/rule/CharRuleTest.cpp
	test/rule/DiscardRuleTest.cpp
	test/rule/EndRuleTest.cpp
	test/rule/IfRuleTest.cpp
	test/rule/NotRuleTest.cpp
	test/rule/OneRuleTest.cpp
	test/rule/OptRuleTest.cpp
	test/rule/PlusRuleTest.cpp
	test/rule/RangeRuleTest.cpp
	test/rule/ReparseRuleTest.cpp
	test/rule/RepRuleTest.cpp
	test/rule/SeqRuleTest.cpp
	test/rule/StarRuleTest.cpp
	test/rule/UntilRuleTest.cpp
	test/GeneratorTest.cpp
	test/JsonTest.cpp
	test/ParsingStateMachineTest.cpp
)
set_target_properties("${PROJECT_NAME}Test" PROPERTIES CXX_STANDARD 20)
target_link_libraries("${PROJECT_NAME}Test" PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})

include(CTest)
include(Catch)
catch_discover_tests("${PROJECT_NAME}Test")

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin/ VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
