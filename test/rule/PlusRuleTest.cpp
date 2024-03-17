#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

TEST_CASE( "PlusRule test", "[PlusRule][psm]" )
{
	Parser< Plus< Char< 'A' > > > p;
	CHECK( p.parse( "A" ) == ParsingResult{ ParsingStatus::Success, "A"sv } );
	CHECK( p.parse( "AA" ) == ParsingResult{ ParsingStatus::Success, "AA"sv } );
	CHECK( p.parse( "AAA" ) == ParsingResult{ ParsingStatus::Success, "AAA"sv } );
	CHECK( p.parse( "AAABBB" ) == ParsingResult{ ParsingStatus::Success, "AAA"sv } );
	CHECK( p.parse( "BBB" ).status == ParsingStatus::Fail );

	CHECK( p.parse( "A", false ).status == ParsingStatus::Incomplete );
	CHECK( p.parse( "AA", false ).status == ParsingStatus::Incomplete );
	CHECK( p.parse( "AAA", false ).status == ParsingStatus::Incomplete );
	CHECK( p.parse( "AAABBB", false ) == ParsingResult{ ParsingStatus::Success, "AAA"sv } );
	CHECK( p.parse( "BBB", false ).status == ParsingStatus::Fail );
}