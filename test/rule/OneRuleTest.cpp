#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "OneRule test", "[OneRule][psm]" )
{
	Parser< One< Char< 'A' >, Char< 'B' >, Char< 'C' > > > p;
	CHECK( p.parse( "ABCD" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "A" ) } );
	CHECK( p.parse( "BCDA" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "B" ) } );
	CHECK( p.parse( "CDAB" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "C" ) } );
	CHECK( p.parse( "DABC" ).status == ParsingStatus::Fail );
}