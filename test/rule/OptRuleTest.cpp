#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

TEST_CASE( "OptRule test", "[OptRule][psm]" )
{
	Parser< Seq< Char< '_' >, Opt< Char< 'A' > > > > p;
	CHECK( p.parse( "_A" ) == ParsingResult{ ParsingStatus::Success, "_A"sv } );
	CHECK( p.parse( "_B" ) == ParsingResult{ ParsingStatus::Success, "_"sv } );
}