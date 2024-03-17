#pragma once

#include "CharRule.h"

namespace psm
{
	template< char... Chars >
	class Ranges : public RuleBase
	{
		static_assert( sizeof...( Chars ) >= 2 && ( sizeof...( Chars ) & 1 ) == 0 );
		using Storage = detail::CharsStorage< Chars... >;

	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.empty() )
				return { RuleMatchCode::NotTrueYet };
			char c = *input.current();
			if constexpr( sizeof...( Chars ) == 2 )
			{
				if( c >= Storage::array[0] && c <= Storage::array[1] )
				{
					input.consume();
					return { RuleMatchCode::True };
				}
			}
			else
			{
				for( size_t i = 0; i < sizeof...( Chars ); i += 2 )
				{
					if( c >= Storage::array[i] && c <= Storage::array[i + 1] )
					{
						input.consume();
						return { RuleMatchCode::True };
					}
				}
			}
			return { RuleMatchCode::False };
		}
	};

	template< char... Chars >
	class NotRanges : public RuleBase
	{
		static_assert( sizeof...( Chars ) >= 2 && ( sizeof...( Chars ) & 1 ) == 0 );
		using Storage = detail::CharsStorage< Chars... >;

	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.empty() )
				return { RuleMatchCode::NotTrueYet };
			char c = *input.current();
			if constexpr( sizeof...( Chars ) == 2 )
			{
				if( c >= Storage::array[0] && c <= Storage::array[1] )
					return { RuleMatchCode::False };
			}
			else
			{
				for( size_t i = 0; i < sizeof...( Chars ); i += 2 )
				{
					if( c >= Storage::array[i] && c <= Storage::array[i + 1] )
						return { RuleMatchCode::False };
				}
			}
			input.consume();
			return { RuleMatchCode::True };
		}
	};

	template< char Begin, char End >
	using Range = Ranges< Begin, End >;

	template< char Begin, char End >
	using NotRange = NotRanges< Begin, End >;

	class Digit : public Range< '0', '9' >
	{
	};
} // namespace psm