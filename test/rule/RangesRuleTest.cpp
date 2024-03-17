#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "RangeRule test", "[RangeRule][psm]" )
{
	{
		Parser< Range< 'A', 'Z' > > p;
		CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "A" ) } );
		CHECK( p.parse( "BCD" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "B" ) } );
		CHECK( p.parse( "CDE" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "C" ) } );
		CHECK( p.parse( "ZAB" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "Z" ) } );
		CHECK( p.parse( "abc" ).status == ParsingStatus::Fail );
	}

	{
		Parser< Star< Range< 'A', 'Z' > > > p;
		CHECK( p.parse( "A", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "AB", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "ABC", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "ABC" ) } );
		CHECK( p.parse( "ABC012", false ) == ParsingResult{ ParsingStatus::Success, std::string_view( "ABC" ) } );
	}
}

TEST_CASE( "RangesRule test", "[RangesRule][psm]" )
{
	{
		Parser< Ranges< 'A', 'Z', 'a', 'z' > > p;
		CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "A" ) } );
		CHECK( p.parse( "BCD" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "B" ) } );
		CHECK( p.parse( "cde" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "c" ) } );
		CHECK( p.parse( "zab" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "z" ) } );
		CHECK( p.parse( "123" ).status == ParsingStatus::Fail );
	}
	{
		Parser< Star< Ranges< 'A', 'Z', 'a', 'z' > > > p;
		CHECK( p.parse( "ABCdef012" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "ABCdef" ) } );
		CHECK( p.parse( "ABC", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "ABCdef", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "ABCdef012", false ) == ParsingResult{ ParsingStatus::Success, std::string_view( "ABCdef" ) } );
	}

	{
		Parser< NotRanges< 'A', 'Z', 'a', 'z' > > p;
		CHECK( p.parse( "123abc" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "1" ) } );
		CHECK( p.parse( "ABC" ).status == ParsingStatus::Fail );
		CHECK( p.parse( "abc" ).status == ParsingStatus::Fail );
	}
	{
		Parser< Star< NotRanges< 'A', 'Z', 'a', 'z' > > > p;
		CHECK( p.parse( "123ABCabc" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "123" ) } );
		CHECK( p.parse( "123abcABC" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "123" ) } );
		CHECK( p.parse( "123", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "123ABC", false ) == ParsingResult{ ParsingStatus::Success, std::string_view( "123" ) } );
	}
}