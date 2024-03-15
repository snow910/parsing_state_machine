#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "AnyRule test", "[AnyRule][psm]" )
{
	{
		Parser< Any > p;
		CHECK( p.parse( "0123" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "0" ) } );
	}

	{
		Parser< AnyN< 5 > > p;
		CHECK( p.parse( "q1_*@0GG42" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "q1_*@" ) } );
	}

	{
		Parser< AnyN< 5 > > p;
		CHECK( p.parse( "q1_0", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "q1_0@0GG42", false ) == ParsingResult{ ParsingStatus::Success, std::string_view( "q1_0@" ) } );
	}
}