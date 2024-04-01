// Copyright (c) 2024 snow910
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include "CharRule.h"

namespace psm
{
	template< char C, char... Cs >
	class Str : public RuleBase
	{
		using Storage = detail::CharsStorage< C, Cs... >;

	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			while( !input.empty() )
			{
				if( *input.current() == Storage::array[input.position()] )
					input.consume();
				else
				{
					input.discard();
					return { RuleMatchCode::False };
				}
				if( input.position() == sizeof...( Cs ) + 1 )
					return { RuleMatchCode::True };
			}
			return { RuleMatchCode::NotTrueYet };
		}
	};
} // namespace psm