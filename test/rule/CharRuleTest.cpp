#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "CharRule test", "[CharRule][psm]" )
{
	{
		Parser< Char< 'A' > > p;
		CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "A" ) } );
		CHECK( p.parse( "BC" ).type == ParsingResult::Type::False );
	}
	{
		Parser< Seq< Char< 'A' >, Char< 'B' > > > p;
		CHECK( p.parse( "ABC" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "AB" ) } );
		CHECK( p.parse( "A", false ).type == ParsingResult::Type::Incomplete );
		CHECK( p.parse( "ABC", false ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "AB" ) } );
		CHECK( p.parse( "0ABC", false ).type == ParsingResult::Type::False );
	}

	{
		Parser< NotChar< 'A' > > p;
		CHECK( p.parse( "BC" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "B" ) } );
		CHECK( p.parse( "ABC" ).type == ParsingResult::Type::False );
	}
	{
		Parser< Seq< NotChar< 'A' >, NotChar< 'B' > > > p;
		CHECK( p.parse( "CDE" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "CD" ) } );
		CHECK( p.parse( "C", false ).type == ParsingResult::Type::Incomplete );
		CHECK( p.parse( "CDE", false ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "CD" ) } );
		CHECK( p.parse( "ACD", false ).type == ParsingResult::Type::False );
	}

	{
		Parser< Str< 'A', 'B', 'C' > > p;
		CHECK( p.parse( "ABCDE" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "ABC" ) } );

		CHECK( p.parse( "A", false ).type == ParsingResult::Type::Incomplete );
		CHECK( p.parse( "AB", false ).type == ParsingResult::Type::Incomplete );
		CHECK( p.parse( "ABC", false ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "ABC" ) } );

		CHECK( p.parse( "0ABCD", false ).type == ParsingResult::Type::False );
	}
}