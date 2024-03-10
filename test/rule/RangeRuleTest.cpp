#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "RangeRule test", "[RangeRule][psm]" )
{
	{
		Parser< Range< 'A', 'Z' > > p;
		CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "A" ) } );
		CHECK( p.parse( "BCD" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "B" ) } );
		CHECK( p.parse( "CDE" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "C" ) } );
		CHECK( p.parse( "ZAB" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "Z" ) } );
		CHECK( p.parse( "abc" ).type == ParsingResult::Type::False );
	}

	{
		Parser< Star< Range< 'A', 'Z' > > > p;
		CHECK( p.parse( "A", false ).type == ParsingResult::Type::Incomplete );
		CHECK( p.parse( "AB", false ).type == ParsingResult::Type::Incomplete );
		CHECK( p.parse( "ABC", false ).type == ParsingResult::Type::Incomplete );
		CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "ABC" ) } );
		CHECK( p.parse( "ABC012", false ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "ABC" ) } );
	}
}