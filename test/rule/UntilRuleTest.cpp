#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "UntilRule test", "[UntilRule][psm]" )
{
	{
		Parser< Until< Char< 'B' > > > p;
		CHECK( p.parse( "AB" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "A" ) } );
		CHECK( p.parse( "AAB" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "AA" ) } );
		CHECK( p.parse( "AAABBB" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "AAA" ) } );
		CHECK( p.parse( "B" ).status == ParsingStatus::Fail );
		CHECK( p.parse( "A" ).status == ParsingStatus::Fail );
	}
	{
		Parser< UntilMax< Char< 'B' >, 2 > > p;
		CHECK( p.parse( "AB" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "A" ) } );
		CHECK( p.parse( "AAB" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "AA" ) } );
		CHECK( p.parse( "AAABBB" ).status == ParsingStatus::Fail );
	}
}