#pragma once

#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

struct Action
{
	using Rules = std::tuple< psm::Integer, psm::Plus< psm::Digit > >;
	template< typename Rule >
	void operator()( const Rule& rule, std::size_t pos, std::string_view string ) const
	{
	}
};

TEST_CASE( "44", "[gg]" )
{
	psm::Parser< psm::Integer, Action > parser;
	auto res = parser.parse( "+1456AAAA" );

	REQUIRE( 1 == 1 );
	int a = 5;
}