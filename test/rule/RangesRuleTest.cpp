#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

TEST_CASE( "RangeRule test", "[RangeRule][psm]" )
{
	{
		Parser< Range< 'A', 'Z' > > p;
		CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingStatus::Success, "A"sv } );
		CHECK( p.parse( "BCD" ) == ParsingResult{ ParsingStatus::Success, "B"sv } );
		CHECK( p.parse( "CDE" ) == ParsingResult{ ParsingStatus::Success, "C"sv } );
		CHECK( p.parse( "ZAB" ) == ParsingResult{ ParsingStatus::Success, "Z"sv } );
		CHECK( p.parse( "abc" ).status == ParsingStatus::Fail );
	}

	{
		Parser< Star< Range< 'A', 'Z' > > > p;
		CHECK( p.parse( "A", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "AB", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "ABC", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingStatus::Success, "ABC"sv } );
		CHECK( p.parse( "ABC012", false ) == ParsingResult{ ParsingStatus::Success, "ABC"sv } );
	}
}

TEST_CASE( "RangesRule test", "[RangesRule][psm]" )
{
	{
		Parser< Ranges< 'A', 'Z', 'a', 'z' > > p;
		CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingStatus::Success, "A"sv } );
		CHECK( p.parse( "BCD" ) == ParsingResult{ ParsingStatus::Success, "B"sv } );
		CHECK( p.parse( "cde" ) == ParsingResult{ ParsingStatus::Success, "c"sv } );
		CHECK( p.parse( "zab" ) == ParsingResult{ ParsingStatus::Success, "z"sv } );
		CHECK( p.parse( "123" ).status == ParsingStatus::Fail );
	}
	{
		Parser< Star< Ranges< 'A', 'Z', 'a', 'z' > > > p;
		CHECK( p.parse( "ABCdef012" ) == ParsingResult{ ParsingStatus::Success, "ABCdef"sv } );
		CHECK( p.parse( "ABC", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "ABCdef", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "ABCdef012", false ) == ParsingResult{ ParsingStatus::Success, "ABCdef"sv } );
	}

	{
		Parser< NotRanges< 'A', 'Z', 'a', 'z' > > p;
		CHECK( p.parse( "123abc" ) == ParsingResult{ ParsingStatus::Success, "1"sv } );
		CHECK( p.parse( "ABC" ).status == ParsingStatus::Fail );
		CHECK( p.parse( "abc" ).status == ParsingStatus::Fail );
	}
	{
		Parser< Star< NotRanges< 'A', 'Z', 'a', 'z' > > > p;
		CHECK( p.parse( "123ABCabc" ) == ParsingResult{ ParsingStatus::Success, "123"sv } );
		CHECK( p.parse( "123abcABC" ) == ParsingResult{ ParsingStatus::Success, "123"sv } );
		CHECK( p.parse( "123", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "123ABC", false ) == ParsingResult{ ParsingStatus::Success, "123"sv } );
	}
}