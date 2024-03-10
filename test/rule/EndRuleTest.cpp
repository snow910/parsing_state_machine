#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "EndRule test", "[EndRule][psm]" )
{
	Parser< Seq< Str< 'A', 'B', 'C' >, End > > p;
	CHECK( p.parse( "ABCDEF" ).type == ParsingResult::Type::False );
	CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "ABC" ) } );
}