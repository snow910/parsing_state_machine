#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "RepRule test", "[RepRule][psm]" )
{
	{
		Parser< RepMinMax< 4, 6, Char< 'A' > > > p;
		CHECK( p.parse( "A" ).type == ParsingResult::Type::False );
		CHECK( p.parse( "AA" ).type == ParsingResult::Type::False );
		CHECK( p.parse( "AAA" ).type == ParsingResult::Type::False );
		CHECK( p.parse( "AAAA" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "AAAA" ) } );
		CHECK( p.parse( "AAAAA" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "AAAAA" ) } );
		CHECK( p.parse( "AAAAAA" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "AAAAAA" ) } );
		CHECK( p.parse( "AAAAAAA" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "AAAAAA" ) } );
	}
}