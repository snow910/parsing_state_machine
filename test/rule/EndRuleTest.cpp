#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "EndRule test", "[EndRule][psm]" )
{
	Parser< Seq< Str< 'A', 'B', 'C' >, End > > p;
	CHECK( p.parse( "ABCDEF" ).status == ParsingStatus::Fail );
	CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "ABC" ) } );
}