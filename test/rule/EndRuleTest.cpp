#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

TEST_CASE( "EndRule test", "[EndRule][psm]" )
{
	Parser< Seq< Str< 'A', 'B', 'C' >, End > > p;
	CHECK( p.parse( "ABCDEF" ).status == ParsingStatus::Fail );
	CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingStatus::Success, "ABC"sv } );
}