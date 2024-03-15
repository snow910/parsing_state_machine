#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "PlusRule test", "[PlusRule][psm]" )
{
	Parser< Plus< Char< 'A' > > > p;
	CHECK( p.parse( "A" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "A" ) } );
	CHECK( p.parse( "AA" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "AA" ) } );
	CHECK( p.parse( "AAA" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "AAA" ) } );
	CHECK( p.parse( "AAABBB" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "AAA" ) } );
	CHECK( p.parse( "BBB" ).status == ParsingStatus::Fail );

	CHECK( p.parse( "A", false ).status == ParsingStatus::Incomplete );
	CHECK( p.parse( "AA", false ).status == ParsingStatus::Incomplete );
	CHECK( p.parse( "AAA", false ).status == ParsingStatus::Incomplete );
	CHECK( p.parse( "AAABBB", false ) == ParsingResult{ ParsingStatus::Success, std::string_view( "AAA" ) } );
	CHECK( p.parse( "BBB", false ).status == ParsingStatus::Fail );
}