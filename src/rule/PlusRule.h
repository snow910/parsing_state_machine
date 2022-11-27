#pragma once

#include "../Rule.h"

namespace psm
{
	template< typename Rule >
	class Plus : public RuleBase
	{
		bool one_ = false;

	public:
		using Rules = std::tuple< Rule >;

		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( !nestedResult.result )
			{
				if( !one_ )
					return { RuleMatchCode::False };
				return { RuleMatchCode::True };
			}
			one_ = true;
			input.setCurrent( nestedResult.input.current() );
			return { RuleMatchCode::CallNested, 0 };
		}
	};
} // namespace psm