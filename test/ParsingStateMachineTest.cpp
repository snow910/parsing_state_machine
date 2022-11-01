#pragma once

#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

struct Action
{
	using Rules = std::tuple< psm::Integer, psm::Plus< psm::Digit > >;
	template< typename Rule >
	void operator()( const Rule& rule, std::size_t beginPos, const char* begin, const char* end ) const
	{
	}

	template< typename Rule >
	void operator()( const std::span< const std::size_t >& rulePos, const Rule& rule, std::size_t beginPos, const char* begin, const char* end ) const
	{
	}
};

TEST_CASE( "44", "[gg]" )
{
	psm::Parser< psm::Integer, Action > parser;
	auto res = parser.parseString( "+1456AAAA" );

	REQUIRE( 1 == 1 );
	int a = 5;
}