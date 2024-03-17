#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

TEST_CASE( "OneRule test", "[OneRule][psm]" )
{
	Parser< One< Char< 'A' >, Char< 'B' >, Char< 'C' > > > p;
	CHECK( p.parse( "ABCD" ) == ParsingResult{ ParsingStatus::Success, "A"sv } );
	CHECK( p.parse( "BCDA" ) == ParsingResult{ ParsingStatus::Success, "B"sv } );
	CHECK( p.parse( "CDAB" ) == ParsingResult{ ParsingStatus::Success, "C"sv } );
	CHECK( p.parse( "DABC" ).status == ParsingStatus::Fail );
}