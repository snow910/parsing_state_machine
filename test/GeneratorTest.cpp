#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "GenA", "[GenA][psm]" )
{
	Parser< GenA< Tag0, 3, Seq< Range< 'a', 'z' >, If< Char< '{' >, Ref< Tag0 > > > > > p;
	CHECK( p.parse( "a{b{c{d123" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "a{b{c{d" ) } );
}