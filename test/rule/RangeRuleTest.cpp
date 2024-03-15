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