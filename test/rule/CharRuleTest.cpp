#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "CharRule test", "[CharRule][psm]" )
{
	{
		Parser< Char< 'A' > > p;
		CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "A" ) } );
		CHECK( p.parse( "BC" ).status == ParsingStatus::Fail );
	}
	{
		Parser< Seq< Char< 'A' >, Char< 'B' > > > p;
		CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "AB" ) } );
		CHECK( p.parse( "A", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "ABC", false ) == ParsingResult{ ParsingStatus::Success, std::string_view( "AB" ) } );
		CHECK( p.parse( "0ABC", false ).status == ParsingStatus::Fail );
	}

	{
		Parser< NotChar< 'A' > > p;
		CHECK( p.parse( "BC" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "B" ) } );
		CHECK( p.parse( "ABC" ).status == ParsingStatus::Fail );
	}
	{
		Parser< Seq< NotChar< 'A' >, NotChar< 'B' > > > p;
		CHECK( p.parse( "CDE" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "CD" ) } );
		CHECK( p.parse( "C", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "CDE", false ) == ParsingResult{ ParsingStatus::Success, std::string_view( "CD" ) } );
		CHECK( p.parse( "ACD", false ).status == ParsingStatus::Fail );
	}

	{
		Parser< Str< 'A', 'B', 'C' > > p;
		CHECK( p.parse( "ABCDE" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "ABC" ) } );

		CHECK( p.parse( "A", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "AB", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "ABC", false ) == ParsingResult{ ParsingStatus::Success, std::string_view( "ABC" ) } );

		CHECK( p.parse( "0ABCD", false ).status == ParsingStatus::Fail );
	}
}