#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "IfRule test", "[IfRule][psm]" )
{
	{
		Parser< Seq< Char< '_' >, If< Char< 'A' >, Char< '0' > > > > p;
		CHECK( p.parse( "_A0B1" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "_A0" ) } );
		CHECK( p.parse( "_B1A0" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "_" ) } );
		CHECK( p.parse( "_A1" ).type == ParsingResult::Type::False );
	}

	{
		Parser< Seq< Char< '_' >, If< Char< 'A' >, Char< '0' >, Char< '1' > > > > p;
		CHECK( p.parse( "_A0B1" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "_A0" ) } );
		CHECK( p.parse( "_1A0" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "_1" ) } );
		CHECK( p.parse( "_8A0" ).type == ParsingResult::Type::False );
	}

	{
		Parser< Seq< Char< '_' >, If< Char< 'A' >, NaR, Char< '1' > > > > p;
		CHECK( p.parse( "_A0B1" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "_A" ) } );
		CHECK( p.parse( "_1A0" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "_1" ) } );
		CHECK( p.parse( "_8A0" ).type == ParsingResult::Type::False );
	}
}