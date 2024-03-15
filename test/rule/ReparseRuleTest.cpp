#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

using namespace psm;
using namespace std::string_view_literals;

namespace ReparseRuleTest
{
	struct SuccessAction
	{
		using Rules = std::tuple< SuccessAll >;
		template< typename Rule >
		void operator()( const Rule& rule, std::size_t pos, const std::string_view& sv )
		{
			if( n == 0 )
				CHECK( ( sv == "012"sv ) );
			else if( n == 1 )
				CHECK( ( sv == "35678"sv ) );
			else if( n == 2 )
				CHECK( ( sv == "9"sv ) );
			n++;
		}
		int n = 0;
	};

	struct SuccessActionX2
	{
		using Rules = std::tuple< SuccessAll >;
		template< typename Rule >
		void operator()( const Rule& rule, std::size_t pos, const std::string_view& sv )
		{
			if( n / 2 == 0 )
				CHECK( ( sv == "012"sv ) );
			else if( n / 2 == 1 )
				CHECK( ( sv == "35678"sv ) );
			else if( n / 2 == 2 )
				CHECK( ( sv == "9"sv ) );
			n++;
		}
		int n = 0;
	};

	struct SuccessActionV2
	{
		using Rules = std::tuple< SuccessAll >;
		template< typename Rule >
		void operator()( const Rule& rule, std::size_t pos, const std::string_view& sv )
		{
			static const std::string_view strs[] = {
				"012"sv,
				"abc"sv,
				"ABC"sv,
				"35678"sv,
				"def"sv,
				"DEF"sv,
				"9"sv,
				"ghi"sv,
				"GHI"sv
			};
			if( n < 9 )
				CHECK( ( sv == strs[n] ) );
			n++;
		}
		int n = 0;
	};

} // namespace ReparseRuleTest
using namespace ReparseRuleTest;

TEST_CASE( "ReparseRule test", "[ReparseRule][psm]" )
{
	// one reparse level
	{
		Parser< Plus< Seq< Reparse< Plus< NotChar< '*' > >, SuccessAll >, Any > >, SuccessAction > p;
		CHECK( p.parse( "012*35678*9*" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "012*35678*9*" ) } );
		CHECK( p.actionFunction().n == 3 );
	}

	// two reparse levels
	{
		Parser< Plus< Seq< Reparse< Plus< NotChar< '*' > >, Reparse< SuccessAll, SuccessAll > >, Any > >, SuccessActionX2 > p;
		CHECK( p.parse( "012*35678*9*" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "012*35678*9*" ) } );
		CHECK( p.actionFunction().n == 6 );
	}
	{
		Parser< Plus< Seq< Reparse< Plus< NotChar< '*' > >, Reparse< SuccessAll, SuccessAll > >, Any > >, SuccessActionX2 > p;
		std::string_view str = "012*35678*9*"sv;
		for( size_t n = 1; n < str.size(); ++n )
			CHECK( p.parse( str.substr( 0, n ), false ).type == ParsingResult::Type::Incomplete );
		CHECK( p.parse( str, true ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "012*35678*9*" ) } );
		CHECK( p.actionFunction().n == 6 );
	}

	// reparse level, then sequence level, then second reparse level
	{
		using ReparseDigit = Reparse< Star< Range< '0', '9' > >, SuccessAll >;
		using ReparseUpper = Reparse< Star< Range< 'A', 'Z' > >, SuccessAll >;
		using ReparseLower = Reparse< Star< Range< 'a', 'z' > >, SuccessAll >;
		Parser< Plus< Seq< Reparse< Plus< NotChar< '*' > >, Seq< ReparseDigit, ReparseLower, ReparseUpper > >, Any > >, SuccessActionV2 > p;
		CHECK( p.parse( "012abcABC*35678defDEF*9ghiGHI*" ) == ParsingResult{ ParsingResult::Type::True, std::string_view( "012abcABC*35678defDEF*9ghiGHI*" ) } );
		CHECK( p.actionFunction().n == 9 );
	}

	struct Position
	{
		uint16_t ruleIndex;
		uint16_t nestedIndex : 15;
		uint16_t callOnMatch : 1;
		std::size_t pos;
	};
	sizeof( Position );
}