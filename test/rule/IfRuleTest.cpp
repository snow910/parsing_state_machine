#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

TEST_CASE( "IfRule test", "[IfRule][psm]" )
{
	{
		Parser< Seq< Char< '_' >, If< Char< 'A' >, Char< '0' > > > > p;
		CHECK( p.parse( "_A0B1" ) == ParsingResult{ ParsingStatus::Success, "_A0"sv } );
		CHECK( p.parse( "_B1A0" ) == ParsingResult{ ParsingStatus::Success, "_"sv } );
		CHECK( p.parse( "_A1" ).status == ParsingStatus::Fail );
	}

	{
		Parser< Seq< Char< '_' >, If< Char< 'A' >, Char< '0' >, Char< '1' > > > > p;
		CHECK( p.parse( "_A0B1" ) == ParsingResult{ ParsingStatus::Success, "_A0"sv } );
		CHECK( p.parse( "_1A0" ) == ParsingResult{ ParsingStatus::Success, "_1"sv } );
		CHECK( p.parse( "_8A0" ).status == ParsingStatus::Fail );
	}

	{
		Parser< Seq< Char< '_' >, If< Char< 'A' >, NaR, Char< '1' > > > > p;
		CHECK( p.parse( "_A0B1" ) == ParsingResult{ ParsingStatus::Success, "_A"sv } );
		CHECK( p.parse( "_1A0" ) == ParsingResult{ ParsingStatus::Success, "_1"sv } );
		CHECK( p.parse( "_8A0" ).status == ParsingStatus::Fail );
	}
}