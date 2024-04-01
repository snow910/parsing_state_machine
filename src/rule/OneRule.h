// Copyright (c) 2024 snow910
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include "../Rule.h"

namespace psm
{
	template< typename Rule, typename... Others >
	class One : public RuleBase
	{
	public:
		using Rules = std::tuple< Rule, Others... >;
		static constexpr std::size_t ChildrenCount = std::tuple_size_v< Rules >;
		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( nestedResult.result )
			{
				input.setCurrent( nestedResult.input.current() );
				return { RuleMatchCode::True };
			}
			else if( nestedResult.index != ( ChildrenCount - 1 ) )
				return { RuleMatchCode::CallNested, nestedResult.index + 1 };
			return { RuleMatchCode::False };
		}
	};
} // namespace psm