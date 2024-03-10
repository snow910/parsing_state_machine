#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "OptRule test", "[OptRule][psm]" )
{
	Parser< Seq< Char< '_' >, Opt< Char< 'A' > > > > p;
	CHECK( p.parse( "_A" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "_A" ) } );
	CHECK( p.parse( "_B" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "_" ) } );
}