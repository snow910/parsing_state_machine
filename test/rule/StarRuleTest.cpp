#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "StarRule test", "[StarRule][psm]" )
{
	Parser< Seq< Char< '_' >, Star< Char< 'A' > > > > p;
	CHECK( p.parse( "_A" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "_A" ) } );
	CHECK( p.parse( "_AA" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "_AA" ) } );
	CHECK( p.parse( "_AAA" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "_AAA" ) } );
	CHECK( p.parse( "_AAABBB" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "_AAA" ) } );
	CHECK( p.parse( "_" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "_" ) } );
	CHECK( p.parse( "_BBB" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "_" ) } );
}