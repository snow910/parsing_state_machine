#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

TEST_CASE( "EndingWithRule test", "[EndingWithRule][psm]" )
{
	{
		Parser< EndingWith< Str< 'A', 'B' > > > p;
		CHECK( p.parse( "012AB" ) == ParsingResult{ ParsingStatus::Success, std::string_view( "012AB" ) } );
		CHECK( p.parse( "012" ).status == ParsingStatus::Fail );
	}
	{
		Parser< EndingWith< Str< 'A', 'B' > > > p;
		CHECK( p.parse( "012", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "012A", false ).status == ParsingStatus::Incomplete );
		CHECK( p.parse( "012AB", false ) == ParsingResult{ ParsingStatus::Success, std::string_view( "012AB" ) } );
	}

	// with reparse
	{
		parseString< EndingWith< Str< 'A', 'B' >, SuccessAll > >(
		    "012AB",
		    []< typename Rule >( const Rule&, std::size_t pos, const std::string_view& sv )
		    {
			    if constexpr( !std::is_same_v< Rule, SuccessAll > )
				    return;
			    CHECK( ( sv == "012"sv ) );
		    } );
	}
}