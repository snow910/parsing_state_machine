// Copyright (c) 2024 snow910
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include "../Rule.h"

namespace psm
{
	class Any : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.consume() )
				return { RuleMatchCode::True };
			return { RuleMatchCode::NotTrueYet };
		}
	};

	template< std::size_t N >
	class AnyN : public RuleBase
	{
		static_assert( N != 0, "wrong parameter" );

	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.consume( N - input.position() ) )
				return { RuleMatchCode::True };
			return { RuleMatchCode::NotTrueYet };
		}
	};
} // namespace psm