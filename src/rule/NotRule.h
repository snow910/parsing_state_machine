#pragma once

#include "../Rule.h"

namespace psm
{
	template< typename Rule >
	class Not : public RuleBase
	{
	public:
		using Rules = std::tuple< Rule >;
		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( nestedResult.result )
				return { RuleMatchCode::False };
			else
				return { RuleMatchCode::True };
		}
	};
} // namespace psm