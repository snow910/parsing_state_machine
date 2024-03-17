#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

TEST_CASE( "StarRule test", "[StarRule][psm]" )
{
	Parser< Seq< Char< '_' >, Star< Char< 'A' > > > > p;
	CHECK( p.parse( "_A" ) == ParsingResult{ ParsingStatus::Success, "_A"sv } );
	CHECK( p.parse( "_AA" ) == ParsingResult{ ParsingStatus::Success, "_AA"sv } );
	CHECK( p.parse( "_AAA" ) == ParsingResult{ ParsingStatus::Success, "_AAA"sv } );
	CHECK( p.parse( "_AAABBB" ) == ParsingResult{ ParsingStatus::Success, "_AAA"sv } );
	CHECK( p.parse( "_" ) == ParsingResult{ ParsingStatus::Success, "_"sv } );
	CHECK( p.parse( "_BBB" ) == ParsingResult{ ParsingStatus::Success, "_"sv } );
}