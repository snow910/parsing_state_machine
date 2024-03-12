#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "SeqRule test", "[SeqRule][psm]" )
{
	Parser< Seq< Char< 'A' >, Char< 'B' >, Char< 'C' > > > p;
	CHECK( p.parse( "A" ).type == ParsingResult::Type::False );
	CHECK( p.parse( "AB" ).type == ParsingResult::Type::False );
	CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "ABC" ) } );

	CHECK( p.parse( "A", false ).type == ParsingResult::Type::Incomplete );
	CHECK( p.parse( "AB", false ).type == ParsingResult::Type::Incomplete );
	CHECK( p.parse( "ABC012", false ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "ABC" ) } );
}