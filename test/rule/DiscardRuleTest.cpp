#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "DiscardRule test", "[DiscardRule][psm]" )
{
	{
		Parser< Seq< Str< 'A', 'B' >, Discard< Str< '0', '1' > > > > p;
		CHECK( p.parse( "AB01234" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "AB" ) } );
		CHECK( p.parse( "AB234" ).type == ParsingResult::Type::False );
	}
}