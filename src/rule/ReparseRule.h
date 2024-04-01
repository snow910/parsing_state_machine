// Copyright (c) 2024 snow910
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include "../Rule.h"

namespace psm
{
	template< typename Rule, typename Secondary, typename... Others >
	class Reparse : public RuleBase
	{
	public:
		using Rules = std::tuple< Rule, Secondary, Others... >;

		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( !nestedResult.result )
				return { RuleMatchCode::False };
			if( nestedResult.index == 0 )
			{
				input.setCurrent( nestedResult.input.current() );
				return { RuleMatchCode::CallNestedOnMatch, 1 };
			}
			if( nestedResult.index == ( sizeof...( Others ) + 1 ) )
				return { RuleMatchCode::True };
			return { RuleMatchCode::CallNestedOnMatch, nestedResult.index + 1 };
		}
	};
} // namespace psm