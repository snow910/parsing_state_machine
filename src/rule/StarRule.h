// Copyright (c) 2024 snow910
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include "../Rule.h"

namespace psm
{
	template< typename Rule >
	class Star : public RuleBase
	{
	public:
		using Rules = std::tuple< Rule >;

		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( !nestedResult.result )
				return { RuleMatchCode::True };
			input.setCurrent( nestedResult.input.current() );
			return { RuleMatchCode::CallNested, 0 };
		}
	};
} // namespace psm