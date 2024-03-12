#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;

TEST_CASE( "GenA", "[GenA][psm]" )
{
	using TR0 = GenA< Tag0, 3, Char< 'A' > >;
	static_assert( detail::rule_info< TR0 >::max_deep == 3 );
	static_assert( detail::rule_info< TR0 >::stack_size == 3 * sizeof( Char< 'A' > ) );

	using TR1 = GenA< Tag0, 3, Seq< Char< 'A' >, GenA< Tag1, 4, Char< 'A' > > > >;
	static_assert( detail::rule_info< TR1 >::max_deep == 3 * 2 + 4 );
	static_assert( detail::rule_info< TR1 >::stack_size == 3 * ( sizeof( Seq< Char< 'A' > > ) + sizeof( Char< 'A' > ) ) + 4 * sizeof( Char< 'A' > ) );

	using TR2 = GenM< Tag0, 3, Seq< Char< 'A' >, GenA< Tag1, 4, Char< 'A' > > > >;
	static_assert( detail::rule_info< TR2 >::max_deep == 3 * 2 + 4 );
	static_assert( detail::rule_info< TR2 >::stack_size == 3 * ( sizeof( Seq< Char< 'A' > > ) + sizeof( Char< 'A' > ) ) + 4 * sizeof( Char< 'A' > ) );

	Parser< GenA< Tag0, 3, Seq< Range< 'a', 'z' >, If< Char< '{' >, Ref< Tag0 > > > > > p;
	CHECK( p.parse( "a{b{c{d123" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "a{b{c{d" ) } );
}

TEST_CASE( "GenM", "[GenM][psm]" )
{
	using TR0 = GenM< Tag0, 3, Char< 'A' > >;
	static_assert( detail::rule_info< TR0 >::max_deep == 3 );
	static_assert( detail::rule_info< TR0 >::stack_size == 3 * sizeof( Char< 'A' > ) );

	using TR1 = GenM< Tag0, 3, Seq< Char< 'A' >, GenM< Tag1, 4, Char< 'A' > > > >;
	static_assert( detail::rule_info< TR1 >::max_deep == 3 * ( 1 + 4 ) );
	static_assert( detail::rule_info< TR1 >::stack_size == 3 * ( sizeof( Seq< Char< 'A' > > ) + 4 * sizeof( Char< 'A' > ) ) );

	using TR2 = GenA< Tag0, 3, Seq< Char< 'A' >, GenM< Tag1, 4, Char< 'A' > > > >;
	static_assert( detail::rule_info< TR2 >::max_deep == 3 * ( 1 + 4 ) );
	static_assert( detail::rule_info< TR2 >::stack_size == 3 * ( sizeof( Seq< Char< 'A' > > ) + 4 * sizeof( Char< 'A' > ) ) );

	Parser< GenM< Tag0, 3, Seq< Range< 'a', 'z' >, If< Char< '{' >, Ref< Tag0 > > > > > p;
	CHECK( p.parse( "a{b{c{d123" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "a{b{c{d" ) } );
}