#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "NotRule test", "[NotRule][psm]" )
{
	Parser< Seq< Str< 'A', 'B' >, Not< Str< '0', '1' > > > > p;
	CHECK( p.parse( "AB234" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "AB" ) } );
	CHECK( p.parse( "AB01234" ).status == ParsingStatus::Fail );
}