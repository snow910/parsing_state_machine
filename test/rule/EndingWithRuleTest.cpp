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
		auto res = parseStringView< EndingWith< Str< 'A', 'B' >, SuccessAll > >(
		    "012AB"sv,
		    []< typename Rule >( const Rule&, std::size_t pos, const std::string_view& sv )
		    {
			    if constexpr( !std::is_same_v< Rule, SuccessAll > )
				    return;
			    CHECK( ( sv == "012"sv ) );
		    } );
		CHECK( res == ParsingResult{ ParsingStatus::Success, std::string_view( "012AB" ) } );

		int n = 0;
		res = parseStringView< Star< EndingWith< Str< 'A', 'B' >, SuccessAll > > >(
		    "AB012AB345AB678ABAB"sv,
		    [&n]< typename Rule >( const Rule&, std::size_t pos, const std::string_view& sv )
		    {
			    if constexpr( !std::is_same_v< Rule, SuccessAll > )
				    return;
			    static constexpr std::string_view strs[] = {
				    ""sv,
				    "012"sv,
				    "345"sv,
				    "678"sv,
				    ""sv
			    };
			    if( n < 5 )
				    CHECK( ( sv == strs[n] ) );
			    n++;
		    } );
		CHECK( res == ParsingResult{ ParsingStatus::Success, std::string_view( "AB012AB345AB678ABAB" ) } );
		CHECK( n == 5 );
	}
}