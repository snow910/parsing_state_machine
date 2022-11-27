#pragma once

#include "../Rule.h"

namespace psm
{
	template< typename Rule, std::size_t Max >
	class UntilMax : public RuleBase
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
				return { RuleMatchCode::True };
			if( input.position() < Max )
			{
				if( input.consume() )
					return { RuleMatchCode::CallNested, 0 };
				return { RuleMatchCode::NotTrueYet };
			}
			return { RuleMatchCode::False };
		}
	};

	template< typename Rule >
	class Until : public UntilMax< Rule, ( std::size_t )-1 >
	{
	};
}