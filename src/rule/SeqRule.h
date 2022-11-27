#pragma once

#include "../Rule.h"

namespace psm
{
	template< typename Rule, typename... Others >
	class Seq : public RuleBase
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
				if( nestedResult.index == ( ChildrenCount - 1 ) )
					return { RuleMatchCode::True };
				return { RuleMatchCode::CallNested, nestedResult.index + 1 };
			}
			return { RuleMatchCode::False };
		}
	};
} // namespace psm