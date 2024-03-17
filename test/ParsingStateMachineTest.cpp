#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace psm;
using namespace std::string_view_literals;

struct ASymbolCounterAction
{
	using Rules = std::tuple< Range< 'A', 'Z' > >;
	template< typename Rule >
	void operator()( const Rule& rule, std::size_t pos, const std::string_view& sv )
	{
		if( sv.size() == 1 && sv.front() == 'A' + n )
			++n;
	}
	int n = 0;
};

struct ABSeqCounterAction
{
	using Rules = std::tuple< Char< 'A' >, Char< 'B' >, Seq< Char< 'A' >, Char< 'B' > > >;
	template< typename Rule >
	void operator()( const Rule& rule, std::size_t pos, const std::string_view& sv )
	{
		if constexpr( std::is_same_v< Rule, Char< 'A' > > ||
			      std::is_same_v< Rule, Char< 'B' > > ||
			      std::is_same_v< Rule, Seq< Char< 'A' >, Char< 'B' > > > )
			++nCorrect;
		else
			++nIncorrect;
	}
	int nCorrect = 0;
	int nIncorrect = 0;
};

struct AllCounterAction
{
	template< typename Rule >
	void operator()( const Rule& rule, std::size_t pos, const std::string_view& sv )
	{
		++n;
	}
	int n = 0;
};

struct AbortAction
{
	using Rules = std::tuple< Char< '!' > >;
	template< typename Rule >
	bool operator()( const Rule& rule, std::size_t pos, const std::string_view& sv )
	{
		return false;
	}
};

TEST_CASE( "Parse continuation ability test", "[psm]" )
{
	int n = 0;
	using ASymbol = Range< 'A', 'Z' >;
	Parser< Seq< ASymbol, Seq< ASymbol, ASymbol, Seq< ASymbol >, ASymbol >, ASymbol >, ASymbolCounterAction > p;
	const char* str = "ABCDEF";
	for( int i = 1; i < 6; ++i )
		CHECK( p.parse( str, str + i, false ).status == ParsingStatus::Incomplete );
	CHECK( p.parse( str, str + 6, false ) == ParsingResult{ ParsingStatus::Success, std::string_view( str ) } );
	CHECK( p.actionFunction().n == 6 );
}

TEST_CASE( "Action rule filtering test", "[psm]" )
{
	{
		// A, B, AB
		Parser< Seq< Seq< Char< 'A' >, Char< 'B' > >, Char< 'C' > >, ABSeqCounterAction > p;
		p.parse( "ABC" );
		CHECK( p.actionFunction().nCorrect == 3 );
		CHECK( p.actionFunction().nIncorrect == 0 );
	}

	{
		// All
		Parser< Seq< Seq< Char< 'A' >, Char< 'B' > >, Char< 'C' > >, AllCounterAction > p;
		p.parse( "ABC" );
		CHECK( p.actionFunction().n == 5 );
	}
}

TEST_CASE( "Quiet test", "[psm]" )
{
	using NotDigit = NotRange< '0', '9' >;
	int n = 0;

	// Quiet in the middle
	parseStringView< Seq< Integer, NotDigit, Quiet< Seq< Integer, NotDigit, Integer > >, NotDigit, Integer > >(
	    "111 222 333 444"sv,
	    [&n]< typename Rule >( const Rule&, std::size_t pos, const std::string_view& sv )
	    {
		    if constexpr( !std::is_same_v< Rule, Integer > )
			    return;
		    switch( ++n )
		    {
		    case 1:
			    CHECK( ( sv == "111"sv ) );
			    break;
		    case 2:
			    CHECK( ( sv == "444"sv ) );
			    break;
		    default:
			    REQUIRE( false );
			    break;
		    }
	    } );
	CHECK( n == 2 );

	// Total quiet
	n = 0;
	parseStringView< Quiet< Seq< Integer, NotDigit, Seq< Integer, NotDigit, Integer >, NotDigit, Integer > > >(
	    "111 222 333 444"sv,
	    [&n]< typename Rule >( const Rule&, std::size_t pos, const std::string_view& sv )
	    {
		    ++n;
	    } );
	CHECK( n == 0 );

	// Nested quiet
	n = 0;
	parseStringView< Seq< Integer, NotDigit, Quiet< Seq< Integer, NotDigit, Quiet< Integer > > >, NotDigit, Integer > >(
	    "111 222 333 444"sv,
	    [&n]< typename Rule >( const Rule&, std::size_t pos, const std::string_view& sv )
	    {
		    if constexpr( !std::is_same_v< Rule, Integer > )
			    return;
		    switch( ++n )
		    {
		    case 1:
			    CHECK( ( sv == "111"sv ) );
			    break;
		    case 2:
			    CHECK( ( sv == "444"sv ) );
			    break;
		    default:
			    REQUIRE( false );
			    break;
		    }
	    } );
	CHECK( n == 2 );
}

