#pragma once

#include "../Rule.h"

namespace psm
{
	class End : public RuleBase
	{
	public:
		using Rules = std::tuple<>;
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.empty() )
				return { RuleMatchCode::True };
			return { RuleMatchCode::False };
		}
	};
} // namespace psm