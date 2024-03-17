#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

TEST_CASE( "RepRule test", "[RepRule][psm]" )
{
	Parser< RepMinMax< 4, 6, Char< 'A' > > > p;
	CHECK( p.parse( "A" ).status == ParsingStatus::Fail );
	CHECK( p.parse( "AA" ).status == ParsingStatus::Fail );
	CHECK( p.parse( "AAA" ).status == ParsingStatus::Fail );
	CHECK( p.parse( "AAAA" ) == ParsingResult{ ParsingStatus::Success, "AAAA"sv } );
	CHECK( p.parse( "AAAAA" ) == ParsingResult{ ParsingStatus::Success, "AAAAA"sv } );
	CHECK( p.parse( "AAAAAA" ) == ParsingResult{ ParsingStatus::Success, "AAAAAA"sv } );
	CHECK( p.parse( "AAAAAAA" ) == ParsingResult{ ParsingStatus::Success, "AAAAAA"sv } );

	CHECK( p.parse( "A", false ).status == ParsingStatus::Incomplete );
	CHECK( p.parse( "AA", false ).status == ParsingStatus::Incomplete );
	CHECK( p.parse( "AAA", false ).status == ParsingStatus::Incomplete );
	CHECK( p.parse( "AAAAB", false ) == ParsingResult{ ParsingStatus::Success, "AAAA"sv } );
}