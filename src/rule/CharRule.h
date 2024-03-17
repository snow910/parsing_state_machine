#pragma once

#include "../Rule.h"

namespace psm
{
	namespace detail
	{
		template< char... Chars_ >
		struct CharsStorage
		{
			static constexpr char array[] = { Chars_... };
		};
	} // namespace detail

	template< char C, char... Cs >
	class Char : public RuleBase
	{
		using Storage = detail::CharsStorage< C, Cs... >;

	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.empty() )
				return { RuleMatchCode::NotTrueYet };
			char c = *input.current();
			if constexpr( sizeof...( Cs ) == 0 )
			{
				if( c == Storage::array[0] )
				{
					input.consume();
					return { RuleMatchCode::True };
				}
			}
			else
			{
				for( size_t i = 0; i < sizeof...( Cs ) + 1; ++i )
				{
					if( c == Storage::array[i] )
					{
						input.consume();
						return { RuleMatchCode::True };
					}
				}
			}
			return { RuleMatchCode::False };
		}
	};

	template< char C, char... Cs >
	class NotChar : public RuleBase
	{
		using Storage = detail::CharsStorage< C, Cs... >;

	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.empty() )
				return { RuleMatchCode::NotTrueYet };
			char c = *input.current();
			if constexpr( sizeof...( Cs ) == 0 )
			{
				if( c == Storage::array[0] )
					return { RuleMatchCode::False };
				input.consume();
				return { RuleMatchCode::True };
			}
			else
			{
				for( size_t i = 0; i < sizeof...( Cs ) + 1; ++i )
				{
					if( c == Storage::array[i] )
						return { RuleMatchCode::False };
				}
				input.consume();
				return { RuleMatchCode::True };
			}
		}
	};
} // namespace psm