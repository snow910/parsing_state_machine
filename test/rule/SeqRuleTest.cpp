#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

TEST_CASE( "SeqRule test", "[SeqRule][psm]" )
{
	Parser< Seq< Char< 'A' >, Char< 'B' >, Char< 'C' > > > p;
	CHECK( p.parse( "A" ).status == ParsingStatus::Fail );
	CHECK( p.parse( "AB" ).status == ParsingStatus::Fail );
	CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingStatus::Success, "ABC"sv } );

	CHECK( p.parse( "A", false ).status == ParsingStatus::Incomplete );
	CHECK( p.parse( "AB", false ).status == ParsingStatus::Incomplete );
	CHECK( p.parse( "ABC012", false ) == ParsingResult{ ParsingStatus::Success, "ABC"sv } );
}