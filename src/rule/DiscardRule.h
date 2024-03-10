#pragma once

#include "../Rule.h"

namespace psm
{
	template< typename Rule >
	class Discard : public RuleBase
	{
	public:
		using Rules = std::tuple< Rule >;
		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			return { nestedResult.result ? RuleMatchCode::True : RuleMatchCode::False, 0 };
		}
	};
} // namespace psm