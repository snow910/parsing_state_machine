#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "UntilRule test", "[UntilRule][psm]" )
{
	{
		Parser< Until< Char< 'B' > > > p;
		CHECK( p.parse( "AB" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "A" ) } );
		CHECK( p.parse( "AAB" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "AA" ) } );
		CHECK( p.parse( "AAABBB" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "AAA" ) } );
		CHECK( p.parse( "B" ).type == ParsingResult::Type::False );
		CHECK( p.parse( "A" ).type == ParsingResult::Type::False );
	}
	{
		Parser< UntilMax< Char< 'B' >, 2 > > p;
		CHECK( p.parse( "AB" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "A" ) } );
		CHECK( p.parse( "AAB" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "AA" ) } );
		CHECK( p.parse( "AAABBB" ).type == ParsingResult::Type::False );
	}
}