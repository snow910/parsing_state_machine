#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

TEST_CASE( "UntilRule test", "[UntilRule][psm]" )
{
	{
		Parser< Until< Char< 'B' > > > p;
		CHECK( p.parse( "AB" ) == ParsingResult{ ParsingStatus::Success, "A"sv } );
		CHECK( p.parse( "AAB" ) == ParsingResult{ ParsingStatus::Success, "AA"sv } );
		CHECK( p.parse( "AAABBB" ) == ParsingResult{ ParsingStatus::Success, "AAA"sv } );
		CHECK( p.parse( "B" ).status == ParsingStatus::Fail );
		CHECK( p.parse( "A" ).status == ParsingStatus::Fail );
	}
	{
		Parser< UntilMax< Char< 'B' >, 2 > > p;
		CHECK( p.parse( "AB" ) == ParsingResult{ ParsingStatus::Success, "A"sv } );
		CHECK( p.parse( "AAB" ) == ParsingResult{ ParsingStatus::Success, "AA"sv } );
		CHECK( p.parse( "AAABBB" ).status == ParsingStatus::Fail );
	}
}