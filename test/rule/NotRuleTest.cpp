#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

TEST_CASE( "NotRule test", "[NotRule][psm]" )
{
	Parser< Seq< Str< 'A', 'B' >, Not< Str< '0', '1' > > > > p;
	CHECK( p.parse( "AB234" ) == ParsingResult{ ParsingStatus::Success, "AB"sv } );
	CHECK( p.parse( "AB01234" ).status == ParsingStatus::Fail );
}