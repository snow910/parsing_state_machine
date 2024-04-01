// Copyright (c) 2024 snow910
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt

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