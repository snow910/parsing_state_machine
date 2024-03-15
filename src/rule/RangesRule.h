#pragma once

#include "../Rule.h"

namespace psm
{
	namespace detail
	{
		template< char... Chars >
		struct RangesStorage
		{
			static constexpr char ranges[] = { Chars... };
		};

		/*template< char... Chars >
		const char RangesStorage< Chars... >::ranges[] = { Chars... };*/
	} // namespace detail

	template< char... Chars >
	class Ranges : public RuleBase
	{
		static_assert( sizeof...( Chars ) >= 2 && ( sizeof...( Chars ) & 1 ) == 0 );
		using Storage = detail::RangesStorage< Chars... >;

	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.empty() )
				return { RuleMatchCode::NotTrueYet };
			char c = *input.current();
			for( size_t i = 0; i < sizeof...( Chars ); i += 2 )
			{
				if( c >= Storage::ranges[i] && c <= Storage::ranges[i + 1] )
				{
					input.consume();
					return { RuleMatchCode::True };
				}
			}
			return { RuleMatchCode::False };
		}
	};

	template< char... Chars >
	class NotRanges : public RuleBase
	{
		static_assert( sizeof...( Chars ) >= 2 && ( sizeof...( Chars ) & 1 ) == 0 );
		using Storage = detail::RangesStorage< Chars... >;

	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.empty() )
				return { RuleMatchCode::NotTrueYet };
			char c = *input.current();
			for( size_t i = 0; i < sizeof...( Chars ); i += 2 )
			{
				if( c >= Storage::ranges[i] && c <= Storage::ranges[i + 1] )
					return { RuleMatchCode::False };
			}
			input.consume();
			return { RuleMatchCode::True };
		}
	};
} // namespace psm