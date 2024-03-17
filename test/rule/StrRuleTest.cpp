#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

TEST_CASE( "StrRule test", "[StrRule][psm]" )
{
	{
		Parser< Str< 'A', 'B', 'C' > > p;
		CHECK( p.parse( "ABCDE" ) == ParsingResult{ ParsingStatus::Success, "ABC"sv } );

		CHECK( p.parse( "A", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "AB", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "ABC", false ) == ParsingResult{ ParsingStatus::Success, "ABC"sv } );

		CHECK( p.parse( "0ABCD", false ).status == ParsingStatus::Fail );
	}

	{
		Parser< One< Str< 'Z', 'B', '0' >, Str< 'A', 'B', '0' >, Str< 'A', 'B', '1' > > > p;
		CHECK( p.parse( "AB1" ) == ParsingResult{ ParsingStatus::Success, "AB1"sv } );
	}
}