#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "PlusRule test", "[PlusRule][psm]" )
{
	Parser< Plus< Char< 'A' > > > p;
	CHECK( p.parse( "A" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "A" ) } );
	CHECK( p.parse( "AA" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "AA" ) } );
	CHECK( p.parse( "AAA" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "AAA" ) } );
	CHECK( p.parse( "AAABBB" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "AAA" ) } );
	CHECK( p.parse( "BBB" ).type == ParsingResult::Type::False );
}