TEST_CASE( "parseStringView test (action rule filtering)", "[psm]" )
{
	size_t nCorrect = 0;
	size_t nIncorrect = 0;
	parseStringView< Plus< One< Char< 'A' >, Char< 'B' >, Char< 'C' > > >, std::tuple< Char< 'A' > > >(
	    "ABC"sv,
	    [&]< typename Rule >( const Rule& rule, std::size_t pos, const std::string_view& match )
	    {
		    if constexpr( std::is_same_v< Rule, Char< 'A' > > )
			    ++nCorrect;
		    else
			    ++nIncorrect;
	    } );
	CHECK( nCorrect == 1 );
	CHECK( nIncorrect == 0 );

	nCorrect = nIncorrect = 0;
	parseStringView< Plus< One< Char< 'A' >, Char< 'B' >, Char< 'C' > > >, std::tuple< Char< 'A' >, Char< 'B' > > >(
	    "ABC"sv,
	    [&]< typename Rule >( const Rule& rule, std::size_t pos, const std::string_view& match )
	    {
		    if constexpr( std::is_same_v< Rule, Char< 'A' > > || std::is_same_v< Rule, Char< 'B' > > )
			    ++nCorrect;
		    else
			    ++nIncorrect;
	    } );
	CHECK( nCorrect == 2 );
	CHECK( nIncorrect == 0 );

	nCorrect = nIncorrect = 0;
	parseStringView< Plus< One< Char< 'A' >, Char< 'B' >, Char< 'C' > > > >(
	    "ABC"sv,
	    [&]< typename Rule >( const Rule& rule, std::size_t pos, const std::string_view& match )
	    {
		    ++nCorrect;
	    } );
	CHECK( nCorrect == 7 );
}

TEST_CASE( "parseStringView function test (parsing abort)", "[psm]" )
{
	auto res = parseStringView< Plus< One< Ranges< 'A', 'Z', 'a', 'z', '0', '9' >, Char< '!' > > >, std::tuple< Char< '!' > > >(
	    "abc012!DEF345"sv,
	    []( const auto& rule, std::size_t pos, const std::string_view& match )
	    {
		    return false;
	    } );
	CHECK( res == ParsingResult{ ParsingStatus::Aborted, "!"sv } );
}

TEST_CASE( "Parsing reset test", "[psm]" )
{
	Parser< Plus< Range< 'A', 'Z' > >, ASymbolCounterAction > p;
	p.parse( "ABC", false );
	p.parse( "ABCDEF", false );
	CHECK( p.actionFunction().n == 6 );
	p.reset();
	p.actionFunction().n = 0;
	p.parse( "ABCDEF", false );
	CHECK( p.actionFunction().n == 6 );
}

TEST_CASE( "Parsing abort test", "[psm]" )
{
	Parser< Plus< One< Ranges< 'A', 'Z', 'a', 'z', '0', '9' >, Char< '!' > > >, AbortAction > p;
	CHECK( p.parse( "abc012!DEF345" ) == ParsingResult{ ParsingStatus::Aborted, "!"sv } );
}

TEST_CASE( "Parsing overflow test", "[psm]" )
{
	Parser< GenA< Tag0, 5, Plus< If< Char< '|' >, Ref< Tag0 >, Range< '0', '9' > > > > > p;
	CHECK( p.parse( "012|3|45|6|78|9" ) == ParsingResult{ ParsingStatus::Success, "012|3|45|6|78|9"sv } );
	CHECK( p.parse( "012|3|4|5|6|78|9" ).status == ParsingStatus::Overflow );
}