// Copyright (c) 2024 snow910
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include "../Rule.h"

namespace psm
{
	template< typename Rule, typename ReparseRule = NaR >
	class EndingWith : public RuleBase
	{
		std::size_t n_;

	public:
		using Rules = std::conditional_t< std::is_same_v< ReparseRule, NaR >, std::tuple< Rule >, std::tuple< Rule, ReparseRule > >;
		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if constexpr( !std::is_same_v< ReparseRule, NaR > )
				if( nestedResult.index == 1 )
				{
					input.consume( n_ );
					return { RuleMatchCode::True };
				}
			if( nestedResult.result )
			{
				if constexpr( !std::is_same_v< ReparseRule, NaR > )
				{
					n_ = nestedResult.input.position();
					return { RuleMatchCode::CallNestedOnMatch, 1 };
				}
				input.setCurrent( nestedResult.input.current() );
				return { RuleMatchCode::True };
			}
			if( input.consume() )
				return { RuleMatchCode::CallNested, 0 };
			return { RuleMatchCode::NotTrueYet };
		}
	};
} // namespace psm