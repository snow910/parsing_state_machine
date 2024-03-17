#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

TEST_CASE( "DiscardRule test", "[DiscardRule][psm]" )
{
	{
		Parser< Seq< Str< 'A', 'B' >, Discard< Str< '0', '1' > > > > p;
		CHECK( p.parse( "AB01234" ) == ParsingResult{ ParsingStatus::Success, "AB"sv } );
		CHECK( p.parse( "AB234" ).status == ParsingStatus::Fail );
	}
}